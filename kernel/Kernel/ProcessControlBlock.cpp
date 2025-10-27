#include "ProcessControlBlock.h"
#include "Global.h"
#include "MemoryManager.h"
#include "Logging.h"
#include "Timer.h"

// Global process manager instance
ProcessManager* process_manager = nullptr;

// ProcessManager implementation
ProcessManager::ProcessManager() {
    current_process = nullptr;
    process_list_head = nullptr;
    next_pid = MIN_PID;
    current_mode = SCHEDULING_MODE_COOPERATIVE;  // Start with cooperative mode
}

ProcessManager::~ProcessManager() {
    // Clean up all processes
    ProcessControlBlock* pcb = process_list_head;
    while (pcb) {
        ProcessControlBlock* next = pcb->next;
        // Free PCB memory if allocated dynamically 
        // (in actual implementation, this would be more complex)
        pcb = next;
    }
    process_list_head = nullptr;
    current_process = nullptr;
}

ProcessControlBlock* ProcessManager::CreateProcess(void* entry_point, const char* name, uint32 priority) {
    // Allocate memory for the new PCB
    ProcessControlBlock* new_pcb = (ProcessControlBlock*)malloc(sizeof(ProcessControlBlock));
    if (!new_pcb) {
        LOG("Failed to allocate memory for new process control block");
        return nullptr;
    }
    
    // Initialize the PCB
    new_pcb->pid = GetNextPID();
    new_pcb->parent_pid = current_process ? current_process->pid : KERNEL_PID;
    new_pcb->uid = 0;  // For now, all processes have UID 0
    new_pcb->gid = 0;  // For now, all processes have GID 0
    new_pcb->pgid = INVALID_PGID;  // Will be set by process group manager
    new_pcb->sid = INVALID_SID;    // Will be set by process group manager
    
    new_pcb->state = PROCESS_STATE_NEW;
    new_pcb->previous_state = PROCESS_STATE_NEW;
    new_pcb->priority = priority;
    
    // Initialize memory management fields
    if (global && global->paging_manager) {
        new_pcb->page_directory = global->paging_manager->CreatePageDirectory();
    } else {
        new_pcb->page_directory = nullptr;
    }
    new_pcb->heap_start = 0;
    new_pcb->heap_end = 0;
    new_pcb->stack_pointer = 0;
    new_pcb->stack_start = 0;
    
    // Initialize CPU state
    new_pcb->registers = nullptr;  // Will be allocated when needed
    new_pcb->instruction_pointer = (uint32)entry_point;
    
    // Initialize scheduling information
    new_pcb->ticks_remaining = g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
    new_pcb->total_cpu_time = 0;
    
    // Initialize timing
    new_pcb->start_time = 0;  // Will be set when process starts running
    new_pcb->last_run_time = 0;
    new_pcb->creation_time = global_timer ? global_timer->GetTickCount() : 0;
    new_pcb->termination_time = 0;
    new_pcb->last_state_change = global_timer ? global_timer->GetTickCount() : 0;
    new_pcb->state_duration = 0;
    
    // Initialize advanced scheduling fields
    new_pcb->time_slice_remaining = g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
    new_pcb->total_cpu_time_used = 0;
    new_pcb->wait_time = 0;
    new_pcb->response_time = 0;
    new_pcb->turnaround_time = 0;
    new_pcb->first_run_time = 0;
    new_pcb->last_preemption_time = 0;
    new_pcb->preemption_count = 0;
    new_pcb->voluntary_yield_count = 0;
    new_pcb->context_switch_count = 0;
    
    // Initialize MLFQ scheduling fields
    new_pcb->mlfq_level = 0;  // Start at highest priority level
    new_pcb->mlfq_time_slice = g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
    new_pcb->mlfq_total_time = 0;
    new_pcb->mlfq_boost_time = global_timer ? (global_timer->GetTickCount() + 
                               (g_kernel_config ? g_kernel_config->mlfq_boost_interval : 1000)) : 0;
    
    // Initialize priority aging fields
    new_pcb->base_priority = priority;
    new_pcb->current_priority = priority;
    new_pcb->priority_boost_count = 0;
    new_pcb->last_priority_boost = global_timer ? global_timer->GetTickCount() : 0;
    
    // Initialize fair-share scheduling fields
    new_pcb->user_id = new_pcb->uid;
    new_pcb->group_id = new_pcb->gid;
    new_pcb->cpu_shares = 1024;  // Default CPU shares (Linux-like)
    new_pcb->cpu_quota_used = 0;
    new_pcb->cpu_quota_period = 100000;  // Default quota period in ticks
    
    // Initialize real-time scheduling fields
    new_pcb->rt_policy = RT_SCHED_FIFO;        // Default real-time policy
    new_pcb->rt_priority = RT_DEFAULT_PRIORITY; // Default real-time priority
    new_pcb->rt_execution_time = 0;            // Not set
    new_pcb->rt_period = 0;                    // Not set (aperiodic)
    new_pcb->rt_deadline = 0;                  // Not set
    new_pcb->rt_release_time = 0;              // Not set
    new_pcb->rt_deadline_misses = 0;           // No missed deadlines yet
    new_pcb->rt_completions = 0;               // No completions yet
    new_pcb->rt_budget = 0;                    // No budget set
    new_pcb->rt_budget_used = 0;               // No budget used yet
    new_pcb->rt_budget_period = 0;             // No budget period set
    new_pcb->rt_is_periodic = false;           // Initially aperiodic
    new_pcb->rt_is_soft_realtime = true;       // Soft real-time by default (misses allowed)
    new_pcb->rt_criticality_level = 0;           // Not critical by default
    new_pcb->rt_jitter_tolerance = 0;          // No jitter tolerance set
    new_pcb->rt_phase_offset = 0;              // No phase offset
    new_pcb->rt_relative_deadline = 0;         // No relative deadline set
    new_pcb->rt_criticality_level = 0;         // Lowest criticality
    new_pcb->rt_importance_factor = 50;        // Medium importance
    new_pcb->rt_resource_requirements = 0;     // No resource requirements
    new_pcb->rt_affinity_mask = 0xFFFFFFFF;    // Run on any CPU
    
    // Initialize synchronization and IPC
    new_pcb->waiting_on_semaphore = nullptr;
    new_pcb->event_flags = nullptr;
    new_pcb->waiting_on_mutex = nullptr;
    new_pcb->waiting_on_event = nullptr;
    new_pcb->message_queue = nullptr;
    new_pcb->opened_files = nullptr;
    
    // Initialize process state management enhancements
    new_pcb->previous_state = PROCESS_STATE_NEW;
    new_pcb->blocking_reason = 0;
    new_pcb->wait_timeout = 0;
    new_pcb->exit_code = 0;
    new_pcb->suspend_count = 0;
    
    // Initialize process name
    if (name) {
        for (int i = 0; i < 31 && name[i] != '\0'; i++) {
            new_pcb->name[i] = name[i];
        }
        new_pcb->name[31] = '\0';
    } else {
        new_pcb->name[0] = '\0';
    }
    
    // Initialize queue links
    new_pcb->next = process_list_head;
    new_pcb->prev = nullptr;
    if (process_list_head) {
        process_list_head->prev = new_pcb;
    }
    process_list_head = new_pcb;
    
    // Initialize flags
    new_pcb->flags = 0;
    
    // Create the main thread for this process if thread manager is available
    if (thread_manager) {
        ThreadAttributes attr;
        attr.stack_size = 4096;  // 4KB default stack
        attr.priority = priority;
        attr.policy = THREAD_SCHED_POLICY_OTHER;
        attr.detached = false;
        attr.stack_addr = nullptr;
        
        ThreadControlBlock* main_thread = thread_manager->CreateThread(
            (struct ProcessControlBlock*)new_pcb, (void*)entry_point, new_pcb->name, &attr);
        
        if (!main_thread) {
            LOG("Failed to create main thread for process PID: " << new_pcb->pid);
            // Clean up the PCB
            DestroyProcess(new_pcb->pid);
            return nullptr;
        }
        
        DLOG("Created main thread TID: " << main_thread->tid << " for process PID: " << new_pcb->pid);
    }
    
    // Set state to ready after successful creation
    TransitionProcessState(new_pcb->pid, PROCESS_STATE_READY);
    
    DLOG("Created process with PID: " << new_pcb->pid << ", name: " << new_pcb->name);
    
    return new_pcb;
}

bool ProcessManager::DestroyProcess(uint32 pid) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to destroy non-existent process with PID: " << pid);
        return false;
    }
    
    // Remove from process list
    if (target->prev) {
        target->prev->next = target->next;
    } else {
        process_list_head = target->next;
    }
    
    if (target->next) {
        target->next->prev = target->prev;
    }
    
    // Free allocated memory
    if (target->registers) {
        free(target->registers);
    }
    
    // If this was the current process, update current_process
    if (current_process == target) {
        current_process = nullptr;
    }
    
    free(target);
    
    DLOG("Destroyed process with PID: " << pid);
    
    return true;
}

bool ProcessManager::TerminateProcess(uint32 pid) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to terminate non-existent process with PID: " << pid);
        return false;
    }
    
    // Update termination time
    if (global_timer) {
        target->termination_time = global_timer->GetTickCount();
    }
    
    // Transition to terminated state
    if (!TransitionProcessState(pid, PROCESS_STATE_TERMINATED)) {
        LOG("Failed to transition process " << pid << " to terminated state");
        return false;
    }
    
    // If the process was running, schedule a new one
    if (current_process == target) {
        current_process = nullptr;
    }
    
    // Perform cleanup for this process
    // For now, just destroy it (in real implementation, there might be more complex cleanup)
    return DestroyProcess(pid);
}

ProcessControlBlock* ProcessManager::GetProcessById(uint32 pid) {
    ProcessControlBlock* current = process_list_head;
    while (current) {
        if (current->pid == pid) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

ProcessControlBlock* ProcessManager::GetCurrentProcess() {
    return current_process;
}

uint32 ProcessManager::GetNextPID() {
    uint32 pid = next_pid++;
    // If we exceed max PID, wrap around (in a real system, there would be more complex logic)
    if (next_pid > MAX_PID) {
        next_pid = MIN_PID;
    }
    return pid;
}

bool ProcessManager::SetProcessState(uint32 pid, ProcessState new_state) {
    // Use the enhanced TransitionProcessState that validates transitions
    return TransitionProcessState(pid, new_state);
}

ProcessState ProcessManager::GetProcessState(uint32 pid) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to get state for non-existent process with PID: " << pid);
        return PROCESS_STATE_TERMINATED; // Return terminated for invalid processes
    }
    return target->state;
}

bool ProcessManager::TransitionProcessState(uint32 pid, ProcessState new_state) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to transition state for non-existent process with PID: " << pid);
        return false;
    }
    
    // Validate state transition (basic validation)
    switch (target->state) {
        case PROCESS_STATE_NEW:
            if (new_state != PROCESS_STATE_READY && new_state != PROCESS_STATE_TERMINATED) {
                LOG("Invalid state transition for process " << pid << ": NEW -> " << new_state);
                return false;
            }
            break;
        case PROCESS_STATE_RUNNING:
            if (new_state != PROCESS_STATE_READY && 
                new_state != PROCESS_STATE_WAITING && 
                new_state != PROCESS_STATE_BLOCKED &&
                new_state != PROCESS_STATE_SUSPENDED &&
                new_state != PROCESS_STATE_TERMINATED) {
                LOG("Invalid state transition for process " << pid << ": RUNNING -> " << new_state);
                return false;
            }
            break;
        case PROCESS_STATE_READY:
            if (new_state != PROCESS_STATE_RUNNING && 
                new_state != PROCESS_STATE_SUSPENDED &&
                new_state != PROCESS_STATE_TERMINATED) {
                LOG("Invalid state transition for process " << pid << ": READY -> " << new_state);
                return false;
            }
            break;
        case PROCESS_STATE_WAITING:
        case PROCESS_STATE_BLOCKED:
            if (new_state != PROCESS_STATE_READY && 
                new_state != PROCESS_STATE_SUSPENDED &&
                new_state != PROCESS_STATE_TERMINATED) {
                LOG("Invalid state transition for process " << pid << ": " << target->state << " -> " << new_state);
                return false;
            }
            break;
        case PROCESS_STATE_SUSPENDED:
            if (new_state != PROCESS_STATE_READY && 
                new_state != PROCESS_STATE_WAITING && 
                new_state != PROCESS_STATE_BLOCKED &&
                new_state != PROCESS_STATE_TERMINATED) {
                LOG("Invalid state transition for process " << pid << ": SUSPENDED -> " << new_state);
                return false;
            }
            break;
        case PROCESS_STATE_ZOMBIE:
        case PROCESS_STATE_TERMINATED:
            // Cannot transition from zombie or terminated
            LOG("Cannot transition from terminated or zombie state for process " << pid);
            return false;
    }
    
    // Record the transition
    target->previous_state = target->state;
    target->state = new_state;
    
    // Update timing information
    if (global_timer) {
        uint32 current_time = global_timer->GetTickCount();
        target->last_state_change = current_time;
        target->state_duration = 0; // Reset duration counter
    }
    
    DLOG("Process PID " << pid << " transitioned from " << target->previous_state << " to " << new_state);
    return true;
}

ProcessState ProcessManager::GetPreviousState(uint32 pid) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to get previous state for non-existent process with PID: " << pid);
        return PROCESS_STATE_TERMINATED;
    }
    return target->previous_state;
}

uint32 ProcessManager::GetStateDuration(uint32 pid) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target || !global_timer) {
        return 0;
    }
    
    uint32 current_time = global_timer->GetTickCount();
    return current_time - target->last_state_change;
}

uint32 ProcessManager::GetBlockingReason(uint32 pid) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to get blocking reason for non-existent process with PID: " << pid);
        return 0;
    }
    return target->blocking_reason;
}

bool ProcessManager::SetBlockingReason(uint32 pid, uint32 reason) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to set blocking reason for non-existent process with PID: " << pid);
        return false;
    }
    target->blocking_reason = reason;
    return true;
}

bool ProcessManager::SuspendProcess(uint32 pid) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to suspend non-existent process with PID: " << pid);
        return false;
    }
    
    // Record original state if not already suspended
    if (target->state != PROCESS_STATE_SUSPENDED) {
        target->previous_state = target->state;
    }
    
    // Increment suspend counter for nested suspends
    target->suspend_count++;
    
    // Only change state if process is not already suspended
    if (target->state != PROCESS_STATE_SUSPENDED) {
        ProcessState original_state = target->state;
        if (TransitionProcessState(pid, PROCESS_STATE_SUSPENDED)) {
            DLOG("Process PID " << pid << " suspended, was in state " << original_state);
            return true;
        }
        return false;
    }
    
    DLOG("Process PID " << pid << " was already suspended, incrementing suspend count to " << target->suspend_count);
    return true;
}

bool ProcessManager::ResumeProcess(uint32 pid) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to resume non-existent process with PID: " << pid);
        return false;
    }
    
    // Decrement suspend counter
    if (target->suspend_count > 0) {
        target->suspend_count--;
    } else {
        LOG("Warning: Attempted to resume unsuspended process " << pid);
        return false; // Process was not suspended
    }
    
    // Only change state if all suspend calls have been undone
    if (target->suspend_count == 0 && target->state == PROCESS_STATE_SUSPENDED) {
        // Transition back to the previous state
        if (TransitionProcessState(pid, target->previous_state)) {
            DLOG("Process PID " << pid << " resumed to state " << target->previous_state);
            return true;
        }
        return false;
    } else if (target->suspend_count > 0) {
        DLOG("Process PID " << pid << " still suspended, suspend count: " << target->suspend_count);
        return true; // Successfully decremented, but still suspended
    }
    
    DLOG("Process PID " << pid << " was not in suspended state");
    return false; // Process was not suspended
}

bool ProcessManager::BlockProcess(uint32 pid, uint32 reason) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to block non-existent process with PID: " << pid);
        return false;
    }
    
    // Save the original state before blocking
    if (target->state != PROCESS_STATE_SUSPENDED && 
        target->state != PROCESS_STATE_BLOCKED && 
        target->state != PROCESS_STATE_ZOMBIE && 
        target->state != PROCESS_STATE_TERMINATED) {
        target->previous_state = target->state;
    }
    
    target->blocking_reason = reason;
    
    if (TransitionProcessState(pid, PROCESS_STATE_BLOCKED)) {
        DLOG("Process PID " << pid << " blocked with reason " << reason);
        return true;
    }
    return false;
}

bool ProcessManager::UnblockProcess(uint32 pid) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to unblock non-existent process with PID: " << pid);
        return false;
    }
    
    if (target->state == PROCESS_STATE_BLOCKED) {
        // Transition to ready state when unblocked
        if (TransitionProcessState(pid, PROCESS_STATE_READY)) {
            DLOG("Process PID " << pid << " unblocked");
            return true;
        }
    }
    
    return false;
}

bool ProcessManager::WakeProcess(uint32 pid) {
    return UnblockProcess(pid);  // For now, wake and unblock are the same
}

bool ProcessManager::SetProcessExitCode(uint32 pid, uint32 exit_code) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to set exit code for non-existent process with PID: " << pid);
        return false;
    }
    target->exit_code = exit_code;
    return true;
}

uint32 ProcessManager::GetProcessExitCode(uint32 pid) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to get exit code for non-existent process with PID: " << pid);
        return 0xFFFFFFFF; // Error code
    }
    return target->exit_code;
}

const char* ProcessManager::GetProcessStateName(ProcessState state) {
    switch (state) {
        case PROCESS_STATE_NEW: return "NEW";
        case PROCESS_STATE_READY: return "READY";
        case PROCESS_STATE_RUNNING: return "RUNNING";
        case PROCESS_STATE_WAITING: return "WAITING";
        case PROCESS_STATE_BLOCKED: return "BLOCKED";
        case PROCESS_STATE_SUSPENDED: return "SUSPENDED";
        case PROCESS_STATE_ZOMBIE: return "ZOMBIE";
        case PROCESS_STATE_TERMINATED: return "TERMINATED";
        default: return "INVALID";
    }
}

const char* ProcessManager::GetSchedulingModeName(SchedulingMode mode) {
    switch (mode) {
        case SCHEDULING_MODE_COOPERATIVE: return "COOPERATIVE";
        case SCHEDULING_MODE_PREEMPTIVE: return "PREEMPTIVE";
        case SCHEDULING_MODE_ROUND_ROBIN: return "ROUND_ROBIN";
        case SCHEDULING_MODE_PRIORITY: return "PRIORITY";
        case SCHEDULING_MODE_MLFQ: return "MLFQ";
        case SCHEDULING_MODE_FAIR_SHARE: return "FAIR_SHARE";
        case SCHEDULING_MODE_REALTIME: return "REALTIME";
        default: return "UNKNOWN";
    }
}

void ProcessManager::PrintProcessStateHistory(uint32 pid) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to print state history for non-existent process with PID: " << pid);
        return;
    }
    
    LOG("Process PID " << pid << " (" << target->name << ") state history:");
    LOG("  Current State: " << GetProcessStateName(target->state));
    LOG("  Previous State: " << GetProcessStateName(target->previous_state));
    LOG("  State Duration: " << GetStateDuration(pid) << " ticks");
    LOG("  Creation Time: " << target->creation_time);
    LOG("  Last State Change: " << target->last_state_change);
    LOG("  Blocking Reason: " << target->blocking_reason);
    LOG("  Suspend Count: " << target->suspend_count);
    LOG("  Exit Code: " << target->exit_code);
}

ProcessControlBlock* ProcessManager::ScheduleNextProcess() {
    // Dispatch to appropriate scheduling algorithm based on current mode
    switch (current_mode) {
        case SCHEDULING_MODE_ROUND_ROBIN:
            return ScheduleNextProcessRR();
        case SCHEDULING_MODE_MLFQ:
            return ScheduleNextProcessMLFQ();
        case SCHEDULING_MODE_FAIR_SHARE:
            return ScheduleNextProcessFairShare();
        case SCHEDULING_MODE_REALTIME:
            return ScheduleNextProcessRealtime();
        case SCHEDULING_MODE_PRIORITY:
        default:
            // Fall through to standard priority-based scheduling
            break;
    }
    
    // Standard priority-based scheduling
    ProcessControlBlock* best_candidate = nullptr;
    uint32 highest_priority = 0xFFFFFFFF;  // Lower number = higher priority
    
    ProcessControlBlock* current = process_list_head;
    while (current) {
        // Check if the process is in a schedulable state (ready to run)
        if (current->state == PROCESS_STATE_READY || 
            current->state == PROCESS_STATE_NEW) {  // NEW processes should transition to READY
            // In priority-based scheduling, lower priority number means higher priority
            if (current->current_priority < highest_priority) {
                highest_priority = current->current_priority;
                best_candidate = current;
            } else if (current->current_priority == highest_priority) {
                // Tie-breaker: use creation time (older processes get preference)
                if (best_candidate && current->creation_time < best_candidate->creation_time) {
                    best_candidate = current;
                }
            }
        }
        current = current->next;
    }
    
    // If no ready processes, return nullptr
    return best_candidate;
}

bool ProcessManager::AddToReadyQueue(ProcessControlBlock* pcb) {
    if (!pcb) {
        return false;
    }
    
    // In our simple implementation, this is just setting the state
    return SetProcessState(pcb->pid, PROCESS_STATE_READY);
}

ProcessControlBlock* ProcessManager::RemoveFromReadyQueue() {
    // Find the highest priority process in the ready state and return it
    ProcessControlBlock* next_process = ScheduleNextProcess();
    if (next_process) {
        SetProcessState(next_process->pid, PROCESS_STATE_RUNNING);
        return next_process;
    }
    return nullptr;
}

bool ProcessManager::YieldCurrentProcess() {
    if (!current_process) {
        return false;
    }
    
    // Only allow yielding from RUNNING state
    if (current_process->state != PROCESS_STATE_RUNNING) {
        DLOG("Process " << current_process->pid << " attempted to yield but was not in RUNNING state");
        return false;
    }
    
    // In cooperative mode, always yield to another process
    // In preemptive mode, the scheduler handles this via timer interrupts
    if (current_mode == SCHEDULING_MODE_COOPERATIVE) {
        // Set the current process back to ready state
        if (!TransitionProcessState(current_process->pid, PROCESS_STATE_READY)) {
            return false;
        }
        
        // Update voluntary yield count and context switch statistics
        current_process->voluntary_yield_count++;
        current_process->context_switch_count++;
        
        // Find next process to run
        ProcessControlBlock* next_process = ScheduleNextProcess();
        if (next_process) {
            current_process = next_process;
            if (!TransitionProcessState(current_process->pid, PROCESS_STATE_RUNNING)) {
                return false;
            }
            
            // Reset time slice for the new process based on scheduling mode
            switch (current_mode) {
                case SCHEDULING_MODE_MLFQ:
                    current_process->time_slice_remaining = current_process->mlfq_time_slice;
                    break;
                case SCHEDULING_MODE_ROUND_ROBIN:
                    current_process->time_slice_remaining = 
                        g_kernel_config ? g_kernel_config->round_robin_quantum : 10;
                    break;
                case SCHEDULING_MODE_REALTIME:
                    // Real-time processes may have different time slices
                    if (current_process->current_priority < 10) {
                        // High-priority real-time processes get shorter time slices
                        current_process->time_slice_remaining = 5;  // 5ms
                    } else {
                        current_process->time_slice_remaining = 
                            g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
                    }
                    break;
                case SCHEDULING_MODE_FAIR_SHARE:
                case SCHEDULING_MODE_PRIORITY:
                case SCHEDULING_MODE_PREEMPTIVE:
                case SCHEDULING_MODE_COOPERATIVE:
                default:
                    current_process->time_slice_remaining = 
                        g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
                    break;
            }
            
            // Record first run time if this is the first time running
            if (current_process->first_run_time == 0) {
                current_process->first_run_time = global_timer ? global_timer->GetTickCount() : 0;
                current_process->response_time = current_process->first_run_time - 
                                                  current_process->creation_time;
            }
            
            // Update last run time
            current_process->last_run_time = global_timer ? global_timer->GetTickCount() : 0;
            
            // Switch to the new process's page directory for memory protection
            if (global && global->paging_manager && current_process->page_directory) {
                global->paging_manager->SwitchPageDirectory(current_process->page_directory);
            }
            
            return true;
        }
        
        // If no other process to run, keep current process running
        if (!TransitionProcessState(current_process->pid, PROCESS_STATE_RUNNING)) {
            return false;
        }
        return true;
    } else {
        // In preemptive mode, we still allow explicit yields but they're not required
        // Set the current process back to ready state
        if (!TransitionProcessState(current_process->pid, PROCESS_STATE_READY)) {
            return false;
        }
        
        // Update voluntary yield count and context switch statistics
        current_process->voluntary_yield_count++;
        current_process->context_switch_count++;
        
        // Find next process to run
        ProcessControlBlock* next_process = ScheduleNextProcess();
        if (next_process) {
            current_process = next_process;
            if (!TransitionProcessState(current_process->pid, PROCESS_STATE_RUNNING)) {
                return false;
            }
            
            // Reset time slice for the new process based on scheduling mode
            switch (current_mode) {
                case SCHEDULING_MODE_MLFQ:
                    current_process->time_slice_remaining = current_process->mlfq_time_slice;
                    break;
                case SCHEDULING_MODE_ROUND_ROBIN:
                    current_process->time_slice_remaining = 
                        g_kernel_config ? g_kernel_config->round_robin_quantum : 10;
                    break;
                case SCHEDULING_MODE_REALTIME:
                    // Real-time processes may have different time slices
                    if (current_process->current_priority < 10) {
                        // High-priority real-time processes get shorter time slices
                        current_process->time_slice_remaining = 5;  // 5ms
                    } else {
                        current_process->time_slice_remaining = 
                            g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
                    }
                    break;
                case SCHEDULING_MODE_FAIR_SHARE:
                case SCHEDULING_MODE_PRIORITY:
                case SCHEDULING_MODE_PREEMPTIVE:
                default:
                    current_process->time_slice_remaining = 
                        g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
                    break;
            }
            
            // Record first run time if this is the first time running
            if (current_process->first_run_time == 0) {
                current_process->first_run_time = global_timer ? global_timer->GetTickCount() : 0;
                current_process->response_time = current_process->first_run_time - 
                                                  current_process->creation_time;
            }
            
            // Update last run time
            current_process->last_run_time = global_timer ? global_timer->GetTickCount() : 0;
            
            // Switch to the new process's page directory for memory protection
            if (global && global->paging_manager && current_process->page_directory) {
                global->paging_manager->SwitchPageDirectory(current_process->page_directory);
            }
            
            return true;
        }
        
        // If no other process to run, keep current process running
        if (!TransitionProcessState(current_process->pid, PROCESS_STATE_RUNNING)) {
            return false;
        }
        return true;
    }
}

bool ProcessManager::SleepCurrentProcess(uint32 sleep_ticks) {
    if (!current_process) {
        return false;
    }
    
    // In a real implementation, we'd need to track when to wake up this process
    // For now, just set it to waiting state
    if (!TransitionProcessState(current_process->pid, PROCESS_STATE_WAITING)) {
        return false;
    }
    
    // Set the timeout for when the process should wake up
    current_process->wait_timeout = (global_timer ? global_timer->GetTickCount() : 0) + sleep_ticks;
    
    // Yield to another process
    return YieldCurrentProcess();
}

uint32 ProcessManager::GetProcessCount() {
    uint32 count = 0;
    ProcessControlBlock* current = process_list_head;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

void ProcessManager::PrintProcessList() {
    LOG("Process List:");
    ProcessControlBlock* current = process_list_head;
    while (current) {
        LOG("  PID: " << current->pid << 
            ", Name: " << current->name << 
            ", State: " << GetProcessStateName(current->state) << 
            ", Priority: " << current->priority <<
            ", Suspend Count: " << current->suspend_count);
        current = current->next;
    }
    
    LOG("Total processes: " << GetProcessCount());
}

void ProcessManager::SetSchedulingMode(SchedulingMode mode) {
    DLOG("Setting scheduling mode from " << current_mode << " to " << mode);
    current_mode = mode;
}

SchedulingMode ProcessManager::GetSchedulingMode() {
    return current_mode;
}

void ProcessManager::Schedule() {
    // Update time-based scheduling parameters
    AgeProcessPriorities();
    
    // Check if we need to schedule based on current mode
    switch (current_mode) {
        case SCHEDULING_MODE_PREEMPTIVE:
        case SCHEDULING_MODE_ROUND_ROBIN:
        case SCHEDULING_MODE_MLFQ:
        case SCHEDULING_MODE_FAIR_SHARE:
        case SCHEDULING_MODE_REALTIME:
        case SCHEDULING_MODE_PRIORITY:
            // In all preemptive modes, we force a context switch when quantum expires
            if (current_process && current_process->time_slice_remaining > 0) {
                current_process->time_slice_remaining--;
                current_process->ticks_remaining--;  // Legacy field for compatibility
            }
            
            // If no current process or time quantum expired, schedule a new one
            if (!current_process || current_process->time_slice_remaining <= 0) {
                ProcessControlBlock* next_process = ScheduleNextProcess();
                if (next_process) {
                    if (current_process && current_process->state == PROCESS_STATE_RUNNING) {
                        // Save the current process state - only if it's still running
                        // If it's in any other state, we don't want to change it back to ready
                        if (current_process->state == PROCESS_STATE_RUNNING) {
                            // Update process timing information
                            current_process->total_cpu_time_used += 
                                (g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10) - 
                                current_process->time_slice_remaining;
                            current_process->last_preemption_time = 
                                global_timer ? global_timer->GetTickCount() : 0;
                            current_process->preemption_count++;
                            
                            TransitionProcessState(current_process->pid, PROCESS_STATE_READY);
                        }
                    }
                    
                    // Update context switch statistics
                    if (current_process) {
                        current_process->context_switch_count++;
                    }
                    
                    current_process = next_process;
                    TransitionProcessState(current_process->pid, PROCESS_STATE_RUNNING);
                    
                    // Reset quantum for the scheduled process based on scheduling mode
                    switch (current_mode) {
                        case SCHEDULING_MODE_MLFQ:
                            current_process->time_slice_remaining = current_process->mlfq_time_slice;
                            break;
                        case SCHEDULING_MODE_ROUND_ROBIN:
                            current_process->time_slice_remaining = 
                                g_kernel_config ? g_kernel_config->round_robin_quantum : 10;
                            break;
                        case SCHEDULING_MODE_REALTIME:
                            // Real-time processes may have different time slices
                            if (current_process->current_priority < 10) {
                                // High-priority real-time processes get shorter time slices
                                current_process->time_slice_remaining = 5;  // 5ms
                            } else {
                                current_process->time_slice_remaining = 
                                    g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
                            }
                            break;
                        case SCHEDULING_MODE_FAIR_SHARE:
                        case SCHEDULING_MODE_PRIORITY:
                        case SCHEDULING_MODE_PREEMPTIVE:
                        default:
                            current_process->time_slice_remaining = 
                                g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
                            break;
                    }
                    
                    // Record first run time if this is the first time running
                    if (current_process->first_run_time == 0) {
                        current_process->first_run_time = global_timer ? global_timer->GetTickCount() : 0;
                        current_process->response_time = current_process->first_run_time - 
                                                          current_process->creation_time;
                    }
                    
                    // Update last run time
                    current_process->last_run_time = global_timer ? global_timer->GetTickCount() : 0;
                    
                    // Switch to the new process's page directory for memory protection
                    if (global && global->paging_manager && current_process->page_directory) {
                        global->paging_manager->SwitchPageDirectory(current_process->page_directory);
                    }
                    
                    // Log context switch for debugging
                    DLOG(GetSchedulingModeName(current_mode) << " context switch to PID " << current_process->pid 
                         << ", name: " << current_process->name << ", priority: " << current_process->current_priority);
                }
            }
            break;
            
        case SCHEDULING_MODE_COOPERATIVE:
        default:
            // In cooperative mode, only schedule when current process yields
            // The actual scheduling happens in YieldCurrentProcess()
            
            // Still update timing if a process is running
            if (current_process) {
                current_process->total_cpu_time++;
                current_process->total_cpu_time_used++;
            }
            break;
    }
    
    // Handle waking up sleep processes if their timeout has expired
    ProcessControlBlock* current = process_list_head;
    while (current) {
        if (current->state == PROCESS_STATE_WAITING && 
            global_timer && 
            current->wait_timeout > 0 && 
            global_timer->GetTickCount() >= current->wait_timeout) {
            // Wake up the waiting process
            TransitionProcessState(current->pid, PROCESS_STATE_READY);
            current->wait_timeout = 0;  // Reset timeout
            
            // Update wait time statistics
            current->wait_time += global_timer->GetTickCount() - current->last_state_change;
            
            DLOG("Process " << current->pid << " (" << current->name << ") woken up after timeout");
        }
        current = current->next;
    }
}