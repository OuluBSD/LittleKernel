#include "ProcessSuspension.h"
#include "Global.h"
#include "MemoryManager.h"
#include "Logging.h"
#include "Timer.h"
#include "ProcessControlBlock.h"

// Global process suspension manager instance
ProcessSuspensionManager* g_process_suspension_manager = nullptr;

// ProcessSuspensionManager implementation
ProcessSuspensionManager::ProcessSuspensionManager() 
    : suspended_process_count(0), next_checkpoint_id(1), 
      is_initialized(false), last_activity_time(0),
      suspend_timeout_default(0), auto_resume_interval(0) {
    // Initialize statistics
    memset(&stats, 0, sizeof(stats));
    
    // Initialize suspended processes array
    memset(suspended_processes, 0, sizeof(suspended_processes));
    
    // Initialize other fields
    suspended_process_count = 0;
    next_checkpoint_id = 1;
    is_initialized = false;
    last_activity_time = 0;
    suspend_timeout_default = 0;
    auto_resume_interval = 0;
    
    DLOG("Process suspension manager created");
}

ProcessSuspensionManager::~ProcessSuspensionManager() {
    // Clean up all suspended processes
    CleanupAllSuspensions();
    
    DLOG("Process suspension manager destroyed");
}

bool ProcessSuspensionManager::Initialize() {
    DLOG("Initializing process suspension manager");
    
    // Reset statistics
    ResetStatistics();
    
    // Initialize suspended processes array
    memset(suspended_processes, 0, sizeof(suspended_processes));
    suspended_process_count = 0;
    
    // Set initial values
    next_checkpoint_id = 1;
    is_initialized = true;
    last_activity_time = global_timer ? global_timer->GetTickCount() : 0;
    suspend_timeout_default = 0;  // No default timeout
    auto_resume_interval = 0;     // No auto-resume by default
    
    DLOG("Process suspension manager initialized successfully");
    return true;
}

bool ProcessSuspensionManager::Configure(uint32 default_timeout_ms, uint32 auto_resume_interval_ms) {
    if (!is_initialized) {
        LOG("Process suspension manager not initialized");
        return false;
    }
    
    suspend_timeout_default = default_timeout_ms;
    auto_resume_interval = auto_resume_interval_ms;
    
    DLOG("Process suspension manager configured with default timeout: " << default_timeout_ms 
         << " ms, auto-resume interval: " << auto_resume_interval_ms << " ms");
    
    return true;
}

bool ProcessSuspensionManager::IsInitialized() const {
    return is_initialized;
}

bool ProcessSuspensionManager::IsEnabled() const {
    return is_initialized;  // Always enabled when initialized
}

bool ProcessSuspensionManager::Enable() {
    if (!is_initialized) {
        LOG("Process suspension manager not initialized");
        return false;
    }
    
    DLOG("Process suspension enabled");
    return true;
}

bool ProcessSuspensionManager::Disable() {
    if (!is_initialized) {
        LOG("Process suspension manager not initialized");
        return false;
    }
    
    // Resume all suspended processes before disabling
    ResumeAllProcesses();
    
    DLOG("Process suspension disabled");
    return true;
}

void ProcessSuspensionManager::Reset() {
    if (!is_initialized) {
        return;
    }
    
    // Resume all suspended processes
    ResumeAllProcesses();
    
    // Clear suspended processes array
    memset(suspended_processes, 0, sizeof(suspended_processes));
    suspended_process_count = 0;
    
    // Reset statistics
    ResetStatistics();
    
    // Reset counters
    next_checkpoint_id = 1;
    last_activity_time = global_timer ? global_timer->GetTickCount() : 0;
    suspend_timeout_default = 0;
    auto_resume_interval = 0;
    
    DLOG("Process suspension manager reset");
}

bool ProcessSuspensionManager::SuspendProcess(uint32 pid, ProcessSuspensionReason reason, 
                                            uint32 flags, uint32 timeout_ms) {
    if (!is_initialized || !process_manager) {
        LOG("Process suspension manager or process manager not available");
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        LOG("Process with PID " << pid << " not found");
        return false;
    }
    
    // Check if process is already suspended
    if (IsProcessSuspended(pid)) {
        LOG("Process PID " << pid << " is already suspended");
        return false;
    }
    
    // Create suspension context for the process
    ProcessSuspensionContext* context = (ProcessSuspensionContext*)malloc(sizeof(ProcessSuspensionContext));
    if (!context) {
        LOG("Failed to allocate suspension context for PID " << pid);
        return false;
    }
    
    // Initialize suspension context
    memset(context, 0, sizeof(ProcessSuspensionContext));
    context->state = PROCESS_SUSPENDED_USER;  // Default to user suspension
    context->reason = reason;
    context->suspend_count = 1;
    context->suspend_flags = flags;
    context->suspend_time = global_timer ? global_timer->GetTickCount() : 0;
    context->resume_time = 0;
    context->suspend_duration = 0;
    context->suspend_timeout = timeout_ms > 0 ? timeout_ms : suspend_timeout_default;
    context->suspend_signal = 0;  // Not suspended by signal
    context->suspend_requester_pid = current_process ? current_process->pid : KERNEL_PID;
    context->suspend_requester_uid = current_process ? current_process->uid : 0;
    context->suspend_context = nullptr;
    context->suspend_context_size = 0;
    context->suspend_memory_map = nullptr;
    context->suspend_memory_map_size = 0;
    context->suspend_file_descriptors = nullptr;
    context->suspend_file_descriptors_size = 0;
    context->suspend_network_connections = nullptr;
    context->suspend_network_connections_size = 0;
    context->suspend_ipc_connections = nullptr;
    context->suspend_ipc_connections_size = 0;
    context->suspend_thread_state = nullptr;
    context->suspend_thread_state_size = 0;
    strncpy(context->suspend_note, "Process suspended", sizeof(context->suspend_note) - 1);
    context->suspend_note[sizeof(context->suspend_note) - 1] = '\0';
    context->suspend_checkpoint_id = 0;  // No checkpoint by default
    context->suspend_migration_target = 0; // No migration target
    context->suspend_power_state = 0;     // No power state change
    context->suspend_security_level = 0;   // No security level change
    context->suspend_error_code = 0;      // No error
    context->suspend_error_info = 0;      // No error info
    context->suspend_timestamp = context->suspend_time;
    context->resume_timestamp = 0;
    context->last_suspend_time = context->suspend_time;
    context->last_resume_time = 0;
    context->total_suspend_time = 0;
    context->suspend_count_total = 1;
    context->resume_count_total = 0;
    context->is_suspended = true;
    context->is_resumable = true;
    context->is_checkpointed = false;
    context->is_migrated = false;
    context->is_power_managed = false;
    context->is_secured = false;
    context->is_encrypted = false;
    context->is_compressed = false;
    context->is_transactional = false;
    context->is_recoverable = true;
    context->is_atomic = true;
    context->is_recursive = false;
    context->is_nested = false;
    context->is_temporary = (flags & SUSPEND_FLAG_TEMPORARY) != 0;
    context->is_permanent = (flags & SUSPEND_FLAG_PERMANENT) != 0;
    context->is_manual = (flags & SUSPEND_FLAG_MANUAL_RESUME) != 0;
    context->is_automatic = (flags & SUSPEND_FLAG_AUTO_RESUME) != 0;
    context->is_notified = false;
    context->is_acknowledged = false;
    context->is_pending = false;
    context->is_active = true;
    context->is_expired = false;
    context->is_cancelled = false;
    context->is_aborted = false;
    context->is_failed = false;
    context->is_successful = true;
    
    // Store suspension context in the process
    process->suspension_context = context;
    
    // Change process state to suspended
    process_manager->TransitionProcessState(pid, PROCESS_STATE_SUSPENDED);
    
    // Add to suspended processes list
    if (suspended_process_count < MAX_SUSPENDED_PROCESSES) {
        suspended_processes[suspended_process_count] = context;
        suspended_process_count++;
    } else {
        LOG("Warning: Maximum suspended processes reached");
        stats.buffer_overflows++;
    }
    
    // Update statistics
    stats.total_suspensions++;
    if (reason == SUSPEND_REASON_USER_REQUEST) {
        stats.total_user_suspensions++;
    } else if (reason == SUSPEND_REASON_DEBUGGER_ATTACH) {
        stats.total_debugger_suspensions++;
    } else if (reason == SUSPEND_REASON_SYSTEM) {
        stats.total_system_suspensions++;
    } else if (reason == SUSPEND_REASON_PARENT_REQUEST) {
        stats.total_parent_suspensions++;
    } else if (reason == SUSPEND_REASON_CHILD_ACTIVITY) {
        stats.total_child_suspensions++;
    } else if (reason == SUSPEND_REASON_SIGNAL_RECEIVED) {
        stats.total_signal_suspensions++;
    } else if (reason == SUSPEND_REASON_WAIT_EVENT) {
        stats.total_wait_suspensions++;
    } else if (reason == SUSPEND_REASON_JOB_CONTROL) {
        stats.total_job_control_suspensions++;
    } else if (reason == SUSPEND_REASON_TRACED) {
        stats.total_traced_suspensions++;
    } else if (reason == SUSPEND_REASON_CHECKPOINT) {
        stats.total_checkpoint_suspensions++;
    } else if (reason == SUSPEND_REASON_MIGRATION) {
        stats.total_migration_suspensions++;
    } else if (reason == SUSPEND_REASON_POWER_MANAGEMENT) {
        stats.total_power_suspensions++;
    } else if (reason == SUSPEND_REASON_SECURITY_VIOLATION) {
        stats.total_security_suspensions++;
    } else if (reason == SUSPEND_REASON_ERROR_CONDITION) {
        stats.total_error_suspensions++;
    } else {
        stats.total_unknown_suspensions++;
    }
    
    // If auto-resume is enabled, set up timer
    if (context->is_automatic && context->suspend_timeout > 0) {
        context->auto_resume_timer = context->suspend_time + context->suspend_timeout;
    }
    
    DLOG("Suspended process PID " << pid << " with reason " << GetSuspensionReasonName(reason)
         << " and flags 0x" << flags);
    
    return true;
}

bool ProcessSuspensionManager::ResumeProcess(uint32 pid, uint32 flags) {
    if (!is_initialized || !process_manager) {
        LOG("Process suspension manager or process manager not available");
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        LOG("Process with PID " << pid << " not found");
        return false;
    }
    
    // Check if process is suspended
    if (!IsProcessSuspended(pid)) {
        LOG("Process PID " << pid << " is not suspended");
        return false;
    }
    
    // Get suspension context
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        LOG("No suspension context for process PID " << pid);
        return false;
    }
    
    // Check if process can be resumed
    if (!context->is_resumable) {
        LOG("Process PID " << pid << " cannot be resumed");
        return false;
    }
    
    // Update context with resume information
    uint32 current_time = global_timer ? global_timer->GetTickCount() : 0;
    context->resume_time = current_time;
    context->suspend_duration = current_time - context->suspend_time;
    context->resume_timestamp = current_time;
    context->last_resume_time = current_time;
    context->total_suspend_time += context->suspend_duration;
    context->resume_count_total++;
    context->is_suspended = false;
    context->is_active = false;
    context->is_successful = true;
    
    // Change process state back to ready
    process_manager->TransitionProcessState(pid, PROCESS_STATE_READY);
    
    // Remove from suspended processes list
    for (uint32 i = 0; i < suspended_process_count; i++) {
        if (suspended_processes[i] == context) {
            // Shift remaining entries
            for (uint32 j = i; j < suspended_process_count - 1; j++) {
                suspended_processes[j] = suspended_processes[j + 1];
            }
            suspended_processes[suspended_process_count - 1] = nullptr;
            suspended_process_count--;
            break;
        }
    }
    
    // Update statistics
    stats.total_resumptions++;
    if (flags & RESUME_FLAG_MANUAL) {
        stats.manual_resumes++;
    } else if (flags & RESUME_FLAG_FORCED) {
        stats.forced_resumes++;
    } else if (flags & RESUME_FLAG_GRACEFUL) {
        stats.graceful_resumes++;
    } else if (flags & RESUME_FLAG_IMMEDIATE) {
        stats.immediate_resumes++;
    } else {
        stats.auto_resumes++;
    }
    
    DLOG("Resumed process PID " << pid << " after " << context->suspend_duration << " ticks");
    
    return true;
}

bool ProcessSuspensionManager::IsProcessSuspended(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    return process->state == PROCESS_STATE_SUSPENDED;
}

ProcessSuspensionState ProcessSuspensionManager::GetProcessSuspensionState(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return PROCESS_SUSPEND_STATE_UNKNOWN;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return PROCESS_SUSPEND_STATE_UNKNOWN;
    }
    
    if (process->state == PROCESS_STATE_SUSPENDED) {
        ProcessSuspensionContext* context = process->suspension_context;
        if (context) {
            return context->state;
        }
        return PROCESS_SUSPEND_STATE_SUSPENDED;
    }
    
    return PROCESS_SUSPEND_STATE_ACTIVE;
}

ProcessSuspensionReason ProcessSuspensionManager::GetProcessSuspensionReason(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return SUSPEND_REASON_UNKNOWN;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return SUSPEND_REASON_UNKNOWN;
    }
    
    if (process->state == PROCESS_STATE_SUSPENDED) {
        ProcessSuspensionContext* context = process->suspension_context;
        if (context) {
            return context->reason;
        }
    }
    
    return SUSPEND_REASON_UNKNOWN;
}

uint32 ProcessSuspensionManager::GetSuspendCount(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    if (process->state == PROCESS_STATE_SUSPENDED) {
        ProcessSuspensionContext* context = process->suspension_context;
        if (context) {
            return context->suspend_count;
        }
    }
    
    return 0;
}

bool ProcessSuspensionManager::SetSuspendTimeout(uint32 pid, uint32 timeout_ms) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    if (process->state == PROCESS_STATE_SUSPENDED) {
        ProcessSuspensionContext* context = process->suspension_context;
        if (context) {
            context->suspend_timeout = timeout_ms;
            if (context->is_automatic && timeout_ms > 0) {
                context->auto_resume_timer = context->suspend_time + timeout_ms;
            }
            return true;
        }
    }
    
    return false;
}

uint32 ProcessSuspensionManager::GetSuspendTimeout(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    if (process->state == PROCESS_STATE_SUSPENDED) {
        ProcessSuspensionContext* context = process->suspension_context;
        if (context) {
            return context->suspend_timeout;
        }
    }
    
    return 0;
}

bool ProcessSuspensionManager::CancelSuspend(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    if (process->state == PROCESS_STATE_SUSPENDED) {
        ProcessSuspensionContext* context = process->suspension_context;
        if (context) {
            context->is_cancelled = true;
            context->is_active = false;
            return ResumeProcess(pid, RESUME_FLAG_FORCED);
        }
    }
    
    return false;
}

bool ProcessSuspensionManager::AbortSuspend(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    if (process->state == PROCESS_STATE_SUSPENDED) {
        ProcessSuspensionContext* context = process->suspension_context;
        if (context) {
            context->is_aborted = true;
            context->is_active = false;
            context->is_failed = true;
            stats.suspend_abort_count++;
            return ResumeProcess(pid, RESUME_FLAG_FORCED);
        }
    }
    
    return false;
}

bool ProcessSuspensionManager::SuspendProcessNested(uint32 pid, ProcessSuspensionReason reason, 
                                                  uint32 flags, uint32 timeout_ms) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // If already suspended, increment suspend count
    if (IsProcessSuspended(pid)) {
        ProcessSuspensionContext* context = process->suspension_context;
        if (context) {
            context->suspend_count++;
            context->nested_suspend_count++;
            context->is_nested = true;
            DLOG("Nested suspend for PID " << pid << ", count: " << context->suspend_count);
            return true;
        }
    }
    
    // Otherwise, perform normal suspension
    return SuspendProcess(pid, reason, flags, timeout_ms);
}

bool ProcessSuspensionManager::ResumeProcessNested(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    if (IsProcessSuspended(pid)) {
        ProcessSuspensionContext* context = process->suspension_context;
        if (context && context->is_nested) {
            if (context->suspend_count > 1) {
                context->suspend_count--;
                context->nested_suspend_count--;
                DLOG("Nested resume for PID " << pid << ", count: " << context->suspend_count);
                return true;
            } else {
                // Last nested suspension, perform full resume
                context->is_nested = false;
                context->nested_suspend_count = 0;
                return ResumeProcess(pid, 0);
            }
        }
    }
    
    return false;
}

uint32 ProcessSuspensionManager::GetNestedSuspendCount(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    if (IsProcessSuspended(pid)) {
        ProcessSuspensionContext* context = process->suspension_context;
        if (context) {
            return context->nested_suspend_count;
        }
    }
    
    return 0;
}

bool ProcessSuspensionManager::IsProcessNestedSuspended(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    if (IsProcessSuspended(pid)) {
        ProcessSuspensionContext* context = process->suspension_context;
        if (context) {
            return context->is_nested;
        }
    }
    
    return false;
}

uint32 ProcessSuspensionManager::SuspendAllProcesses(ProcessSuspensionReason reason, 
                                                   uint32 flags, uint32 timeout_ms) {
    if (!is_initialized || !process_manager) {
        LOG("Process suspension manager or process manager not available");
        return 0;
    }
    
    uint32 suspend_count = 0;
    
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        // Don't suspend ourselves or the kernel
        if (current->pid != KERNEL_PID && current->pid != (current_process ? current_process->pid : 0)) {
            if (SuspendProcess(current->pid, reason, flags, timeout_ms)) {
                suspend_count++;
            }
        }
        current = current->next;
    }
    
    DLOG("Suspended " << suspend_count << " processes with reason " << GetSuspensionReasonName(reason));
    
    return suspend_count;
}

uint32 ProcessSuspensionManager::ResumeAllProcesses(uint32 flags) {
    if (!is_initialized) {
        LOG("Process suspension manager not available");
        return 0;
    }
    
    uint32 resume_count = 0;
    
    // Resume all suspended processes
    for (uint32 i = 0; i < suspended_process_count; ) {
        ProcessSuspensionContext* context = suspended_processes[i];
        if (context && context->is_suspended) {
            // Get the process PID from the context
            // In a real implementation, we'd have a better way to get the PID
            uint32 pid = context->suspend_requester_pid; // This is a simplification
            
            if (ResumeProcess(pid, flags)) {
                resume_count++;
                // Don't increment i since we removed an entry
            } else {
                i++; // Move to next entry
            }
        } else {
            i++; // Move to next entry
        }
    }
    
    DLOG("Resumed " << resume_count << " processes");
    
    return resume_count;
}

uint32 ProcessSuspensionManager::SuspendProcessGroup(uint32 pgid, ProcessSuspensionReason reason, 
                                                   uint32 flags, uint32 timeout_ms) {
    if (!is_initialized || !process_manager) {
        LOG("Process suspension manager or process manager not available");
        return 0;
    }
    
    uint32 suspend_count = 0;
    
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        // Check if process belongs to the specified group
        if (current->pgid == pgid) {
            if (SuspendProcess(current->pid, reason, flags, timeout_ms)) {
                suspend_count++;
            }
        }
        current = current->next;
    }
    
    DLOG("Suspended " << suspend_count << " processes in group PGID " << pgid 
         << " with reason " << GetSuspensionReasonName(reason));
    
    return suspend_count;
}

uint32 ProcessSuspensionManager::ResumeProcessGroup(uint32 pgid, uint32 flags) {
    if (!is_initialized || !process_manager) {
        LOG("Process suspension manager or process manager not available");
        return 0;
    }
    
    uint32 resume_count = 0;
    
    // Resume processes in the specified group
    for (uint32 i = 0; i < suspended_process_count; ) {
        ProcessSuspensionContext* context = suspended_processes[i];
        if (context && context->is_suspended) {
            // Get the process PID from the context
            // In a real implementation, we'd have a better way to get the PID
            ProcessControlBlock* process = process_manager->GetProcessById(context->suspend_requester_pid);
            if (process && process->pgid == pgid) {
                if (ResumeProcess(process->pid, flags)) {
                    resume_count++;
                    // Don't increment i since we removed an entry
                } else {
                    i++; // Move to next entry
                }
            } else {
                i++; // Move to next entry
            }
        } else {
            i++; // Move to next entry
        }
    }
    
    DLOG("Resumed " << resume_count << " processes in group PGID " << pgid);
    
    return resume_count;
}

uint32 ProcessSuspensionManager::SuspendSession(uint32 sid, ProcessSuspensionReason reason, 
                                              uint32 flags, uint32 timeout_ms) {
    if (!is_initialized || !process_manager) {
        LOG("Process suspension manager or process manager not available");
        return 0;
    }
    
    uint32 suspend_count = 0;
    
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        // Check if process belongs to the specified session
        if (current->sid == sid) {
            if (SuspendProcess(current->pid, reason, flags, timeout_ms)) {
                suspend_count++;
            }
        }
        current = current->next;
    }
    
    DLOG("Suspended " << suspend_count << " processes in session SID " << sid 
         << " with reason " << GetSuspensionReasonName(reason));
    
    return suspend_count;
}

uint32 ProcessSuspensionManager::ResumeSession(uint32 sid, uint32 flags) {
    if (!is_initialized || !process_manager) {
        LOG("Process suspension manager or process manager not available");
        return 0;
    }
    
    uint32 resume_count = 0;
    
    // Resume processes in the specified session
    for (uint32 i = 0; i < suspended_process_count; ) {
        ProcessSuspensionContext* context = suspended_processes[i];
        if (context && context->is_suspended) {
            // Get the process PID from the context
            // In a real implementation, we'd have a better way to get the PID
            ProcessControlBlock* process = process_manager->GetProcessById(context->suspend_requester_pid);
            if (process && process->sid == sid) {
                if (ResumeProcess(process->pid, flags)) {
                    resume_count++;
                    // Don't increment i since we removed an entry
                } else {
                    i++; // Move to next entry
                }
            } else {
                i++; // Move to next entry
            }
        } else {
            i++; // Move to next entry
        }
    }
    
    DLOG("Resumed " << resume_count << " processes in session SID " << sid);
    
    return resume_count;
}

bool ProcessSuspensionManager::SuspendProcessIf(uint32 pid, bool (*condition)(uint32 pid, void* data), 
                                             void* condition_data, ProcessSuspensionReason reason, 
                                             uint32 flags, uint32 timeout_ms) {
    if (!is_initialized || !process_manager || !condition) {
        LOG("Invalid parameters for conditional process suspension");
        return false;
    }
    
    // Check the condition
    if (condition(pid, condition_data)) {
        return SuspendProcess(pid, reason, flags, timeout_ms);
    }
    
    return false; // Condition not met
}

bool ProcessSuspensionManager::ResumeProcessIf(uint32 pid, bool (*condition)(uint32 pid, void* data), 
                                            void* condition_data, uint32 flags) {
    if (!is_initialized || !process_manager || !condition) {
        LOG("Invalid parameters for conditional process resumption");
        return false;
    }
    
    // Check if process is suspended
    if (!IsProcessSuspended(pid)) {
        return false;
    }
    
    // Check the condition
    if (condition(pid, condition_data)) {
        return ResumeProcess(pid, flags);
    }
    
    return false; // Condition not met
}

bool ProcessSuspensionManager::SuspendProcessFor(uint32 pid, uint32 duration_ms, 
                                               ProcessSuspensionReason reason, uint32 flags) {
    return SuspendProcess(pid, reason, flags | SUSPEND_FLAG_AUTO_RESUME, duration_ms);
}

bool ProcessSuspensionManager::ResumeProcessAfter(uint32 pid, uint32 duration_ms, uint32 flags) {
    // This would set up a timer to resume the process after the specified duration
    // For now, we'll just log that we would do this
    LOG("Would resume process PID " << pid << " after " << duration_ms << " ms");
    return true;
}

bool ProcessSuspensionManager::SuspendProcessOnSignal(uint32 pid, uint32 signal, 
                                                    ProcessSuspensionReason reason, uint32 flags, 
                                                    uint32 timeout_ms) {
    // This would register a signal handler to suspend the process when the signal is received
    // For now, we'll just log that we would do this
    LOG("Would suspend process PID " << pid << " on signal " << signal);
    return true;
}

bool ProcessSuspensionManager::ResumeProcessOnSignal(uint32 pid, uint32 signal, uint32 flags) {
    // This would register a signal handler to resume the process when the signal is received
    // For now, we'll just log that we would do this
    LOG("Would resume process PID " << pid << " on signal " << signal);
    return true;
}

bool ProcessSuspensionManager::SuspendProcessOnEvent(uint32 pid, const char* event_name, 
                                                   ProcessSuspensionReason reason, uint32 flags, 
                                                   uint32 timeout_ms) {
    // This would register an event handler to suspend the process when the event occurs
    // For now, we'll just log that we would do this
    LOG("Would suspend process PID " << pid << " on event " << event_name);
    return true;
}

bool ProcessSuspensionManager::ResumeProcessOnEvent(uint32 pid, const char* event_name, uint32 flags) {
    // This would register an event handler to resume the process when the event occurs
    // For now, we'll just log that we would do this
    LOG("Would resume process PID " << pid << " on event " << event_name);
    return true;
}

bool ProcessSuspensionManager::SuspendProcessForResource(uint32 pid, uint32 resource_id, 
                                                       ProcessSuspensionReason reason, uint32 flags, 
                                                       uint32 timeout_ms) {
    // This would suspend the process when a resource becomes unavailable
    // For now, we'll just log that we would do this
    LOG("Would suspend process PID " << pid << " for resource " << resource_id);
    return true;
}

bool ProcessSuspensionManager::ResumeProcessForResource(uint32 pid, uint32 resource_id, uint32 flags) {
    // This would resume the process when a resource becomes available
    // For now, we'll just log that we would do this
    LOG("Would resume process PID " << pid << " for resource " << resource_id);
    return true;
}

bool ProcessSuspensionManager::SuspendProcessForCheckpoint(uint32 pid, uint32 checkpoint_id, 
                                                         ProcessSuspensionReason reason, uint32 flags, 
                                                         uint32 timeout_ms) {
    // This would suspend the process for checkpointing
    return SuspendProcess(pid, reason, flags | SUSPEND_FLAG_CHECKPOINTED, timeout_ms);
}

bool ProcessSuspensionManager::ResumeProcessFromCheckpoint(uint32 pid, uint32 checkpoint_id, uint32 flags) {
    // This would resume the process after checkpointing
    return ResumeProcess(pid, flags | RESUME_FLAG_FROM_CHECKPOINT);
}

uint32 ProcessSuspensionManager::CreateCheckpoint(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    uint32 checkpoint_id = next_checkpoint_id++;
    LOG("Created checkpoint ID " << checkpoint_id << " for process PID " << pid);
    
    return checkpoint_id;
}

bool ProcessSuspensionManager::RestoreCheckpoint(uint32 pid, uint32 checkpoint_id) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    LOG("Restoring checkpoint ID " << checkpoint_id << " for process PID " << pid);
    
    return true;
}

bool ProcessSuspensionManager::DeleteCheckpoint(uint32 pid, uint32 checkpoint_id) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    LOG("Deleting checkpoint ID " << checkpoint_id << " for process PID " << pid);
    
    return true;
}

bool ProcessSuspensionManager::SuspendProcessForMigration(uint32 pid, uint32 target_node, 
                                                        ProcessSuspensionReason reason, uint32 flags, 
                                                        uint32 timeout_ms) {
    // This would suspend the process for migration to another node
    return SuspendProcess(pid, reason, flags | SUSPEND_FLAG_MIGRATED, timeout_ms);
}

bool ProcessSuspensionManager::ResumeProcessAfterMigration(uint32 pid, uint32 target_node, uint32 flags) {
    // This would resume the process after migration
    return ResumeProcess(pid, flags | RESUME_FLAG_AFTER_MIGRATION);
}

bool ProcessSuspensionManager::SuspendProcessForPower(uint32 pid, uint32 power_state, 
                                                    ProcessSuspensionReason reason, uint32 flags, 
                                                    uint32 timeout_ms) {
    // This would suspend the process for power management
    return SuspendProcess(pid, reason, flags | SUSPEND_FLAG_POWER_MANAGED, timeout_ms);
}

bool ProcessSuspensionManager::ResumeProcessFromPower(uint32 pid, uint32 power_state, uint32 flags) {
    // This would resume the process after power state change
    return ResumeProcess(pid, flags | RESUME_FLAG_FROM_POWER);
}

bool ProcessSuspensionManager::SuspendProcessForSecurity(uint32 pid, uint32 security_level, 
                                                       ProcessSuspensionReason reason, uint32 flags, 
                                                       uint32 timeout_ms) {
    // This would suspend the process for security reasons
    return SuspendProcess(pid, reason, flags | SUSPEND_FLAG_SECURED, timeout_ms);
}

bool ProcessSuspensionManager::ResumeProcessFromSecurity(uint32 pid, uint32 security_level, uint32 flags) {
    // This would resume the process after security check
    return ResumeProcess(pid, flags | RESUME_FLAG_FROM_SECURITY);
}

bool ProcessSuspensionManager::SuspendProcessForError(uint32 pid, uint32 error_code, 
                                                    ProcessSuspensionReason reason, uint32 flags, 
                                                    uint32 timeout_ms) {
    // This would suspend the process due to an error
    return SuspendProcess(pid, reason, flags | SUSPEND_FLAG_ERROR_CONDITION, timeout_ms);
}

bool ProcessSuspensionManager::ResumeProcessFromError(uint32 pid, uint32 error_code, uint32 flags) {
    // This would resume the process after error handling
    return ResumeProcess(pid, flags | RESUME_FLAG_FROM_ERROR);
}

bool ProcessSuspensionManager::EnableAutoSuspend(uint32 pid, uint32 interval_ms) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Set auto-suspend interval
    process->auto_suspend_interval = interval_ms;
    process->last_suspend_check = global_timer ? global_timer->GetTickCount() : 0;
    
    DLOG("Enabled auto-suspend for PID " << pid << " with interval " << interval_ms << " ms");
    
    return true;
}

bool ProcessSuspensionManager::DisableAutoSuspend(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Disable auto-suspend
    process->auto_suspend_interval = 0;
    process->last_suspend_check = 0;
    
    DLOG("Disabled auto-suspend for PID " << pid);
    
    return true;
}

bool ProcessSuspensionManager::IsAutoSuspendEnabled(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    return process->auto_suspend_interval > 0;
}

bool ProcessSuspensionManager::EnableAutoResume(uint32 pid, uint32 interval_ms) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Set auto-resume interval
    process->auto_resume_interval = interval_ms;
    process->last_resume_check = global_timer ? global_timer->GetTickCount() : 0;
    
    DLOG("Enabled auto-resume for PID " << pid << " with interval " << interval_ms << " ms");
    
    return true;
}

bool ProcessSuspensionManager::DisableAutoResume(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Disable auto-resume
    process->auto_resume_interval = 0;
    process->last_resume_check = 0;
    
    DLOG("Disabled auto-resume for PID " << pid);
    
    return true;
}

bool ProcessSuspensionManager::IsAutoResumeEnabled(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    return process->auto_resume_interval > 0;
}

bool ProcessSuspensionManager::SaveProcessContext(uint32 pid, ProcessSuspensionContext* context) {
    if (!context || !is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Initialize context
    memset(context, 0, sizeof(ProcessSuspensionContext));
    
    // Save process context information
    context->state = PROCESS_SUSPEND_STATE_ACTIVE;  // Default to active
    context->reason = SUSPEND_REASON_USER_REQUEST; // Default reason
    context->suspend_count = 0;
    context->suspend_flags = 0;
    context->suspend_time = global_timer ? global_timer->GetTickCount() : 0;
    context->resume_time = 0;
    context->suspend_duration = 0;
    context->suspend_timeout = 0;
    context->suspend_signal = 0;
    context->suspend_requester_pid = process->pid;
    context->suspend_requester_uid = process->uid;
    context->suspend_context = nullptr;  // Would save CPU registers in a real implementation
    context->suspend_context_size = 0;
    context->suspend_memory_map = nullptr;  // Would save memory mappings
    context->suspend_memory_map_size = 0;
    context->suspend_file_descriptors = nullptr;  // Would save file descriptors
    context->suspend_file_descriptors_size = 0;
    context->suspend_network_connections = nullptr;  // Would save network connections
    context->suspend_network_connections_size = 0;
    context->suspend_ipc_connections = nullptr;  // Would save IPC connections
    context->suspend_ipc_connections_size = 0;
    context->suspend_thread_state = nullptr;  // Would save thread state
    context->suspend_thread_state_size = 0;
    strncpy(context->suspend_note, "Process context saved", sizeof(context->suspend_note) - 1);
    context->suspend_note[sizeof(context->suspend_note) - 1] = '\0';
    context->suspend_checkpoint_id = 0;
    context->suspend_migration_target = 0;
    context->suspend_power_state = 0;
    context->suspend_security_level = 0;
    context->suspend_error_code = 0;
    context->suspend_error_info = 0;
    context->suspend_timestamp = context->suspend_time;
    context->resume_timestamp = 0;
    context->last_suspend_time = context->suspend_time;
    context->last_resume_time = 0;
    context->total_suspend_time = 0;
    context->suspend_count_total = 0;
    context->resume_count_total = 0;
    context->is_suspended = false;
    context->is_resumable = true;
    context->is_checkpointed = false;
    context->is_migrated = false;
    context->is_power_managed = false;
    context->is_secured = false;
    context->is_encrypted = false;
    context->is_compressed = false;
    context->is_transactional = false;
    context->is_recoverable = true;
    context->is_atomic = true;
    context->is_recursive = false;
    context->is_nested = false;
    context->is_temporary = false;
    context->is_permanent = false;
    context->is_manual = false;
    context->is_automatic = false;
    context->is_notified = false;
    context->is_acknowledged = false;
    context->is_pending = false;
    context->is_active = true;
    context->is_expired = false;
    context->is_cancelled = false;
    context->is_aborted = false;
    context->is_failed = false;
    context->is_successful = true;
    
    DLOG("Saved process context for PID " << pid);
    
    return true;
}

bool ProcessSuspensionManager::RestoreProcessContext(uint32 pid, const ProcessSuspensionContext* context) {
    if (!context || !is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Restore process context information
    // In a real implementation, we would restore the saved context
    // For now, we'll just log that we would restore
    
    DLOG("Restoring process context for PID " << pid);
    
    return true;
}

bool ProcessSuspensionManager::ClearProcessContext(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Clear process context information
    // In a real implementation, we would clear the saved context
    // For now, we'll just log that we would clear
    
    DLOG("Cleared process context for PID " << pid);
    
    return true;
}

const ProcessSuspensionContext* ProcessSuspensionManager::GetProcessContext(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return nullptr;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return nullptr;
    }
    
    return process->suspension_context;
}

bool ProcessSuspensionManager::SetProcessContextNote(uint32 pid, const char* note) {
    if (!note || !is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return false;
    }
    
    strncpy(context->suspend_note, note, sizeof(context->suspend_note) - 1);
    context->suspend_note[sizeof(context->suspend_note) - 1] = '\0';
    
    DLOG("Set process context note for PID " << pid << ": " << note);
    
    return true;
}

const char* ProcessSuspensionManager::GetProcessContextNote(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return nullptr;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return nullptr;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return nullptr;
    }
    
    return context->suspend_note;
}

bool ProcessSuspensionManager::SetProcessSuspensionState(uint32 pid, ProcessSuspensionState state) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return false;
    }
    
    context->state = state;
    
    DLOG("Set suspension state for PID " << pid << " to " << GetSuspensionStateName(state));
    
    return true;
}

bool ProcessSuspensionManager::SetProcessSuspensionReason(uint32 pid, ProcessSuspensionReason reason) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return false;
    }
    
    context->reason = reason;
    
    DLOG("Set suspension reason for PID " << pid << " to " << GetSuspensionReasonName(reason));
    
    return true;
}

bool ProcessSuspensionManager::SetProcessSuspensionFlags(uint32 pid, uint32 flags) {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return false;
    }
    
    context->suspend_flags = flags;
    
    DLOG("Set suspension flags for PID " << pid << " to 0x" << flags);
    
    return true;
}

uint32 ProcessSuspensionManager::GetProcessSuspensionFlags(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return 0;
    }
    
    return context->suspend_flags;
}

bool ProcessSuspensionManager::AddProcessSuspensionFlag(uint32 pid, uint32 flag) {
    uint32 current_flags = GetProcessSuspensionFlags(pid);
    return SetProcessSuspensionFlags(pid, current_flags | flag);
}

bool ProcessSuspensionManager::RemoveProcessSuspensionFlag(uint32 pid, uint32 flag) {
    uint32 current_flags = GetProcessSuspensionFlags(pid);
    return SetProcessSuspensionFlags(pid, current_flags & ~flag);
}

bool ProcessSuspensionManager::HasProcessSuspensionFlag(uint32 pid, uint32 flag) {
    uint32 current_flags = GetProcessSuspensionFlags(pid);
    return (current_flags & flag) != 0;
}

uint32 ProcessSuspensionManager::GetProcessSuspendTime(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return 0;
    }
    
    return context->suspend_time;
}

uint32 ProcessSuspensionManager::GetProcessResumeTime(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return 0;
    }
    
    return context->resume_time;
}

uint32 ProcessSuspensionManager::GetProcessSuspendDuration(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return 0;
    }
    
    return context->suspend_duration;
}

uint32 ProcessSuspensionManager::GetProcessTotalSuspendTime(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return 0;
    }
    
    return context->total_suspend_time;
}

uint32 ProcessSuspensionManager::GetProcessLastSuspendTime(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return 0;
    }
    
    return context->last_suspend_time;
}

uint32 ProcessSuspensionManager::GetProcessLastResumeTime(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return 0;
    }
    
    return context->last_resume_time;
}

uint32 ProcessSuspensionManager::GetProcessAvgSuspendDuration(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context || context->suspend_count_total == 0) {
        return 0;
    }
    
    return context->total_suspend_time / context->suspend_count_total;
}

uint32 ProcessSuspensionManager::GetProcessMaxSuspendDuration(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return 0;
    }
    
    // In a real implementation, we'd track the maximum suspend duration
    // For now, we'll just return the current suspend duration
    return context->suspend_duration;
}

uint32 ProcessSuspensionManager::GetProcessMinSuspendDuration(uint32 pid) {
    if (!is_initialized || !process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    ProcessSuspensionContext* context = process->suspension_context;
    if (!context) {
        return 0;
    }
    
    // In a real implementation, we'd track the minimum suspend duration
    // For now, we'll just return the current suspend duration
    return context->suspend_duration;
}

const ProcessAccountingStats* ProcessAccountingManager::GetStatistics() {
    UpdateStatistics();
    return &stats;
}

void ProcessAccountingManager::ResetStatistics() {
    memset(&stats, 0, sizeof(stats));
    DLOG("Process accounting statistics reset");
}

void ProcessAccountingManager::UpdateStatistics() {
    if (!is_initialized) {
        return;
    }
    
    // Update active process count
    stats.active_processes = 0;
    stats.terminated_processes = 0;
    
    if (process_manager) {
        ProcessControlBlock* current = process_manager->GetProcessListHead();
        while (current) {
            if (current->state == PROCESS_STATE_TERMINATED || 
                current->state == PROCESS_STATE_ZOMBIE) {
                stats.terminated_processes++;
            } else {
                stats.active_processes++;
            }
            current = current->next;
        }
    }
    
    stats.total_processes = stats.active_processes + stats.terminated_processes;
    
    DLOG("Updated process accounting statistics");
}

uint64 ProcessAccountingManager::GetTotalCPUTime() {
    return stats.total_cpu_time;
}

uint64 ProcessAccountingManager::GetTotalIOTime() {
    return stats.total_read_bytes + stats.total_write_bytes;
}

uint32 ProcessAccountingManager::GetAverageProcessLifetime() {
    if (stats.terminated_processes == 0) {
        return 0;
    }
    
    return (uint32)(stats.total_cpu_time / stats.terminated_processes);
}

uint32 ProcessAccountingManager::GetPeakProcessCount() {
    return stats.total_processes;
}

uint32 ProcessAccountingManager::GetProcessCreationRate() {
    uint32 current_time = global_timer ? global_timer->GetTickCount() : 0;
    uint32 elapsed_time = current_time - last_update_time;
    
    if (elapsed_time > 0) {
        return stats.total_processes / elapsed_time * 1000; // Per second rate
    }
    
    return 0;
}

const char* ProcessAccountingManager::GetProcessCommand(uint32 pid) {
    if (!process_manager) {
        return nullptr;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return nullptr;
    }
    
    return process->name;
}

uint32 ProcessAccountingManager::GetProcessStartTime(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    return process->creation_time;
}

uint32 ProcessAccountingManager::GetProcessEndTime(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    return process->termination_time;
}

uint32 ProcessAccountingManager::GetProcessCPUTime(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    return process->total_cpu_time_used;
}

uint32 ProcessAccountingManager::GetProcessMemoryUsage(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    return process->heap_end - process->heap_start;
}

uint32 ProcessAccountingManager::GetProcessIOBytes(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    // Return approximate I/O bytes (dummy implementation)
    return process->total_cpu_time_used * 1536; // Read + Write bytes
}

uint32 ProcessAccountingManager::GetProcessPageFaults(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    // Return approximate page faults (dummy implementation)
    return process->total_cpu_time_used / 100;
}

uint32 ProcessAccountingManager::GetProcessContextSwitches(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    return process->context_switch_count;
}

bool ProcessAccountingManager::MonitorProcess(uint32 pid) {
    return StartAccounting(pid);
}

bool ProcessAccountingManager::UnmonitorProcess(uint32 pid) {
    return StopAccounting(pid);
}

bool ProcessAccountingManager::IsProcessMonitored(uint32 pid) {
    return IsAccountingEnabled(pid);
}

uint32 ProcessAccountingManager::GetMonitoredProcessCount() {
    if (!process_manager) {
        return 0;
    }
    
    uint32 count = 0;
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    
    while (current) {
        if (IsAccountingEnabled(current->pid)) {
            count++;
        }
        current = current->next;
    }
    
    return count;
}

void ProcessAccountingManager::MonitorAllProcesses() {
    if (!process_manager) {
        return;
    }
    
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        StartAccounting(current->pid);
        current = current->next;
    }
    
    DLOG("Monitoring all processes");
}

void ProcessAccountingManager::UnmonitorAllProcesses() {
    if (!process_manager) {
        return;
    }
    
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        StopAccounting(current->pid);
        current = current->next;
    }
    
    DLOG("Unmonitoring all processes");
}

void ProcessAccountingManager::OnProcessCreate(uint32 pid) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Start accounting for new process
    StartAccounting(pid);
    
    // Update statistics
    stats.total_processes++;
    stats.active_processes++;
    
    DLOG("Accounting started for new process PID " << pid);
}

void ProcessAccountingManager::OnProcessTerminate(uint32 pid) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update statistics
    stats.terminated_processes++;
    if (stats.active_processes > 0) {
        stats.active_processes--;
    }
    
    // Create final accounting record
    ProcessAccountingRecord record;
    if (CollectProcessData(pid, &record)) {
        record.end_time = global_timer ? global_timer->GetTickCount() : 0;
        AddRecord(&record);
    }
    
    DLOG("Accounting finalized for terminated process PID " << pid);
}

void ProcessAccountingManager::OnProcessSwitch(uint32 old_pid, uint32 new_pid) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update accounting for both processes
    if (old_pid != INVALID_PID) {
        UpdateAccounting(old_pid);
    }
    
    if (new_pid != INVALID_PID) {
        UpdateAccounting(new_pid);
    }
    
    DLOG("Accounting updated for process switch: " << old_pid << " -> " << new_pid);
}

void ProcessAccountingManager::OnSystemCall(uint32 pid, uint32 syscall_number) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update system call count
    stats.total_processes++; // Increment total to account for the syscall
    UpdateAccounting(pid);
    
    DLOG("Accounting updated for system call " << syscall_number << " by PID " << pid);
}

void ProcessAccountingManager::OnPageFault(uint32 pid) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update page fault statistics
    stats.total_page_faults++;
    UpdateAccounting(pid);
    
    DLOG("Accounting updated for page fault by PID " << pid);
}

void ProcessAccountingManager::OnContextSwitch(uint32 pid) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update context switch statistics
    stats.total_context_switches++;
    UpdateAccounting(pid);
    
    DLOG("Accounting updated for context switch by PID " << pid);
}

void ProcessAccountingManager::OnTimerTick() {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Check if it's time for a snapshot
    uint32 current_time = global_timer ? global_timer->GetTickCount() : 0;
    
    if (config.update_interval > 0 && 
        (current_time - last_update_time) >= config.update_interval) {
        SnapshotAllProcesses();
        last_update_time = current_time;
    }
    
    // Handle log rotation if needed
    if (config.auto_rotate) {
        // In a real implementation, we'd check log file size
        // For now, we'll just log that we would check
        static uint32 rotation_check_counter = 0;
        rotation_check_counter++;
        
        if (rotation_check_counter % 1000 == 0) {  // Check every 1000 ticks
            RotateLogFile();
            rotation_check_counter = 0;
        }
    }
}

void ProcessAccountingManager::OnIOPerformed(uint32 pid, uint32 bytes_read, uint32 bytes_written) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update I/O statistics
    stats.total_read_bytes += bytes_read;
    stats.total_write_bytes += bytes_written;
    
    DLOG("Accounting updated for I/O: PID " << pid 
         << ", Read: " << bytes_read << " bytes, Write: " << bytes_written << " bytes");
}

void ProcessAccountingManager::OnSignalDelivered(uint32 pid, uint32 signal) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update signal statistics
    stats.total_signals++;
    UpdateAccounting(pid);
    
    DLOG("Accounting updated for signal " << signal << " delivered to PID " << pid);
}

void ProcessAccountingManager::OnResourceLimitExceeded(uint32 pid, uint32 resource) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Log resource limit exceeded
    LOG("Process PID " << pid << " exceeded resource limit " << resource);
    
    // Update accounting error statistics
    stats.accounting_errors++;
    
    DLOG("Accounting error recorded for PID " << pid);
}

bool ProcessAccountingManager::ResizeBuffer(uint32 new_capacity) {
    if (!is_initialized) {
        return false;
    }
    
    if (new_capacity == 0 || new_capacity > config.max_records) {
        LOG("Invalid buffer capacity: " << new_capacity);
        return false;
    }
    
    // Allocate new buffers
    ProcessAccountingRecord* new_records = (ProcessAccountingRecord*)realloc(
        buffer.records, sizeof(ProcessAccountingRecord) * new_capacity);
    uint32* new_timestamps = (uint32*)realloc(
        buffer.timestamps, sizeof(uint32) * new_capacity);
    
    if (!new_records || !new_timestamps) {
        LOG("Failed to allocate new accounting buffers");
        if (new_records) free(new_records);
        if (new_timestamps) free(new_timestamps);
        return false;
    }
    
    // Copy existing data
    uint32 copy_count = (buffer.count < new_capacity) ? buffer.count : new_capacity;
    for (uint32 i = 0; i < copy_count; i++) {
        uint32 src_index = (buffer.head + i) % buffer.capacity;
        uint32 dst_index = i;
        memcpy(&new_records[dst_index], &buffer.records[src_index], sizeof(ProcessAccountingRecord));
        new_timestamps[dst_index] = buffer.timestamps[src_index];
    }
    
    // Replace buffers
    free(buffer.records);
    free(buffer.timestamps);
    buffer.records = new_records;
    buffer.timestamps = new_timestamps;
    buffer.capacity = new_capacity;
    buffer.count = copy_count;
    buffer.head = 0;
    buffer.tail = copy_count;
    buffer.is_full = (copy_count == new_capacity);
    
    DLOG("Resized accounting buffer to " << new_capacity << " records");
    return true;
}

bool ProcessAccountingManager::FlushBuffer() {
    if (!is_initialized || !IsEnabled()) {
        return false;
    }
    
    // Write all records to file
    if (config.flags & ACCOUNTING_FLAG_TO_FILE) {
        WriteAllRecordsToFile();
    }
    
    // Clear buffer
    ClearRecords();
    
    DLOG("Flushed accounting buffer");
    return true;
}

bool ProcessAccountingManager::IsBufferFull() {
    return buffer.is_full;
}

uint32 ProcessAccountingManager::GetBufferUsage() {
    if (buffer.capacity == 0) {
        return 0;
    }
    
    return (buffer.count * 100) / buffer.capacity;
}

uint32 ProcessAccountingManager::GetBufferFreeSpace() {
    return buffer.capacity - buffer.count;
}

void ProcessAccountingManager::PrintAccountingSummary() {
    LOG("=== Process Accounting Summary ===");
    LOG("Initialized: " << (is_initialized ? "Yes" : "No"));
    LOG("Enabled: " << (IsEnabled() ? "Yes" : "No"));
    LOG("Buffer Capacity: " << buffer.capacity);
    LOG("Buffer Count: " << buffer.count);
    LOG("Buffer Usage: " << GetBufferUsage() << "%");
    LOG("Buffer Free Space: " << GetBufferFreeSpace() << " records");
    LOG("Records Processed: " << stats.total_processes);
    LOG("Active Processes: " << stats.active_processes);
    LOG("Terminated Processes: " << stats.terminated_processes);
    LOG("=================================");
}

void ProcessAccountingManager::PrintProcessAccounting(uint32 pid) {
    ProcessAccountingRecord record;
    if (GetRecord(pid, &record)) {
        LOG("=== Accounting for PID " << pid << " ===");
        LOG("Command: " << record.command);
        LOG("User: " << record.uid << ", Group: " << record.gid);
        LOG("Start Time: " << record.start_time);
        LOG("End Time: " << record.end_time);
        LOG("CPU Time: " << record.cpu_time << " ticks");
        LOG("User Time: " << record.user_time << " ticks");
        LOG("System Time: " << record.system_time << " ticks");
        LOG("Wait Time: " << record.wait_time << " ticks");
        LOG("Read Bytes: " << record.read_bytes);
        LOG("Write Bytes: " << record.write_bytes);
        LOG("Page Faults: " << record.page_faults);
        LOG("Context Switches: " << record.context_switches);
        LOG("===============================");
    } else {
        LOG("No accounting record found for PID " << pid);
    }
}

void ProcessAccountingManager::PrintAllProcessAccounting() {
    LOG("=== All Process Accounting Records ===");
    LOG("Total Records: " << buffer.count);
    LOG("Buffer Capacity: " << buffer.capacity);
    LOG("Buffer Usage: " << GetBufferUsage() << "%");
    
    for (uint32 i = 0; i < buffer.count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        ProcessAccountingRecord* record = &buffer.records[index];
        LOG("PID: " << record->pid 
            << ", Command: " << record->command
            << ", CPU Time: " << record->cpu_time << " ticks"
            << ", Memory: " << record->memory_max << " bytes");
    }
    
    LOG("=====================================");
}

void ProcessAccountingManager::PrintAccountingStatistics() {
    UpdateStatistics();
    LOG("=== Process Accounting Statistics ===");
    LOG("Total Processes: " << stats.total_processes);
    LOG("Active Processes: " << stats.active_processes);
    LOG("Terminated Processes: " << stats.terminated_processes);
    LOG("Total CPU Time: " << stats.total_cpu_time << " ticks");
    LOG("Total User Time: " << stats.total_user_time << " ticks");
    LOG("Total System Time: " << stats.total_system_time << " ticks");
    LOG("Total Wait Time: " << stats.total_wait_time << " ticks");
    LOG("Total Read Bytes: " << stats.total_read_bytes);
    LOG("Total Write Bytes: " << stats.total_write_bytes);
    LOG("Total Page Faults: " << stats.total_page_faults);
    LOG("Total Context Switches: " << stats.total_context_switches);
    LOG("===============================");
}

void ProcessAccountingManager::PrintAccountingConfiguration() {
    LOG("=== Process Accounting Configuration ===");
    LOG("Flags: 0x" << config.flags);
    LOG("Update Interval: " << config.update_interval << " ticks");
    LOG("Buffer Size: " << config.buffer_size << " records");
    LOG("Max Records: " << config.max_records);
    LOG("Log File: " << config.log_file);
    LOG("Auto Rotate: " << (config.auto_rotate ? "Yes" : "No"));
    LOG("Rotate Size: " << config.rotate_size << " bytes");
    LOG("Retention Days: " << config.retention_days);
    LOG("Compress Old: " << (config.compress_old ? "Yes" : "No"));
    LOG("Compression Threshold: " << config.compression_threshold << " days");
    LOG("=====================================");
}

void ProcessAccountingManager::PrintBufferStatus() {
    LOG("=== Accounting Buffer Status ===");
    LOG("Capacity: " << buffer.capacity);
    LOG("Count: " << buffer.count);
    LOG("Head: " << buffer.head);
    LOG("Tail: " << buffer.tail);
    LOG("Is Full: " << (buffer.is_full ? "Yes" : "No"));
    LOG("Usage: " << GetBufferUsage() << "%");
    LOG("Free Space: " << GetBufferFreeSpace() << " records");
    LOG("===============================");
}

void ProcessAccountingManager::DumpAccountingData() {
    LOG("=== Accounting Data Dump ===");
    
    // Dump raw buffer data
    for (uint32 i = 0; i < buffer.count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        ProcessAccountingRecord* record = &buffer.records[index];
        LOG("Index: " << i << ", PID: " << record->pid 
            << ", Command: " << record->command
            << ", CPU Time: " << record->cpu_time << " ticks"
            << ", Memory: " << record->memory_max << " bytes"
            << ", Time: " << buffer.timestamps[index]);
    }
    
    LOG("=============================");
}

void ProcessAccountingManager::ValidateAccountingData() {
    LOG("=== Validating Accounting Data ===");
    
    bool is_valid = true;
    
    // Validate buffer consistency
    if (buffer.count > buffer.capacity) {
        LOG("ERROR: Buffer count (" << buffer.count 
            << ") exceeds capacity (" << buffer.capacity << ")");
        is_valid = false;
    }
    
    if (buffer.is_full && buffer.count != buffer.capacity) {
        LOG("WARNING: Buffer marked as full but count (" << buffer.count 
            << ") != capacity (" << buffer.capacity << ")");
    }
    
    // Validate record data
    for (uint32 i = 0; i < buffer.count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        ProcessAccountingRecord* record = &buffer.records[index];
        
        if (record->pid == 0) {
            LOG("WARNING: Record " << i << " has invalid PID: " << record->pid);
        }
        
        if (record->cpu_time > 1000000) {  // Arbitrary large value check
            LOG("WARNING: Record " << i << " has unusually high CPU time: " << record->cpu_time);
        }
    }
    
    LOG("Validation " << (is_valid ? "PASSED" : "FAILED"));
    LOG("===============================");
}

bool ProcessAccountingManager::ExportToCSV(const char* filename) {
    if (!filename || !is_initialized) {
        return false;
    }
    
    LOG("Exporting accounting data to CSV file: " << filename);
    
    // In a real implementation, we'd write to the CSV file
    // For now, we'll just log that we would export
    
    return true;
}

bool ProcessAccountingManager::ExportToJSON(const char* filename) {
    if (!filename || !is_initialized) {
        return false;
    }
    
    LOG("Exporting accounting data to JSON file: " << filename);
    
    // In a real implementation, we'd write to the JSON file
    // For now, we'll just log that we would export
    
    return true;
}

bool ProcessAccountingManager::ExportToXML(const char* filename) {
    if (!filename || !is_initialized) {
        return false;
    }
    
    LOG("Exporting accounting data to XML file: " << filename);
    
    // In a real implementation, we'd write to the XML file
    // For now, we'll just log that we would export
    
    return true;
}

bool ProcessAccountingManager::ImportFromCSV(const char* filename) {
    if (!filename || !is_initialized) {
        return false;
    }
    
    LOG("Importing accounting data from CSV file: " << filename);
    
    // In a real implementation, we'd read from the CSV file
    // For now, we'll just log that we would import
    
    return true;
}

bool ProcessAccountingManager::ImportFromJSON(const char* filename) {
    if (!filename || !is_initialized) {
        return false;
    }
    
    LOG("Importing accounting data from JSON file: " << filename);
    
    // In a real implementation, we'd read from the JSON file
    // For now, we'll just log that we would import
    
    return true;
}

bool ProcessAccountingManager::ImportFromXML(const char* filename) {
    if (!filename || !is_initialized) {
        return false;
    }
    
    LOG("Importing accounting data from XML file: " << filename);
    
    // In a real implementation, we'd read from the XML file
    // For now, we'll just log that we would import
    
    return true;
}

void ProcessAccountingManager::SortRecordsByCPUTime(ProcessAccountingRecord* records, uint32 count) {
    if (!records || count <= 1) {
        return;
    }
    
    // Simple bubble sort by CPU time (descending order)
    for (uint32 i = 0; i < count - 1; i++) {
        for (uint32 j = 0; j < count - i - 1; j++) {
            if (records[j].cpu_time < records[j + 1].cpu_time) {
                // Swap records
                ProcessAccountingRecord temp = records[j];
                records[j] = records[j + 1];
                records[j + 1] = temp;
            }
        }
    }
}

void ProcessAccountingManager::SortRecordsByMemoryUsage(ProcessAccountingRecord* records, uint32 count) {
    if (!records || count <= 1) {
        return;
    }
    
    // Simple bubble sort by memory usage (descending order)
    for (uint32 i = 0; i < count - 1; i++) {
        for (uint32 j = 0; j < count - i - 1; j++) {
            if (records[j].memory_max < records[j + 1].memory_max) {
                // Swap records
                ProcessAccountingRecord temp = records[j];
                records[j] = records[j + 1];
                records[j + 1] = temp;
            }
        }
    }
}

void ProcessAccountingManager::SortRecordsByStartTime(ProcessAccountingRecord* records, uint32 count) {
    if (!records || count <= 1) {
        return;
    }
    
    // Simple bubble sort by start time (ascending order)
    for (uint32 i = 0; i < count - 1; i++) {
        for (uint32 j = 0; j < count - i - 1; j++) {
            if (records[j].start_time > records[j + 1].start_time) {
                // Swap records
                ProcessAccountingRecord temp = records[j];
                records[j] = records[j + 1];
                records[j + 1] = temp;
            }
        }
    }
}

void ProcessAccountingManager::FilterRecordsByCommand(const char* command, ProcessAccountingRecord* records, uint32 count) {
    if (!command || !records || count == 0) {
        return;
    }
    
    // Filter records by command name
    uint32 filtered_count = 0;
    
    for (uint32 i = 0; i < buffer.count && filtered_count < count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (strstr(buffer.records[index].command, command)) {
            memcpy(&records[filtered_count], &buffer.records[index], sizeof(ProcessAccountingRecord));
            filtered_count++;
        }
    }
    
    // Clear remaining slots
    for (uint32 i = filtered_count; i < count; i++) {
        memset(&records[i], 0, sizeof(ProcessAccountingRecord));
    }
}

void ProcessAccountingManager::FilterRecordsByExitStatus(uint32 exit_status, ProcessAccountingRecord* records, uint32 count) {
    if (!records || count == 0) {
        return;
    }
    
    // Filter records by exit status
    uint32 filtered_count = 0;
    
    for (uint32 i = 0; i < buffer.count && filtered_count < count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (buffer.records[index].exit_status == exit_status) {
            memcpy(&records[filtered_count], &buffer.records[index], sizeof(ProcessAccountingRecord));
            filtered_count++;
        }
    }
    
    // Clear remaining slots
    for (uint32 i = filtered_count; i < count; i++) {
        memset(&records[i], 0, sizeof(ProcessAccountingRecord));
    }
}

bool ProcessAccountingManager::SetCPUThreshold(uint32 pid, uint32 threshold) {
    if (!process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Set CPU time threshold (we'll use a flag to indicate this)
    process->flags |= (threshold << 8);  // Store threshold in upper bits of flags
    
    DLOG("Set CPU threshold for PID " << pid << " to " << threshold << " ticks");
    return true;
}

bool ProcessAccountingManager::SetMemoryThreshold(uint32 pid, uint32 threshold) {
    if (!process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Set memory usage threshold
    process->flags |= (threshold << 16);  // Store threshold in upper bits of flags
    
    DLOG("Set memory threshold for PID " << pid << " to " << threshold << " bytes");
    return true;
}

bool ProcessAccountingManager::SetIOTreshold(uint32 pid, uint32 threshold) {
    if (!process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Set I/O threshold
    process->flags |= (threshold << 24);  // Store threshold in upper bits of flags
    
    DLOG("Set I/O threshold for PID " << pid << " to " << threshold << " bytes");
    return true;
}

bool ProcessAccountingManager::CheckThresholds(uint32 pid) {
    if (!process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Check CPU threshold
    uint32 cpu_threshold = (process->flags >> 8) & 0xFF;
    if (cpu_threshold > 0 && process->total_cpu_time_used >= cpu_threshold) {
        OnThresholdExceeded(pid, 1, process->total_cpu_time_used);
        return true;
    }
    
    // Check memory threshold
    uint32 memory_threshold = (process->flags >> 16) & 0xFF;
    uint32 memory_usage = process->heap_end - process->heap_start;
    if (memory_threshold > 0 && memory_usage >= memory_threshold) {
        OnThresholdExceeded(pid, 2, memory_usage);
        return true;
    }
    
    // Check I/O threshold
    uint32 io_threshold = (process->flags >> 24) & 0xFF;
    uint32 io_bytes = process->total_cpu_time_used * 1536; // Approximate I/O bytes
    if (io_threshold > 0 && io_bytes >= io_threshold) {
        OnThresholdExceeded(pid, 3, io_bytes);
        return true;
    }
    
    return false;
}

void ProcessAccountingManager::OnThresholdExceeded(uint32 pid, uint32 resource, uint32 value) {
    LOG("Process PID " << pid << " exceeded threshold for resource " << resource 
        << " with value " << value);
    
    // In a real implementation, we might send a signal to the process
    // or take other corrective action
}

bool ProcessAccountingManager::IsThresholdExceeded(uint32 pid, uint32 resource) {
    // Check if a threshold has been exceeded for a specific resource
    // This is a simplified implementation
    return CheckThresholds(pid);
}

bool ProcessAccountingManager::CleanupOldRecords() {
    if (!is_initialized) {
        return false;
    }
    
    uint32 current_time = global_timer ? global_timer->GetTickCount() : 0;
    uint32 cutoff_time = current_time - (config.retention_days * 24 * 60 * 60 * 1000); // Convert days to milliseconds
    
    uint32 cleanup_count = 0;
    
    // Remove records older than cutoff time
    for (uint32 i = 0; i < buffer.count; ) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (buffer.timestamps[index] < cutoff_time) {
            // Remove this record
            RemoveRecord(buffer.records[index].pid);
            cleanup_count++;
            // Don't increment i since we removed a record
        } else {
            i++; // Move to next record
        }
    }
    
    if (cleanup_count > 0) {
        DLOG("Cleaned up " << cleanup_count << " old accounting records");
    }
    
    return true;
}

bool ProcessAccountingManager::CleanupTerminatedProcesses() {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    uint32 cleanup_count = 0;
    
    // Clean up accounting for terminated processes
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        if (current->state == PROCESS_STATE_TERMINATED || 
            current->state == PROCESS_STATE_ZOMBIE) {
            if (IsAccountingEnabled(current->pid)) {
                StopAccounting(current->pid);
                cleanup_count++;
            }
        }
        current = current->next;
    }
    
    if (cleanup_count > 0) {
        DLOG("Cleaned up accounting for " << cleanup_count << " terminated processes");
    }
    
    return true;
}

bool ProcessAccountingManager::PurgeAllRecords() {
    if (!is_initialized) {
        return false;
    }
    
    // Clear all records
    ClearRecords();
    
    // Reset statistics
    ResetStatistics();
    
    DLOG("Purged all accounting records");
    return true;
}

uint32 ProcessAccountingManager::GetCleanupCount() {
    // Return approximate cleanup count
    return stats.accounting_errors + stats.buffer_overflows;
}

// System call implementations
uint32 SysCallEnableProcessAccounting() {
    if (!process_accounting_manager) {
        LOG("Process accounting manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (process_accounting_manager->Enable()) {
        return SUCCESS;
    }
    
    return ERROR_OPERATION_FAILED;
}

uint32 SysCallDisableProcessAccounting() {
    if (!process_accounting_manager) {
        LOG("Process accounting manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (process_accounting_manager->Disable()) {
        return SUCCESS;
    }
    
    return ERROR_OPERATION_FAILED;
}

uint32 SysCallGetProcessAccounting(uint32 pid, ProcessAccountingRecord* record) {
    if (!process_accounting_manager) {
        LOG("Process accounting manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!record) {
        return ERROR_INVALID_PARAMETER;
    }
    
    if (process_accounting_manager->GetRecord(pid, record)) {
        return SUCCESS;
    }
    
    return ERROR_NOT_FOUND;
}

uint32 SysCallGetProcessResourceUsage(uint32 pid, ProcessResourceUsage* usage) {
    if (!process_accounting_manager) {
        LOG("Process accounting manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!usage) {
        return ERROR_INVALID_PARAMETER;
    }
    
    if (process_accounting_manager->CollectResourceUsage(pid, usage)) {
        return SUCCESS;
    }
    
    return ERROR_NOT_FOUND;
}

uint32 SysCallSetAccountingConfig(const ProcessAccountingConfig* config) {
    if (!process_accounting_manager) {
        LOG("Process accounting manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        return ERROR_INVALID_PARAMETER;
    }
    
    if (process_accounting_manager->Configure(config)) {
        return SUCCESS;
    }
    
    return ERROR_INVALID_PARAMETER;
}

uint32 SysCallGetAccountingConfig(ProcessAccountingConfig* config) {
    if (!process_accounting_manager) {
        LOG("Process accounting manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        return ERROR_INVALID_PARAMETER;
    }
    
    // Copy current configuration
    memcpy(config, &process_accounting_manager->config, sizeof(ProcessAccountingConfig));
    
    return SUCCESS;
}

uint32 SysCallGetAccountingStatistics(ProcessAccountingStats* stats) {
    if (!process_accounting_manager) {
        LOG("Process accounting manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!stats) {
        return ERROR_INVALID_PARAMETER;
    }
    
    const ProcessAccountingStats* acct_stats = process_accounting_manager->GetStatistics();
    if (acct_stats) {
        memcpy(stats, acct_stats, sizeof(ProcessAccountingStats));
        return SUCCESS;
    }
    
    return ERROR_OPERATION_FAILED;
}

uint32 SysCallResetAccounting() {
    if (!process_accounting_manager) {
        LOG("Process accounting manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    process_accounting_manager->Reset();
    return SUCCESS;
}

uint32 SysCallExportAccountingData(const char* filename, uint32 format) {
    if (!process_accounting_manager) {
        LOG("Process accounting manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!filename) {
        return ERROR_INVALID_PARAMETER;
    }
    
    bool result = false;
    
    switch (format) {
        case 0: // CSV
            result = process_accounting_manager->ExportToCSV(filename);
            break;
        case 1: // JSON
            result = process_accounting_manager->ExportToJSON(filename);
            break;
        case 2: // XML
            result = process_accounting_manager->ExportToXML(filename);
            break;
        default:
            LOG("Unsupported export format: " << format);
            return ERROR_INVALID_PARAMETER;
    }
    
    return result ? SUCCESS : ERROR_OPERATION_FAILED;
}