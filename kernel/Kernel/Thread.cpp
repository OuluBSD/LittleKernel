#include "Thread.h"
#include "Global.h"
#include "MemoryManager.h"
#include "Logging.h"
#include "Timer.h"

// Global thread manager instance
ThreadManager* thread_manager = nullptr;

// ThreadManager implementation
ThreadManager::ThreadManager() {
    current_thread = nullptr;
    thread_list_head = nullptr;
    next_tid = MIN_TID;
    current_mode = THREAD_SCHEDULING_MODE_COOPERATIVE;  // Start with cooperative mode
}

ThreadManager::~ThreadManager() {
    // Clean up all threads
    ThreadControlBlock* tcb = thread_list_head;
    while (tcb) {
        ThreadControlBlock* next = tcb->next;
        // Free TCB memory if allocated dynamically 
        // (in actual implementation, this would be more complex)
        tcb = next;
    }
    thread_list_head = nullptr;
    current_thread = nullptr;
}

ThreadControlBlock* ThreadManager::CreateThread(ProcessControlBlock* parent_process, 
                                                void* entry_point, 
                                                const char* name, 
                                                const ThreadAttributes* attr) {
    // Allocate memory for the new TCB
    ThreadControlBlock* new_tcb = (ThreadControlBlock*)malloc(sizeof(ThreadControlBlock));
    if (!new_tcb) {
        LOG("Failed to allocate memory for new thread control block");
        return nullptr;
    }
    
    // Initialize the TCB
    new_tcb->tid = GetNextTID();
    new_tcb->pid = parent_process ? parent_process->pid : INVALID_PID;
    new_tcb->parent_tid = current_thread ? current_thread->tid : MAIN_THREAD_TID;
    new_tcb->parent_process = parent_process;
    
    new_tcb->state = THREAD_STATE_NEW;
    new_tcb->previous_state = THREAD_STATE_NEW;
    new_tcb->priority = attr ? attr->priority : DEFAULT_THREAD_ATTRIBUTES.priority;
    new_tcb->sched_policy = attr ? attr->policy : DEFAULT_THREAD_ATTRIBUTES.policy;
    
    // Initialize memory management fields
    uint32 stack_size = attr ? attr->stack_size : DEFAULT_THREAD_ATTRIBUTES.stack_size;
    new_tcb->stack_size = stack_size;
    
    // Allocate stack memory for the thread
    new_tcb->stack_start = (uint32)malloc(stack_size);
    if (!new_tcb->stack_start) {
        LOG("Failed to allocate stack memory for thread TID: " << new_tcb->tid);
        free(new_tcb);
        return nullptr;
    }
    new_tcb->stack_pointer = (uint32*)(new_tcb->stack_start + stack_size);
    
    // Initialize CPU state
    new_tcb->registers = nullptr;  // Will be allocated when needed
    new_tcb->instruction_pointer = (uint32)entry_point;
    new_tcb->base_pointer = 0;
    
    // Initialize scheduling information
    new_tcb->ticks_remaining = g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
    new_tcb->total_cpu_time = 0;
    
    // Initialize timing
    new_tcb->start_time = 0;  // Will be set when thread starts running
    new_tcb->last_run_time = 0;
    new_tcb->creation_time = global_timer ? global_timer->GetTickCount() : 0;
    new_tcb->termination_time = 0;
    new_tcb->last_state_change = global_timer ? global_timer->GetTickCount() : 0;
    new_tcb->state_duration = 0;
    
    // Initialize synchronization
    new_tcb->waiting_on_semaphore = nullptr;
    new_tcb->event_flags = nullptr;
    new_tcb->waiting_on_mutex = nullptr;
    new_tcb->waiting_on_event = nullptr;
    new_tcb->blocking_reason = 0;
    new_tcb->wait_timeout = 0;
    new_tcb->suspend_count = 0;
    
    // Initialize thread-specific data
    new_tcb->thread_local_storage = nullptr;
    new_tcb->tls_size = 0;
    
    // Initialize thread name
    if (name) {
        for (int i = 0; i < 31 && name[i] != '\0'; i++) {
            new_tcb->name[i] = name[i];
        }
        new_tcb->name[31] = '\0';
    } else {
        snprintf(new_tcb->name, sizeof(new_tcb->name), "Thread-%u", new_tcb->tid);
    }
    
    // Initialize queue links
    new_tcb->next = thread_list_head;
    new_tcb->prev = nullptr;
    if (thread_list_head) {
        thread_list_head->prev = new_tcb;
    }
    thread_list_head = new_tcb;
    
    // Initialize flags
    new_tcb->flags = 0;
    
    // Set state to ready after successful creation
    TransitionThreadState(new_tcb->tid, THREAD_STATE_READY);
    
    DLOG("Created thread with TID: " << new_tcb->tid << ", name: " << new_tcb->name 
         << ", PID: " << new_tcb->pid);
    
    return new_tcb;
}

bool ThreadManager::DestroyThread(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to destroy non-existent thread with TID: " << tid);
        return false;
    }
    
    // Remove from thread list
    if (target->prev) {
        target->prev->next = target->next;
    } else {
        thread_list_head = target->next;
    }
    
    if (target->next) {
        target->next->prev = target->prev;
    }
    
    // Free allocated memory
    if (target->registers) {
        free(target->registers);
    }
    
    if (target->stack_start) {
        free((void*)target->stack_start);
    }
    
    if (target->thread_local_storage) {
        free(target->thread_local_storage);
    }
    
    // If this was the current thread, update current_thread
    if (current_thread == target) {
        current_thread = nullptr;
    }
    
    free(target);
    
    DLOG("Destroyed thread with TID: " << tid);
    
    return true;
}

bool ThreadManager::TerminateThread(uint32 tid, uint32 exit_code) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to terminate non-existent thread with TID: " << tid);
        return false;
    }
    
    // Update termination time
    if (global_timer) {
        target->termination_time = global_timer->GetTickCount();
    }
    
    // Set exit code
    target->flags |= exit_code;  // Simple way to store exit code in flags
    
    // Transition to terminated state
    if (!TransitionThreadState(tid, THREAD_STATE_TERMINATED)) {
        LOG("Failed to transition thread " << tid << " to terminated state");
        return false;
    }
    
    // If the thread was running, update current_thread
    if (current_thread == target) {
        current_thread = nullptr;
    }
    
    // Perform cleanup for this thread
    // For now, just destroy it (in real implementation, there might be more complex cleanup)
    return DestroyThread(tid);
}

ThreadControlBlock* ThreadManager::GetThreadById(uint32 tid) {
    ThreadControlBlock* current = thread_list_head;
    while (current) {
        if (current->tid == tid) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

ThreadControlBlock* ThreadManager::GetCurrentThread() {
    return current_thread;
}

uint32 ThreadManager::GetNextTID() {
    uint32 tid = next_tid++;
    // If we exceed max TID, wrap around (in a real system, there would be more complex logic)
    if (next_tid > MAX_TID) {
        next_tid = MIN_TID;
    }
    return tid;
}

bool ThreadManager::SetThreadState(uint32 tid, ThreadState new_state) {
    // Use the enhanced TransitionThreadState that validates transitions
    return TransitionThreadState(tid, new_state);
}

bool ThreadManager::TransitionThreadState(uint32 tid, ThreadState new_state) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to transition state for non-existent thread with TID: " << tid);
        return false;
    }
    
    // Validate state transition (basic validation)
    switch (target->state) {
        case THREAD_STATE_NEW:
            if (new_state != THREAD_STATE_READY && new_state != THREAD_STATE_TERMINATED) {
                LOG("Invalid state transition for thread " << tid << ": NEW -> " << new_state);
                return false;
            }
            break;
        case THREAD_STATE_RUNNING:
            if (new_state != THREAD_STATE_READY && 
                new_state != THREAD_STATE_WAITING && 
                new_state != THREAD_STATE_BLOCKED &&
                new_state != THREAD_STATE_SUSPENDED &&
                new_state != THREAD_STATE_TERMINATED) {
                LOG("Invalid state transition for thread " << tid << ": RUNNING -> " << new_state);
                return false;
            }
            break;
        case THREAD_STATE_READY:
            if (new_state != THREAD_STATE_RUNNING && 
                new_state != THREAD_STATE_SUSPENDED &&
                new_state != THREAD_STATE_TERMINATED) {
                LOG("Invalid state transition for thread " << tid << ": READY -> " << new_state);
                return false;
            }
            break;
        case THREAD_STATE_WAITING:
        case THREAD_STATE_BLOCKED:
            if (new_state != THREAD_STATE_READY && 
                new_state != THREAD_STATE_SUSPENDED &&
                new_state != THREAD_STATE_TERMINATED) {
                LOG("Invalid state transition for thread " << tid << ": " << target->state << " -> " << new_state);
                return false;
            }
            break;
        case THREAD_STATE_SUSPENDED:
            if (new_state != THREAD_STATE_READY && 
                new_state != THREAD_STATE_WAITING && 
                new_state != THREAD_STATE_BLOCKED &&
                new_state != THREAD_STATE_TERMINATED) {
                LOG("Invalid state transition for thread " << tid << ": SUSPENDED -> " << new_state);
                return false;
            }
            break;
        case THREAD_STATE_TERMINATED:
            // Cannot transition from terminated
            LOG("Cannot transition from terminated state for thread " << tid);
            return false;
    }
    
    // Record the transition
    target->previous_state = target->state;
    target->state = new_state;
    
    // Update timing information
    if (global_timer) {
        uint32_t current_time = global_timer->GetTickCount();
        target->last_state_change = current_time;
        target->state_duration = 0; // Reset duration counter
    }
    
    DLOG("Thread TID " << tid << " transitioned from " << GetThreadStateName(target->previous_state) 
         << " to " << GetThreadStateName(new_state));
    return true;
}

ThreadState ThreadManager::GetThreadState(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to get state for non-existent thread with TID: " << tid);
        return THREAD_STATE_TERMINATED; // Return terminated for invalid threads
    }
    return target->state;
}

ThreadState ThreadManager::GetPreviousState(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to get previous state for non-existent thread with TID: " << tid);
        return THREAD_STATE_TERMINATED;
    }
    return target->previous_state;
}

uint32 ThreadManager::GetStateDuration(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target || !global_timer) {
        return 0;
    }
    
    uint32_t current_time = global_timer->GetTickCount();
    return current_time - target->last_state_change;
}

uint32 ThreadManager::GetBlockingReason(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to get blocking reason for non-existent thread with TID: " << tid);
        return 0;
    }
    return target->blocking_reason;
}

bool ThreadManager::SetBlockingReason(uint32 tid, uint32 reason) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to set blocking reason for non-existent thread with TID: " << tid);
        return false;
    }
    target->blocking_reason = reason;
    return true;
}

ThreadControlBlock* ThreadManager::ScheduleNextThread() {
    ThreadControlBlock* best_candidate = nullptr;
    uint32 highest_priority = 0xFFFFFFFF;  // Lower number = higher priority
    
    ThreadControlBlock* current = thread_list_head;
    while (current) {
        // Check if the thread is in a schedulable state (ready to run)
        if (current->state == THREAD_STATE_READY || 
            current->state == THREAD_STATE_NEW) {  // NEW threads should transition to READY
            // In priority-based scheduling, lower priority number means higher priority
            if (current->priority < highest_priority) {
                highest_priority = current->priority;
                best_candidate = current;
            }
        }
        current = current->next;
    }
    
    // If no ready threads, return nullptr
    return best_candidate;
}

bool ThreadManager::AddToReadyQueue(ThreadControlBlock* tcb) {
    if (!tcb) {
        return false;
    }
    
    // In our simple implementation, this is just setting the state
    return SetThreadState(tcb->tid, THREAD_STATE_READY);
}

ThreadControlBlock* ThreadManager::RemoveFromReadyQueue() {
    // Find the highest priority thread in the ready state and return it
    ThreadControlBlock* next_thread = ScheduleNextThread();
    if (next_thread) {
        SetThreadState(next_thread->tid, THREAD_STATE_RUNNING);
        return next_thread;
    }
    return nullptr;
}

bool ThreadManager::YieldCurrentThread() {
    if (!current_thread) {
        return false;
    }
    
    // Only allow yielding from RUNNING state
    if (current_thread->state != THREAD_STATE_RUNNING) {
        DLOG("Thread " << current_thread->tid << " attempted to yield but was not in RUNNING state");
        return false;
    }
    
    // In cooperative mode, always yield to another thread
    // In preemptive mode, the scheduler handles this via timer interrupts
    if (current_mode == THREAD_SCHEDULING_MODE_COOPERATIVE) {
        // Set the current thread back to ready state
        if (!TransitionThreadState(current_thread->tid, THREAD_STATE_READY)) {
            return false;
        }
        
        // Find next thread to run
        ThreadControlBlock* next_thread = ScheduleNextThread();
        if (next_thread) {
            current_thread = next_thread;
            if (!TransitionThreadState(current_thread->tid, THREAD_STATE_RUNNING)) {
                return false;
            }
            
            return true;
        }
        
        // If no other thread to run, keep current thread running
        if (!TransitionThreadState(current_thread->tid, THREAD_STATE_RUNNING)) {
            return false;
        }
        return true;
    } else {
        // In preemptive mode, we still allow explicit yields but they're not required
        // Set the current thread back to ready state
        if (!TransitionThreadState(current_thread->tid, THREAD_STATE_READY)) {
            return false;
        }
        
        // Find next thread to run
        ThreadControlBlock* next_thread = ScheduleNextThread();
        if (next_thread) {
            current_thread = next_thread;
            if (!TransitionThreadState(current_thread->tid, THREAD_STATE_RUNNING)) {
                return false;
            }
            
            return true;
        }
        
        // If no other thread to run, keep current thread running
        if (!TransitionThreadState(current_thread->tid, THREAD_STATE_RUNNING)) {
            return false;
        }
        return true;
    }
}

bool ThreadManager::SleepCurrentThread(uint32 sleep_ticks) {
    if (!current_thread) {
        return false;
    }
    
    // In a real implementation, we'd need to track when to wake up this thread
    // For now, just set it to waiting state
    if (!TransitionThreadState(current_thread->tid, THREAD_STATE_WAITING)) {
        return false;
    }
    
    // Set the timeout for when the thread should wake up
    current_thread->wait_timeout = (global_timer ? global_timer->GetTickCount() : 0) + sleep_ticks;
    
    // Yield to another thread
    return YieldCurrentThread();
}

void ThreadManager::SetSchedulingMode(ThreadSchedulingMode mode) {
    DLOG("Setting thread scheduling mode from " << current_mode << " to " << mode);
    current_mode = mode;
}

ThreadSchedulingMode ThreadManager::GetSchedulingMode() {
    return current_mode;
}

void ThreadManager::Schedule() {
    // Check if we need to schedule based on current mode
    if (current_mode == THREAD_SCHEDULING_MODE_PREEMPTIVE || current_mode == THREAD_SCHEDULING_MODE_ROUND_ROBIN) {
        // In both preemptive and round-robin modes, we force a context switch when quantum expires
        if (current_thread && current_thread->ticks_remaining > 0) {
            current_thread->ticks_remaining--;
        }
        
        // If no current thread or time quantum expired, schedule a new one
        if (!current_thread || current_thread->ticks_remaining <= 0) {
            ThreadControlBlock* next_thread = ScheduleNextThread();
            if (next_thread) {
                if (current_thread && current_thread->state == THREAD_STATE_RUNNING) {
                    // Save the current thread state
                    if (current_thread->state == THREAD_STATE_RUNNING) {
                        TransitionThreadState(current_thread->tid, THREAD_STATE_READY);
                    }
                }
                
                current_thread = next_thread;
                TransitionThreadState(current_thread->tid, THREAD_STATE_RUNNING);
                
                // Reset quantum for the scheduled thread
                current_thread->ticks_remaining = g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
                
                // Log context switch for debugging
                if (current_mode == THREAD_SCHEDULING_MODE_PREEMPTIVE) {
                    DLOG("Preemptive context switch to TID " << current_thread->tid 
                         << ", name: " << current_thread->name << " (state: " << GetThreadStateName(current_thread->state) << ")");
                } else {  // Round-robin
                    DLOG("Round-robin context switch to TID " << current_thread->tid 
                         << ", name: " << current_thread->name << " (state: " << GetThreadStateName(current_thread->state) << ")");
                }
            }
        }
    } else {  // Cooperative mode
        // In cooperative mode, only schedule when current thread yields
        // The actual scheduling happens in YieldCurrentThread()
        
        // Still update timing if a thread is running
        if (current_thread) {
            current_thread->total_cpu_time++;
        }
    }
    
    // Handle waking up sleep threads if their timeout has expired
    ThreadControlBlock* current = thread_list_head;
    while (current) {
        if (current->state == THREAD_STATE_WAITING && 
            global_timer && 
            current->wait_timeout > 0 && 
            global_timer->GetTickCount() >= current->wait_timeout) {
            // Wake up the waiting thread
            TransitionThreadState(current->tid, THREAD_STATE_READY);
            current->wait_timeout = 0;  // Reset timeout
            DLOG("Thread " << current->tid << " (" << current->name << ") woken up after timeout");
        }
        current = current->next;
    }
}

bool ThreadManager::SuspendThread(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to suspend non-existent thread with TID: " << tid);
        return false;
    }
    
    // Record original state if not already suspended
    if (target->state != THREAD_STATE_SUSPENDED) {
        target->previous_state = target->state;
    }
    
    // Increment suspend counter for nested suspends
    target->suspend_count++;
    
    // Only change state if thread is not already suspended
    if (target->state != THREAD_STATE_SUSPENDED) {
        ThreadState original_state = target->state;
        if (TransitionThreadState(tid, THREAD_STATE_SUSPENDED)) {
            DLOG("Thread TID " << tid << " suspended, was in state " << GetThreadStateName(original_state));
            return true;
        }
        return false;
    }
    
    DLOG("Thread TID " << tid << " was already suspended, incrementing suspend count to " << target->suspend_count);
    return true;
}

bool ThreadManager::ResumeThread(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to resume non-existent thread with TID: " << tid);
        return false;
    }
    
    // Decrement suspend counter
    if (target->suspend_count > 0) {
        target->suspend_count--;
    } else {
        LOG("Warning: Attempted to resume unsuspended thread " << tid);
        return false; // Thread was not suspended
    }
    
    // Only change state if all suspend calls have been undone
    if (target->suspend_count == 0 && target->state == THREAD_STATE_SUSPENDED) {
        // Transition back to the previous state
        if (TransitionThreadState(tid, target->previous_state)) {
            DLOG("Thread TID " << tid << " resumed to state " << GetThreadStateName(target->previous_state));
            return true;
        }
        return false;
    } else if (target->suspend_count > 0) {
        DLOG("Thread TID " << tid << " still suspended, suspend count: " << target->suspend_count);
        return true; // Successfully decremented, but still suspended
    }
    
    DLOG("Thread TID " << tid << " was not in suspended state");
    return false; // Thread was not suspended
}

bool ThreadManager::BlockThread(uint32 tid, uint32 reason) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to block non-existent thread with TID: " << tid);
        return false;
    }
    
    // Save the original state before blocking
    if (target->state != THREAD_STATE_SUSPENDED && 
        target->state != THREAD_STATE_BLOCKED && 
        target->state != THREAD_STATE_TERMINATED) {
        target->previous_state = target->state;
    }
    
    target->blocking_reason = reason;
    
    if (TransitionThreadState(tid, THREAD_STATE_BLOCKED)) {
        DLOG("Thread TID " << tid << " blocked with reason " << reason);
        return true;
    }
    return false;
}

bool ThreadManager::UnblockThread(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to unblock non-existent thread with TID: " << tid);
        return false;
    }
    
    if (target->state == THREAD_STATE_BLOCKED) {
        // Transition to ready state when unblocked
        if (TransitionThreadState(tid, THREAD_STATE_READY)) {
            DLOG("Thread TID " << tid << " unblocked");
            return true;
        }
    }
    
    return false;
}

bool ThreadManager::WakeThread(uint32 tid) {
    return UnblockThread(tid);  // For now, wake and unblock are the same
}

bool ThreadManager::SetThreadPriority(uint32 tid, uint32 priority) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to set priority for non-existent thread with TID: " << tid);
        return false;
    }
    target->priority = priority;
    return true;
}

uint32 ThreadManager::GetThreadPriority(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to get priority for non-existent thread with TID: " << tid);
        return 0xFFFFFFFF; // Error code
    }
    return target->priority;
}

bool ThreadManager::SetThreadSchedulingPolicy(uint32 tid, ThreadSchedulingPolicy policy) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to set scheduling policy for non-existent thread with TID: " << tid);
        return false;
    }
    target->sched_policy = policy;
    return true;
}

ThreadSchedulingPolicy ThreadManager::GetThreadSchedulingPolicy(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to get scheduling policy for non-existent thread with TID: " << tid);
        return THREAD_SCHED_POLICY_OTHER; // Default policy
    }
    return target->sched_policy;
}

bool ThreadManager::JoinThread(uint32 tid, void** retval) {
    // Implementation would wait for the thread to terminate and get its return value
    // For now, just return success
    return true;
}

bool ThreadManager::DetachThread(uint32 tid) {
    // Implementation would detach the thread (make it not joinable)
    // For now, just return success
    return true;
}

uint32 ThreadManager::GetThreadCount() {
    uint32 count = 0;
    ThreadControlBlock* current = thread_list_head;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

void ThreadManager::PrintThreadList() {
    LOG("Thread List:");
    ThreadControlBlock* current = thread_list_head;
    while (current) {
        LOG("  TID: " << current->tid << 
            ", PID: " << current->pid <<
            ", Name: " << current->name << 
            ", State: " << GetThreadStateName(current->state) << 
            ", Priority: " << current->priority <<
            ", Suspend Count: " << current->suspend_count);
        current = current->next;
    }
    
    LOG("Total threads: " << GetThreadCount());
}

const char* ThreadManager::GetThreadStateName(ThreadState state) {
    switch (state) {
        case THREAD_STATE_NEW: return "NEW";
        case THREAD_STATE_READY: return "READY";
        case THREAD_STATE_RUNNING: return "RUNNING";
        case THREAD_STATE_WAITING: return "WAITING";
        case THREAD_STATE_BLOCKED: return "BLOCKED";
        case THREAD_STATE_SUSPENDED: return "SUSPENDED";
        case THREAD_STATE_TERMINATED: return "TERMINATED";
        default: return "INVALID";
    }
}

void ThreadManager::PrintThreadStateHistory(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to print state history for non-existent thread with TID: " << tid);
        return;
    }
    
    LOG("Thread TID " << tid << " (" << target->name << ") state history:");
    LOG("  Current State: " << GetThreadStateName(target->state));
    LOG("  Previous State: " << GetThreadStateName(target->previous_state));
    LOG("  State Duration: " << GetStateDuration(tid) << " ticks");
    LOG("  Creation Time: " << target->creation_time);
    LOG("  Last State Change: " << target->last_state_change);
    LOG("  Blocking Reason: " << target->blocking_reason);
    LOG("  Suspend Count: " << target->suspend_count);
}

bool ThreadManager::AllocThreadLocalStorage(uint32 tid, uint32 size) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to allocate TLS for non-existent thread with TID: " << tid);
        return false;
    }
    
    // Free existing TLS if any
    if (target->thread_local_storage) {
        free(target->thread_local_storage);
        target->thread_local_storage = nullptr;
        target->tls_size = 0;
    }
    
    // Allocate new TLS
    target->thread_local_storage = malloc(size);
    if (!target->thread_local_storage) {
        LOG("Failed to allocate TLS for thread TID: " << tid);
        return false;
    }
    
    target->tls_size = size;
    memset(target->thread_local_storage, 0, size);  // Initialize to zero
    return true;
}

bool ThreadManager::FreeThreadLocalStorage(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to free TLS for non-existent thread with TID: " << tid);
        return false;
    }
    
    if (target->thread_local_storage) {
        free(target->thread_local_storage);
        target->thread_local_storage = nullptr;
        target->tls_size = 0;
    }
    
    return true;
}

void* ThreadManager::GetThreadLocalStorage(uint32 tid) {
    ThreadControlBlock* target = GetThreadById(tid);
    if (!target) {
        LOG("Attempted to get TLS for non-existent thread with TID: " << tid);
        return nullptr;
    }
    
    return target->thread_local_storage;
}