#ifndef _Kernel_ProcessControlBlock_h_
#define _Kernel_ProcessControlBlock_h_

#include "Defs.h"
#include "KernelConfig.h"  // Include the kernel configuration
#include "Paging.h"        // Include paging structures

// Process states
enum ProcessState {
    PROCESS_STATE_NEW = 0,
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_WAITING,
    PROCESS_STATE_TERMINATED
};

// Process control block structure
struct ProcessControlBlock {
    // Process identification
    uint32 pid;                    // Process ID
    uint32 parent_pid;            // Parent process ID
    uint32 uid;                   // User ID
    uint32 gid;                   // Group ID
    
    // Process state information
    ProcessState state;           // Current state of the process
    uint32 priority;              // Process priority (lower number = higher priority)
    
    // Memory management
    PageDirectory* page_directory; // Page directory for this process
    uint32 heap_start;            // Start of heap memory
    uint32 heap_end;              // End of heap memory
    uint32 stack_pointer;         // Current stack pointer
    uint32 stack_start;           // Start of stack memory
    
    // CPU state (for context switching)
    uint32* registers;            // CPU register state
    uint32 instruction_pointer;   // Current instruction pointer
    
    // Scheduling information
    uint32 ticks_remaining;       // Remaining time slice ticks
    uint32 total_cpu_time;        // Total CPU time consumed (in ticks)
    
    // Process timing
    uint32 start_time;            // Process start time
    uint32 last_run_time;         // Last time process was scheduled
    
    // Synchronization primitives
    ProcessControlBlock* waiting_on_semaphore; // Next process in semaphore wait queue
    uint32* event_flags;          // Event flags for this process
    ProcessControlBlock* waiting_on_mutex;     // Next process in mutex wait queue
    ProcessControlBlock* waiting_on_event;     // Next process in event wait queue
    
    // Inter-process communication
    uint32* message_queue;        // Messages waiting for this process
    uint32* opened_files;         // Array of file descriptors
    
    // Process name/description (for debugging)
    char name[32];                // Process name string
    
    // Links for scheduler queues
    ProcessControlBlock* next;    // Next PCB in the queue
    ProcessControlBlock* prev;    // Previous PCB in the queue
    
    // Additional flags
    uint32 flags;                 // Additional process flags
};

// Process management constants
const uint32 INVALID_PID = 0xFFFFFFFF;
const uint32 KERNEL_PID = 0;      // PID for kernel process
const uint32 MIN_PID = 1;         // Minimum user process PID
const uint32 MAX_PID = 0xFFFF;    // Maximum process ID (keeping it reasonable)

// Process scheduling modes
enum SchedulingMode {
    SCHEDULING_MODE_COOPERATIVE = 0,  // Processes yield control voluntarily
    SCHEDULING_MODE_PREEMPTIVE,       // Scheduler forces context switches
    SCHEDULING_MODE_ROUND_ROBIN       // Round-robin scheduling with time slices
};

// Process management functions
class ProcessManager {
private:
    ProcessControlBlock* current_process;
    ProcessControlBlock* process_list_head;
    uint32 next_pid;
    SchedulingMode current_mode;  // Current scheduling mode
    
public:
    ProcessManager();
    ~ProcessManager();
    
    // Process creation and destruction
    ProcessControlBlock* CreateProcess(void* entry_point, const char* name, uint32 priority);
    bool DestroyProcess(uint32 pid);
    bool TerminateProcess(uint32 pid);
    
    // Process lifecycle
    ProcessControlBlock* GetProcessById(uint32 pid);
    ProcessControlBlock* GetCurrentProcess();
    uint32 GetNextPID();
    
    // Process state management
    bool SetProcessState(uint32 pid, ProcessState new_state);
    ProcessState GetProcessState(uint32 pid);
    
    // Process scheduling
    ProcessControlBlock* ScheduleNextProcess();  // For scheduler to select next process
    bool AddToReadyQueue(ProcessControlBlock* pcb);
    ProcessControlBlock* RemoveFromReadyQueue();
    
    // Scheduling mode control
    void SetSchedulingMode(SchedulingMode mode);
    SchedulingMode GetSchedulingMode();
    void Schedule();  // Main scheduler function called by timer interrupt
    
    // Process control
    bool YieldCurrentProcess();  // Allow current process to yield CPU
    bool SleepCurrentProcess(uint32 sleep_ticks);  // Put current process to sleep
    
    // Utility functions
    uint32 GetProcessCount();
    void PrintProcessList();  // For debugging
};

// Global process manager instance
extern ProcessManager* process_manager;

#endif