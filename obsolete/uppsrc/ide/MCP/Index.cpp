#include "MCP.h"

NAMESPACE_UPP

McpIndex sMcpIndex;

static String MakeNodeId(const String& file, int line, int col) {
    return String().Cat() << file << ":" << line << ":" << col;
}

McpNode McpIndex::Locate(const String& file, int line, int col) const {
    McpNode n;
    n.id = MakeNodeId(file, line, col);
    n.kind = "unknown";
    n.name = GetFileName(file);
    n.file = file;
    n.start_line = line;
    n.start_col = col;
    n.end_line = line;
    n.end_col = col+1;
    return n;
}

McpNode McpIndex::Get(const String& id) const {
    McpNode n;
    n.id = id;
    // naive parse back
    Vector<String> p = Split(id, ':');
    if(p.GetCount() >= 3) {
        n.file = p[0];
        n.start_line = StrInt(p[1]);
        n.start_col = StrInt(p[2]);
        n.end_line = n.start_line;
        n.end_col = n.start_col+1;
        n.kind = "unknown";
        n.name = GetFileName(n.file);
    }
    return n;
}

END_UPP_NAMESPACE

