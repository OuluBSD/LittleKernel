#include "Synchronization.h"
#include "Logging.h"
#include "Global.h"
#include "MemoryManager.h"

// Global synchronization manager instance
SyncManager* sync_manager = nullptr;

// Semaphore constructor
Semaphore::Semaphore(int32 initial_count, uint32 max_val) {
    count = initial_count;
    max_count = max_val;
    waiting_list = nullptr;
}

// Mutex constructor
Mutex::Mutex() {
    is_locked = false;
    owner_pid = 0;  // No owner initially
    waiting_list = nullptr;
}

// Event constructor
Event::Event(bool initial_state) {
    is_signaled = initial_state;
    waiting_list = nullptr;
}

// SyncManager constructor
SyncManager::SyncManager() {
    // Initialize the synchronization manager
    DLOG("Synchronization manager initialized");
}

// SyncManager destructor
SyncManager::~SyncManager() {
    // Clean up any remaining synchronization objects
    DLOG("Synchronization manager destroyed");
}

// Semaphore operations
Semaphore* SyncManager::CreateSemaphore(int32 initial_count, uint32 max_count) {
    Semaphore* sem = (Semaphore*)malloc(sizeof(Semaphore));
    if (!sem) {
        LOG("Failed to allocate memory for semaphore");
        return nullptr;
    }
    
    // Initialize the semaphore
    sem->count = initial_count;
    sem->max_count = max_count;
    sem->waiting_list = nullptr;
    
    LOG("Created semaphore with initial count: " << initial_count << ", max: " << max_count);
    return sem;
}

bool SyncManager::DestroySemaphore(Semaphore* sem) {
    if (!sem) {
        return false;
    }
    
    // Wake up any waiting processes
    // In a real implementation, we would iterate through the waiting list
    // and wake up all processes waiting on this semaphore
    
    free(sem);
    LOG("Destroyed semaphore");
    return true;
}

bool SyncManager::SemaphoreWait(Semaphore* sem) {
    if (!sem) {
        LOG("ERROR: Invalid semaphore for wait operation");
        return false;
    }
    
    // This is a simplified implementation
    // In a real system, we would need to disable interrupts during this operation
    
    if (sem->count > 0) {
        // Decrement the count and continue
        sem->count--;
        return true;
    } else {
        // No resources available, put current process on waiting list
        ProcessControlBlock* current_process = process_manager->GetCurrentProcess();
        if (!current_process) {
            LOG("ERROR: No current process to wait on semaphore");
            return false;
        }
        
        // Add process to semaphore's waiting list
        current_process->waiting_on_semaphore = sem->waiting_list;
        sem->waiting_list = current_process;
        
        process_manager->SetProcessState(current_process->pid, PROCESS_STATE_WAITING);
        
        // Yield to scheduler
        process_manager->YieldCurrentProcess();
        
        return true;
    }
}

bool SyncManager::SemaphoreSignal(Semaphore* sem) {
    if (!sem) {
        LOG("ERROR: Invalid semaphore for signal operation");
        return false;
    }
    
    // In a real system, we would need to disable interrupts during this operation
    
    // Check if we can increment without exceeding max
    if (sem->count < (int32)sem->max_count) {
        sem->count++;
        
        // If there are waiting processes, wake one up
        if (sem->waiting_list) {
            ProcessControlBlock* waiting_process = sem->waiting_list;
            
            // Remove the process from the waiting list
            sem->waiting_list = waiting_process->waiting_on_semaphore;
            waiting_process->waiting_on_semaphore = nullptr;
            
            // Change the process state to ready
            process_manager->SetProcessState(waiting_process->pid, PROCESS_STATE_READY);
            
            LOG("Woke up process " << waiting_process->pid << " waiting on semaphore");
        }
        
        return true;
    } else {
        LOG("ERROR: Semaphore signal would exceed maximum count");
        return false;
    }
}

// Mutex operations
Mutex* SyncManager::CreateMutex() {
    Mutex* mutex = (Mutex*)malloc(sizeof(Mutex));
    if (!mutex) {
        LOG("Failed to allocate memory for mutex");
        return nullptr;
    }
    
    // Initialize the mutex
    mutex->is_locked = false;
    mutex->owner_pid = 0;
    mutex->waiting_list = nullptr;
    
    LOG("Created mutex");
    return mutex;
}

bool SyncManager::DestroyMutex(Mutex* mutex) {
    if (!mutex) {
        return false;
    }
    
    // Ensure mutex is unlocked before destroying
    if (mutex->is_locked) {
        LOG("WARNING: Destroying locked mutex");
    }
    
    free(mutex);
    LOG("Destroyed mutex");
    return true;
}

bool SyncManager::MutexLock(Mutex* mutex) {
    if (!mutex) {
        LOG("ERROR: Invalid mutex for lock operation");
        return false;
    }
    
    // In a real implementation, we would disable interrupts during this operation
    
    ProcessControlBlock* current_process = process_manager->GetCurrentProcess();
    if (!current_process) {
        LOG("ERROR: No current process to acquire mutex");
        return false;
    }
    
    if (!mutex->is_locked) {
        // Mutex is available, acquire it
        mutex->is_locked = true;
        mutex->owner_pid = current_process->pid;
        return true;
    } else {
        // Mutex is locked by another process, wait for it
        if (mutex->owner_pid == current_process->pid) {
            // Process already owns the mutex - this might be a recursive mutex case
            LOG("WARNING: Process attempting to lock mutex it already owns");
            // For now, return false to prevent deadlock
            return false;
        }
        
        // Add process to mutex's waiting list
        current_process->waiting_on_mutex = mutex->waiting_list;
        mutex->waiting_list = current_process;
        
        process_manager->SetProcessState(current_process->pid, PROCESS_STATE_WAITING);
        
        // Yield to scheduler
        process_manager->YieldCurrentProcess();
        
        // When the process wakes up, it should have the mutex
        // NOTE: In a real implementation, we'd need to ensure the mutex is properly assigned
        // when the process resumes, but this simplified version assumes it will be
        mutex->is_locked = true;
        mutex->owner_pid = current_process->pid;
        return true;
    }
}

bool SyncManager::MutexUnlock(Mutex* mutex) {
    if (!mutex) {
        LOG("ERROR: Invalid mutex for unlock operation");
        return false;
    }
    
    ProcessControlBlock* current_process = process_manager->GetCurrentProcess();
    if (current_process && current_process->pid != mutex->owner_pid) {
        LOG("ERROR: Process attempting to unlock mutex it doesn't own");
        return false;
    }
    
    // Release the mutex
    mutex->is_locked = false;
    mutex->owner_pid = 0;
    
    // If there are waiting processes, wake one up
    if (mutex->waiting_list) {
        ProcessControlBlock* waiting_process = mutex->waiting_list;
        
        // Remove the process from the waiting list
        mutex->waiting_list = waiting_process->waiting_on_mutex;
        waiting_process->waiting_on_mutex = nullptr;
        
        // Change the process state to ready
        process_manager->SetProcessState(waiting_process->pid, PROCESS_STATE_READY);
        
        LOG("Woke up process " << waiting_process->pid << " waiting on mutex");
        
        // The awakened process will acquire the mutex when it runs
    }
    
    return true;
}

// Event operations
Event* SyncManager::CreateEvent(bool initial_state) {
    Event* event = (Event*)malloc(sizeof(Event));
    if (!event) {
        LOG("Failed to allocate memory for event");
        return nullptr;
    }
    
    // Initialize the event
    event->is_signaled = initial_state;
    event->waiting_list = nullptr;
    
    LOG("Created event with initial state: " << initial_state);
    return event;
}

bool SyncManager::DestroyEvent(Event* event) {
    if (!event) {
        return false;
    }
    
    free(event);
    LOG("Destroyed event");
    return true;
}

bool SyncManager::SetEvent(Event* event) {
    if (!event) {
        LOG("ERROR: Invalid event for set operation");
        return false;
    }
    
    event->is_signaled = true;
    
    // Wake up all processes waiting on this event
    ProcessControlBlock* waiting_process = event->waiting_list;
    while (waiting_process) {
        ProcessControlBlock* next = waiting_process->waiting_on_event;
        
        // Remove the process from the waiting list and change its state
        waiting_process->waiting_on_event = nullptr;
        process_manager->SetProcessState(waiting_process->pid, PROCESS_STATE_READY);
        
        LOG("Woke up process " << waiting_process->pid << " waiting on event");
        
        waiting_process = next;
    }
    
    // Clear the waiting list
    event->waiting_list = nullptr;
    
    LOG("Set event to signaled state and woke up waiting processes");
    return true;
}

bool SyncManager::ResetEvent(Event* event) {
    if (!event) {
        LOG("ERROR: Invalid event for reset operation");
        return false;
    }
    
    event->is_signaled = false;
    LOG("Reset event to non-signaled state");
    return true;
}

bool SyncManager::WaitForEvent(Event* event) {
    if (!event) {
        LOG("ERROR: Invalid event for wait operation");
        return false;
    }
    
    if (event->is_signaled) {
        // Event is already signaled
        return true;
    } else {
        // Event is not signaled, wait for it
        ProcessControlBlock* current_process = process_manager->GetCurrentProcess();
        if (!current_process) {
            LOG("ERROR: No current process to wait on event");
            return false;
        }
        
        // Add process to event's waiting list
        current_process->waiting_on_event = event->waiting_list;
        event->waiting_list = current_process;
        
        process_manager->SetProcessState(current_process->pid, PROCESS_STATE_WAITING);
        
        // Yield to scheduler
        process_manager->YieldCurrentProcess();
        
        // When the process wakes up, the event should be signaled
        return true;
    }
}