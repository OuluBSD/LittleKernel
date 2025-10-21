#ifndef _ide_LLDB_LLDBCommandLine_h_
#define _ide_LLDB_LLDBCommandLine_h_

// TODO: convert to variant to represent either user command, stdout, sterr, or log
// message
struct CommandLineEntry : Moveable<CommandLineEntry> {
    String input;
    String output;
    String error_msg;  // TODO: use Opt and eliminate 'succeeded' bool
    bool succeeded;
};

class LLDBCommandLine {
    lldb::SBCommandInterpreter m_interpreter;
    Vector<CommandLineEntry> m_history;

public:
    LLDBCommandLine(lldb::SBDebugger& debugger);

    lldb::SBCommandReturnObject run_command(const char* command,
                                            bool hide_from_history = false);
    Opt<String> expand_and_unalias_command(const char* command);

    // TODO: for_each_history_entry
    const Vector<CommandLineEntry>& get_history() const { return m_history; }
};

#endif
