#ifndef _ide_MCP_Log_h_
#define _ide_MCP_Log_h_


inline void MCPLOG(int level, int client_id, const String& req_id, const String& method, const String& msg, int duration_ms = -1) {
    McpLogEntry e;
    e.ts = GetSysTime();
    e.level = level;
    e.client_id = client_id;
    e.req_id = req_id;
    e.method = method;
    e.message = msg;
    e.duration_ms = duration_ms;
    sMcpLog.Add(e);
}


#endif

