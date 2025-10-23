#ifndef _Kernel_ProcessControlBlock_h_
#define _Kernel_ProcessControlBlock_h_

#include "Defs.h"
#include "KernelConfig.h"  // Include the kernel configuration

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
    uint32* page_directory;       // Page directory for this process
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
    uint32* waiting_on_semaphore; // Semaphore this process is waiting on
    uint32* event_flags;          // Event flags for this process
    
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

// Process management functions
class ProcessManager {
private:
    ProcessControlBlock* current_process;
    ProcessControlBlock* process_list_head;
    uint32 next_pid;
    
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