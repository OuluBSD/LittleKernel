#ifndef _Kernel_ProcessSuspension_h_
#define _Kernel_ProcessSuspension_h_

#include "Defs.h"
#include "ProcessControlBlock.h"

// Process suspension states
enum ProcessSuspensionState {
    PROCESS_NOT_SUSPENDED = 0,     // Process is not suspended
    PROCESS_SUSPENDED_USER,         // Process suspended by user request
    PROCESS_SUSPENDED_DEBUGGER,     // Process suspended by debugger
    PROCESS_SUSPENDED_SYSTEM,       // Process suspended by system (e.g., resource constraints)
    PROCESS_SUSPENDED_PARENT,       // Process suspended by parent process
    PROCESS_SUSPENDED_CHILD,        // Process suspended due to child process activity
    PROCESS_SUSPENDED_SIGNAL,       // Process suspended due to signal
    PROCESS_SUSPENDED_WAIT,         // Process suspended waiting for event/resource
    PROCESS_SUSPENDED_STOP,         // Process stopped by job control
    PROCESS_SUSPENDED_TRACED,       // Process being traced (stopped)
    PROCESS_SUSPENDED_CHECKPOINT,    // Process suspended for checkpointing
    PROCESS_SUSPENDED_MIGRATION,    // Process suspended for migration
    PROCESS_SUSPENDED_POWER,        // Process suspended for power management
    PROCESS_SUSPENDED_SECURITY,     // Process suspended for security reasons
    PROCESS_SUSPENDED_ERROR,        // Process suspended due to error condition
    PROCESS_SUSPENDED_UNKNOWN       // Process suspended for unknown reason
};

// Process suspension reasons
enum ProcessSuspensionReason {
    SUSPEND_REASON_NONE = 0,        // No suspension reason
    SUSPEND_REASON_USER_REQUEST,    // User requested suspension
    SUSPEND_REASON_DEBUGGER_ATTACH, // Debugger attached
    SUSPEND_REASON_RESOURCE_LIMIT,   // Resource limit exceeded
    SUSPEND_REASON_PARENT_REQUEST,   // Parent process requested suspension
    SUSPEND_REASON_CHILD_ACTIVITY,   // Child process activity
    SUSPEND_REASON_SIGNAL_RECEIVED,  // Signal received (SIGSTOP, SIGTSTP, etc.)
    SUSPEND_REASON_WAIT_EVENT,       // Waiting for event/resource
    SUSPEND_REASON_JOB_CONTROL,      // Job control stop
    SUSPEND_REASON_TRACED,          // Being traced/debugged
    SUSPEND_REASON_CHECKPOINT,       // Checkpointing requested
    SUSPEND_REASON_MIGRATION,        // Process migration
    SUSPEND_REASON_POWER_MANAGEMENT, // Power management
    SUSPEND_REASON_SECURITY_VIOLATION, // Security violation detected
    SUSPEND_REASON_ERROR_CONDITION,   // Error condition occurred
    SUSPEND_REASON_SYSTEM,           // System requested suspension
    SUSPEND_REASON_UNKNOWN           // Unknown reason
};

// Process suspension flags
const uint32 SUSPEND_FLAG_IMMEDIATE = 0x00000001;     // Suspend immediately
const uint32 SUSPEND_FLAG_GRACEFUL = 0x00000002;     // Suspend gracefully (allow cleanup)
const uint32 SUSPEND_FLAG_FORCE = 0x00000004;        // Force suspension (even if critical)
const uint32 SUSPEND_FLAG_NOTIFY_PARENT = 0x00000008; // Notify parent on suspension
const uint32 SUSPEND_FLAG_NOTIFY_DEBUGGER = 0x00000010; // Notify debugger on suspension
const uint32 SUSPEND_FLAG_SAVE_STATE = 0x00000020;    // Save process state
const uint32 SUSPEND_FLAG_RESTORE_STATE = 0x00000040;  // Restore process state on resume
const uint32 SUSPEND_FLAG_PRESERVE_TIMING = 0x00000080; // Preserve timing information
const uint32 SUSPEND_FLAG_PRESERVE_RESOURCES = 0x00000100; // Preserve resource allocations
const uint32 SUSPEND_FLAG_PRESERVE_MEMORY = 0x00000200;   // Preserve memory mappings
const uint32 SUSPEND_FLAG_PRESERVE_FILES = 0x00000400;    // Preserve file descriptors
const uint32 SUSPEND_FLAG_PRESERVE_NETWORK = 0x00000800;   // Preserve network connections
const uint32 SUSPEND_FLAG_PRESERVE_IPC = 0x00001000;      // Preserve IPC connections
const uint32 SUSPEND_FLAG_PRESERVE_THREADS = 0x00002000;   // Preserve thread state
const uint32 SUSPEND_FLAG_PRESERVE_CONTEXT = 0x00004000;   // Preserve CPU context
const uint32 SUSPEND_FLAG_PRESERVE_ALL = 0x00007FFF;       // Preserve everything
const uint32 SUSPEND_FLAG_ALLOW_RESUME = 0x00008000;       // Allow resumption
const uint32 SUSPEND_FLAG_NO_RESUME = 0x00010000;          // Prevent resumption
const uint32 SUSPEND_FLAG_AUTO_RESUME = 0x00020000;        // Auto-resume after timeout
const uint32 SUSPEND_FLAG_MANUAL_RESUME = 0x00040000;      // Manual resume required
const uint32 SUSPEND_FLAG_TEMPORARY = 0x00080000;          // Temporary suspension
const uint32 SUSPEND_FLAG_PERMANENT = 0x00100000;          // Permanent suspension
const uint32 SUSPEND_FLAG_RECURSIVE = 0x00200000;          // Recursive suspension
const uint32 SUSPEND_FLAG_NESTED = 0x00400000;             // Nested suspension
const uint32 SUSPEND_FLAG_ATOMIC = 0x00800000;             // Atomic suspension operation
const uint32 SUSPEND_FLAG_TRANSACTIONAL = 0x01000000;      // Transactional suspension
const uint32 SUSPEND_FLAG_RECOVERABLE = 0x02000000;        // Recoverable suspension
const uint32 SUSPEND_FLAG_IRRECOVERABLE = 0x04000000;      // Irrecoverable suspension
const uint32 SUSPEND_FLAG_SECURE = 0x08000000;             // Secure suspension
const uint32 SUSPEND_FLAG_ENCRYPTED = 0x10000000;          // Encrypted suspension state
const uint32 SUSPEND_FLAG_COMPRESSED = 0x20000000;         // Compressed suspension state
const uint32 SUSPEND_FLAG_CHECKPOINTED = 0x40000000;       // Checkpointed suspension state

// Process suspension context
struct ProcessSuspensionContext {
    ProcessSuspensionState state;      // Current suspension state
    ProcessSuspensionReason reason;    // Reason for suspension
    uint32 suspend_count;              // Number of times suspended (for nested suspension)
    uint32 suspend_flags;              // Suspension flags
    uint32 suspend_time;               // Time when process was suspended (ticks)
    uint32 resume_time;                // Time when process was resumed (ticks)
    uint32 suspend_duration;           // Duration of suspension (ticks)
    uint32 suspend_timeout;            // Timeout for auto-resume (0 = no timeout)
    uint32 suspend_signal;             // Signal that caused suspension
    uint32 suspend_requester_pid;       // PID of process that requested suspension
    uint32 suspend_requester_uid;       // UID of process that requested suspension
    void* suspend_context;             // Saved CPU context (registers, etc.)
    uint32 suspend_context_size;        // Size of saved context
    void* suspend_memory_map;           // Saved memory mappings
    uint32 suspend_memory_map_size;      // Size of saved memory mappings
    void* suspend_file_descriptors;     // Saved file descriptors
    uint32 suspend_file_descriptors_size; // Size of saved file descriptors
    void* suspend_network_connections;   // Saved network connections
    uint32 suspend_network_connections_size; // Size of saved network connections
    void* suspend_ipc_connections;      // Saved IPC connections
    uint32 suspend_ipc_connections_size; // Size of saved IPC connections
    void* suspend_thread_state;         // Saved thread state
    uint32 suspend_thread_state_size;    // Size of saved thread state
    char suspend_note[256];             // Note about suspension for debugging
    uint32 suspend_checkpoint_id;        // Checkpoint ID if suspended for checkpointing
    uint32 suspend_migration_target;     // Migration target if suspended for migration
    uint32 suspend_power_state;         // Power state if suspended for power management
    uint32 suspend_security_level;       // Security level if suspended for security
    uint32 suspend_error_code;          // Error code if suspended due to error
    uint32 suspend_error_info;          // Additional error information
    uint32 suspend_timestamp;            // Timestamp of suspension
    uint32 resume_timestamp;            // Timestamp of resumption
    uint32 last_suspend_time;            // Time of last suspension
    uint32 last_resume_time;            // Time of last resumption
    uint32 total_suspend_time;          // Total time suspended
    uint32 suspend_count_total;         // Total number of suspensions
    uint32 resume_count_total;          // Total number of resumptions
    bool is_suspended;                  // Whether process is currently suspended
    bool is_resumable;                 // Whether process can be resumed
    bool is_checkpointed;               // Whether process state is checkpointed
    bool is_migrated;                  // Whether process has been migrated
    bool is_power_managed;              // Whether process is power-managed
    bool is_secured;                   // Whether process state is secured
    bool is_encrypted;                 // Whether process state is encrypted
    bool is_compressed;                // Whether process state is compressed
    bool is_transactional;             // Whether suspension is transactional
    bool is_recoverable;               // Whether suspension is recoverable
    bool is_atomic;                    // Whether suspension is atomic
    bool is_recursive;                 // Whether suspension is recursive
    bool is_nested;                    // Whether suspension is nested
    bool is_temporary;                 // Whether suspension is temporary
    bool is_permanent;                 // Whether suspension is permanent
    bool is_manual;                    // Whether manual resume is required
    bool is_automatic;                 // Whether auto-resume is enabled
    bool is_notified;                  // Whether suspension has been notified
    bool is_acknowledged;              // Whether suspension has been acknowledged
    bool is_pending;                   // Whether suspension is pending
    bool is_active;                    // Whether suspension is active
    bool is_expired;                   // Whether suspension has expired
    bool is_cancelled;                 // Whether suspension has been cancelled
    bool is_aborted;                   // Whether suspension has been aborted
    bool is_failed;                    // Whether suspension failed
    bool is_successful;                // Whether suspension was successful
};

// Process suspension statistics
struct ProcessSuspensionStats {
    uint32 total_suspensions;           // Total number of process suspensions
    uint32 total_resumptions;           // Total number of process resumptions
    uint32 total_suspend_failures;      // Total number of suspension failures
    uint32 total_resume_failures;       // Total number of resumption failures
    uint32 total_nested_suspensions;    // Total number of nested suspensions
    uint32 total_recursive_suspensions;  // Total number of recursive suspensions
    uint32 total_forced_suspensions;    // Total number of forced suspensions
    uint32 total_graceful_suspensions;   // Total number of graceful suspensions
    uint32 total_immediate_suspensions;  // Total number of immediate suspensions
    uint32 total_timeout_suspensions;    // Total number of timeout-based suspensions
    uint32 total_signal_suspensions;     // Total number of signal-based suspensions
    uint32 total_user_suspensions;       // Total number of user-requested suspensions
    uint32 total_debugger_suspensions;   // Total number of debugger-initiated suspensions
    uint32 total_system_suspensions;     // Total number of system-initiated suspensions
    uint32 total_parent_suspensions;     // Total number of parent-initiated suspensions
    uint32 total_child_suspensions;      // Total number of child-initiated suspensions
    uint32 total_wait_suspensions;       // Total number of wait-based suspensions
    uint32 total_stop_suspensions;       // Total number of stop-based suspensions
    uint32 total_traced_suspensions;     // Total number of traced suspensions
    uint32 total_checkpoint_suspensions;  // Total number of checkpoint-based suspensions
    uint32 total_migration_suspensions;   // Total number of migration-based suspensions
    uint32 total_power_suspensions;       // Total number of power-management suspensions
    uint32 total_security_suspensions;    // Total number of security-based suspensions
    uint32 total_error_suspensions;       // Total number of error-based suspensions
    uint32 total_unknown_suspensions;     // Total number of unknown suspensions
    uint64 total_suspend_time;          // Total time spent suspended (ticks)
    uint64 total_resume_time;           // Total time spent resuming (ticks)
    uint32 max_suspend_duration;        // Maximum suspension duration (ticks)
    uint32 avg_suspend_duration;        // Average suspension duration (ticks)
    uint32 min_suspend_duration;        // Minimum suspension duration (ticks)
    uint32 max_resume_duration;         // Maximum resumption duration (ticks)
    uint32 avg_resume_duration;         // Average resumption duration (ticks)
    uint32 min_resume_duration;         // Minimum resumption duration (ticks)
    uint32 suspend_timeouts;            // Number of suspension timeouts
    uint32 auto_resumes;                // Number of automatic resumptions
    uint32 manual_resumes;              // Number of manual resumptions
    uint32 forced_resumes;              // Number of forced resumptions
    uint32 graceful_resumes;            // Number of graceful resumptions
    uint32 immediate_resumes;           // Number of immediate resumptions
    uint32 delayed_resumes;             // Number of delayed resumptions
    uint32 pending_resumes;             // Number of pending resumptions
    uint32 active_resumes;              // Number of active resumptions
    uint32 expired_resumes;             // Number of expired resumptions
    uint32 cancelled_resumes;           // Number of cancelled resumptions
    uint32 aborted_resumes;             // Number of aborted resumptions
    uint32 failed_resumes;              // Number of failed resumptions
    uint32 successful_resumes;          // Number of successful resumptions
    uint32 checkpoint_suspensions;       // Number of checkpoint suspensions
    uint32 migration_suspensions;        // Number of migration suspensions
    uint32 power_management_suspensions; // Number of power management suspensions
    uint32 security_suspensions;         // Number of security suspensions
    uint32 error_suspensions;            // Number of error suspensions
    uint32 unknown_suspensions;          // Number of unknown suspensions
    uint32 buffer_overflows;             // Number of buffer overflows
};

// Process suspension manager
class ProcessSuspensionManager {
private:
    static const uint32 MAX_SUSPENDED_PROCESSES = 1024;
    ProcessSuspensionContext* suspended_processes[MAX_SUSPENDED_PROCESSES];
    uint32 suspended_process_count;
    ProcessSuspensionStats stats;
    uint32 next_checkpoint_id;
    bool is_initialized;
    uint32 last_activity_time;
    ProcessControlBlock* monitored_processes;
    uint32 suspend_timeout_default;  // Default timeout for suspensions
    uint32 auto_resume_interval;     // Auto-resume interval in milliseconds

public:
    ProcessSuspensionManager();
    ~ProcessSuspensionManager();
    
    // Initialization and configuration
    bool Initialize();
    bool Configure(const ProcessSuspensionContext* config);
    bool IsInitialized() const;
    bool IsEnabled() const;
    bool Enable();
    bool Disable();
    void Reset();
    
    // Process suspension management
    bool SuspendProcess(uint32 pid, ProcessSuspensionReason reason = SUSPEND_REASON_USER_REQUEST, 
                       uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    bool ResumeProcess(uint32 pid, uint32 flags = 0);
    bool IsProcessSuspended(uint32 pid);
    ProcessSuspensionState GetProcessSuspensionState(uint32 pid);
    ProcessSuspensionReason GetProcessSuspensionReason(uint32 pid);
    uint32 GetSuspendCount(uint32 pid);
    bool SetSuspendTimeout(uint32 pid, uint32 timeout_ms);
    uint32 GetSuspendTimeout(uint32 pid);
    bool CancelSuspend(uint32 pid);
    bool AbortSuspend(uint32 pid);
    
    // Nested suspension support
    bool SuspendProcessNested(uint32 pid, ProcessSuspensionReason reason = SUSPEND_REASON_USER_REQUEST, 
                             uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    bool ResumeProcessNested(uint32 pid);
    uint32 GetNestedSuspendCount(uint32 pid);
    bool IsProcessNestedSuspended(uint32 pid);
    
    // Recursive suspension support
    bool SuspendProcessRecursive(uint32 pid, ProcessSuspensionReason reason = SUSPEND_REASON_USER_REQUEST, 
                                uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    bool ResumeProcessRecursive(uint32 pid);
    uint32 GetRecursiveSuspendCount(uint32 pid);
    bool IsProcessRecursivelySuspended(uint32 pid);
    
    // Batch suspension operations
    uint32 SuspendAllProcesses(ProcessSuspensionReason reason = SUSPEND_REASON_SYSTEM, 
                              uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    uint32 ResumeAllProcesses(uint32 flags = 0);
    uint32 SuspendProcessGroup(uint32 pgid, ProcessSuspensionReason reason = SUSPEND_REASON_PARENT_REQUEST, 
                              uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    uint32 ResumeProcessGroup(uint32 pgid, uint32 flags = 0);
    uint32 SuspendSession(uint32 sid, ProcessSuspensionReason reason = SUSPEND_REASON_SYSTEM, 
                         uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    uint32 ResumeSession(uint32 sid, uint32 flags = 0);
    
    // Conditional suspension
    bool SuspendProcessIf(uint32 pid, bool (*condition)(uint32 pid, void* data), void* condition_data,
                         ProcessSuspensionReason reason = SUSPEND_REASON_USER_REQUEST, 
                         uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    bool ResumeProcessIf(uint32 pid, bool (*condition)(uint32 pid, void* data), void* condition_data,
                        uint32 flags = 0);
    
    // Timed suspension
    bool SuspendProcessFor(uint32 pid, uint32 duration_ms, 
                         ProcessSuspensionReason reason = SUSPEND_REASON_USER_REQUEST, 
                         uint32 flags = SUSPEND_FLAG_GRACEFUL);
    bool ResumeProcessAfter(uint32 pid, uint32 duration_ms, uint32 flags = 0);
    
    // Signal-based suspension
    bool SuspendProcessOnSignal(uint32 pid, uint32 signal, 
                               ProcessSuspensionReason reason = SUSPEND_REASON_SIGNAL_RECEIVED, 
                               uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    bool ResumeProcessOnSignal(uint32 pid, uint32 signal, uint32 flags = 0);
    
    // Event-based suspension
    bool SuspendProcessOnEvent(uint32 pid, const char* event_name, 
                              ProcessSuspensionReason reason = SUSPEND_REASON_WAIT_EVENT, 
                              uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    bool ResumeProcessOnEvent(uint32 pid, const char* event_name, uint32 flags = 0);
    
    // Resource-based suspension
    bool SuspendProcessForResource(uint32 pid, uint32 resource_id, 
                                 ProcessSuspensionReason reason = SUSPEND_REASON_RESOURCE_LIMIT, 
                                 uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    bool ResumeProcessForResource(uint32 pid, uint32 resource_id, uint32 flags = 0);
    
    // Checkpoint-based suspension
    bool SuspendProcessForCheckpoint(uint32 pid, uint32 checkpoint_id, 
                                   ProcessSuspensionReason reason = SUSPEND_REASON_CHECKPOINT, 
                                   uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    bool ResumeProcessFromCheckpoint(uint32 pid, uint32 checkpoint_id, uint32 flags = 0);
    uint32 CreateCheckpoint(uint32 pid);
    bool RestoreCheckpoint(uint32 pid, uint32 checkpoint_id);
    bool DeleteCheckpoint(uint32 pid, uint32 checkpoint_id);
    
    // Migration-based suspension
    bool SuspendProcessForMigration(uint32 pid, uint32 target_node, 
                                  ProcessSuspensionReason reason = SUSPEND_REASON_MIGRATION, 
                                  uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    bool ResumeProcessAfterMigration(uint32 pid, uint32 target_node, uint32 flags = 0);
    
    // Power management suspension
    bool SuspendProcessForPower(uint32 pid, uint32 power_state, 
                               ProcessSuspensionReason reason = SUSPEND_REASON_POWER_MANAGEMENT, 
                               uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    bool ResumeProcessFromPower(uint32 pid, uint32 power_state, uint32 flags = 0);
    
    // Security-based suspension
    bool SuspendProcessForSecurity(uint32 pid, uint32 security_level, 
                                 ProcessSuspensionReason reason = SUSPEND_REASON_SECURITY_VIOLATION, 
                                 uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    bool ResumeProcessFromSecurity(uint32 pid, uint32 security_level, uint32 flags = 0);
    
    // Error-based suspension
    bool SuspendProcessForError(uint32 pid, uint32 error_code, 
                               ProcessSuspensionReason reason = SUSPEND_REASON_ERROR_CONDITION, 
                               uint32 flags = SUSPEND_FLAG_GRACEFUL, uint32 timeout_ms = 0);
    bool ResumeProcessFromError(uint32 pid, uint32 error_code, uint32 flags = 0);
    
    // Automatic suspension/resumption
    bool EnableAutoSuspend(uint32 pid, uint32 interval_ms);
    bool DisableAutoSuspend(uint32 pid);
    bool IsAutoSuspendEnabled(uint32 pid);
    bool EnableAutoResume(uint32 pid, uint32 interval_ms);
    bool DisableAutoResume(uint32 pid);
    bool IsAutoResumeEnabled(uint32 pid);
    
    // Suspension context management
    bool SaveProcessContext(uint32 pid, ProcessSuspensionContext* context);
    bool RestoreProcessContext(uint32 pid, const ProcessSuspensionContext* context);
    bool ClearProcessContext(uint32 pid);
    const ProcessSuspensionContext* GetProcessContext(uint32 pid);
    bool SetProcessContextNote(uint32 pid, const char* note);
    const char* GetProcessContextNote(uint32 pid);
    
    // Suspension state management
    bool SetProcessSuspensionState(uint32 pid, ProcessSuspensionState state);
    bool SetProcessSuspensionReason(uint32 pid, ProcessSuspensionReason reason);
    bool SetProcessSuspensionFlags(uint32 pid, uint32 flags);
    uint32 GetProcessSuspensionFlags(uint32 pid);
    bool AddProcessSuspensionFlag(uint32 pid, uint32 flag);
    bool RemoveProcessSuspensionFlag(uint32 pid, uint32 flag);
    bool HasProcessSuspensionFlag(uint32 pid, uint32 flag);
    
    // Suspension timing management
    uint32 GetProcessSuspendTime(uint32 pid);
    uint32 GetProcessResumeTime(uint32 pid);
    uint32 GetProcessSuspendDuration(uint32 pid);
    uint32 GetProcessTotalSuspendTime(uint32 pid);
    uint32 GetProcessLastSuspendTime(uint32 pid);
    uint32 GetProcessLastResumeTime(uint32 pid);
    uint32 GetProcessAvgSuspendDuration(uint32 pid);
    uint32 GetProcessMaxSuspendDuration(uint32 pid);
    uint32 GetProcessMinSuspendDuration(uint32 pid);
    
    // Suspension statistics management
    const ProcessSuspensionStats* GetStatistics();
    void ResetStatistics();
    void UpdateStatistics();
    uint32 GetTotalSuspensions();
    uint32 GetTotalResumptions();
    uint32 GetTotalSuspendFailures();
    uint32 GetTotalResumeFailures();
    uint32 GetSuspendCountTotal();
    uint32 GetResumeCountTotal();
    uint64 GetTotalSuspendTime();
    uint64 GetTotalResumeTime();
    uint32 GetAverageSuspendDuration();
    uint32 GetMaxSuspendDuration();
    uint32 GetMinSuspendDuration();
    uint32 GetAverageResumeDuration();
    uint32 GetMaxResumeDuration();
    uint32 GetMinResumeDuration();
    uint32 GetSuspendTimeouts();
    uint32 GetAutoResumes();
    uint32 GetManualResumes();
    uint32 GetForcedResumes();
    uint32 GetGracefulResumes();
    uint32 GetImmediateResumes();
    uint32 GetDelayedResumes();
    uint32 GetPendingResumes();
    uint32 GetActiveResumes();
    uint32 GetExpiredResumes();
    uint32 GetCancelledResumes();
    uint32 GetAbortedResumes();
    uint32 GetFailedResumes();
    uint32 GetSuccessfulResumes();
    uint32 GetCheckpointSuspensions();
    uint32 GetMigrationSuspensions();
    uint32 GetPowerManagementSuspensions();
    uint32 GetSecuritySuspensions();
    uint32 GetErrorSuspensions();
    uint32 GetUnknownSuspensions();
    
    // Suspension list management
    uint32 GetSuspendedProcessList(uint32* pids, uint32 max_pids);
    uint32 GetSuspendedProcessCount();
    bool IsProcessInSuspendedList(uint32 pid);
    uint32* GetSuspendedProcessIds(uint32* count);
    
    // Suspension utilities
    const char* GetSuspensionStateName(ProcessSuspensionState state);
    const char* GetSuspensionReasonName(ProcessSuspensionReason reason);
    bool IsSuspensionReasonValid(ProcessSuspensionReason reason);
    bool IsSuspensionStateValid(ProcessSuspensionState state);
    uint32 GetSuspensionPriority(ProcessSuspensionReason reason);
    bool IsHigherSuspensionPriority(ProcessSuspensionReason reason1, ProcessSuspensionReason reason2);
    
    // Suspension debugging and diagnostics
    void PrintSuspensionSummary();
    void PrintProcessSuspension(uint32 pid);
    void PrintAllProcessSuspensions();
    void PrintSuspensionStatistics();
    void PrintSuspensionConfiguration();
    void PrintSuspendedProcessList();
    void DumpSuspensionData();
    void ValidateSuspensionData();
    
    // Suspension export and import
    bool ExportToCSV(const char* filename);
    bool ExportToJSON(const char* filename);
    bool ExportToXML(const char* filename);
    bool ImportFromCSV(const char* filename);
    bool ImportFromJSON(const char* filename);
    bool ImportFromXML(const char* filename);
    
    // Suspension filtering and sorting
    void SortSuspensionsBySuspendTime(ProcessSuspensionContext* suspensions, uint32 count);
    void SortSuspensionsByResumeTime(ProcessSuspensionContext* suspensions, uint32 count);
    void SortSuspensionsByDuration(ProcessSuspensionContext* suspensions, uint32 count);
    void FilterSuspensionsByReason(ProcessSuspensionReason reason, 
                                  ProcessSuspensionContext* suspensions, uint32 count);
    void FilterSuspensionsByState(ProcessSuspensionState state, 
                                 ProcessSuspensionContext* suspensions, uint32 count);
    void FilterSuspensionsByProcess(uint32 pid, 
                                   ProcessSuspensionContext* suspensions, uint32 count);
    
    // Suspension thresholds and alerts
    bool SetSuspendThreshold(uint32 pid, uint32 threshold);
    bool SetResumeThreshold(uint32 pid, uint32 threshold);
    bool CheckThresholds(uint32 pid);
    void OnThresholdExceeded(uint32 pid, uint32 resource, uint32 value);
    bool IsThresholdExceeded(uint32 pid, uint32 resource);
    
    // Suspension cleanup
    bool CleanupOldSuspensions();
    bool CleanupTerminatedProcesses();
    bool PurgeAllSuspensions();
    uint32 GetCleanupCount();
    
    // Suspension monitoring
    // void OnTimerTick();
    void OnContextSwitch();
    void OnSystemCall(uint32 pid, uint32 syscall_number);
    void OnPageFault(uint32 pid);
    void OnContextSwitch(uint32 pid);
    // void OnTimerTick();  // Duplicate declaration
    void OnIOPerformed(uint32 pid, uint32 bytes_read, uint32 bytes_written);
    void OnSignalDelivered(uint32 pid, uint32 signal);
    void OnResourceLimitExceeded(uint32 pid, uint32 resource);
    // void OnThresholdExceeded(uint32 pid, uint32 resource, uint32 value);  // Duplicate declaration
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

// Process suspension system calls
uint32 SysCallSuspendProcess(uint32 pid, ProcessSuspensionReason reason, uint32 flags, uint32 timeout_ms);
uint32 SysCallResumeProcess(uint32 pid, uint32 flags);
uint32 SysCallIsProcessSuspended(uint32 pid);
uint32 SysCallGetProcessSuspensionState(uint32 pid, ProcessSuspensionState* state);
uint32 SysCallGetProcessSuspensionReason(uint32 pid, ProcessSuspensionReason* reason);
uint32 SysCallGetSuspendCount(uint32 pid, uint32* count);
uint32 SysCallSetSuspendTimeout(uint32 pid, uint32 timeout_ms);
uint32 SysCallGetSuspendTimeout(uint32 pid, uint32* timeout_ms);
uint32 SysCallCancelSuspend(uint32 pid);
uint32 SysCallAbortSuspend(uint32 pid);
uint32 SysCallSuspendAllProcesses(ProcessSuspensionReason reason, uint32 flags, uint32 timeout_ms);
uint32 SysCallResumeAllProcesses(uint32 flags);
uint32 SysCallSuspendProcessGroup(uint32 pgid, ProcessSuspensionReason reason, uint32 flags, uint32 timeout_ms);
uint32 SysCallResumeProcessGroup(uint32 pgid, uint32 flags);
uint32 SysCallSuspendSession(uint32 sid, ProcessSuspensionReason reason, uint32 flags, uint32 timeout_ms);
uint32 SysCallResumeSession(uint32 sid, uint32 flags);
uint32 SysCallSuspendProcessFor(uint32 pid, uint32 duration_ms, ProcessSuspensionReason reason, uint32 flags);
uint32 SysCallResumeProcessAfter(uint32 pid, uint32 duration_ms, uint32 flags);
uint32 SysCallSuspendProcessOnSignal(uint32 pid, uint32 signal, ProcessSuspensionReason reason, uint32 flags, uint32 timeout_ms);
uint32 SysCallResumeProcessOnSignal(uint32 pid, uint32 signal, uint32 flags);
uint32 SysCallSuspendProcessOnEvent(uint32 pid, const char* event_name, ProcessSuspensionReason reason, uint32 flags, uint32 timeout_ms);
uint32 SysCallResumeProcessOnEvent(uint32 pid, const char* event_name, uint32 flags);
uint32 SysCallSuspendProcessForResource(uint32 pid, uint32 resource_id, ProcessSuspensionReason reason, uint32 flags, uint32 timeout_ms);
uint32 SysCallResumeProcessForResource(uint32 pid, uint32 resource_id, uint32 flags);
uint32 SysCallSuspendProcessForCheckpoint(uint32 pid, uint32 checkpoint_id, ProcessSuspensionReason reason, uint32 flags, uint32 timeout_ms);
uint32 SysCallResumeProcessFromCheckpoint(uint32 pid, uint32 checkpoint_id, uint32 flags);
uint32 SysCallCreateCheckpoint(uint32 pid, uint32* checkpoint_id);
uint32 SysCallRestoreCheckpoint(uint32 pid, uint32 checkpoint_id);
uint32 SysCallDeleteCheckpoint(uint32 pid, uint32 checkpoint_id);
uint32 SysCallSuspendProcessForMigration(uint32 pid, uint32 target_node, ProcessSuspensionReason reason, uint32 flags, uint32 timeout_ms);
uint32 SysCallResumeProcessAfterMigration(uint32 pid, uint32 target_node, uint32 flags);
uint32 SysCallSuspendProcessForPower(uint32 pid, uint32 power_state, ProcessSuspensionReason reason, uint32 flags, uint32 timeout_ms);
uint32 SysCallResumeProcessFromPower(uint32 pid, uint32 power_state, uint32 flags);
uint32 SysCallSuspendProcessForSecurity(uint32 pid, uint32 security_level, ProcessSuspensionReason reason, uint32 flags, uint32 timeout_ms);
uint32 SysCallResumeProcessFromSecurity(uint32 pid, uint32 security_level, uint32 flags);
uint32 SysCallSuspendProcessForError(uint32 pid, uint32 error_code, ProcessSuspensionReason reason, uint32 flags, uint32 timeout_ms);
uint32 SysCallResumeProcessFromError(uint32 pid, uint32 error_code, uint32 flags);
uint32 SysCallEnableAutoSuspend(uint32 pid, uint32 interval_ms);
uint32 SysCallDisableAutoSuspend(uint32 pid);
uint32 SysCallIsAutoSuspendEnabled(uint32 pid);
uint32 SysCallEnableAutoResume(uint32 pid, uint32 interval_ms);
uint32 SysCallDisableAutoResume(uint32 pid);
uint32 SysCallIsAutoResumeEnabled(uint32 pid);
uint32 SysCallSaveProcessContext(uint32 pid, ProcessSuspensionContext* context);
uint32 SysCallRestoreProcessContext(uint32 pid, const ProcessSuspensionContext* context);
uint32 SysCallClearProcessContext(uint32 pid);
uint32 SysCallGetProcessContext(uint32 pid, ProcessSuspensionContext* context);
uint32 SysCallSetProcessContextNote(uint32 pid, const char* note);
uint32 SysCallGetProcessContextNote(uint32 pid, char* note, uint32 max_length);
uint32 SysCallSetProcessSuspensionState(uint32 pid, ProcessSuspensionState state);
uint32 SysCallSetProcessSuspensionReason(uint32 pid, ProcessSuspensionReason reason);
uint32 SysCallSetProcessSuspensionFlags(uint32 pid, uint32 flags);
uint32 SysCallGetProcessSuspensionFlags(uint32 pid, uint32* flags);
uint32 SysCallAddProcessSuspensionFlag(uint32 pid, uint32 flag);
uint32 SysCallRemoveProcessSuspensionFlag(uint32 pid, uint32 flag);
uint32 SysCallHasProcessSuspensionFlag(uint32 pid, uint32 flag);
uint32 SysCallGetProcessSuspendTime(uint32 pid, uint32* time);
uint32 SysCallGetProcessResumeTime(uint32 pid, uint32* time);
uint32 SysCallGetProcessSuspendDuration(uint32 pid, uint32* duration);
uint32 SysCallGetProcessTotalSuspendTime(uint32 pid, uint32* time);
uint32 SysCallGetProcessLastSuspendTime(uint32 pid, uint32* time);
uint32 SysCallGetProcessLastResumeTime(uint32 pid, uint32* time);
uint32 SysCallGetProcessAvgSuspendDuration(uint32 pid, uint32* duration);
uint32 SysCallGetProcessMaxSuspendDuration(uint32 pid, uint32* duration);
uint32 SysCallGetProcessMinSuspendDuration(uint32 pid, uint32* duration);
uint32 SysCallGetSuspensionStatistics(ProcessSuspensionStats* stats);
uint32 SysCallResetSuspensionStatistics();
uint32 SysCallUpdateSuspensionStatistics();
uint32 SysCallGetTotalSuspensions(uint32* count);
uint32 SysCallGetTotalResumptions(uint32* count);
uint32 SysCallGetTotalSuspendFailures(uint32* count);
uint32 SysCallGetTotalResumeFailures(uint32* count);
uint32 SysCallGetSuspendCountTotal(uint32* count);
uint32 SysCallGetResumeCountTotal(uint32* count);
uint32 SysCallGetTotalSuspendTime(uint64* time);
uint32 SysCallGetTotalResumeTime(uint64* time);
uint32 SysCallGetAverageSuspendDuration(uint32* duration);
uint32 SysCallGetMaxSuspendDuration(uint32* duration);
uint32 SysCallGetMinSuspendDuration(uint32* duration);
uint32 SysCallGetAverageResumeDuration(uint32* duration);
uint32 SysCallGetMaxResumeDuration(uint32* duration);
uint32 SysCallGetMinResumeDuration(uint32* duration);
uint32 SysCallGetSuspendTimeouts(uint32* count);
uint32 SysCallGetAutoResumes(uint32* count);
uint32 SysCallGetManualResumes(uint32* count);
uint32 SysCallGetForcedResumes(uint32* count);
uint32 SysCallGetGracefulResumes(uint32* count);
uint32 SysCallGetImmediateResumes(uint32* count);
uint32 SysCallGetDelayedResumes(uint32* count);
uint32 SysCallGetPendingResumes(uint32* count);
uint32 SysCallGetActiveResumes(uint32* count);
uint32 SysCallGetExpiredResumes(uint32* count);
uint32 SysCallGetCancelledResumes(uint32* count);
uint32 SysCallGetAbortedResumes(uint32* count);
uint32 SysCallGetFailedResumes(uint32* count);
uint32 SysCallGetSuccessfulResumes(uint32* count);
uint32 SysCallGetCheckpointSuspensions(uint32* count);
uint32 SysCallGetMigrationSuspensions(uint32* count);
uint32 SysCallGetPowerManagementSuspensions(uint32* count);
uint32 SysCallGetSecuritySuspensions(uint32* count);
uint32 SysCallGetErrorSuspensions(uint32* count);
uint32 SysCallGetUnknownSuspensions(uint32* count);
uint32 SysCallGetSuspendedProcessList(uint32* pids, uint32 max_pids, uint32* actual_count);
uint32 SysCallGetSuspendedProcessCount(uint32* count);
uint32 SysCallIsProcessInSuspendedList(uint32 pid);
uint32 SysCallGetSuspendedProcessIds(uint32* pids, uint32 max_pids, uint32* actual_count);
uint32 SysCallGetSuspensionStateName(ProcessSuspensionState state, char* name, uint32 max_length);
uint32 SysCallGetSuspensionReasonName(ProcessSuspensionReason reason, char* name, uint32 max_length);
uint32 SysCallIsSuspensionReasonValid(ProcessSuspensionReason reason);
uint32 SysCallIsSuspensionStateValid(ProcessSuspensionState state);
uint32 SysCallGetSuspensionPriority(ProcessSuspensionReason reason, uint32* priority);
uint32 SysCallIsHigherSuspensionPriority(ProcessSuspensionReason reason1, ProcessSuspensionReason reason2);
uint32 SysCallPrintSuspensionSummary();
uint32 SysCallPrintProcessSuspension(uint32 pid);
uint32 SysCallPrintAllProcessSuspensions();
uint32 SysCallPrintSuspensionStatistics();
uint32 SysCallPrintSuspensionConfiguration();
uint32 SysCallPrintSuspendedProcessList();
uint32 SysCallDumpSuspensionData();
uint32 SysCallValidateSuspensionData();
uint32 SysCallExportSuspensionDataToCSV(const char* filename);
uint32 SysCallExportSuspensionDataToJSON(const char* filename);
uint32 SysCallExportSuspensionDataToXML(const char* filename);
uint32 SysCallImportSuspensionDataFromCSV(const char* filename);
uint32 SysCallImportSuspensionDataFromJSON(const char* filename);
uint32 SysCallImportSuspensionDataFromXML(const char* filename);
uint32 SysCallSortSuspensionsBySuspendTime(ProcessSuspensionContext* suspensions, uint32 count);
uint32 SysCallSortSuspensionsByResumeTime(ProcessSuspensionContext* suspensions, uint32 count);
uint32 SysCallSortSuspensionsByDuration(ProcessSuspensionContext* suspensions, uint32 count);
uint32 SysCallFilterSuspensionsByReason(ProcessSuspensionReason reason, ProcessSuspensionContext* suspensions, uint32 count);
uint32 SysCallFilterSuspensionsByState(ProcessSuspensionState state, ProcessSuspensionContext* suspensions, uint32 count);
uint32 SysCallFilterSuspensionsByProcess(uint32 pid, ProcessSuspensionContext* suspensions, uint32 count);
uint32 SysCallSetSuspendThreshold(uint32 pid, uint32 threshold);
uint32 SysCallSetResumeThreshold(uint32 pid, uint32 threshold);
uint32 SysCallCheckThresholds(uint32 pid);
uint32 SysCallOnThresholdExceeded(uint32 pid, uint32 resource, uint32 value);
uint32 SysCallIsThresholdExceeded(uint32 pid, uint32 resource);
uint32 SysCallCleanupOldSuspensions();
uint32 SysCallCleanupTerminatedProcesses();
uint32 SysCallPurgeAllSuspensions();
uint32 SysCallGetCleanupCount(uint32* count);

// Global process suspension manager instance
extern ProcessSuspensionManager* g_process_suspension_manager;

// Initialize process suspension system
bool InitializeProcessSuspension();

#endif // _Kernel_ProcessSuspension_h_