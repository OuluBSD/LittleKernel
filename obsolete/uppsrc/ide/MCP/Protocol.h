#ifndef _ide_MCP_Protocol_h_
#define _ide_MCP_Protocol_h_


struct McpConfig {
	
};

struct McpRequest {
    String id;
    String method;
    Value  params; // JSON object
};

struct McpResponse {
    String id;
    Value  result; // or error
    bool   is_error = false;
};

// JSON-RPC 2.0 error helpers
enum McpErrorCode {
    PARSE_ERROR     = -32700,
    INVALID_REQUEST = -32600,
    METHOD_NOT_FOUND= -32601,
    INVALID_PARAMS  = -32602,
    INTERNAL_ERROR  = -32603,
};

inline bool ParseRequest(const String& json, McpRequest& out) {
    Value v = ParseJSON(json);
    if(!IsValueMap(v)) return false;
    ValueMap m = v;
    String ver = AsString(m.Get("jsonrpc", Value()));
    if(ver != "2.0" && ver != "") {
        return false;
    }
    out.id = AsString(m.Get("id", Value()));
    out.method = AsString(m.Get("method", Value()));
    out.params = m.Get("params", Value());
    return true;
}

// Minimal node/edit protocol types (stubs)
struct McpLocation { String file; int line = 0; int column = 0; };
struct McpRange { McpLocation start; McpLocation end; };
struct McpEditHunk { McpRange range; String text; };
struct McpPlannedEdit { String file; Array<McpEditHunk> hunks; };

inline String MakeResult(const String& id, const Value& result) {
    ValueMap m;
    m.Add("jsonrpc", "2.0");
    m.Add("id", id);
    m.Add("result", result);
    return AsJSON(m);
}

inline String MakeError(const String& id, int code, const String& message) {
    ValueMap err;
    err.Add("code", code);
    err.Add("message", message);
    ValueMap m;
    m.Add("jsonrpc", "2.0");
    m.Add("id", IsNull(id) ? Value() : Value(id));
    m.Add("error", err);
    return AsJSON(m);
}

inline bool IsNotification(const McpRequest& r) {
    return IsNull(r.id);
}

#endif
