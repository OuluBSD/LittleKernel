#include "MCP.h"
#include <ide/ide.h>

NAMESPACE_UPP

McpServer sMcpServer;

bool McpServer::Start(int port) {
    if(!port && TheIde())
        port = TheIde()->mcp_server_port;
    Thread::AtShutdown(StopMcpServer);
    return McpServerCore::Start(port ? port : 7326);
}

void McpServer::Stop() {
    McpServerCore::Stop();
}

String McpServer::HandleExtended(const McpRequest& req) {
    if(req.method == "workspace.info") {
        ValueMap r; r.Add("name", GetCurrentWorkspaceName()); r.Add("packages", GetCurrentWorkspacePackageCount());
        return MakeResult(req.id, r);
    }
    if(req.method == "mcp.index.status") {
        ValueMap r; String builder = "unknown"; bool ready = false;
        if(const MakeBuild* make = TheIde()) { VectorMap<String, String> bm = GetMethodVars(make->method); builder = bm.Get("BUILDER", "GCC"); ready = ToUpper(builder) == "SCRIPT"; }
        IdeMetaEnvironment& ienv = IdeMetaEnv(); MetaEnvironment& env = ienv.env; (void)env;
        r.Add("ready", ready); r.Add("builder", builder); r.Add("last_update", (int)0); r.Add("stale_files", 0);
        if(!ready) r.Add("note", "SCRIPT builder required to populate AST/index via ScriptBuilder");
        return MakeResult(req.id, r);
    }
    if(req.method == "mcp.index.refresh") { ValueMap r; r.Add("accepted", true); r.Add("mode", "script_build"); return MakeResult(req.id, r); }
    if(req.method == "node.locate") {
        if(!IsValueMap(req.params)) return MakeError(req.id, INVALID_PARAMS, "Expected params object");
        ValueMap p = req.params; String file = AsString(p.Get("file", Value())); int line = (int)(p.Get("line", 0)); int col = (int)(p.Get("column", 0));
        ValueMap r; EnvStatusInfo st = EnvStatus();
        if(st.initialized) { EnvNodeInfo ei = EnvLocate(file, line, col); r.Add("id", ei.id); r.Add("kind", ei.kind); r.Add("name", ei.name); r.Add("file", ei.file); r.Add("start_line", ei.start_line); r.Add("start_col", ei.start_col); r.Add("end_line", ei.end_line); r.Add("end_col", ei.end_col); }
        else { McpNode n = sMcpIndex.Locate(file, line, col); r.Add("id", n.id); r.Add("kind", n.kind); r.Add("name", n.name); r.Add("file", n.file); r.Add("start_line", n.start_line); r.Add("start_col", n.start_col); r.Add("end_line", n.end_line); r.Add("end_col", n.end_col); r.Add("note", "index_not_ready"); }
        return MakeResult(req.id, r);
    }
    if(req.method == "node.get") {
        if(!IsValueMap(req.params)) return MakeError(req.id, INVALID_PARAMS, "Expected params object");
        ValueMap p = req.params; String id = AsString(p.Get("id", Value())); ValueMap r; EnvStatusInfo st = EnvStatus();
        if(st.initialized) { EnvNodeInfo ei = EnvGet(id); r.Add("id", ei.id); r.Add("kind", ei.kind); r.Add("name", ei.name); r.Add("file", ei.file); r.Add("start_line", ei.start_line); r.Add("start_col", ei.start_col); r.Add("end_line", ei.end_line); r.Add("end_col", ei.end_col); r.Add("code", EnvCodeById(id)); }
        else { McpNode n = sMcpIndex.Get(id); r.Add("id", n.id); r.Add("kind", n.kind); r.Add("name", n.name); r.Add("file", n.file); r.Add("start_line", n.start_line); r.Add("start_col", n.start_col); r.Add("end_line", n.end_line); r.Add("end_col", n.end_col); r.Add("code", String()); r.Add("note", "index_not_ready"); }
        return MakeResult(req.id, r);
    }
    if(req.method == "node.definition") {
        if(!IsValueMap(req.params)) return MakeError(req.id, INVALID_PARAMS, "Expected params object");
        ValueMap p = req.params; String id = AsString(p.Get("id", Value())); ValueArray items; EnvStatusInfo st = EnvStatus();
        if(st.initialized) { Vector<EnvNodeInfo> defs = EnvDefinition(id); for(const auto& d : defs) { ValueMap v; v.Add("id", d.id); v.Add("file", d.file); v.Add("start_line", d.start_line); v.Add("start_col", d.start_col); v.Add("end_line", d.end_line); v.Add("end_col", d.end_col); items.Add(v); } }
        ValueMap r; r.Add("items", items); if(!st.initialized) r.Add("note", "index_not_ready"); return MakeResult(req.id, r);
    }
    if(req.method == "node.references") {
        if(!IsValueMap(req.params)) return MakeError(req.id, INVALID_PARAMS, "Expected params object");
        ValueMap p = req.params; String id = AsString(p.Get("id", Value())); String page = AsString(p.Get("page_token", String())); int limit = (int)(p.Get("limit", 200));
        ValueArray items; String next; EnvStatusInfo st = EnvStatus();
        if(st.initialized) { EnvRefPage pg = EnvReferences(id, page, limit); next = pg.next_page_token; for(const auto& it : pg.items) { ValueMap v; v.Add("id", it.id); v.Add("file", it.file); v.Add("start_line", it.start_line); v.Add("start_col", it.start_col); v.Add("end_line", it.end_line); v.Add("end_col", it.end_col); items.Add(v); } }
        ValueMap r; r.Add("items", items); r.Add("next_page_token", next); if(!st.initialized) r.Add("note", "index_not_ready"); return MakeResult(req.id, r);
    }
    if(req.method == "edits.apply") {
        if(!IsValueArray(req.params)) return MakeError(req.id, INVALID_PARAMS, "Expected params as array of edits");
        ValueArray a = req.params; for(const Value& v : a) if(!IsValueMap(v)) return MakeError(req.id, INVALID_PARAMS, "Edit must be object"); ValueMap r; r.Add("applied", a.GetCount()); return MakeResult(req.id, r);
    }
    return String();
}

bool StartMcpServer(const McpConfig&) { Thread::AtShutdown(StopMcpServer); return sMcpServer.Start(); }
void StopMcpServer() { sMcpServer.Stop(); }
bool McpIsRunning() { return sMcpServer.IsRunning(); }

END_UPP_NAMESPACE
