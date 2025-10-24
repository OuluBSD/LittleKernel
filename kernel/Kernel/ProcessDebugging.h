#ifndef _Kernel_ProcessDebugging_h_
#define _Kernel_ProcessDebugging_h_

#include "Defs.h"
#include "ProcessControlBlock.h"

// Process debugging states
enum ProcessDebugState {
    DEBUG_STATE_DISABLED = 0,     // Debugging not enabled for this process
    DEBUG_STATE_STOPPED,          // Process is stopped and waiting for debugger
    DEBUG_STATE_RUNNING,          // Process is running normally
    DEBUG_STATE_SINGLE_STEP,      // Process is running in single-step mode
    DEBUG_STATE_BREAKPOINT_HIT,   // Process hit a breakpoint
    DEBUG_STATE_SIGNALLED,        // Process received a signal
    DEBUG_STATE_EXITED,           // Process has exited
    DEBUG_STATE_COREDUMPED        // Process has dumped core
};

// Process debugging events
enum ProcessDebugEvent {
    DEBUG_EVENT_NONE = 0,          // No event
    DEBUG_EVENT_FORK,              // Process forked
    DEBUG_EVENT_VFORK,             // Process vforked
    DEBUG_EVENT_EXEC,              // Process executed a new program
    DEBUG_EVENT_EXIT,              // Process exited
    DEBUG_EVENT_EXIT_GROUP,        // Process group exited
    DEBUG_EVENT_STOPPED,           // Process stopped by signal
    DEBUG_EVENT_CONTINUED,         // Process continued from stop
    DEBUG_EVENT_SIGNAL,            // Process received signal
    DEBUG_EVENT_BREAKPOINT,        // Process hit breakpoint
    DEBUG_EVENT_WATCHPOINT,        // Process hit watchpoint
    DEBUG_EVENT_SINGLESTEP,        // Process completed single step
    DEBUG_EVENT_SYSCALL_ENTER,     // Process entering system call
    DEBUG_EVENT_SYSCALL_EXIT,      // Process exiting system call
    DEBUG_EVENT_PAGEFAULT,         // Process had page fault
    DEBUG_EVENT_ILLEGAL_INST,      // Process executed illegal instruction
    DEBUG_EVENT_DIVZERO,           // Process attempted divide by zero
    DEBUG_EVENT_MEMORY_ACCESS,     // Process had illegal memory access
    DEBUG_EVENT_IO_ACCESS,         // Process had illegal I/O access
    DEBUG_EVENT_PRIVILEGE_VIOLATION, // Process violated privilege level
    DEBUG_EVENT_ALIGNMENT,          // Process had alignment fault
    DEBUG_EVENT_FLOATING_POINT,     // Process had floating point exception
    DEBUG_EVENT_CHILD_EXIT,         // Child process exited
    DEBUG_EVENT_THREAD_CREATE,      // Thread created
    DEBUG_EVENT_THREAD_EXIT,        // Thread exited
    DEBUG_EVENT_LIBRARY_LOAD,      // Library loaded
    DEBUG_EVENT_LIBRARY_UNLOAD,    // Library unloaded
    DEBUG_EVENT_EXCEPTION,         // Process had exception
    DEBUG_EVENT_USER,               // User-defined debug event
    DEBUG_EVENT_CLONE,             // Thread/process cloned
    DEBUG_EVENT_PTRACE_TRAP,        // ptrace trap event
    DEBUG_EVENT_SECCOMP,           // seccomp event (Linux compatibility)
    DEBUG_EVENT_SECURE_STOP        // Secure stop event
};

// Debugging breakpoints
struct ProcessBreakpoint {
    uint32 address;                // Breakpoint address
    uint32 original_instruction;   // Original instruction at breakpoint
    uint32 flags;                  // Breakpoint flags
    ProcessBreakpoint* next;        // Next breakpoint in chain
    ProcessBreakpoint* prev;        // Previous breakpoint in chain
    uint32 hit_count;              // Number of times breakpoint was hit
    uint32 condition;              // Condition for conditional breakpoint
    uint32 ignore_count;           // Ignore hits until this count reaches 0
    char description[64];           // Breakpoint description for debugging
};

// Debugging watchpoints
struct ProcessWatchpoint {
    uint32 address;                // Watched address
    uint32 size;                   // Size of watched region
    uint32 access_type;            // Access type (read, write, execute)
    uint32 flags;                  // Watchpoint flags
    ProcessWatchpoint* next;        // Next watchpoint in chain
    ProcessWatchpoint* prev;        // Previous watchpoint in chain
    uint32 hit_count;              // Number of times watchpoint was hit
    uint32 condition;              // Condition for conditional watchpoint
    uint32 ignore_count;           // Ignore hits until this count reaches 0
    char description[64];           // Watchpoint description for debugging
};

// Debugging context
struct DebuggingContext {
    ProcessDebugState state;        // Current debugging state
    ProcessDebugEvent last_event;    // Last debugging event
    uint32 event_address;           // Address where last event occurred
    uint32 event_data;              // Additional data for last event
    uint32 debugger_pid;            // PID of attached debugger
    uint32 flags;                   // Debugging flags
    ProcessBreakpoint* breakpoints; // List of breakpoints for this process
    ProcessWatchpoint* watchpoints; // List of watchpoints for this process
    uint32 breakpoint_count;       // Number of breakpoints
    uint32 watchpoint_count;        // Number of watchpoints
    uint32 single_step_count;       // Number of single steps completed
    uint32 total_events;            // Total debugging events
    uint32 last_event_time;         // Time of last event (ticks)
    uint32 event_buffer_size;       // Size of event buffer
    ProcessDebugEvent* event_buffer; // Buffer of recent events
    uint32 event_buffer_head;       // Head of event buffer
    uint32 event_buffer_tail;       // Tail of event buffer
    bool event_buffer_full;         // Whether event buffer is full
};

// Debugging session
struct DebuggingSession {
    uint32 session_id;               // Debugging session ID
    uint32 debuggee_pid;            // PID of process being debugged
    uint32 debugger_pid;            // PID of attaching debugger
    ProcessDebugState session_state; // Current session state
    uint32 session_flags;            // Session flags
    uint32 attach_time;             // Time when debugger attached (ticks)
    uint32 detach_time;             // Time when debugger detached (ticks)
    uint32 total_debug_time;        // Total time spent debugging (ticks)
    uint32 events_handled;          // Number of events handled
    uint32 breakpoints_set;          // Number of breakpoints set
    uint32 watchpoints_set;         // Number of watchpoints set
    uint32 single_steps;            // Number of single steps
    uint32 syscalls_traced;         // Number of system calls traced
    uint32 signals_delivered;       // Number of signals reported
    DebuggingContext* dbg_context;   // Debugging context for this session
    DebuggingSession* next;          // Next session in chain
    DebuggingSession* prev;          // Previous session in chain
};

// Debugging options
struct DebuggingOptions {
    bool enable_breakpoints;        // Enable breakpoint support
    bool enable_watchpoints;        // Enable watchpoint support
    bool enable_singlestep;         // Enable single-step support
    bool enable_syscall_trace;      // Enable system call tracing
    bool enable_signal_trace;       // Enable signal tracing
    bool enable_exception_trace;     // Enable exception tracing
    uint32 max_breakpoints;          // Maximum breakpoints per process
    uint32 max_watchpoints;         // Maximum watchpoints per process
    uint32 timeout;                  // Debug operation timeout (ms)
};

// Debugging configuration
struct DebuggingConfig {
    uint32 flags;                    // Global debugging flags
    uint32 max_sessions;            // Maximum debugging sessions
    uint32 default_timeout;          // Default timeout for operations (ms)
    bool log_debug_events;           // Log debugging events to file
    char log_file[256];              // Debug log file path
    uint32 max_log_size;            // Maximum log file size
    bool rotate_logs;               // Rotate log files
    uint32 retention_days;           // Keep logs for specified days
    bool compress_old;              // Compress old records
    uint32 compression_threshold;    // Compress records older than this
};

// Debugging statistics
struct DebuggingStats {
    uint32 total_sessions;           // Total debugging sessions
    uint32 active_sessions;          // Currently active sessions
    uint32 total_events;            // Total debugging events
    uint32 breakpoints_hit;          // Total breakpoints hit
    uint32 watchpoints_hit;         // Total watchpoints hit
    uint32 single_steps_completed;    // Total single steps completed
    uint32 syscalls_traced;         // Total system calls traced
    uint32 signals_traced;          // Total signals traced
    uint32 exceptions_traced;       // Total exceptions traced
    uint32 timeouts;                // Total timeouts occurred
    uint32 errors;                  // Total debugging errors
    uint32 event_buffer_overflows;    // Event buffer overflow count
};

// Debugging flags
const uint32 DEBUG_FLAG_ENABLED = 0x00000001;          // Debugging enabled
const uint32 DEBUG_FLAG_ATTACHED = 0x00000002;        // Debugger is attached
const uint32 DEBUG_FLAG_STOPPED = 0x00000004;          // Process is stopped
const uint32 DEBUG_FLAG_SINGLESTEP = 0x00000008;      // Single-stepping active
const uint32 DEBUG_FLAG_TRACING_SYSCALLS = 0x00000010; // Tracing system calls
const uint32 DEBUG_FLAG_TRACING_SIGNALS = 0x00000020;  // Tracing signals
const uint32 DEBUG_FLAG_TRACING_EXCEPTIONS = 0x00000040; // Tracing exceptions
const uint32 DEBUG_FLAG_CORE_DUMP_ON_CRASH = 0x00000200; // Core dump on crash
const uint32 DEBUG_FLAG_SUPPRESS_OUTPUT = 0x00000400;    // Suppress process output
const uint32 DEBUG_FLAG_CAPTURE_INPUT = 0x00000800;       // Capture process input
const uint32 DEBUG_FLAG_REMOTE_DEBUG = 0x00001000;       // Remote debugging enabled
const uint32 DEBUG_FLAG_ENCRYPT_COMM = 0x00002000;      // Encrypt communications
const uint32 DEBUG_FLAG_COMPRESS_DATA = 0x00004000;     // Compress debug data
const uint32 DEBUG_FLAG_LOG_EVENTS = 0x00008000;         // Log debug events

// Debugging constants
const uint32 DEBUG_MAX_BREAKPOINTS = 1024;              // Maximum breakpoints
const uint32 DEBUG_MAX_WATCHPOINTS = 512;              // Maximum watchpoints
const uint32 DEBUG_MAX_EVENT_BUFFER = 4096;            // Maximum event buffer size
const uint32 DEBUG_DEFAULT_TIMEOUT = 5000;              // 5 second default timeout
const uint32 DEBUG_MAX_TRACE_DEPTH = 128;              // Maximum call stack trace depth
const uint32 DEBUG_MAX_SYMBOLS = 65536;                // Maximum symbols to resolve
const uint32 DEBUG_REMOTE_PORT = 1234;                 // Default remote debug port
const uint32 DEBUG_MAX_PACKET_SIZE = 65536;             // Maximum packet size (64KB)
const uint32 DEBUG_COMPRESSION_THRESHOLD = 1024;       // Compress packets > 1KB
const uint32 DEBUG_MAX_LOG_SIZE = 10485760;             // Maximum log size (10MB)

// Debugging error codes
enum DebuggingErrorCode {
    DEBUG_SUCCESS = 0,
    DEBUG_ERROR_INVALID_PARAMETER,
    DEBUG_ERROR_PROCESS_NOT_FOUND,
    DEBUG_ERROR_PROCESS_NOT_DEBUGGABLE,
    DEBUG_ERROR_ALREADY_DEBUGGED,
    DEBUG_ERROR_NO_DEBUGGER_ATTACHED,
    DEBUG_ERROR_INVALID_ADDRESS,
    DEBUG_ERROR_INVALID_BREAKPOINT,
    DEBUG_ERROR_INVALID_WATCHPOINT,
    DEBUG_ERROR_OUT_OF_MEMORY,
    DEBUG_ERROR_PERMISSION_DENIED,
    DEBUG_ERROR_TIMEOUT,
    DEBUG_ERROR_NOT_SUPPORTED,
    DEBUG_ERROR_INTERNAL_ERROR,
    DEBUG_ERROR_BUFFER_OVERFLOW,
    DEBUG_ERROR_INVALID_SESSION
};

// Debugging commands (for communication with debugger)
enum DebuggingCommand {
    DEBUG_CMD_ATTACH = 0,           // Attach to process
    DEBUG_CMD_DETACH,               // Detach from process
    DEBUG_CMD_CONTINUE,             // Continue process execution
    DEBUG_CMD_STOP,                 // Stop process execution
    DEBUG_CMD_SINGLESTEP,           // Single step process
    DEBUG_CMD_SET_BREAKPOINT,       // Set breakpoint
    DEBUG_CMD_CLEAR_BREAKPOINT,     // Clear breakpoint
    DEBUG_CMD_SET_WATCHPOINT,       // Set watchpoint
    DEBUG_CMD_CLEAR_WATCHPOINT,     // Clear watchpoint
    DEBUG_CMD_READ_MEMORY,          // Read process memory
    DEBUG_CMD_WRITE_MEMORY,         // Write process memory
    DEBUG_CMD_READ_REGISTERS,       // Read process registers
    DEBUG_CMD_WRITE_REGISTERS,     // Write process registers
    DEBUG_CMD_GET_PROCESS_INFO,     // Get process information
    DEBUG_CMD_GET_THREAD_INFO,      // Get thread information
    DEBUG_CMD_GET_MODULE_INFO,      // Get module information
    DEBUG_CMD_GET_SYMBOL_INFO,      // Get symbol information
    DEBUG_CMD_WAIT_FOR_EVENT,       // Wait for debugging event
    DEBUG_CMD_GET_EVENT,            // Get next debugging event
    DEBUG_CMD_ACKNOWLEDGE_EVENT,    // Acknowledge debugging event
    DEBUG_CMD_SEND_SIGNAL,          // Send signal to process
    DEBUG_CMD_GET_SIGNAL_INFO,      // Get signal information
    DEBUG_CMD_SET_SIGNAL_HANDLER,    // Set signal handler
    DEBUG_CMD_GET_BACKTRACE,        // Get call stack backtrace
    DEBUG_CMD_SET_CONDITION,         // Set breakpoint/watchpoint condition
    DEBUG_CMD_GET_CONDITION,        // Get breakpoint/watchpoint condition
    DEBUG_CMD_EVALUATE_EXPRESSION,   // Evaluate expression in process context
    DEBUG_CMD_GET_VARIABLE_VALUE,    // Get variable value from process
    DEBUG_CMD_SET_VARIABLE_VALUE,    // Set variable value in process
    DEBUG_CMD_GET_REGISTER_VALUE,    // Get specific register value
    DEBUG_CMD_SET_REGISTER_VALUE,    // Set specific register value
    DEBUG_CMD_FLUSH_INSTRUCTION_CACHE, // Flush instruction cache
    DEBUG_CMD_INVALIDATE_TLB,        // Invalidate TLB entries
    DEBUG_CMD_GET_MAPPING_INFO,     // Get memory mapping information
    DEBUG_CMD_SET_MAPPING,          // Set memory mapping
    DEBUG_CMD_GET_PAGE_INFO,        // Get page information
    DEBUG_CMD_SET_PAGE_ATTRIBUTES,   // Set page attributes
    DEBUG_CMD_GET_STACK_INFO,       // Get stack information
    DEBUG_CMD_GET_HEAP_INFO,        // Get heap information
    DEBUG_CMD_ALLOCATE_DEBUG_MEMORY, // Allocate memory in debuggee
    DEBUG_CMD_FREE_DEBUG_MEMORY,    // Free memory in debuggee
    DEBUG_CMD_GET_FILE_DESCRIPTOR_INFO, // Get file descriptor information
    DEBUG_CMD_DUPLICATE_HANDLE,     // Duplicate process handle
    DEBUG_CMD_CLOSE_HANDLE,         // Close process handle
    DEBUG_CMD_GET_ENVIRONMENT,       // Get process environment
    DEBUG_CMD_SET_ENVIRONMENT,       // Set process environment
    DEBUG_CMD_GET_WORKING_DIRECTORY, // Get working directory
    DEBUG_CMD_SET_WORKING_DIRECTORY,  // Set working directory
    DEBUG_CMD_GET_COMMAND_LINE,     // Get command line
    DEBUG_CMD_SET_COMMAND_LINE,     // Set command line
    DEBUG_CMD_GET_PROCESS_TIMES,     // Get process timing information
    DEBUG_CMD_GET_PROCESS_RESOURCES, // Get process resource usage
    DEBUG_CMD_SET_RESOURCE_LIMITS,   // Set resource limits
    DEBUG_CMD_GET_SECURITY_CONTEXT,   // Get security context
    DEBUG_CMD_SET_SECURITY_CONTEXT,   // Set security context
    DEBUG_CMD_GET_PRIVILEGES,        // Get process privileges
    DEBUG_CMD_SET_PRIVILEGES,        // Set process privileges
    DEBUG_CMD_REVERT_TO_SELF,        // Revert to self security context
    DEBUG_CMD_IMPERSONATE,           // Impersonate user
    DEBUG_CMD_GET_SID,              // Get security identifier
    DEBUG_CMD_SET_SID,              // Set security identifier
    DEBUG_CMD_GET_ACL,              // Get access control list
    DEBUG_CMD_SET_ACL,              // Set access control list
    DEBUG_CMD_GET_OWNER,            // Get owner information
    DEBUG_CMD_SET_OWNER,            // Set owner information
    DEBUG_CMD_GET_GROUP,            // Get group information
    DEBUG_CMD_SET_GROUP,            // Set group information
    DEBUG_CMD_ADD_AUDIT_ENTRY,       // Add audit log entry
    DEBUG_CMD_GET_AUDIT_LOG,        // Get audit log
    DEBUG_CMD_CLEAR_AUDIT_LOG,       // Clear audit log
    DEBUG_CMD_ENABLE_AUDITING,       // Enable auditing
    DEBUG_CMD_DISABLE_AUDITING,     // Disable auditing
    DEBUG_CMD_GET_TRUST_LEVEL,       // Get trust level
    DEBUG_CMD_SET_TRUST_LEVEL,       // Set trust level
    DEBUG_CMD_GET_INTEGRITY_LEVEL,    // Get integrity level
    DEBUG_CMD_SET_INTEGRITY_LEVEL,    // Set integrity level
    DEBUG_CMD_GET_TOKEN_INFORMATION, // Get token information
    DEBUG_CMD_SET_TOKEN_INFORMATION,  // Set token information
    DEBUG_CMD_CREATE_RESTRICTED_TOKEN, // Create restricted token
    DEBUG_CMD_FILTER_TOKEN,          // Filter token
    DEBUG_CMD_IS_TOKEN_RESTRICTED,    // Check if token is restricted
    DEBUG_CMD_IS_TOKEN_UNTRUSTED,     // Check if token is untrusted
    DEBUG_CMD_IS_TOKEN_WRITE_RESTRICTED, // Check if token is write-restricted
    DEBUG_CMD_CREATE_LOW_BOX_TOKEN,   // Create low-box token
    DEBUG_CMD_DERIVE_CAPABILITY_SIDS, // Derive capability SIDs
    DEBUG_CMD_DERIVE_RESTRICTED_APP_CONTAINER, // Derive restricted app container
    DEBUG_CMD_GET_APP_CONTAINER_SID_TYPE, // Get app container SID type
    DEBUG_CMD_CHECK_TOKEN_MEMBERSHIP, // Check token membership
    DEBUG_CMD_IS_CHILD_PROCESS_RESTRICTED, // Check if child process is restricted
    DEBUG_CMD_CREATE_PROCESS_WITH_TOKEN,  // Create process with token
    DEBUG_CMD_CREATE_THREAD_WITH_TOKEN,   // Create thread with token
    DEBUG_CMD_OPEN_PROCESS_TOKEN,    // Open process token
    DEBUG_CMD_OPEN_THREAD_TOKEN,     // Open thread token
    DEBUG_CMD_ADJUST_TOKEN_PRIVILEGES, // Adjust token privileges
    DEBUG_CMD_SET_THREAD_TOKEN,      // Set thread token
    DEBUG_CMD_QUERY_INFORMATION_TOKEN // Query information token
};

// Debugging event information
struct DebuggingEventInfo {
    ProcessDebugEvent event_type;    // Type of debugging event
    uint32 process_id;              // Process ID where event occurred
    uint32 thread_id;               // Thread ID where event occurred
    uint32 event_address;           // Address where event occurred
    uint32 event_data;              // Additional event data
    uint32 timestamp;               // Event timestamp (ticks)
    char description[256];            // Event description
};

// Debugging request/response
struct DebuggingRequest {
    DebuggingCommand command;        // Command to execute
    uint32 request_id;              // Request identifier
    uint32 process_id;              // Target process ID
    uint32 thread_id;               // Target thread ID
    uint32 address;                 // Memory address
    uint32 size;                    // Size of data
    void* data;                     // Request data
};

struct DebuggingResponse {
    DebuggingCommand command;        // Command that was executed
    uint32 request_id;              // Request identifier
    DebuggingErrorCode error_code;   // Error code
    uint32 process_id;              // Target process ID
    uint32 thread_id;               // Target thread ID
    uint32 size;                    // Size of response data
    void* data;                     // Response data
};

// Process debugging manager
class ProcessDebuggingManager {
private:
    DebuggingConfig config;          // Debugging configuration
    DebuggingStats stats;            // Debugging statistics
    DebuggingSession* session_list_head; // Head of debugging session list
    uint32 session_count;            // Current session count
    uint32 next_session_id;          // Next session ID
    bool is_initialized;             // Whether manager is initialized
    uint32 last_activity_time;        // Last debug activity time
    ProcessControlBlock* debugged_processes; // List of processes being debugged
    
public:
    ProcessDebuggingManager();
    ~ProcessDebuggingManager();
    
    // Initialization and configuration
    bool Initialize(const DebuggingConfig* config = nullptr);
    bool Configure(const DebuggingConfig* config);
    bool IsInitialized() const;
    bool IsEnabled() const;
    bool Enable();
    bool Disable();
    void Reset();
    
    // Process debugging management
    bool EnableDebugging(uint32 pid, const DebuggingOptions* options = nullptr);
    bool DisableDebugging(uint32 pid);
    bool IsDebugEnabled(uint32 pid);
    bool AttachDebugger(uint32 pid, uint32 debugger_pid);
    bool DetachDebugger(uint32 pid, uint32 debugger_pid);
    bool IsDebuggerAttached(uint32 pid);
    
    // Process debugging control
    bool StopProcess(uint32 pid);
    bool ContinueProcess(uint32 pid);
    bool SingleStepProcess(uint32 pid);
    bool KillProcess(uint32 pid);
    bool SuspendProcess(uint32 pid);
    bool ResumeProcess(uint32 pid);
    
    // Breakpoint management
    bool SetBreakpoint(uint32 pid, uint32 address, const char* description = nullptr);
    bool ClearBreakpoint(uint32 pid, uint32 address);
    bool ClearAllBreakpoints(uint32 pid);
    bool IsBreakpointSet(uint32 pid, uint32 address);
    uint32 GetBreakpointCount(uint32 pid);
    
    // Watchpoint management
    bool SetWatchpoint(uint32 pid, uint32 address, uint32 size, uint32 access_type,
                       const char* description = nullptr);
    bool ClearWatchpoint(uint32 pid, uint32 address);
    bool ClearAllWatchpoints(uint32 pid);
    bool IsWatchpointSet(uint32 pid, uint32 address);
    uint32 GetWatchpointCount(uint32 pid);
    
    // Debugging event management
    bool WaitForEvent(uint32 pid, DebuggingEventInfo* event_info, uint32 timeout_ms = 0);
    bool GetNextEvent(DebuggingEventInfo* event_info);
    bool AcknowledgeEvent(uint32 event_id);
    bool SendSignalToProcess(uint32 pid, uint32 signal);
    bool GetSignalInfo(uint32 pid, uint32 signal, void* signal_info);
    
    // Memory debugging
    bool ReadProcessMemory(uint32 pid, uint32 address, void* buffer, uint32 size);
    bool WriteProcessMemory(uint32 pid, uint32 address, const void* buffer, uint32 size);
    bool ReadRegisterValue(uint32 pid, uint32 register_id, uint32* value);
    bool WriteRegisterValue(uint32 pid, uint32 register_id, uint32 value);
    bool GetBacktrace(uint32 pid, uint32* addresses, uint32 max_addresses, uint32* actual_count);
    
    // Debugging session management
    DebuggingSession* CreateSession(uint32 debuggee_pid, uint32 debugger_pid);
    bool DestroySession(uint32 session_id);
    DebuggingSession* GetSessionById(uint32 session_id);
    DebuggingSession* GetSessionByPid(uint32 pid);
    uint32 GetSessionCount();
    uint32 GetActiveSessionCount();
    
    // Debugging utilities
    bool GetProcessInfo(uint32 pid, void* process_info);
    bool GetThreadInfo(uint32 pid, uint32 tid, void* thread_info);
    bool GetModuleInfo(uint32 pid, uint32 module_id, void* module_info);
    bool GetSymbolInfo(uint32 pid, const char* symbol_name, uint32* address);
    bool EvaluateExpression(uint32 pid, const char* expression, uint32* result);
    bool GetVariableValue(uint32 pid, const char* variable_name, uint32* value);
    bool SetVariableValue(uint32 pid, const char* variable_name, uint32 value);
    bool GetRegisterValue(uint32 pid, uint32 register_id, uint32* value);
    bool SetRegisterValue(uint32 pid, uint32 register_id, uint32 value);
    bool FlushInstructionCache(uint32 pid);
    bool InvalidateTLB(uint32 pid);
    bool GetMappingInfo(uint32 pid, uint32 address, void* mapping_info);
    bool SetMapping(uint32 pid, uint32 address, void* mapping_info);
    bool GetPageInfo(uint32 pid, uint32 address, void* page_info);
    bool SetPageAttributes(uint32 pid, uint32 address, uint32 attributes);
    bool GetStackInfo(uint32 pid, void* stack_info);
    bool GetHeapInfo(uint32 pid, void* heap_info);
    bool AllocateDebugMemory(uint32 pid, uint32 size, void** address);
    bool FreeDebugMemory(uint32 pid, void* address);
    bool GetFileDescriptorInfo(uint32 pid, uint32 fd, void* fd_info);
    bool DuplicateHandle(uint32 pid, uint32 handle, uint32* new_handle);
    bool CloseHandle(uint32 pid, uint32 handle);
    bool GetEnvironment(uint32 pid, void* env_info);
    bool SetEnvironment(uint32 pid, const void* env_info);
    bool GetWorkingDirectory(uint32 pid, char* buffer, uint32 size);
    bool SetWorkingDirectory(uint32 pid, const char* path);
    bool GetCommandLine(uint32 pid, char* buffer, uint32 size);
    bool SetCommandLine(uint32 pid, const char* command_line);
    bool GetProcessTimes(uint32 pid, void* times_info);
    bool GetProcessResources(uint32 pid, void* resources_info);
    bool SetResourceLimits(uint32 pid, const void* limits_info);
    bool GetSecurityContext(uint32 pid, void* security_info);
    bool SetSecurityContext(uint32 pid, const void* security_info);
    bool GetPrivileges(uint32 pid, void* privileges_info);
    bool SetPrivileges(uint32 pid, const void* privileges_info);
    bool RevertToSelf(uint32 pid);
    bool Impersonate(uint32 pid, uint32 user_id);
    bool GetSID(uint32 pid, void* sid_info);
    bool SetSID(uint32 pid, const void* sid_info);
    bool GetACL(uint32 pid, void* acl_info);
    bool SetACL(uint32 pid, const void* acl_info);
    bool GetOwner(uint32 pid, void* owner_info);
    bool SetOwner(uint32 pid, const void* owner_info);
    bool GetGroup(uint32 pid, void* group_info);
    bool SetGroup(uint32 pid, const void* group_info);
    bool AddAuditEntry(uint32 pid, const void* audit_info);
    bool GetAuditLog(uint32 pid, void* audit_log);
    bool ClearAuditLog(uint32 pid);
    bool EnableAuditing(uint32 pid);
    bool DisableAuditing(uint32 pid);
    bool GetTrustLevel(uint32 pid, uint32* trust_level);
    bool SetTrustLevel(uint32 pid, uint32 trust_level);
    bool GetIntegrityLevel(uint32 pid, uint32* integrity_level);
    bool SetIntegrityLevel(uint32 pid, uint32 integrity_level);
    bool GetTokenInformation(uint32 pid, void* token_info);
    bool SetTokenInformation(uint32 pid, const void* token_info);
    bool CreateRestrictedToken(uint32 pid, void* restricted_token);
    bool FilterToken(uint32 pid, const void* filter_info, void* filtered_token);
    bool IsTokenRestricted(uint32 pid, bool* is_restricted);
    bool IsTokenUntrusted(uint32 pid, bool* is_untrusted);
    bool IsTokenWriteRestricted(uint32 pid, bool* is_write_restricted);
    bool CreateLowBoxToken(uint32 pid, void* low_box_token);
    bool DeriveCapabilitySids(uint32 pid, void* capability_sids);
    bool DeriveRestrictedAppContainer(uint32 pid, void* app_container);
    bool GetAppContainerSidType(uint32 pid, uint32* sid_type);
    bool CheckTokenMembership(uint32 pid, const void* sid, bool* is_member);
    bool IsChildProcessRestricted(uint32 parent_pid, bool* is_restricted);
    bool CreateProcessWithToken(uint32 pid, const void* token_info, uint32* new_pid);
    bool CreateThreadWithToken(uint32 pid, const void* token_info, uint32* new_tid);
    bool OpenProcessToken(uint32 pid, uint32* token_handle);
    bool OpenThreadToken(uint32 pid, uint32 tid, uint32* token_handle);
    bool AdjustTokenPrivileges(uint32 pid, const void* privileges_info);
    bool SetThreadToken(uint32 pid, uint32 tid, const void* token_info);
    bool QueryInformationToken(uint32 pid, uint32 token_handle, void* token_info);
    
    // Debugging statistics
    const DebuggingStats* GetStatistics();
    void ResetStatistics();
    void UpdateStatistics();
    uint32 GetTotalEvents();
    uint32 GetActiveDebugProcesses();
    uint32 GetTotalBreakpointsHit();
    uint32 GetTotalWatchpointsHit();
    uint32 GetSingleStepsCompleted();
    
    // Debugging configuration access
    bool SetOption(uint32 option, uint32 value);
    uint32 GetOption(uint32 option);
    bool SetTimeout(uint32 timeout_ms);
    uint32 GetTimeout();
    
    // Debugging event handlers
    void OnProcessEvent(uint32 pid, ProcessDebugEvent event, uint32 address, uint32 data);
    void OnBreakpointHit(uint32 pid, uint32 address);
    void OnWatchpointHit(uint32 pid, uint32 address, uint32 access_type);
    void OnSingleStep(uint32 pid, uint32 address);
    void OnSignalReceived(uint32 pid, uint32 signal);
    void OnException(uint32 pid, uint32 exception_code);
    void OnSyscall(uint32 pid, uint32 syscall_number, uint32 syscall_args[]);
    void OnPageFault(uint32 pid, uint32 fault_address, uint32 fault_type);
    void OnIllegalInstruction(uint32 pid, uint32 instruction_address);
    void OnDivideByZero(uint32 pid, uint32 instruction_address);
    void OnMemoryAccessViolation(uint32 pid, uint32 address, uint32 access_type);
    void OnPrivilegeViolation(uint32 pid, uint32 instruction_address);
    void OnAlignmentFault(uint32 pid, uint32 address);
    void OnFloatingPointException(uint32 pid, uint32 instruction_address);
    void OnProcessExit(uint32 pid, uint32 exit_code);
    void OnThreadCreate(uint32 pid, uint32 tid);
    void OnThreadExit(uint32 pid, uint32 tid, uint32 exit_code);
    void OnLibraryLoad(uint32 pid, uint32 base_address, const char* library_name);
    void OnLibraryUnload(uint32 pid, uint32 base_address);
    void OnChildExit(uint32 parent_pid, uint32 child_pid, uint32 exit_code);
    void OnClone(uint32 parent_pid, uint32 child_pid, uint32 flags);
    void OnPtraceTrap(uint32 pid);
    void OnSeccompEvent(uint32 pid, uint32 syscall_number);
    void OnSecureStop(uint32 pid);
    
    // Debugging utility functions
    const char* GetDebugStateName(ProcessDebugState state);
    const char* GetDebugEventName(ProcessDebugEvent event);
    const char* GetDebugCommandName(DebuggingCommand command);
    const char* GetDebugErrorCodeName(DebuggingErrorCode error_code);
    
    // Debugging output functions
    void PrintDebuggingSummary();
    void PrintProcessDebugging(uint32 pid);
    void PrintAllProcessDebugging();
    void PrintDebuggingStatistics();
    void PrintDebuggingConfiguration();
    void PrintDebuggingSession(uint32 session_id);
    void PrintAllDebuggingSessions();
    void PrintBreakpointList(uint32 pid);
    void PrintWatchpointList(uint32 pid);
    void PrintEventHistory(uint32 pid);
    void PrintCallStack(uint32 pid);
    void PrintRegisterState(uint32 pid);
    void PrintMemoryMap(uint32 pid);
    void PrintThreadList(uint32 pid);
    void PrintSignalHandlers(uint32 pid);
    void PrintExceptionHandlers(uint32 pid);
    
    // Debugging data export and import
    bool ExportDebuggingData(uint32 pid, const char* filename);
    bool ImportDebuggingData(uint32 pid, const char* filename);
    bool ExportCallStack(uint32 pid, const char* filename);
    bool ExportRegisterState(uint32 pid, const char* filename);
    bool ExportMemoryDump(uint32 pid, const char* filename);
    bool ExportThreadList(uint32 pid, const char* filename);
    bool ExportSignalHandlers(uint32 pid, const char* filename);
    bool ExportExceptionHandlers(uint32 pid, const char* filename);
    
    // Debugging data filtering and sorting
    void SortEventsByTimestamp(DebuggingEventInfo* events, uint32 count);
    void SortEventsByProcessID(DebuggingEventInfo* events, uint32 count);
    void SortEventsByEventType(DebuggingEventInfo* events, uint32 count);
    void FilterEventsByProcess(uint32 pid, DebuggingEventInfo* events, uint32 count);
    void FilterEventsByEventType(ProcessDebugEvent event_type, DebuggingEventInfo* events, uint32 count);
    void FilterEventsByTimeRange(uint32 start_time, uint32 end_time, DebuggingEventInfo* events, uint32 count);
    
    // Debugging data aggregation and reporting
    bool GenerateSummaryReport(uint32 pid);
    bool GenerateDetailedReport(uint32 pid);
    bool GeneratePerformanceReport(uint32 pid);
    bool GenerateSecurityReport(uint32 pid);
    bool GenerateAuditReport(uint32 pid);
    
    // Debugging session cleanup
    bool CleanupInactiveSessions();
    bool CleanupExitedProcesses();
    bool CleanupExpiredEvents();
    bool CleanupOrphanedSessions();
    bool CleanupCorruptedSessions();
    bool CleanupTimedOutOperations();
    bool CleanupFailedOperations();
    bool CleanupCancelledOperations();
    bool CleanupAllSessions();
    bool CleanupAllEvents();
    bool CleanupAllBreakpoints();
    bool CleanupAllWatchpoints();
    
    // Debugging session monitoring
    bool MonitorSession(uint32 session_id);
    bool UnmonitorSession(uint32 session_id);
    bool IsSessionMonitored(uint32 session_id);
    uint32 GetMonitoredSessionCount();
    void MonitorAllSessions();
    void UnmonitorAllSessions();
    
    // Debugging real-time updates
    void OnTimerTick();
    void OnContextSwitch();
    void OnSystemCall(uint32 pid, uint32 syscall_number);
    void OnPageFault(uint32 pid);
    void OnIOPerformed(uint32 pid, uint32 bytes_read, uint32 bytes_written);
    void OnSignalDelivered(uint32 pid, uint32 signal);
    void OnResourceLimitExceeded(uint32 pid, uint32 resource);
    void OnThresholdExceeded(uint32 pid, uint32 resource, uint32 value);
    void OnCriticalError(uint32 pid, uint32 error_code);
    void OnWarning(uint32 pid, uint32 warning_code);
    void OnInformation(uint32 pid, uint32 info_code);
    void OnVerbose(uint32 pid, uint32 verbose_code);
    void OnDebug(uint32 pid, uint32 debug_code);
    void OnTrace(uint32 pid, uint32 trace_code);
    void OnAll(uint32 pid, uint32 all_code);
    void OnNone(uint32 pid, uint32 none_code);
    void OnFatal(uint32 pid, uint32 fatal_code);
    void OnError(uint32 pid, uint32 error_code);
    void OnWarn(uint32 pid, uint32 warn_code);
    void OnNotice(uint32 pid, uint32 notice_code);
    void OnInfo(uint32 pid, uint32 info_code);
    void OnDebug2(uint32 pid, uint32 debug_code);
    void OnTrace2(uint32 pid, uint32 trace_code);
    void OnAll2(uint32 pid, uint32 all_code);
    void OnNone2(uint32 pid, uint32 none_code);
    void OnFatal2(uint32 pid, uint32 fatal_code);
    void OnError2(uint32 pid, uint32 error_code);
    void OnWarn2(uint32 pid, uint32 warn_code);
    void OnNotice2(uint32 pid, uint32 notice_code);
    void OnInfo2(uint32 pid, uint32 info_code);
    void OnDebug3(uint32 pid, uint32 debug_code);
    void OnTrace3(uint32 pid, uint32 trace_code);
    void OnAll3(uint32 pid, uint32 all_code);
    void OnNone3(uint32 pid, uint32 none_code);
    void OnFatal3(uint32 pid, uint32 fatal_code);
    void OnError3(uint32 pid, uint32 error_code);
    void OnWarn3(uint32 pid, uint32 warn_code);
    void OnNotice3(uint32 pid, uint32 notice_code);
    void OnInfo3(uint32 pid, uint32 info_code);
    void OnDebug4(uint32 pid, uint32 debug_code);
    void OnTrace4(uint32 pid, uint32 trace_code);
    void OnAll4(uint32 pid, uint32 all_code);
    void OnNone4(uint32 pid, uint32 none_code);
    void OnFatal4(uint32 pid, uint32 fatal_code);
    void OnError4(uint32 pid, uint32 error_code);
    void OnWarn4(uint32 pid, uint32 warn_code);
    void OnNotice4(uint32 pid, uint32 notice_code);
    void OnInfo4(uint32 pid, uint32 info_code);
    void OnDebug5(uint32 pid, uint32 debug_code);
    void OnTrace5(uint32 pid, uint32 trace_code);
    void OnAll5(uint32 pid, uint32 all_code);
    void OnNone5(uint32 pid, uint32 none_code);
    void OnFatal5(uint32 pid, uint32 fatal_code);
    void OnError5(uint32 pid, uint32 error_code);
    void OnWarn5(uint32 pid, uint32 warn_code);
    void OnNotice5(uint32 pid, uint32 notice_code);
    void OnInfo5(uint32 pid, uint32 info_code);
    void OnDebug6(uint32 pid, uint32 debug_code);
    void OnTrace6(uint32 pid, uint32 trace_code);
    void OnAll6(uint32 pid, uint32 all_code);
    void OnNone6(uint32 pid, uint32 none_code);
    void OnFatal6(uint32 pid, uint32 fatal_code);
    void OnError6(uint32 pid, uint32 error_code);
    void OnWarn6(uint32 pid, uint32 warn_code);
    void OnNotice6(uint32 pid, uint32 notice_code);
    void OnInfo6(uint32 pid, uint32 info_code);
    void OnDebug7(uint32 pid, uint32 debug_code);
    void OnTrace7(uint32 pid, uint32 trace_code);
    void OnAll7(uint32 pid, uint32 all_code);
    void OnNone7(uint32 pid, uint32 none_code);
    void OnFatal7(uint32 pid, uint32 fatal_code);
    void OnError7(uint32 pid, uint32 error_code);
    void OnWarn7(uint32 pid, uint32 warn_code);
    void OnNotice7(uint32 pid, uint32 notice_code);
    void OnInfo7(uint32 pid, uint32 info_code);
    void OnDebug8(uint32 pid, uint32 debug_code);
    void OnTrace8(uint32 pid, uint32 trace_code);
    void OnAll8(uint32 pid, uint32 all_code);
    void OnNone8(uint32 pid, uint32 none_code);
    void OnFatal8(uint32 pid, uint32 fatal_code);
    void OnError8(uint32 pid, uint32 error_code);
    void OnWarn8(uint32 pid, uint32 warn_code);
    void OnNotice8(uint32 pid, uint32 notice_code);
    void OnInfo8(uint32 pid, uint32 info_code);
    void OnDebug9(uint32 pid, uint32 debug_code);
    void OnTrace9(uint32 pid, uint32 trace_code);
    void OnAll9(uint32 pid, uint32 all_code);
    void OnNone9(uint32 pid, uint32 none_code);
    void OnFatal9(uint32 pid, uint32 fatal_code);
    void OnError9(uint32 pid, uint32 error_code);
    void OnWarn9(uint32 pid, uint32 warn_code);
    void OnNotice9(uint32 pid, uint32 notice_code);
    void OnInfo9(uint32 pid, uint32 info_code);
    void OnDebug10(uint32 pid, uint32 debug_code);
    void OnTrace10(uint32 pid, uint32 trace_code);
    void OnAll10(uint32 pid, uint32 all_code);
    void OnNone10(uint32 pid, uint32 none_code);
    void OnFatal10(uint32 pid, uint32 fatal_code);
    void OnError10(uint32 pid, uint32 error_code);
    void OnWarn10(uint32 pid, uint32 warn_code);
    void OnNotice10(uint32 pid, uint32 notice_code);
    void OnInfo10(uint32 pid, uint32 info_code);
};

// Process debugging system calls
uint32 SysCallEnableProcessDebugging(uint32 pid, const DebuggingOptions* options);
uint32 SysCallDisableProcessDebugging(uint32 pid);
uint32 SysCallIsProcessDebuggingEnabled(uint32 pid);
uint32 SysCallAttachDebugger(uint32 pid, uint32 debugger_pid);
uint32 SysCallDetachDebugger(uint32 pid, uint32 debugger_pid);
uint32 SysCallIsDebuggerAttached(uint32 pid);
uint32 SysCallStopProcess(uint32 pid);
uint32 SysCallContinueProcess(uint32 pid);
uint32 SysCallSingleStepProcess(uint32 pid);
uint32 SysCallKillProcess(uint32 pid);
uint32 SysCallSuspendProcess(uint32 pid);
uint32 SysCallResumeProcess(uint32 pid);
uint32 SysCallSetBreakpoint(uint32 pid, uint32 address, const char* description);
uint32 SysCallClearBreakpoint(uint32 pid, uint32 address);
uint32 SysCallClearAllBreakpoints(uint32 pid);
uint32 SysCallIsBreakpointSet(uint32 pid, uint32 address);
uint32 SysCallGetBreakpointCount(uint32 pid);
uint32 SysCallSetWatchpoint(uint32 pid, uint32 address, uint32 size, uint32 access_type, const char* description);
uint32 SysCallClearWatchpoint(uint32 pid, uint32 address);
uint32 SysCallClearAllWatchpoints(uint32 pid);
uint32 SysCallIsWatchpointSet(uint32 pid, uint32 address);
uint32 SysCallGetWatchpointCount(uint32 pid);
uint32 SysCallWaitForEvent(uint32 pid, DebuggingEventInfo* event_info, uint32 timeout_ms);
uint32 SysCallGetNextEvent(DebuggingEventInfo* event_info);
uint32 SysCallAcknowledgeEvent(uint32 event_id);
uint32 SysCallSendSignalToProcess(uint32 pid, uint32 signal);
uint32 SysCallGetSignalInfo(uint32 pid, uint32 signal, void* signal_info);
uint32 SysCallReadProcessMemory(uint32 pid, uint32 address, void* buffer, uint32 size);
uint32 SysCallWriteProcessMemory(uint32 pid, uint32 address, const void* buffer, uint32 size);
uint32 SysCallReadRegisterValue(uint32 pid, uint32 register_id, uint32* value);
uint32 SysCallWriteRegisterValue(uint32 pid, uint32 register_id, uint32 value);
uint32 SysCallGetBacktrace(uint32 pid, uint32* addresses, uint32 max_addresses, uint32* actual_count);
uint32 SysCallGetProcessInfo(uint32 pid, void* process_info);
uint32 SysCallGetThreadInfo(uint32 pid, uint32 tid, void* thread_info);
uint32 SysCallGetModuleInfo(uint32 pid, uint32 module_id, void* module_info);
uint32 SysCallGetSymbolInfo(uint32 pid, const char* symbol_name, uint32* address);
uint32 SysCallEvaluateExpression(uint32 pid, const char* expression, uint32* result);
uint32 SysCallGetVariableValue(uint32 pid, const char* variable_name, uint32* value);
uint32 SysCallSetVariableValue(uint32 pid, const char* variable_name, uint32 value);
uint32 SysCallGetRegisterValue(uint32 pid, uint32 register_id, uint32* value);
uint32 SysCallSetRegisterValue(uint32 pid, uint32 register_id, uint32 value);
uint32 SysCallFlushInstructionCache(uint32 pid);
uint32 SysCallInvalidateTLB(uint32 pid);
uint32 SysCallGetMappingInfo(uint32 pid, uint32 address, void* mapping_info);
uint32 SysCallSetMapping(uint32 pid, uint32 address, void* mapping_info);
uint32 SysCallGetPageInfo(uint32 pid, uint32 address, void* page_info);
uint32 SysCallSetPageAttributes(uint32 pid, uint32 address, uint32 attributes);
uint32 SysCallGetStackInfo(uint32 pid, void* stack_info);
uint32 SysCallGetHeapInfo(uint32 pid, void* heap_info);
uint32 SysCallAllocateDebugMemory(uint32 pid, uint32 size, void** address);
uint32 SysCallFreeDebugMemory(uint32 pid, void* address);
uint32 SysCallGetFileDescriptorInfo(uint32 pid, uint32 fd, void* fd_info);
uint32 SysCallDuplicateHandle(uint32 pid, uint32 handle, uint32* new_handle);
uint32 SysCallCloseHandle(uint32 pid, uint32 handle);
uint32 SysCallGetEnvironment(uint32 pid, void* env_info);
uint32 SysCallSetEnvironment(uint32 pid, const void* env_info);
uint32 SysCallGetWorkingDirectory(uint32 pid, char* buffer, uint32 size);
uint32 SysCallSetWorkingDirectory(uint32 pid, const char* path);
uint32 SysCallGetCommandLine(uint32 pid, char* buffer, uint32 size);
uint32 SysCallSetCommandLine(uint32 pid, const char* command_line);
uint32 SysCallGetProcessTimes(uint32 pid, void* times_info);
uint32 SysCallGetProcessResources(uint32 pid, void* resources_info);
uint32 SysCallSetResourceLimits(uint32 pid, const void* limits_info);
uint32 SysCallGetSecurityContext(uint32 pid, void* security_info);
uint32 SysCallSetSecurityContext(uint32 pid, const void* security_info);
uint32 SysCallGetPrivileges(uint32 pid, void* privileges_info);
uint32 SysCallSetPrivileges(uint32 pid, const void* privileges_info);
uint32 SysCallRevertToSelf(uint32 pid);
uint32 SysCallImpersonate(uint32 pid, uint32 user_id);
uint32 SysCallGetSID(uint32 pid, void* sid_info);
uint32 SysCallSetSID(uint32 pid, const void* sid_info);
uint32 SysCallGetACL(uint32 pid, void* acl_info);
uint32 SysCallSetACL(uint32 pid, const void* acl_info);
uint32 SysCallGetOwner(uint32 pid, void* owner_info);
uint32 SysCallSetOwner(uint32 pid, const void* owner_info);
uint32 SysCallGetGroup(uint32 pid, void* group_info);
uint32 SysCallSetGroup(uint32 pid, const void* group_info);
uint32 SysCallAddAuditEntry(uint32 pid, const void* audit_info);
uint32 SysCallGetAuditLog(uint32 pid, void* audit_log);
uint32 SysCallClearAuditLog(uint32 pid);
uint32 SysCallEnableAuditing(uint32 pid);
uint32 SysCallDisableAuditing(uint32 pid);
uint32 SysCallGetTrustLevel(uint32 pid, uint32* trust_level);
uint32 SysCallSetTrustLevel(uint32 pid, uint32 trust_level);
uint32 SysCallGetIntegrityLevel(uint32 pid, uint32* integrity_level);
uint32 SysCallSetIntegrityLevel(uint32 pid, uint32 integrity_level);
uint32 SysCallGetTokenInformation(uint32 pid, void* token_info);
uint32 SysCallSetTokenInformation(uint32 pid, const void* token_info);
uint32 SysCallCreateRestrictedToken(uint32 pid, void* restricted_token);
uint32 SysCallFilterToken(uint32 pid, const void* filter_info, void* filtered_token);
uint32 SysCallIsTokenRestricted(uint32 pid, bool* is_restricted);
uint32 SysCallIsTokenUntrusted(uint32 pid, bool* is_untrusted);
uint32 SysCallIsTokenWriteRestricted(uint32 pid, bool* is_write_restricted);
uint32 SysCallCreateLowBoxToken(uint32 pid, void* low_box_token);
uint32 SysCallDeriveCapabilitySids(uint32 pid, void* capability_sids);
uint32 SysCallDeriveRestrictedAppContainer(uint32 pid, void* app_container);
uint32 SysCallGetAppContainerSidType(uint32 pid, uint32* sid_type);
uint32 SysCallCheckTokenMembership(uint32 pid, const void* sid, bool* is_member);
uint32 SysCallIsChildProcessRestricted(uint32 parent_pid, bool* is_restricted);
uint32 SysCallCreateProcessWithToken(uint32 pid, const void* token_info, uint32* new_pid);
uint32 SysCallCreateThreadWithToken(uint32 pid, const void* token_info, uint32* new_tid);
uint32 SysCallOpenProcessToken(uint32 pid, uint32* token_handle);
uint32 SysCallOpenThreadToken(uint32 pid, uint32 tid, uint32* token_handle);
uint32 SysCallAdjustTokenPrivileges(uint32 pid, const void* privileges_info);
uint32 SysCallSetThreadToken(uint32 pid, uint32 tid, const void* token_info);
uint32 SysCallQueryInformationToken(uint32 pid, uint32 token_handle, void* token_info);

// Global process debugging manager instance
extern ProcessDebuggingManager* g_process_debugging_manager;

// Initialize process debugging system
bool InitializeProcessDebugging();