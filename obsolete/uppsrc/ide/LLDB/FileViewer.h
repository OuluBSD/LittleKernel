#ifndef _ide_LLDB_FileViewer_h_
#define _ide_LLDB_FileViewer_h_

class FileViewer {
    Vector<String> m_lines;
    ArrayMap<FileHandle, Index<int>> m_breakpoint_cache;
    Index<int>* m_breakpoints = 0;

    Opt<int> m_highlighted_line = {};
    bool m_highlight_line_needs_focus = false;

public:
    void Show(FileHandle handle);
    Opt<int> Render();
    void SynchronizeBreakpointCache(lldb::SBTarget target);

    inline void SetHighlightLine(int line)
    {
        m_highlighted_line = line;
        m_highlight_line_needs_focus = true;
    }

    inline void UnsetHighlightLine()
    {
        m_highlighted_line = {};
        m_highlight_line_needs_focus = false;
    }
};


#endif
