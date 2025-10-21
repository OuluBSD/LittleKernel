#ifndef _ide_LLDB_Application_h_
#define _ide_LLDB_Application_h_


// TODO: this struct doesn't need to be publically exposed
struct UserInterface {
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

    ImFont* font = nullptr;
    GLFWwindow* window = nullptr;

    static Opt<UserInterface> init();

private:
    UserInterface() = default;
};

struct Application {
    lldb::SBDebugger debugger;
    lldb::SBListener listener;
    LLDBCommandLine cmdline;
    StreamBuffer _stdout;
    StreamBuffer _stderr;

    OpenFiles open_files;
    One<FileBrowserNode> file_browser;
    UserInterface ui;
    FileViewer file_viewer;
    FPSTimer fps_timer;

    Application(const UserInterface&, Opt<VfsPath>);
    ~Application();

    Application() = delete;
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;
};

int main_loop(Application& app);

lldb::SBCommandReturnObject run_lldb_command(Application& app, const char* command,
                                             bool hide_from_history = false);


bool process_is_stopped(lldb::SBProcess& process);

#endif
