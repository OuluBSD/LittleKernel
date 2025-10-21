#ifndef _ide_LLDB_DebuggerApp_h_
#define _ide_LLDB_DebuggerApp_h_


class LLDBDebuggerApp : public TopWindow {
	Splitter hsplit, lsplit, csplit, bsplit, rsplit;
	TreeCtrl files;
	TabCtrl ftabs, btabs;
	WithConsoleLayout<Ctrl> console;
	DocEdit normal, error;
	ArrayMap<String,CodeEditor> codes;
	WithThreadsLayout<Ctrl> threads;
	TabCtrl loctabs, breaktabs;
	TreeCtrl locals;
	ArrayCtrl stack, regs, breaks, watchs;
	DocEdit dbg_stream;
	
	
	Opt<lldb::SBProcess> process;
	
    uint32 viewed_thread_index = 0;
    uint32 viewed_frame_index = 0;
    uint32 viewed_breakpoint_index = 0;

    float window_width = -1.f;   // in pixels
    float window_height = -1.f;  // in pixels

    float file_browser_width = -1.f;
    float file_viewer_width = -1.f;
    float file_viewer_height = -1.f;
    float console_height = -1.f;

    bool request_manual_tab_change = false;
    bool ran_command_last_frame = false;
    bool window_resized_last_frame = false;

    size_t frames_rendered = 0;
	
public:
	typedef LLDBDebuggerApp CLASSNAME;
	LLDBDebuggerApp();
	
	void Data();
	void DataLocalRecursive(int parnode, lldb::SBValue local);
	
};


#endif
