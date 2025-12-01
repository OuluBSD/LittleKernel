ProcessControlBlock* ProcessManager::ScheduleNextProcessRealtime() {
    // Real-time scheduling algorithm with multiple policies
    // Prioritizes real-time processes with deadlines based on their policy
    
    ProcessControlBlock* best_candidate = nullptr;
    uint32 highest_rt_priority = 0;  // Higher number = higher RT priority
    
    ProcessControlBlock* current = process_list_head;
    while (current) {
        // Check if the process is in a schedulable state
        if (current->state == PROCESS_STATE_READY || 
            current->state == PROCESS_STATE_NEW || 
            current->state == PROCESS_STATE_RUNNING) {
            
            // Check if this is a real-time process
            // In a real implementation, we'd have a flag or specific field
            // For now, we'll use a convention and also check real-time parameters
            if (current->current_priority < 10 || 
                IsProcessRealTime(current->pid)) {
                
                // Determine scheduling policy for this process
                RealTimeSchedulingPolicy policy = RT_SCHED_FIFO;  // Default
                if (g_real_time_scheduler) {
                    RealTimeParams params;
                    if (g_real_time_scheduler->GetRealTimeParams(current->pid, &params)) {
                        policy = params.policy;
                    }
                }
                
                // Dispatch to appropriate real-time scheduling algorithm
                ProcessControlBlock* candidate = nullptr;
                switch (policy) {
                    case RT_SCHED_FIFO:
                        candidate = ScheduleNextProcessFIFO(current);
                        break;
                    case RT_SCHED_RR:
                        candidate = ScheduleNextProcessRR(current);
                        break;
                    case RT_SCHED_DEADLINE:
                        candidate = ScheduleNextProcessDeadline(current);
                        break;
                    case RT_SCHED_EDF:
                        candidate = ScheduleNextProcessEDF(current);
                        break;
                    case RT_SCHED_RM:
                        candidate = ScheduleNextProcessRM(current);
                        break;
                    case RT_SCHED_DM:
                        candidate = ScheduleNextProcessDM(current);
                        break;
                    case RT_SCHED_LST:
                        candidate = ScheduleNextProcessLST(current);
                        break;
                    case RT_SCHED_GS:
                        candidate = ScheduleNextProcessGS(current);
                        break;
                    case RT_SCHED_CBS:
                        candidate = ScheduleNextProcessCBS(current);
                        break;
                    case RT_SCHED_DVS:
                        candidate = ScheduleNextProcessDVS(current);
                        break;
                    case RT_SCHED_DPS:
                        candidate = ScheduleNextProcessDPS(current);
                        break;
                    case RT_SCHED_AE:
                        candidate = ScheduleNextProcessAE(current);
                        break;
                    case RT_SCHED_BG:
                        candidate = ScheduleNextProcessBG(current);
                        break;
                    case RT_SCHED_IDLE:
                        candidate = ScheduleNextProcessIdle(current);
                        break;
                    case RT_SCHED_CUSTOM:
                        candidate = ScheduleNextProcessCustom(current);
                        break;
                    default:
                        candidate = current;  // Default to current process
                        break;
                }
                
                // Compare candidates
                if (candidate) {
                    if (!best_candidate || 
                        IsHigherPriority(candidate, best_candidate)) {
                        best_candidate = candidate;
                        highest_rt_priority = GetRealTimePriority(candidate->pid);
                    } else if (candidate == best_candidate) {
                        // Tie-breaker: earliest deadline
                        if (g_real_time_scheduler) {
                            uint32 candidate_deadline = g_real_time_scheduler->GetProcessDeadline(candidate->pid);
                            uint32 best_deadline = g_real_time_scheduler->GetProcessDeadline(best_candidate->pid);
                            if (candidate_deadline < best_deadline) {
                                best_candidate = candidate;
                                highest_rt_priority = GetRealTimePriority(candidate->pid);
                            }
                        }
                    }
                }
            }
        }
        current = current->next;
    }
    
    // If we found a real-time process, return it immediately
    if (best_candidate) {
        return best_candidate;
    }
    
    // No real-time processes ready, fall back to normal scheduling
    return ScheduleNextProcess();
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessFIFO(ProcessControlBlock* current) {
    // FIFO scheduling: select the highest priority real-time process
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessRR(ProcessControlBlock* current) {
    // Round-robin scheduling for real-time processes
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessDeadline(ProcessControlBlock* current) {
    // Deadline-based scheduling
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessEDF(ProcessControlBlock* current) {
    // Earliest Deadline First scheduling
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessRM(ProcessControlBlock* current) {
    // Rate Monotonic scheduling
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessDM(ProcessControlBlock* current) {
    // Deadline Monotonic scheduling
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessLST(ProcessControlBlock* current) {
    // Least Slack Time scheduling
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessGS(ProcessControlBlock* current) {
    // Guaranteed Scheduling
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessCBS(ProcessControlBlock* current) {
    // Constant Bandwidth Server scheduling
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessDVS(ProcessControlBlock* current) {
    // Dynamic Voltage Scaling scheduling
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessDPS(ProcessControlBlock* current) {
    // Dynamic Priority Scheduling
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessAE(ProcessControlBlock* current) {
    // Aperiodic Events scheduling
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessBG(ProcessControlBlock* current) {
    // Background scheduling
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessIdle(ProcessControlBlock* current) {
    // Idle scheduling
    return current;
}

ProcessControlBlock* ProcessManager::ScheduleNextProcessCustom(ProcessControlBlock* current) {
    // Custom scheduling
    return current;
}

bool ProcessManager::IsHigherPriority(ProcessControlBlock* task1, ProcessControlBlock* task2) {
    if (!task1 || !task2) {
        return false;
    }
    
    // Check if either task is real-time
    bool task1_rt = IsProcessRealTime(task1->pid);
    bool task2_rt = IsProcessRealTime(task2->pid);
    
    // Real-time processes have higher priority than non-real-time
    if (task1_rt && !task2_rt) {
        return true;
    } else if (!task1_rt && task2_rt) {
        return false;
    } else if (task1_rt && task2_rt) {
        // Both are real-time, compare real-time priorities
        uint32 priority1 = GetRealTimePriority(task1->pid);
        uint32 priority2 = GetRealTimePriority(task2->pid);
        return priority1 > priority2;  // Higher number = higher priority
    }
    
    // Neither is real-time, compare regular priorities
    return task1->current_priority < task2->current_priority;  // Lower number = higher priority
}

bool ProcessManager::IsProcessRealTime(uint32 pid) {
    // Check if a process is a real-time process
    ProcessControlBlock* process = GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Processes with priority < 10 are considered real-time
    // Also check if real-time scheduler has info about this process
    return (process->current_priority < 10) || 
           (g_real_time_scheduler && g_real_time_scheduler->IsProcessRealTime(pid));
}

uint32 ProcessManager::GetRealTimePriority(uint32 pid) {
    if (g_real_time_scheduler) {
        return g_real_time_scheduler->GetRealTimePriority(pid);
    }
    
    // Fallback to regular priority for real-time processes
    ProcessControlBlock* process = GetProcessById(pid);
    if (process && process->current_priority < 10) {
        return process->current_priority;
    }
    
    return 0;
}

uint32 ProcessManager::GetProcessDeadline(uint32 pid) {
    if (g_real_time_scheduler) {
        return g_real_time_scheduler->GetProcessDeadline(pid);
    }
    
    // Return a dummy value for non-real-time scheduler
    return global_timer ? (global_timer->GetTickCount() + 1000) : 1000;
}

uint32 ProcessManager::GetProcessPeriod(uint32 pid) {
    if (g_real_time_scheduler) {
        return g_real_time_scheduler->GetProcessPeriod(pid);
    }
    
    // Return a dummy value for non-real-time scheduler
    return 100;  // 100ms period
}

uint32 ProcessManager::GetProcessExecutionTime(uint32 pid) {
    if (g_real_time_scheduler) {
        return g_real_time_scheduler->GetProcessExecutionTime(pid);
    }
    
    // Return a dummy value for non-real-time scheduler
    return 10;  // 10ms execution time
}

bool ProcessManager::AdjustProcessPriority(uint32 pid, int adjustment) {
    // Adjust a process's priority by a relative amount
    ProcessControlBlock* target = GetProcessById(pid);
    if (!target) {
        LOG("Attempted to adjust priority for non-existent process with PID: " << pid);
        return false;
    }
    
    // Apply adjustment
    int new_priority = (int)target->current_priority + adjustment;
    
    // Clamp to valid range
    if (new_priority < 1) new_priority = 1;
    if (new_priority > 32) new_priority = 32;  // Assuming 32 levels
    
    target->current_priority = (uint32)new_priority;
    
    DLOG("Adjusted process PID " << pid << " priority from " << (target->current_priority - adjustment) 
         << " to " << target->current_priority);
    
    return true;
}

bool ProcessManager::ApplyPriorityInheritance(ProcessControlBlock* blocked_process) {
    // Apply priority inheritance to prevent priority inversion
    // When a high-priority process waits for a resource held by a low-priority process,
    // temporarily boost the low-priority process to the high-priority level
    
    if (!blocked_process || !g_kernel_config || !g_kernel_config->starvation_prevention) {
        return false;
    }
    
    // Find the process that is holding the resource this process is waiting for
    // This is a simplified implementation - in a real system, we'd traverse
    // the resource/wait chain to find all processes involved
    
    ProcessControlBlock* resource_holder = nullptr;
    
    // For demonstration, we'll assume the resource holder is the previous process
    // In a real implementation, we'd need to track resource ownership
    ProcessControlBlock* current = process_list_head;
    while (current) {
        if (current != blocked_process && 
            (current->state == PROCESS_STATE_RUNNING || current->state == PROCESS_STATE_READY)) {
            resource_holder = current;
            break;
        }
        current = current->next;
    }
    
    if (resource_holder && resource_holder->current_priority > blocked_process->current_priority) {
        // The resource holder has lower priority than the blocked process
        // Temporarily boost the resource holder's priority
        uint32 original_priority = resource_holder->current_priority;
        resource_holder->current_priority = blocked_process->current_priority;
        resource_holder->priority_boost_count++;
        
        DLOG("Priority inheritance: Boosted process PID " << resource_holder->pid 
             << " from priority " << original_priority << " to " << resource_holder->current_priority 
             << " to prevent priority inversion");
        
        return true;
    }
    
    return false;  // No priority inversion detected or no action needed
}

bool ProcessManager::RevertPriorityInheritance(ProcessControlBlock* unblocked_process) {
    // Revert priority inheritance when a process releases a resource
    // Restore the process to its original priority
    
    if (!unblocked_process) {
        return false;
    }
    
    // In a real implementation, we'd check if this process had inherited priority
    // and restore it to its original value
    
    // For this simplified implementation, we'll just log that we're reverting
    // In a complete implementation, we'd store the original priority and restore it
    
    DLOG("Reverting priority inheritance for process PID " << unblocked_process->pid 
         << " (simplified implementation)");
    
    return true;
}

uint32 ProcessManager::GetAverageResponseTime() {
    // Calculate average response time for all processes
    uint32 total_response_time = 0;
    uint32 process_count = 0;
    
    ProcessControlBlock* current = process_list_head;
    while (current) {
        total_response_time += current->response_time;
        process_count++;
        current = current->next;
    }
    
    return process_count > 0 ? total_response_time / process_count : 0;
}

uint32 ProcessManager::GetAverageTurnaroundTime() {
    // Calculate average turnaround time for all processes
    uint32 total_turnaround_time = 0;
    uint32 process_count = 0;
    
    ProcessControlBlock* current = process_list_head;
    while (current) {
        total_turnaround_time += current->turnaround_time;
        process_count++;
        current = current->next;
    }
    
    return process_count > 0 ? total_turnaround_time / process_count : 0;
}

uint32 ProcessManager::GetAverageWaitTime() {
    // Calculate average wait time for all processes
    uint32 total_wait_time = 0;
    uint32 process_count = 0;
    
    ProcessControlBlock* current = process_list_head;
    while (current) {
        total_wait_time += current->wait_time;
        process_count++;
        current = current->next;
    }
    
    return process_count > 0 ? total_wait_time / process_count : 0;
}

uint32 ProcessManager::GetContextSwitchCount() {
    // Calculate total context switches across all processes
    uint32 total_context_switches = 0;
    
    ProcessControlBlock* current = process_list_head;
    while (current) {
        total_context_switches += current->context_switch_count;
        current = current->next;
    }
    
    return total_context_switches;
}

void ProcessManager::PrintSchedulingStatistics() {
    LOG("=== Scheduling Statistics ===");
    LOG("Average Response Time: " << GetAverageResponseTime() << " ms");
    LOG("Average Turnaround Time: " << GetAverageTurnaroundTime() << " ms");
    LOG("Average Wait Time: " << GetAverageWaitTime() << " ms");
    LOG("Total Context Switches: " << GetContextSwitchCount());
    LOG("Current Scheduling Mode: " << (int)current_mode);
    LOG("==============================");
}

void ProcessManager::ResetSchedulingStatistics() {
    // Reset scheduling statistics for all processes
    ProcessControlBlock* current = process_list_head;
    while (current) {
        current->response_time = 0;
        current->turnaround_time = 0;
        current->wait_time = 0;
        current->context_switch_count = 0;
        current->preemption_count = 0;
        current->voluntary_yield_count = 0;
        current = current->next;
    }
    
    DLOG("Scheduling statistics reset");
}

uint32 ProcessManager::GetTotalProcessCount() {
    // Count all processes in the system
    uint32 count = 0;
    ProcessControlBlock* current = process_list_head;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

uint32 ProcessManager::GetTotalContextSwitches() {
    // Calculate total context switches across all processes
    uint32 total_context_switches = 0;

    ProcessControlBlock* current = process_list_head;
    while (current) {
        total_context_switches += current->context_switch_count;
        current = current->next;
    }

    return total_context_switches;
}