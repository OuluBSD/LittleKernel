#ifndef _Kernel_RealTimeExtensions_h_
#define _Kernel_RealTimeExtensions_h_

#include "Defs.h"
#include "ProcessControlBlock.h"
#include "ProcessManager.h"
#include "Scheduler.h"

// Real-time scheduling policies
enum RealTimeSchedulingPolicy {
    RT_SCHED_FIFO = 0,     // First-In-First-Out real-time scheduling
    RT_SCHED_RR,           // Round-Robin real-time scheduling
    RT_SCHED_DEADLINE,     // Earliest Deadline First scheduling
    RT_SCHED_EDF,         // Earliest Deadline First
    RT_SCHED_RM,          // Rate Monotonic scheduling
    RT_SCHED_DM,          // Deadline Monotonic scheduling
    RT_SCHED_LST,         // Least Slack Time scheduling
    RT_SCHED_GS,          // Guaranteed Scheduling
    RT_SCHED_CBS,         // Constant Bandwidth Server scheduling
    RT_SCHED_DVS,         // Dynamic Voltage Scaling scheduling
    RT_SCHED_DPS,         // Dynamic Priority Scheduling
    RT_SCHED_AE,          // Aperiodic Events scheduling
    RT_SCHED_BG,          // Background scheduling
    RT_SCHED_IDLE,        // Idle scheduling
    RT_SCHED_CUSTOM       // Custom scheduling policy
};

// Real-time process parameters
struct RealTimeParams {
    RealTimeSchedulingPolicy policy;  // Scheduling policy
    uint32 priority;                   // Real-time priority (higher number = higher priority)
    uint32 execution_time;            // Worst-case execution time (WCET)
    uint32 period;                    // Period for periodic tasks (in ticks)
    uint32 deadline;                  // Absolute deadline (in ticks)
    uint32 release_time;              // Time when task becomes ready (in ticks)
    uint32 deadline_misses;           // Number of missed deadlines
    uint32 completions;               // Number of successful completions
    bool is_periodic;                 // Whether this is a periodic task
    bool is_soft_realtime;            // Whether this is soft real-time (misses allowed)
    uint32 budget;                    // CPU time budget for this task
    uint32 budget_used;               // CPU time used in current period
    uint32 budget_period;             // Budget replenishment period
};

// Real-time scheduler statistics
struct RealTimeSchedulerStats {
    uint32 total_deadline_misses;     // Total number of missed deadlines
    uint32 total_completions;         // Total number of task completions
    uint32 total_preemptions;          // Total number of preemptions
    uint32 total_context_switches;     // Total number of context switches
    uint32 max_latency;               // Maximum observed latency
    uint32 avg_latency;               // Average observed latency
    uint32 jitter;                    // Jitter in task execution times
    uint32 last_deadline_miss_time;    // Time of last deadline miss
    uint32 last_completion_time;      // Time of last completion
};

// Real-time task states
enum RealTimeTaskState {
    RT_TASK_STATE_INACTIVE = 0,       // Task is not active
    RT_TASK_STATE_READY,              // Task is ready to run
    RT_TASK_STATE_RUNNING,           // Task is currently running
    RT_TASK_STATE_WAITING,            // Task is waiting for resource/event
    RT_TASK_STATE_SUSPENDED,          // Task is suspended
    RT_TASK_STATE_COMPLETED,          // Task has completed execution
    RT_TASK_STATE_DEADLINE_MISSED     // Task missed its deadline
};

// Real-time scheduling constraints
const uint32 RT_MIN_PRIORITY = 1;      // Minimum real-time priority
const uint32 RT_MAX_PRIORITY = 99;      // Maximum real-time priority
const uint32 RT_DEFAULT_PRIORITY = 50; // Default real-time priority
const uint32 RT_QUANTUM_MIN = 1;        // Minimum time quantum (1ms)
const uint32 RT_QUANTUM_MAX = 1000;    // Maximum time quantum (1s)
const uint32 RT_QUANTUM_DEFAULT = 10;  // Default time quantum (10ms)

// Real-time scheduling flags
const uint32 RT_FLAG_CRITICAL = 0x00000001;     // Critical real-time task
const uint32 RT_FLAG_NON_PREEMPTABLE = 0x00000002; // Non-preemptable task
const uint32 RT_FLAG_APERIODIC = 0x00000004;    // Aperiodic task
const uint32 RT_FLAG_SPORADIC = 0x00000008;     // Sporadic task
const uint32 RT_FLAG_SERVER = 0x00000010;       // Server task

// Real-time scheduling class
class RealTimeSchedulerExtension {
private:
    RealTimeSchedulerStats stats;     // Scheduler statistics
    uint32 current_quantum;           // Current time quantum
    bool is_active;                   // Whether real-time scheduler is active
    uint32 next_activation_check;      // Next time to check task activations
    
public:
    RealTimeSchedulerExtension();
    ~RealTimeSchedulerExtension();
    
    // Real-time scheduling management
    bool Initialize();
    bool Activate();
    bool Deactivate();
    bool IsActive() const;
    
    // Real-time task management
    bool SetRealTimeParams(uint32 pid, const RealTimeParams* params);
    bool GetRealTimeParams(uint32 pid, RealTimeParams* params);
    bool UpdateRealTimeParams(uint32 pid, const RealTimeParams* params);
    
    // Real-time scheduling functions
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
    
    // Real-time task lifecycle management
    bool ActivatePeriodicTasks();
    bool CheckTaskDeadlines();
    bool HandleDeadlineMiss(ProcessControlBlock* task);
    bool ReplenishTaskBudget(ProcessControlBlock* task);
    bool UpdateTaskActivationTimes();
    
    // Real-time scheduling utilities
    bool IsProcessRealTime(uint32 pid);
    uint32 GetRealTimePriority(uint32 pid);
    bool SetRealTimePriority(uint32 pid, uint32 priority);
    uint32 GetProcessDeadline(uint32 pid);
    uint32 GetProcessPeriod(uint32 pid);
    uint32 GetProcessExecutionTime(uint32 pid);
    
    // Real-time scheduling algorithms
    bool IsHigherPriority(ProcessControlBlock* task1, ProcessControlBlock* task2);
    bool IsEarlierDeadline(ProcessControlBlock* task1, ProcessControlBlock* task2);
    bool IsHigherRate(ProcessControlBlock* task1, ProcessControlBlock* task2);
    bool IsFeasibleSchedule();
    
    // Real-time scheduling constraints
    bool EnforcePriorityInheritance(ProcessControlBlock* blocked_task);
    bool RevertPriorityInheritance(ProcessControlBlock* unblocked_task);
    bool PreventPriorityInversion();
    bool HandleCriticalSections();
    
    // Real-time scheduling statistics
    const RealTimeSchedulerStats* GetStatistics();
    void ResetStatistics();
    void UpdateStatistics();
    uint32 GetDeadlineMissCount();
    uint32 GetCompletionCount();
    uint32 GetAverageLatency();
    uint32 GetMaxLatency();
    uint32 GetJitter();
    
    // Real-time scheduling debugging
    void PrintRealTimeTaskList();
    void PrintRealTimeStatistics();
    void PrintSchedulingAnalysis();
    const char* GetRealTimePolicyName(RealTimeSchedulingPolicy policy);
    const char* GetRealTimeTaskStateName(RealTimeTaskState state);
    
    // Real-time scheduling utilities
    uint32 CalculateResponseTime(ProcessControlBlock* task);
    uint32 CalculateUtilization();
    bool IsSystemOverloaded();
    uint32 GetSystemUtilization();
    
    // Real-time scheduling time management
    uint32 GetCurrentTime();
    uint32 GetNextDeadline(ProcessControlBlock* task);
    uint32 GetNextActivation(ProcessControlBlock* task);
    bool IsTaskReady(ProcessControlBlock* task);
    bool IsTaskActive(ProcessControlBlock* task);
    
    // Real-time scheduling interrupts
    void OnTimerTick();
    void OnContextSwitch();
    void OnDeadlineMiss(ProcessControlBlock* task);
    void OnTaskCompletion(ProcessControlBlock* task);
    
    // Real-time scheduling configuration
    bool SetQuantum(uint32 quantum_ms);
    uint32 GetQuantum();
    bool SetSchedulingPolicy(RealTimeSchedulingPolicy policy);
    RealTimeSchedulingPolicy GetSchedulingPolicy();
    
    // Real-time scheduling validation
    bool ValidateRealTimeParams(const RealTimeParams* params);
    bool ValidateTaskSchedule(ProcessControlBlock* task);
    bool ValidateSystemSchedule();
};

// Global real-time scheduler extension instance
extern RealTimeSchedulerExtension* g_real_time_extension;

// Initialize real-time scheduling extension
bool InitializeRealTimeSchedulingExtension();

#endif // _Kernel_RealTimeExtensions_h_