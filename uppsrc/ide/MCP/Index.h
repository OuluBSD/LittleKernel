#ifndef _ide_MCP_Index_h_
#define _ide_MCP_Index_h_


struct McpNode {
    String id;
    String kind;
    String name;
    String file;
    int    start_line = 0;
    int    start_col = 0;
    int    end_line = 0;
    int    end_col = 0;
};

// Minimal placeholder index; resolves a node by file+position heuristically.
struct McpIndex {
    bool Ready() const { return true; }
    McpNode Locate(const String& file, int line, int col) const;
    McpNode Get(const String& id) const;
};

extern McpIndex sMcpIndex;


#endif

