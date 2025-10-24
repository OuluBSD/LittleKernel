#ifndef _Kernel_Thread_h_
#define _Kernel_Thread_h_

#include "Defs.h"
#include "ProcessControlBlock.h"

// Thread states (similar to process states but with threading-specific meanings)
enum ThreadState {
    THREAD_STATE_NEW = 0,         // Thread has been created but not yet ready to run
    THREAD_STATE_READY,           // Thread is ready to run and waiting for CPU
    THREAD_STATE_RUNNING,         // Thread is currently running
    THREAD_STATE_WAITING,         // Thread is waiting for an event/synchronization object
    THREAD_STATE_BLOCKED,         // Thread is blocked (e.g., waiting for I/O)
    THREAD_STATE_SUSPENDED,       // Thread is suspended (e.g., by user or debugger)
    THREAD_STATE_TERMINATED      // Thread has completed execution
};

// Thread scheduling policies
enum ThreadSchedulingPolicy {
    THREAD_SCHED_POLICY_FIFO = 0,    // First-In-First-Out
    THREAD_SCHED_POLICY_RR,          // Round Robin
    THREAD_SCHED_POLICY_OTHER        // Other (implementation-defined)
};

// Thread control block structure
struct ThreadControlBlock {
    // Thread identification
    uint32 tid;                    // Thread ID
    uint32 pid;                    // Process ID this thread belongs to
    uint32 parent_tid;             // Parent thread ID (if any)
    
    // Thread state information
    ThreadState state;             // Current state of the thread
    ThreadState previous_state;    // Previous state for state history
    uint32 priority;               // Thread priority (lower number = higher priority)
    ThreadSchedulingPolicy sched_policy; // Scheduling policy
    
    // Memory management
    uint32* stack_pointer;         // Current stack pointer for this thread
    uint32 stack_start;            // Start of stack memory for this thread
    uint32 stack_size;             // Size of stack memory for this thread
    
    // CPU state (for context switching)
    uint32* registers;              // CPU register state for this thread
    uint32 instruction_pointer;     // Current instruction pointer for this thread
    uint32 base_pointer;            // Base pointer for this thread's stack frame
    
    // Scheduling information
    uint32 ticks_remaining;       // Remaining time slice ticks
    uint32 total_cpu_time;         // Total CPU time consumed (in ticks)
    
    // Thread timing
    uint32 start_time;             // Thread start time
    uint32 last_run_time;          // Last time thread was scheduled
    uint32 creation_time;          // Time when thread was created
    uint32 termination_time;       // Time when thread was terminated
    uint32 last_state_change;      // Time of last state change
    uint32 state_duration;        // Duration in current state
    
    // Synchronization primitives
    ThreadControlBlock* waiting_on_semaphore; // Next thread in semaphore wait queue
    uint32* event_flags;           // Event flags for this thread
    ThreadControlBlock* waiting_on_mutex;     // Next thread in mutex wait queue
    ThreadControlBlock* waiting_on_event;     // Next thread in event wait queue
    uint32 blocking_reason;        // Reason for blocking (e.g. semaphore, I/O operation)
    uint32 wait_timeout;           // Timeout for blocking operations (0 = no timeout)
    uint32 suspend_count;          // Number of times thread is suspended (for nested suspend)
    
    // Thread-specific data
    void* thread_local_storage;   // Pointer to thread-local storage
    uint32 tls_size;               // Size of thread-local storage
    
    // Thread name/description (for debugging)
    char name[32];                 // Thread name string
    
    // Links for scheduler queues
    ThreadControlBlock* next;      // Next TCB in the queue
    ThreadControlBlock* prev;      // Previous TCB in the queue
    
    // Additional flags
    uint32 flags;                  // Additional thread flags
    
    // Pointer to parent process
    ProcessControlBlock* parent_process; // Parent process this thread belongs to
};

// Thread management constants
const uint32 INVALID_TID = 0xFFFFFFFF;
const uint32 MAIN_THREAD_TID = 0;        // TID for main thread of a process
const uint32 MIN_TID = 1;                 // Minimum user thread TID
const uint32 MAX_TID = 0xFFFF;            // Maximum thread ID (keeping it reasonable)

// Thread scheduling modes
enum ThreadSchedulingMode {
    THREAD_SCHEDULING_MODE_COOPERATIVE = 0,  // Threads yield control voluntarily
    THREAD_SCHEDULING_MODE_PREEMPTIVE,        // Scheduler forces context switches
    THREAD_SCHEDULING_MODE_ROUND_ROBIN        // Round-robin scheduling with time slices
};

// Thread creation attributes
struct ThreadAttributes {
    uint32 stack_size;              // Stack size for the thread
    uint32 priority;                 // Initial priority for the thread
    ThreadSchedulingPolicy policy; // Scheduling policy
    bool detached;                  // Whether the thread is detached
    void* stack_addr;              // Stack address (NULL for system allocation)
};

// Default thread attributes
const ThreadAttributes DEFAULT_THREAD_ATTRIBUTES = {
    4096,                           // 4KB default stack size
    10,                             // Default priority
    THREAD_SCHED_POLICY_OTHER,      // Default scheduling policy
    false,                          // Not detached by default
    nullptr                         // System allocates stack
};

// Thread management functions
class ThreadManager {
private:
    ThreadControlBlock* current_thread;
    ThreadControlBlock* thread_list_head;
    uint32 next_tid;
    ThreadSchedulingMode current_mode;  // Current scheduling mode
    
public:
    ThreadManager();
    ~ThreadManager();
    
    // Thread creation and destruction
    ThreadControlBlock* CreateThread(ProcessControlBlock* parent_process, 
                                    void* entry_point, 
                                    const char* name, 
                                    const ThreadAttributes* attr = nullptr);
    bool DestroyThread(uint32 tid);
    bool TerminateThread(uint32 tid, uint32 exit_code = 0);
    
    // Thread lifecycle
    ThreadControlBlock* GetThreadById(uint32 tid);
    ThreadControlBlock* GetCurrentThread();
    uint32 GetNextTID();
    
    // Thread state management
    bool SetThreadState(uint32 tid, ThreadState new_state);
    bool TransitionThreadState(uint32 tid, ThreadState new_state);  // Validates state transitions
    ThreadState GetThreadState(uint32 tid);
    ThreadState GetPreviousState(uint32 tid);
    uint32 GetStateDuration(uint32 tid);  // How long has thread been in current state
    uint32 GetBlockingReason(uint32 tid);
    bool SetBlockingReason(uint32 tid, uint32 reason);
    
    // Thread scheduling
    ThreadControlBlock* ScheduleNextThread();  // For scheduler to select next thread
    bool AddToReadyQueue(ThreadControlBlock* tcb);
    ThreadControlBlock* RemoveFromReadyQueue();
    
    // Scheduling mode control
    void SetSchedulingMode(ThreadSchedulingMode mode);
    ThreadSchedulingMode GetSchedulingMode();
    void Schedule();  // Main scheduler function called by timer interrupt
    
    // Thread control
    bool YieldCurrentThread();  // Allow current thread to yield CPU
    bool SleepCurrentThread(uint32 sleep_ticks);  // Put current thread to sleep
    bool SuspendThread(uint32 tid);
    bool ResumeThread(uint32 tid);
    bool BlockThread(uint32 tid, uint32 reason);
    bool UnblockThread(uint32 tid);
    bool WakeThread(uint32 tid);
    
    // Thread-specific operations
    bool SetThreadPriority(uint32 tid, uint32 priority);
    uint32 GetThreadPriority(uint32 tid);
    bool SetThreadSchedulingPolicy(uint32 tid, ThreadSchedulingPolicy policy);
    ThreadSchedulingPolicy GetThreadSchedulingPolicy(uint32 tid);
    
    // Thread joining and detachment
    bool JoinThread(uint32 tid, void** retval);
    bool DetachThread(uint32 tid);
    
    // Utility functions
    uint32 GetThreadCount();
    void PrintThreadList();  // For debugging
    const char* GetThreadStateName(ThreadState state);
    void PrintThreadStateHistory(uint32 tid);
    
    // Thread-local storage
    bool AllocThreadLocalStorage(uint32 tid, uint32 size);
    bool FreeThreadLocalStorage(uint32 tid);
    void* GetThreadLocalStorage(uint32 tid);
};

// Global thread manager instance
extern ThreadManager* thread_manager;

// Thread entry point function type
typedef void* (*ThreadEntryPoint)(void*);

#endif // _Kernel_Thread_h_