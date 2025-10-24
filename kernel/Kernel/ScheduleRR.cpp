ProcessControlBlock* ProcessManager::ScheduleNextProcessRR() {
    // Round-Robin scheduling algorithm
    // Selects the next process in a circular queue, giving each process
    // an equal time slice
    
    // If there's no current process, find any ready process
    if (!current_process) {
        return ScheduleNextProcess(); // Fall back to priority-based scheduling
    }
    
    // If current process still has time remaining and is still runnable, continue with it
    if (current_process->time_slice_remaining > 0 && 
        (current_process->state == PROCESS_STATE_RUNNING || 
         current_process->state == PROCESS_STATE_READY)) {
        return current_process;
    }
    
    // Current process's time slice has expired, find the next ready process
    ProcessControlBlock* candidate = current_process->next;
    
    // Search forward through the list
    while (candidate != current_process) {
        if (candidate == nullptr) {
            // Reached end of list, wrap around to beginning
            candidate = process_list_head;
            if (candidate == current_process) {
                // Only one process in system
                break;
            }
        }
        
        if (candidate->state == PROCESS_STATE_READY) {
            // Found a ready process
            return candidate;
        }
        
        candidate = candidate->next;
        if (candidate == nullptr) {
            // Reached end of list again, wrap around
            candidate = process_list_head;
        }
    }
    
    // If we've gone through the entire list and found nothing ready,
    // check if current process is still runnable
    if (current_process->state == PROCESS_STATE_READY || 
        current_process->state == PROCESS_STATE_RUNNING) {
        return current_process;
    }
    
    // No processes are ready to run
    return nullptr;
}