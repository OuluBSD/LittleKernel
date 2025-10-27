#ifndef _Kernel_ProcessControlBlock_h_
#define _Kernel_ProcessControlBlock_h_

#include "Defs.h"
#include "KernelConfig.h"  // Include the kernel configuration
#include "Paging.h"        // Include paging structures
#include "Thread.h"        // Include thread management structures
#include "RealTimeScheduling.h"  // Include RealTimeParams definition

// Process states
enum ProcessState {
    PROCESS_STATE_NEW = 0,        // Process has been created but not yet ready to run
    PROCESS_STATE_READY,          // Process is ready to run and waiting for CPU
    PROCESS_STATE_RUNNING,        // Process is currently running
    PROCESS_STATE_WAITING,        // Process is waiting for an event/synchronization object
    PROCESS_STATE_BLOCKED,        // Process is blocked (e.g., waiting for I/O)
    PROCESS_STATE_SUSPENDED,      // Process is suspended (e.g., by user or debugger)
    PROCESS_STATE_ZOMBIE,         // Process has terminated but parent hasn't collected exit code
    PROCESS_STATE_TERMINATED      // Process has completed execution
};

// Real-time scheduling parameters (already defined in RealTimeScheduling.h)
// struct RealTimeParams {
//     uint32 rt_policy;            // Real-time scheduling policy (FIFO, RR, etc.)
//     uint32 rt_priority;          // Real-time priority (1-99, where 1 is highest)
//     uint32 rt_period;            // Period in milliseconds for periodic tasks
//     uint32 rt_deadline;          // Deadline in milliseconds relative to release time
//     uint32 rt_execution_time;    // Worst-case execution time in milliseconds
//     uint32 rt_release_time;      // Release time in milliseconds
//     uint32 rt_budget;            // CPU time budget in milliseconds
//     uint32 rt_jitter_tolerance;  // Jitter tolerance in milliseconds
//     uint32 rt_phase_offset;      // Phase offset for periodic tasks
//     uint32 rt_criticality;       // Criticality level (0=non-critical, 1=critical)
//     uint32 rt_importance;        // Importance factor (0=lowest, higher=more important)
//     uint32 rt_affinity_mask;     // CPU affinity mask for multi-core systems
//     bool rt_is_periodic;         // Whether the task is periodic
//     bool rt_is_soft_realtime;    // Whether it's soft real-time (misses are tolerated)
//     bool rt_is_hard_realtime;    // Whether it's hard real-time (misses are catastrophic)
//     uint32 rt_reservation;       // Resource reservation in milliseconds
//     uint32 rt_utilization;       // CPU utilization percentage (0-100)
//     uint32 rt_response_time;     // Expected response time in milliseconds
//     uint32 rt_queueing_delay;    // Maximum queueing delay in milliseconds
//     uint32 rt_migration_cost;    // Cost of migrating to another CPU in milliseconds
// };

// Process control block structure
struct ProcessControlBlock {
    // Process identification
    uint32 pid;                    // Process ID
    uint32 parent_pid;            // Parent process ID
    uint32 uid;                   // User ID
    uint32 gid;                   // Group ID
    uint32 pgid;                  // Process group ID
    uint32 sid;                   // Session ID
    
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
    uint32 creation_time;         // Time when process was created
    uint32 termination_time;      // Time when process was terminated
    uint32 last_state_change;     // Time of last state change
    uint32 state_duration;        // Duration in current state
    
    // Advanced scheduling fields
    uint32 time_slice_remaining;   // Remaining time slice for current execution
    uint32 total_cpu_time_used;    // Total CPU time used by this process
    uint32 wait_time;              // Total time spent waiting
    uint32 response_time;         // Time from process creation to first run
    uint32 turnaround_time;       // Time from process creation to termination
    uint32 first_run_time;        // Time when process was first scheduled
    uint32 last_preemption_time;   // Time of last preemption
    uint32 preemption_count;      // Number of times process was preempted
    uint32 voluntary_yield_count; // Number of voluntary yields
    uint32 context_switch_count;   // Number of context switches for this process
    
    // MLFQ scheduling fields
    uint32 mlfq_level;             // Current MLFQ level (0 = highest priority)
    uint32 mlfq_time_slice;        // Time slice for current MLFQ level
    uint32 mlfq_total_time;        // Total time in current MLFQ level
    uint32 mlfq_boost_time;        // Next boost time for this process
    
    // Priority aging fields
    uint32 base_priority;          // Original priority assigned to process
    uint32 current_priority;      // Current effective priority (may be adjusted)
    uint32 priority_boost_count;   // Number of times priority was boosted
    uint32 last_priority_boost;    // Time of last priority boost
    
    // Fair-share scheduling fields
    uint32 user_id;               // User ID for fair-share scheduling
    uint32 group_id;              // Group ID for fair-share scheduling
    uint32 cpu_shares;            // CPU shares for fair-share scheduling
    uint32 cpu_quota_used;        // CPU quota used in current period
    uint32 cpu_quota_period;      // CPU quota period
    
    // Real-time scheduling fields
    uint32 rt_policy;             // Real-time scheduling policy (FIFO, RR, etc.)
    uint32 rt_priority;           // Real-time priority (1-99, where 1 is highest)
    uint32 rt_execution_time;     // Worst-case execution time in milliseconds
    uint32 rt_period;             // Period in milliseconds for periodic tasks
    uint32 rt_deadline;           // Deadline in milliseconds relative to release time
    uint32 rt_release_time;       // Release time in milliseconds
    uint32 rt_deadline_misses;    // Number of missed deadlines
    uint32 rt_completions;        // Number of successful completions
    uint32 rt_budget;             // CPU time budget in milliseconds
    uint32 rt_budget_used;        // CPU time budget used so far
    uint32 rt_budget_period;      // Budget period in milliseconds
    bool rt_is_periodic;          // Whether the task is periodic
    bool rt_is_soft_realtime;     // Whether it's soft real-time (misses are tolerated)
    bool rt_is_hard_realtime;     // Whether it's hard real-time (misses are catastrophic)
    uint32 rt_reservation;        // Resource reservation in milliseconds
    uint32 rt_utilization;        // CPU utilization percentage (0-100)
    uint32 rt_response_time;      // Expected response time in milliseconds
    uint32 rt_queueing_delay;     // Maximum queueing delay in milliseconds
    uint32 rt_migration_cost;     // Cost of migrating to another CPU in milliseconds
    uint32 rt_jitter_tolerance;   // Jitter tolerance in milliseconds
    uint32 rt_phase_offset;       // Phase offset for periodic tasks
    uint32 rt_relative_deadline;  // Relative deadline in milliseconds
    uint32 rt_criticality_level;  // Criticality level (0=non-critical, 1=critical)
    uint32 rt_importance_factor;  // Importance factor (0=lowest, higher=more important)
    uint32 rt_resource_requirements; // Resource requirements in milliseconds
    uint32 rt_affinity_mask;      // CPU affinity mask for multi-core systems
    
    // Synchronization primitives
    ProcessControlBlock* waiting_on_semaphore; // Next process in semaphore wait queue
    uint32* event_flags;          // Event flags for this process
    ProcessControlBlock* waiting_on_mutex;     // Next process in mutex wait queue
    ProcessControlBlock* waiting_on_event;     // Next process in event wait queue
    
    // Process state management enhancements
    ProcessState previous_state;   // Previous state for state history
    uint32 blocking_reason;        // Reason for blocking (e.g. semaphore, I/O operation)
    uint32 wait_timeout;           // Timeout for blocking operations (0 = no timeout)
    uint32 exit_code;              // Exit code when process terminates
    uint32 suspend_count;          // Number of times process is suspended (for nested suspend)
    
    // Inter-process communication
    uint32* message_queue;        // Messages waiting for this process
    uint32* opened_files;         // Array of file descriptors
    
    // Process name/description (for debugging)
    char name[32];                // Process name string
    
    // Links for scheduler queues
    ProcessControlBlock* next;    // Next PCB in the queue
    ProcessControlBlock* prev;    // Previous PCB in the queue
    
    // ABI-specific context data
    void* abi_context;            // ABI-specific context (AbiContext*)
    
    // Additional flags
    uint32 flags;                 // Additional process flags
};

// Process management constants
const uint32 INVALID_PID = 0xFFFFFFFF;
const uint32 INVALID_PGID = 0xFFFFFFFF;  // Invalid process group ID
const uint32 INVALID_SID = 0xFFFFFFFF;   // Invalid session ID
const uint32 KERNEL_PID = 0;      // PID for kernel process
const uint32 MIN_PID = 1;         // Minimum user process PID
const uint32 MAX_PID = 0xFFFF;    // Maximum process ID (keeping it reasonable)

// Real-time scheduling constants (already defined in RealTimeScheduling.h)
// const uint32 RT_SCHED_FIFO = 1;           // First-In, First-Out scheduling
// const uint32 RT_SCHED_RR = 2;             // Round-Robin scheduling
// const uint32 RT_SCHED_SPORADIC = 3;       // Sporadic server scheduling
// const uint32 RT_DEFAULT_PRIORITY = 50;    // Default real-time priority (1-99, where 1 is highest)
// const uint32 RT_MIN_PRIORITY = 1;         // Minimum real-time priority
// const uint32 RT_MAX_PRIORITY = 99;        // Maximum real-time priority
const uint32 RT_DEFAULT_PERIOD = 1000;    // Default period in milliseconds
const uint32 RT_DEFAULT_DEADLINE = 1000;  // Default deadline in milliseconds
const uint32 RT_DEFAULT_BUDGET = 500;     // Default budget in milliseconds
const uint32 RT_DEFAULT_JITTER = 10;      // Default jitter tolerance in milliseconds

// Process scheduling modes
enum SchedulingMode {
    SCHEDULING_MODE_COOPERATIVE = 0,  // Processes yield control voluntarily
    SCHEDULING_MODE_PREEMPTIVE,       // Scheduler forces context switches
    SCHEDULING_MODE_ROUND_ROBIN,      // Round-robin scheduling with time slices
    SCHEDULING_MODE_PRIORITY,          // Priority-based scheduling
    SCHEDULING_MODE_MLFQ,             // Multi-Level Feedback Queue scheduling
    SCHEDULING_MODE_FAIR_SHARE,       // Fair-share scheduling
    SCHEDULING_MODE_REALTIME          // Real-time scheduling
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
    
    // Enhanced state management
    bool TransitionProcessState(uint32 pid, ProcessState new_state);  // Validates state transitions
    ProcessState GetPreviousState(uint32 pid);
    uint32 GetStateDuration(uint32 pid);  // How long has process been in current state
    uint32 GetBlockingReason(uint32 pid);
    bool SetBlockingReason(uint32 pid, uint32 reason);
    bool SuspendProcess(uint32 pid);
    bool ResumeProcess(uint32 pid);
    bool BlockProcess(uint32 pid, uint32 reason);
    bool UnblockProcess(uint32 pid);
    bool WakeProcess(uint32 pid);
    bool SetProcessExitCode(uint32 pid, uint32 exit_code);
    uint32 GetProcessExitCode(uint32 pid);
    const char* GetProcessStateName(ProcessState state);
    const char* GetSchedulingModeName(SchedulingMode mode);
    void PrintProcessStateHistory(uint32 pid);
    
    // Process scheduling
    ProcessControlBlock* ScheduleNextProcess();  // For scheduler to select next process
    ProcessControlBlock* ScheduleNextProcessRR(); // Round-robin scheduling
    ProcessControlBlock* ScheduleNextProcessMLFQ(); // Multi-Level Feedback Queue scheduling
    ProcessControlBlock* ScheduleNextProcessFairShare(); // Fair-share scheduling
    ProcessControlBlock* ScheduleNextProcessRealtime(); // Real-time scheduling
    
    bool AddToReadyQueue(ProcessControlBlock* pcb);
    ProcessControlBlock* RemoveFromReadyQueue();
    
    // Advanced scheduling functions
    bool AdjustProcessPriority(uint32 pid, int adjustment); // Adjust process priority
    bool BoostStarvingProcesses(); // Boost priority of starving processes
    bool AgeProcessPriorities(); // Age priorities to prevent starvation
    bool UpdateMLFQLevels(); // Update MLFQ levels based on process behavior
    bool ApplyPriorityInheritance(ProcessControlBlock* blocked_process); // Priority inheritance
    bool RevertPriorityInheritance(ProcessControlBlock* unblocked_process); // Revert priority inheritance
    
    // Scheduling mode control
    void SetSchedulingMode(SchedulingMode mode);
    SchedulingMode GetSchedulingMode();
    void Schedule();  // Main scheduler function called by timer interrupt
    
    // Real-time scheduling support
    bool SetRealTimeParams(uint32 pid, const RealTimeParams* params);
    bool GetRealTimeParams(uint32 pid, RealTimeParams* params);
    bool UpdateRealTimeParams(uint32 pid, const RealTimeParams* params);
    bool IsProcessRealTime(uint32 pid);
    uint32 GetRealTimePriority(uint32 pid);
    bool SetRealTimePriority(uint32 pid, uint32 priority);
    uint32 GetProcessDeadline(uint32 pid);
    uint32 GetProcessPeriod(uint32 pid);
    uint32 GetProcessExecutionTime(uint32 pid);
    bool SetProcessDeadline(uint32 pid, uint32 deadline);
    bool SetProcessPeriod(uint32 pid, uint32 period);
    bool SetProcessExecutionTime(uint32 pid, uint32 execution_time);
    bool IsProcessPeriodic(uint32 pid);
    bool SetProcessPeriodic(uint32 pid, bool is_periodic);
    bool IsProcessSoftRealTime(uint32 pid);
    bool SetProcessSoftRealTime(uint32 pid, bool is_soft_realtime);
    bool IsProcessCritical(uint32 pid);
    bool SetProcessCritical(uint32 pid, bool is_critical);
    uint32 GetProcessBudget(uint32 pid);
    bool SetProcessBudget(uint32 pid, uint32 budget);
    uint32 GetProcessBudgetUsed(uint32 pid);
    bool SetProcessBudgetUsed(uint32 pid, uint32 budget_used);
    uint32 GetProcessBudgetPeriod(uint32 pid);
    bool SetProcessBudgetPeriod(uint32 pid, uint32 budget_period);
    uint32 GetProcessJitterTolerance(uint32 pid);
    bool SetProcessJitterTolerance(uint32 pid, uint32 jitter_tolerance);
    uint32 GetProcessPhaseOffset(uint32 pid);
    bool SetProcessPhaseOffset(uint32 pid, uint32 phase_offset);
    uint32 GetProcessRelativeDeadline(uint32 pid);
    bool SetProcessRelativeDeadline(uint32 pid, uint32 relative_deadline);
    uint32 GetProcessCriticalityLevel(uint32 pid);
    bool SetProcessCriticalityLevel(uint32 pid, uint32 criticality_level);
    uint32 GetProcessImportanceFactor(uint32 pid);
    bool SetProcessImportanceFactor(uint32 pid, uint32 importance_factor);
    uint32 GetProcessResourceRequirements(uint32 pid);
    bool SetProcessResourceRequirements(uint32 pid, uint32 resource_requirements);
    uint32 GetProcessAffinityMask(uint32 pid);
    bool SetProcessAffinityMask(uint32 pid, uint32 affinity_mask);
    
    // Real-time scheduling
    ProcessControlBlock* ScheduleNextRealTimeProcess();
    ProcessControlBlock* ScheduleNextFIFOProcess();
    ProcessControlBlock* ScheduleNextRRProcess();
    ProcessControlBlock* ScheduleNextEDFProcess();
    ProcessControlBlock* ScheduleNextRMProcess();
    ProcessControlBlock* ScheduleNextDeadlineProcess();
    ProcessControlBlock* ScheduleNextLSTProcess();
    ProcessControlBlock* ScheduleNextGSProcess();
    ProcessControlBlock* ScheduleNextCBSProcess();
    ProcessControlBlock* ScheduleNextDVSProcess();
    ProcessControlBlock* ScheduleNextDPSProcess();
    ProcessControlBlock* ScheduleNextAEProcess();
    ProcessControlBlock* ScheduleNextBGProcess();
    ProcessControlBlock* ScheduleNextIdleProcess();
    ProcessControlBlock* ScheduleNextCustomProcess();
    
    // Scheduling statistics and metrics
    uint32 GetAverageResponseTime();
    uint32 GetAverageTurnaroundTime();
    uint32 GetAverageWaitTime();
    uint32 GetContextSwitchCount();
    void PrintSchedulingStatistics();
    void ResetSchedulingStatistics();
    
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