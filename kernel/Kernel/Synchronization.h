#ifndef _Kernel_Synchronization_h_
#define _Kernel_Synchronization_h_

#include "Defs.h"
#include "ProcessControlBlock.h"

// Forward declarations
struct ProcessControlBlock;

// Structure for a semaphore
struct Semaphore {
    int32 count;                          // Current count of the semaphore
    ProcessControlBlock* waiting_list;    // List of processes waiting on this semaphore
    uint32 max_count;                     // Maximum value for counting semaphore
    
    // Constructor
    Semaphore(int32 initial_count, uint32 max_val = 0xFFFFFFFF);
};

// Structure for a mutex
struct Mutex {
    bool is_locked;                       // Whether the mutex is currently locked
    uint32 owner_pid;                     // PID of process that owns the mutex
    ProcessControlBlock* waiting_list;    // List of processes waiting on this mutex
    
    // Constructor
    Mutex();
};

// Structure for events
struct Event {
    bool is_signaled;                     // Whether the event is currently signaled
    ProcessControlBlock* waiting_list;    // List of processes waiting on this event
    
    // Constructor
    Event(bool initial_state = false);
};

// Class for synchronization manager
class SyncManager {
private:
    // These would be lists of synchronization objects in a real implementation
    
public:
    SyncManager();
    ~SyncManager();
    
    // Semaphore operations
    Semaphore* CreateSemaphore(int32 initial_count, uint32 max_count = 0xFFFFFFFF);
    bool DestroySemaphore(Semaphore* sem);
    bool SemaphoreWait(Semaphore* sem);    // P operation (wait)
    bool SemaphoreSignal(Semaphore* sem);  // V operation (signal)
    
    // Mutex operations
    Mutex* CreateMutex();
    bool DestroyMutex(Mutex* mutex);
    bool MutexLock(Mutex* mutex);
    bool MutexUnlock(Mutex* mutex);
    
    // Event operations
    Event* CreateEvent(bool initial_state = false);
    bool DestroyEvent(Event* event);
    bool SetEvent(Event* event);
    bool ResetEvent(Event* event);
    bool WaitForEvent(Event* event);
};

extern SyncManager* sync_manager;

#endif