ProcessControlBlock* ProcessManager::ScheduleNextProcessFairShare() {
    // Fair-share scheduling algorithm
    // Distributes CPU time fairly among users/groups based on their shares
    
    // Find the user/group with the lowest CPU usage ratio
    ProcessControlBlock* best_candidate = nullptr;
    float lowest_ratio = 1.0f;  // Ratio of CPU used to shares (lower is better)
    
    ProcessControlBlock* current = process_list_head;
    while (current) {
        // Check if the process is in a schedulable state
        if (current->state == PROCESS_STATE_READY || 
            current->state == PROCESS_STATE_NEW || 
            current->state == PROCESS_STATE_RUNNING) {
            
            // Calculate the CPU usage ratio for this process's user/group
            // This is a simplified implementation - in a real system, we'd track
            // usage per user/group across all processes
            
            // For now, we'll approximate based on individual process usage
            uint32 total_cpu_time = current->total_cpu_time_used;
            uint32 shares = current->cpu_shares;
            
            // Avoid division by zero
            if (shares == 0) shares = 1024;  // Default shares
            
            float ratio = (float)total_cpu_time / (float)shares;
            
            if (ratio < lowest_ratio) {
                lowest_ratio = ratio;
                best_candidate = current;
            } else if (ratio == lowest_ratio) {
                // Tie-breaker: use priority
                if (current->current_priority < best_candidate->current_priority) {
                    best_candidate = current;
                } else if (current->current_priority == best_candidate->current_priority) {
                    // Secondary tie-breaker: creation time
                    if (current->creation_time < best_candidate->creation_time) {
                        best_candidate = current;
                    }
                }
            }
        }
        current = current->next;
    }
    
    return best_candidate;
}

bool ProcessManager::AgeProcessPriorities() {
    // Age process priorities to prevent starvation in priority-based scheduling
    if (!g_kernel_config || !g_kernel_config->starvation_prevention) {
        return true;  // Starvation prevention disabled
    }
    
    ProcessControlBlock* current = process_list_head;
    while (current) {
        // Only age processes that have been waiting
        if (current->state == PROCESS_STATE_READY || 
            current->state == PROCESS_STATE_WAITING) {
            
            // Increase priority of processes that have been waiting a long time
            uint32 current_time = global_timer ? global_timer->GetTickCount() : 0;
            uint32 wait_time = current_time - current->last_run_time;
            
            // If process has been waiting more than 1 second, start aging its priority
            if (wait_time > 1000) {  // Assuming 1 tick = 1ms
                // Boost priority by 1 for every additional 100ms waited
                uint32 boost_amount = (wait_time - 1000) / 100;
                
                // Don't let boosted priority go below 1
                if (current->current_priority > boost_amount) {
                    current->current_priority -= boost_amount;
                } else {
                    current->current_priority = 1;
                }
                
                DLOG("Process PID " << current->pid << " priority aged to " << current->current_priority 
                     << " (was " << (current->current_priority + boost_amount) << ")");
            }
        }
        current = current->next;
    }
    
    return true;
}