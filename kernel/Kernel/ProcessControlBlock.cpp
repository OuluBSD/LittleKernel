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
    
    new_pcb->state = PROCESS_STATE_NEW;
    new_pcb->priority = priority;
    
    // Initialize memory management fields
    new_pcb->page_directory = nullptr;  // Will be set up later
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
    
    // Initialize synchronization and IPC
    new_pcb->waiting_on_semaphore = nullptr;
    new_pcb->event_flags = nullptr;
    new_pcb->waiting_on_mutex = nullptr;
    new_pcb->waiting_on_event = nullptr;
    new_pcb->message_queue = nullptr;
    new_pcb->opened_files = nullptr;
    
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
    
    // Set state to ready after successful creation
    SetProcessState(new_pcb->pid, PROCESS_STATE_READY);
    
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
    
    SetProcessState(pid, PROCESS_STATE_TERMINATED);
    
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
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to set state for non-existent process with PID: " << pid);
        return false;
    }
    
    DLOG("Setting process PID " << pid << " from state " << target->state << " to " << new_state);
    
    target->state = new_state;
    return true;
}

ProcessState ProcessManager::GetProcessState(uint32 pid) {
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to get state for non-existent process with PID: " << pid);
        return PROCESS_STATE_TERMINATED; // Return terminated for invalid processes
    }
    return target->state;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcess() {
    ProcessControlBlock* best_candidate = nullptr;
    uint32 highest_priority = 0xFFFFFFFF;  // Lower number = higher priority
    
    ProcessControlBlock* current = process_list_head;
    while (current) {
        if (current->state == PROCESS_STATE_READY) {
            // In priority-based scheduling, lower priority number means higher priority
            if (current->priority < highest_priority) {
                highest_priority = current->priority;
                best_candidate = current;
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
    
    // In cooperative mode, always yield to another process
    // In preemptive mode, the scheduler handles this via timer interrupts
    if (current_mode == SCHEDULING_MODE_COOPERATIVE) {
        // Set the current process back to ready state
        SetProcessState(current_process->pid, PROCESS_STATE_READY);
        
        // Find next process to run
        ProcessControlBlock* next_process = ScheduleNextProcess();
        if (next_process) {
            current_process = next_process;
            SetProcessState(current_process->pid, PROCESS_STATE_RUNNING);
            return true;
        }
        
        // If no other process to run, keep current process running
        SetProcessState(current_process->pid, PROCESS_STATE_RUNNING);
        return true;
    } else {
        // In preemptive mode, we still allow explicit yields but they're not required
        // Set the current process back to ready state
        SetProcessState(current_process->pid, PROCESS_STATE_READY);
        
        // Find next process to run
        ProcessControlBlock* next_process = ScheduleNextProcess();
        if (next_process) {
            current_process = next_process;
            SetProcessState(current_process->pid, PROCESS_STATE_RUNNING);
            return true;
        }
        
        // If no other process to run, keep current process running
        SetProcessState(current_process->pid, PROCESS_STATE_RUNNING);
        return true;
    }
}

bool ProcessManager::SleepCurrentProcess(uint32 sleep_ticks) {
    if (!current_process) {
        return false;
    }
    
    // In a real implementation, we'd need to track when to wake up this process
    // For now, just set it to waiting state
    SetProcessState(current_process->pid, PROCESS_STATE_WAITING);
    
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
            ", State: " << current->state << 
            ", Priority: " << current->priority);
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
    // Check if we need to schedule based on current mode
    if (current_mode == SCHEDULING_MODE_PREEMPTIVE || current_mode == SCHEDULING_MODE_ROUND_ROBIN) {
        // In both preemptive and round-robin modes, we force a context switch when quantum expires
        if (current_process && current_process->ticks_remaining > 0) {
            current_process->ticks_remaining--;
        }
        
        // If no current process or time quantum expired, schedule a new one
        if (!current_process || current_process->ticks_remaining <= 0) {
            ProcessControlBlock* next_process = ScheduleNextProcess();
            if (next_process) {
                if (current_process) {
                    // Save the current process state
                    SetProcessState(current_process->pid, PROCESS_STATE_READY);
                }
                
                current_process = next_process;
                SetProcessState(current_process->pid, PROCESS_STATE_RUNNING);
                
                // Reset quantum for the scheduled process
                current_process->ticks_remaining = g_kernel_config->scheduler_quantum_ms;
                
                // Log context switch for debugging
                if (current_mode == SCHEDULING_MODE_PREEMPTIVE) {
                    DLOG("Preemptive context switch to PID " << current_process->pid 
                         << ", name: " << current_process->name);
                } else {  // Round-robin
                    DLOG("Round-robin context switch to PID " << current_process->pid 
                         << ", name: " << current_process->name);
                }
            }
        }
    } else {  // Cooperative mode
        // In cooperative mode, only schedule when current process yields
        // The actual scheduling happens in YieldCurrentProcess()
        
        // Still update timing if a process is running
        if (current_process) {
            current_process->total_cpu_time++;
        }
    }
}