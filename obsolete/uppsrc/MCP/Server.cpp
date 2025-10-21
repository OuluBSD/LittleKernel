#include "MCP.h"

NAMESPACE_UPP

bool McpServerCore::Start(int port) {
    if(running) return true;
    listen_port = port ? port : 7326;
    stop = 0;
    thr.Run([=]{ const_cast<McpServerCore*>(this)->Loop(); });
    running = true;
    return true;
}

void McpServerCore::Stop() {
    if(!running) return;
    stop = 1;
    if(thr.IsOpen()) thr.Wait();
    if(clients_thr.IsOpen()) clients_thr.Wait();
    running = false;
}

void McpServerCore::Loop() {
    TcpSocket listener;
    if(!listener.Listen(listen_port, 128, true)) {
        RLOG("MCP: listen failed on port " << listen_port << ": " << listener.GetErrorDesc());
        return;
    }
    listener.NoDelay();
    clients_thr.Start(THISBACK(HandleClients));
    while(!stop) {
        One<TcpSocket> s; s.Create(); s->Timeout(5);
        if(s->Accept(listener)) {
            s->NoDelay();
            McpClient c; c.sock = s.Detach(); c.last_activity = GetSysTime(); c.id = next_client_id++;
            clients_lock.EnterWrite(); clients.Add(pick(c)); clients_lock.LeaveWrite();
        }
    }
}

void McpServerCore::HandleClients() {
    while(!stop) {
        for(int i = 0;; i++) {
            clients_lock.EnterRead();
            if(i >= clients.GetCount()) { clients_lock.LeaveRead(); break; }
            McpClient& c = clients[i];
            clients_lock.LeaveRead();
            if(!c.sock->IsOpen()) { clients_lock.EnterWrite(); clients.Remove(i--); clients_lock.LeaveWrite(); continue; }
            Vector<String> msgs;
            if(ReadFramed(c, msgs)) {
                for(const String& line : msgs) {
                    McpRequest req; String reply;
                    if(ParseRequest(line, req)) {
                        Time start = GetSysTime();
                        reply = Handle(req);
                        int dur = GetSysTime().Get() - start.Get();
                        LogRequest(c.id, req, "ok", dur);
                        if(!IsNotification(req)) { reply.Cat('\n'); c.outbuf.Cat(reply); }
                    } else {
                        reply = MakeError(String(), PARSE_ERROR, "Parse error"); reply.Cat('\n'); c.outbuf.Cat(reply);
                        McpRequest dummy; LogRequest(c.id, dummy, "parse_error", -1);
                    }
                }
            }
            WritePending(c);
            if(c.sock->IsError() || c.sock->IsEof()) { clients_lock.EnterWrite(); clients.Remove(i--); clients_lock.LeaveWrite(); continue; }
        }
        Sleep(5);
    }
    for(auto& c : clients) if(c.sock->IsOpen()) c.sock->Close();
    clients_lock.EnterWrite(); clients.Clear(); clients_lock.LeaveWrite();
}

String McpServerCore::Handle(const McpRequest& req) {
    if(IsNull(req.method)) return MakeError(req.id, INVALID_REQUEST, "Invalid request: missing method");
    if(req.method == "mcp.ping") { ValueMap r; r.Add("text","pong"); return MakeResult(req.id, r); }
    if(req.method == "mcp.capabilities") {
        ValueArray methods;
        methods.Add("mcp.ping"); methods.Add("mcp.capabilities");
        methods.Add("mcp.log.get"); methods.Add("mcp.log.clear");
        methods.Add("mcp.index.status"); methods.Add("mcp.index.refresh");
        ValueMap caps; caps.Add("protocol","jsonrpc-2.0"); caps.Add("supports_batch", false); caps.Add("methods", methods);
        return MakeResult(req.id, caps);
    }
    if(req.method == "mcp.log.get") {
        int limit = 200, min_level = MCP_INFO;
        if(IsValueMap(req.params)) { ValueMap p = req.params; limit = (int)(p.Get("limit", limit)); min_level = (int)(p.Get("min_level", min_level)); }
        ValueArray arr; for(const auto& e : sMcpLog.Snapshot(limit, min_level)) { ValueMap v; v.Add("ts", Format("%", e.ts)); v.Add("level", e.level); v.Add("client_id", e.client_id); v.Add("req_id", e.req_id); v.Add("method", e.method); v.Add("message", e.message); v.Add("duration_ms", e.duration_ms); arr.Add(v);} ValueMap r; r.Add("items", arr); r.Add("size", sMcpLog.Size()); return MakeResult(req.id, r);
    }
    if(req.method == "mcp.log.clear") { int before = sMcpLog.Size(); sMcpLog.Clear(); ValueMap r; r.Add("cleared", before); return MakeResult(req.id, r); }
    if(req.method == "mcp.index.status") { ValueMap r; r.Add("ready", false); r.Add("builder", "unknown"); r.Add("last_update", (int)0); r.Add("stale_files", 0); return MakeResult(req.id, r); }
    if(req.method == "mcp.index.refresh") { ValueMap r; r.Add("accepted", true); r.Add("mode","script_build"); return MakeResult(req.id, r); }
    String ext = HandleExtended(req);
    if(!IsNull(ext)) return ext;
    return MakeError(req.id, METHOD_NOT_FOUND, "Method not found");
}

bool McpServerCore::ReadFramed(McpClient& c, Vector<String>& out_msgs) {
    c.sock->Timeout(0);
    if(!c.sock->Peek()) return false;
    String chunk = c.sock->GetLine(); if(c.sock->IsError()) { c.sock->Close(); return false; }
    if(chunk.IsEmpty()) return false; c.inbuf.Cat(chunk); c.last_activity = GetSysTime();
    if(c.inbuf.GetLength() > max_message_bytes) { c.sock->Close(); return false; }
    for(;;) { int p = c.inbuf.Find('\n'); if(p < 0) break; String line = c.inbuf.Mid(0,p); c.inbuf.Remove(0,p+1); if(line.GetCount()) out_msgs.Add(line); }
    return out_msgs.GetCount();
}

bool McpServerCore::WritePending(McpClient& c) {
    if(c.outbuf.IsEmpty()) return false;
    int n = c.sock->Put(c.outbuf); if(c.sock->IsError()) { c.sock->Close(); return false; }
    if(n > 0) { c.outbuf.Remove(0, n); c.last_activity = GetSysTime(); return true; }
    return false;
}

void McpServerCore::LogRequest(int client_id, const McpRequest& req, const String& status, int duration_ms) {
    int level = status == "ok" ? MCP_INFO : (status == "parse_error" ? MCP_ERROR : MCP_WARN);
    McpLogEntry e; e.ts = GetSysTime(); e.level = level; e.client_id = client_id; e.req_id = req.id; e.method = req.method; e.message = status; e.duration_ms = duration_ms; sMcpLog.Add(e);
}

END_UPP_NAMESPACE

