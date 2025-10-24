ProcessControlBlock* ProcessManager::ScheduleNextProcessMLFQ() {
    // Multi-Level Feedback Queue scheduling algorithm
    // Implements multiple priority queues with different time quanta
    // Processes can move between queues based on their behavior
    
    // Update MLFQ levels based on process behavior
    UpdateMLFQLevels();
    
    // Boost priorities periodically to prevent starvation
    if (g_kernel_config && g_kernel_config->mlfq_boost_enabled) {
        static uint32 last_boost_time = 0;
        uint32 current_time = global_timer ? global_timer->GetTickCount() : 0;
        
        if (current_time - last_boost_time >= g_kernel_config->mlfq_boost_interval) {
            BoostStarvingProcesses();
            last_boost_time = current_time;
        }
    }
    
    // Find the highest priority process in the highest non-empty queue
    ProcessControlBlock* best_candidate = nullptr;
    uint32 highest_priority = 0xFFFFFFFF;
    
    ProcessControlBlock* current = process_list_head;
    while (current) {
        // Check if the process is in a schedulable state
        if (current->state == PROCESS_STATE_READY || 
            current->state == PROCESS_STATE_NEW || 
            current->state == PROCESS_STATE_RUNNING) {
            
            // For MLFQ, the queue level determines priority
            // Lower level numbers = higher priority
            if (current->mlfq_level < highest_priority) {
                highest_priority = current->mlfq_level;
                best_candidate = current;
            } else if (current->mlfq_level == highest_priority) {
                // Same level, use secondary criteria (original priority, creation time, etc.)
                if (current->current_priority < best_candidate->current_priority) {
                    best_candidate = current;
                } else if (current->current_priority == best_candidate->current_priority) {
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

bool ProcessManager::UpdateMLFQLevels() {
    // Update MLFQ levels based on process behavior
    ProcessControlBlock* current = process_list_head;
    while (current) {
        // If process used its entire time slice, demote it
        if (current->time_slice_remaining <= 0 && current->state == PROCESS_STATE_RUNNING) {
            // Demote to next lower priority level (higher number)
            if (current->mlfq_level < (g_kernel_config ? g_kernel_config->mlfq_levels - 1 : 2)) {
                current->mlfq_level++;
                
                // Update time slice for new level
                // Lower levels get shorter time slices (more responsive)
                uint32 base_quantum = g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
                current->mlfq_time_slice = base_quantum * (current->mlfq_level + 1);
                current->time_slice_remaining = current->mlfq_time_slice;
                
                DLOG("Process PID " << current->pid << " demoted to MLFQ level " << current->mlfq_level);
            }
        }
        
        // If process yielded voluntarily, it demonstrated good behavior
        // Keep it at current level or promote if appropriate
        if (current->voluntary_yield_count > current->preemption_count && 
            current->mlfq_level > 0) {
            // Promote to higher priority level (lower number)
            current->mlfq_level--;
            
            // Update time slice for new level
            uint32 base_quantum = g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
            current->mlfq_time_slice = base_quantum * (current->mlfq_level + 1);
            current->time_slice_remaining = current->mlfq_time_slice;
            
            DLOG("Process PID " << current->pid << " promoted to MLFQ level " << current->mlfq_level 
                 << " due to good behavior");
        }
        
        current = current->next;
    }
    
    return true;
}

bool ProcessManager::BoostStarvingProcesses() {
    // Periodically boost all processes to highest priority level
    // to prevent starvation
    
    ProcessControlBlock* current = process_list_head;
    while (current) {
        // Boost to highest priority level (level 0)
        if (current->mlfq_level > 0) {
            current->mlfq_level = 0;
            current->priority_boost_count++;
            current->last_priority_boost = global_timer ? global_timer->GetTickCount() : 0;
            
            // Reset time slice for highest priority
            uint32 base_quantum = g_kernel_config ? g_kernel_config->scheduler_quantum_ms : 10;
            current->mlfq_time_slice = base_quantum;
            current->time_slice_remaining = current->mlfq_time_slice;
            
            DLOG("Process PID " << current->pid << " boosted to MLFQ level 0 (starvation prevention)");
        }
        
        current = current->next;
    }
    
    return true;
}