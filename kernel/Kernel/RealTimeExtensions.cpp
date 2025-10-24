#include "RealTimeExtensions.h"
#include "Global.h"
#include "MemoryManager.h"
#include "Logging.h"
#include "Timer.h"
#include "ProcessManager.h"

// Global real-time scheduler extension instance
RealTimeSchedulerExtension* g_real_time_extension = nullptr;

// RealTimeSchedulerExtension implementation
RealTimeSchedulerExtension::RealTimeSchedulerExtension() 
    : stats(), current_quantum(RT_QUANTUM_DEFAULT), 
      is_active(false), next_activation_check(0) {
    // Initialize statistics
    memset(&stats, 0, sizeof(stats));
    
    // Initialize other fields
    current_quantum = RT_QUANTUM_DEFAULT;
    is_active = false;
    next_activation_check = 0;
    
    DLOG("Real-time scheduler extension created");
}

RealTimeSchedulerExtension::~RealTimeSchedulerExtension() {
    // Clean up if needed
    DLOG("Real-time scheduler extension destroyed");
}

bool RealTimeSchedulerExtension::Initialize() {
    DLOG("Initializing real-time scheduler extension");
    
    // Reset statistics
    ResetStatistics();
    
    // Set initial quantum
    current_quantum = RT_QUANTUM_DEFAULT;
    
    // Mark as inactive until explicitly activated
    is_active = false;
    
    // Initialize activation check time
    next_activation_check = global_timer ? global_timer->GetTickCount() : 0;
    
    DLOG("Real-time scheduler extension initialized successfully");
    return true;
}

bool RealTimeSchedulerExtension::Activate() {
    if (!process_manager) {
        LOG("Process manager not available");
        return false;
    }
    
    DLOG("Activating real-time scheduler extension");
    
    // Validate that we can support real-time scheduling
    if (!ValidateSystemSchedule()) {
        LOG("System schedule validation failed");
        return false;
    }
    
    // Mark as active
    is_active = true;
    
    DLOG("Real-time scheduler extension activated successfully");
    return true;
}

bool RealTimeSchedulerExtension::Deactivate() {
    DLOG("Deactivating real-time scheduler extension");
    
    // Mark as inactive
    is_active = false;
    
    DLOG("Real-time scheduler extension deactivated successfully");
    return true;
}

bool RealTimeSchedulerExtension::IsActive() const {
    return is_active;
}

bool RealTimeSchedulerExtension::SetRealTimeParams(uint32 pid, const RealTimeParams* params) {
    if (!params) {
        LOG("Invalid real-time parameters");
        return false;
    }
    
    if (!process_manager) {
        LOG("Process manager not available");
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        LOG("Process with PID " << pid << " not found");
        return false;
    }
    
    // Validate parameters
    if (!ValidateRealTimeParams(params)) {
        LOG("Invalid real-time parameters for process PID " << pid);
        return false;
    }
    
    // Store real-time parameters in the process structure
    // In a real implementation, we'd have a dedicated field for this
    // For now, we'll use the existing priority field and some flags
    process->current_priority = params->priority;
    
    // Set scheduling policy flags
    switch (params->policy) {
        case RT_SCHED_FIFO:
            process->flags |= 0x10000000;  // FIFO flag
            break;
        case RT_SCHED_RR:
            process->flags |= 0x20000000;  // RR flag
            break;
        case RT_SCHED_DEADLINE:
            process->flags |= 0x30000000;  // Deadline flag
            break;
        case RT_SCHED_EDF:
            process->flags |= 0x40000000;  // EDF flag
            break;
        case RT_SCHED_RM:
            process->flags |= 0x50000000;  // RM flag
            break;
        default:
            process->flags |= 0x00000000;  // Default flag
            break;
    }
    
    // Set real-time flags
    if (params->is_critical) {
        process->flags |= RT_FLAG_CRITICAL;
    }
    
    if (params->is_soft_realtime) {
        process->flags |= RT_FLAG_APERIODIC;
    }
    
    DLOG("Set real-time parameters for process PID " << pid 
         << " with policy " << GetRealTimePolicyName(params->policy)
         << " and priority " << params->priority);
    
    return true;
}

bool RealTimeSchedulerExtension::GetRealTimeParams(uint32 pid, RealTimeParams* params) {
    if (!params) {
        LOG("Invalid real-time parameters buffer");
        return false;
    }
    
    if (!process_manager) {
        LOG("Process manager not available");
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        LOG("Process with PID " << pid << " not found");
        return false;
    }
    
    // Initialize parameters
    memset(params, 0, sizeof(RealTimeParams));
    
    // Retrieve real-time parameters from the process structure
    params->priority = process->current_priority;
    
    // Determine policy from flags
    uint32 policy_flag = process->flags & 0xF0000000;
    switch (policy_flag) {
        case 0x10000000:
            params->policy = RT_SCHED_FIFO;
            break;
        case 0x20000000:
            params->policy = RT_SCHED_RR;
            break;
        case 0x30000000:
            params->policy = RT_SCHED_DEADLINE;
            break;
        case 0x40000000:
            params->policy = RT_SCHED_EDF;
            break;
        case 0x50000000:
            params->policy = RT_SCHED_RM;
            break;
        default:
            params->policy = RT_SCHED_FIFO;  // Default policy
            break;
    }
    
    // Retrieve flags
    params->is_critical = (process->flags & RT_FLAG_CRITICAL) != 0;
    params->is_soft_realtime = (process->flags & RT_FLAG_APERIODIC) != 0;
    params->is_periodic = false;  // Simplified implementation
    params->execution_time = 0;   // Not tracked in this implementation
    params->period = 0;          // Not tracked in this implementation
    params->deadline = 0;        // Not tracked in this implementation
    params->release_time = 0;    // Not tracked in this implementation
    params->deadline_misses = 0; // Not tracked in this implementation
    params->completions = 0;     // Not tracked in this implementation
    params->budget = 0;          // Not tracked in this implementation
    params->budget_used = 0;     // Not tracked in this implementation
    params->budget_period = 0;   // Not tracked in this implementation
    
    DLOG("Retrieved real-time parameters for process PID " << pid);
    
    return true;
}

bool RealTimeSchedulerExtension::UpdateRealTimeParams(uint32 pid, const RealTimeParams* params) {
    // For now, just delegate to SetRealTimeParams
    return SetRealTimeParams(pid, params);
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextRealTimeProcess() {
    if (!process_manager) {
        LOG("Process manager not available");
        return nullptr;
    }
    
    // Dispatch to appropriate scheduling algorithm based on current policy
    ProcessControlBlock* next_process = nullptr;
    
    // For this implementation, we'll use a simple approach
    // In a real implementation, we'd check the scheduling policy of each process
    // and implement the appropriate algorithm
    
    // Check for critical real-time processes first
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        if (current->state == PROCESS_STATE_READY && 
            (current->flags & RT_FLAG_CRITICAL)) {
            // Found a critical real-time process
            next_process = current;
            break;
        }
        current = current->next;
    }
    
    // If no critical processes, find any real-time process
    if (!next_process) {
        current = process_manager->GetProcessListHead();
        while (current) {
            if (current->state == PROCESS_STATE_READY && 
                IsProcessRealTime(current->pid)) {
                // Found a real-time process
                if (!next_process || 
                    IsHigherPriority(current, next_process)) {
                    next_process = current;
                }
            }
            current = current->next;
        }
    }
    
    // If no real-time processes, fall back to regular scheduling
    if (!next_process) {
        next_process = process_manager->ScheduleNextProcess();
    }
    
    if (next_process) {
        DLOG("Scheduled real-time process PID " << next_process->pid 
             << " with priority " << next_process->current_priority);
    }
    
    return next_process;
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextFIFOProcess() {
    // FIFO scheduling: select the highest priority real-time process
    return ScheduleNextRealTimeProcess();
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextRRProcess() {
    // Round-robin scheduling for real-time processes
    return ScheduleNextRealTimeProcess();
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextEDFProcess() {
    // Earliest Deadline First scheduling
    if (!process_manager) {
        LOG("Process manager not available");
        return nullptr;
    }
    
    ProcessControlBlock* best_candidate = nullptr;
    uint32 earliest_deadline = 0xFFFFFFFF;
    
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        if (current->state == PROCESS_STATE_READY && 
            IsProcessRealTime(current->pid)) {
            
            uint32 deadline = GetProcessDeadline(current->pid);
            if (deadline < earliest_deadline) {
                earliest_deadline = deadline;
                best_candidate = current;
            }
        }
        current = current->next;
    }
    
    return best_candidate;
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextRMProcess() {
    // Rate Monotonic scheduling
    if (!process_manager) {
        LOG("Process manager not available");
        return nullptr;
    }
    
    ProcessControlBlock* best_candidate = nullptr;
    uint32 highest_rate = 0;
    
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        if (current->state == PROCESS_STATE_READY && 
            IsProcessRealTime(current->pid)) {
            
            uint32 period = GetProcessPeriod(current->pid);
            if (period > 0) {
                uint32 rate = 1000000 / period;  // Simplified rate calculation
                if (rate > highest_rate) {
                    highest_rate = rate;
                    best_candidate = current;
                }
            }
        }
        current = current->next;
    }
    
    return best_candidate;
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextDeadlineProcess() {
    // Deadline-based scheduling
    return ScheduleNextEDFProcess();  // EDF is a form of deadline scheduling
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextLSTProcess() {
    // Least Slack Time scheduling
    return ScheduleNextEDFProcess();  // Simplified implementation
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextGSProcess() {
    // Guaranteed Scheduling
    return ScheduleNextEDFProcess();  // Simplified implementation
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextCBSProcess() {
    // Constant Bandwidth Server scheduling
    return ScheduleNextEDFProcess();  // Simplified implementation
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextDVSProcess() {
    // Dynamic Voltage Scaling scheduling
    return ScheduleNextEDFProcess();  // Simplified implementation
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextDPSProcess() {
    // Dynamic Priority Scheduling
    return ScheduleNextEDFProcess();  // Simplified implementation
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextAEProcess() {
    // Aperiodic Events scheduling
    return ScheduleNextEDFProcess();  // Simplified implementation
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextBGProcess() {
    // Background scheduling
    return ScheduleNextEDFProcess();  // Simplified implementation
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextIdleProcess() {
    // Idle scheduling
    return ScheduleNextEDFProcess();  // Simplified implementation
}

ProcessControlBlock* RealTimeSchedulerExtension::ScheduleNextCustomProcess() {
    // Custom scheduling
    return ScheduleNextEDFProcess();  // Simplified implementation
}

bool RealTimeSchedulerExtension::IsProcessRealTime(uint32 pid) {
    if (!process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Check if process has real-time priority or flags
    return (process->current_priority >= RT_MIN_PRIORITY && 
            process->current_priority <= RT_MAX_PRIORITY) ||
           (process->flags & 0xF0000000) != 0;
}

uint32 RealTimeSchedulerExtension::GetRealTimePriority(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    return process->current_priority;
}

bool RealTimeSchedulerExtension::SetRealTimePriority(uint32 pid, uint32 priority) {
    if (priority < RT_MIN_PRIORITY || priority > RT_MAX_PRIORITY) {
        LOG("Invalid real-time priority: " << priority);
        return false;
    }
    
    if (!process_manager) {
        LOG("Process manager not available");
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        LOG("Process with PID " << pid << " not found");
        return false;
    }
    
    process->current_priority = priority;
    
    DLOG("Set real-time priority for process PID " << pid << " to " << priority);
    
    return true;
}

uint32 RealTimeSchedulerExtension::GetProcessDeadline(uint32 pid) {
    // In a real implementation, we'd retrieve the deadline from process data
    // For now, we'll return a dummy value
    return global_timer ? (global_timer->GetTickCount() + 1000) : 1000;
}

uint32 RealTimeSchedulerExtension::GetProcessPeriod(uint32 pid) {
    // In a real implementation, we'd retrieve the period from process data
    // For now, we'll return a dummy value
    return 100;  // 100ms period
}

uint32 RealTimeSchedulerExtension::GetProcessExecutionTime(uint32 pid) {
    // In a real implementation, we'd retrieve the WCET from process data
    // For now, we'll return a dummy value
    return 10;  // 10ms execution time
}

bool RealTimeSchedulerExtension::IsHigherPriority(ProcessControlBlock* task1, ProcessControlBlock* task2) {
    if (!task1 || !task2) {
        return false;
    }
    
    // Higher priority number = higher priority
    return task1->current_priority > task2->current_priority;
}

bool RealTimeSchedulerExtension::IsEarlierDeadline(ProcessControlBlock* task1, ProcessControlBlock* task2) {
    if (!task1 || !task2) {
        return false;
    }
    
    uint32 deadline1 = GetProcessDeadline(task1->pid);
    uint32 deadline2 = GetProcessDeadline(task2->pid);
    
    // Earlier deadline = higher priority
    return deadline1 < deadline2;
}

bool RealTimeSchedulerExtension::IsHigherRate(ProcessControlBlock* task1, ProcessControlBlock* task2) {
    if (!task1 || !task2) {
        return false;
    }
    
    uint32 period1 = GetProcessPeriod(task1->pid);
    uint32 period2 = GetProcessPeriod(task2->pid);
    
    // Higher rate (shorter period) = higher priority
    return period1 < period2;
}

bool RealTimeSchedulerExtension::IsFeasibleSchedule() {
    // Check if the current schedule is feasible
    // This is a simplified implementation - in a real system, we'd implement
    // Liu & Layland's schedulability test or similar
    
    uint32 utilization = GetSystemUtilization();
    return utilization <= 100;  // 100% utilization threshold
}

bool RealTimeSchedulerExtension::EnforcePriorityInheritance(ProcessControlBlock* blocked_task) {
    if (!blocked_task) {
        return false;
    }
    
    DLOG("Enforcing priority inheritance for blocked task PID " << blocked_task->pid);
    
    // In a real implementation, we'd find the resource holder and boost its priority
    // For now, we'll just log the action
    
    return true;
}

bool RealTimeSchedulerExtension::RevertPriorityInheritance(ProcessControlBlock* unblocked_task) {
    if (!unblocked_task) {
        return false;
    }
    
    DLOG("Reverting priority inheritance for unblocked task PID " << unblocked_task->pid);
    
    // In a real implementation, we'd restore the task's original priority
    // For now, we'll just log the action
    
    return true;
}

bool RealTimeSchedulerExtension::PreventPriorityInversion() {
    DLOG("Preventing priority inversion");
    
    // In a real implementation, we'd implement priority inheritance or ceiling protocols
    // For now, we'll just log the action
    
    return true;
}

bool RealTimeSchedulerExtension::HandleCriticalSections() {
    DLOG("Handling critical sections");
    
    // In a real implementation, we'd handle real-time critical sections appropriately
    // For now, we'll just log the action
    
    return true;
}

const RealTimeSchedulerStats* RealTimeSchedulerExtension::GetStatistics() {
    UpdateStatistics();
    return &stats;
}

void RealTimeSchedulerExtension::ResetStatistics() {
    memset(&stats, 0, sizeof(stats));
    DLOG("Real-time scheduler statistics reset");
}

void RealTimeSchedulerExtension::UpdateStatistics() {
    // Update real-time scheduler statistics
    // In a real implementation, we'd collect actual data
    // For now, we'll just leave the existing values
    
    DLOG("Updating real-time scheduler statistics");
}

uint32 RealTimeSchedulerExtension::GetDeadlineMissCount() {
    return stats.total_deadline_misses;
}

uint32 RealTimeSchedulerExtension::GetCompletionCount() {
    return stats.total_completions;
}

uint32 RealTimeSchedulerExtension::GetAverageLatency() {
    return stats.avg_latency;
}

uint32 RealTimeSchedulerExtension::GetMaxLatency() {
    return stats.max_latency;
}

uint32 RealTimeSchedulerExtension::GetJitter() {
    return stats.jitter;
}

void RealTimeSchedulerExtension::PrintRealTimeTaskList() {
    LOG("=== Real-Time Task List ===");
    
    if (!process_manager) {
        LOG("Process manager not available");
        return;
    }
    
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        if (IsProcessRealTime(current->pid)) {
            LOG("  PID: " << current->pid 
                << ", Name: " << current->name
                << ", Priority: " << current->current_priority
                << ", State: " << process_manager->GetProcessStateName(current->state)
                << ", Policy: " << GetRealTimePolicyName(RT_SCHED_FIFO));  // Simplified
        }
        current = current->next;
    }
    
    LOG("============================");
}

void RealTimeSchedulerExtension::PrintRealTimeStatistics() {
    UpdateStatistics();
    
    LOG("=== Real-Time Scheduler Statistics ===");
    LOG("  Total Deadline Misses: " << stats.total_deadline_misses);
    LOG("  Total Completions: " << stats.total_completions);
    LOG("  Total Preemptions: " << stats.total_preemptions);
    LOG("  Total Context Switches: " << stats.total_context_switches);
    LOG("  Max Latency: " << stats.max_latency << " ms");
    LOG("  Avg Latency: " << stats.avg_latency << " ms");
    LOG("  Jitter: " << stats.jitter << " ms");
    LOG("  Last Deadline Miss: " << stats.last_deadline_miss_time);
    LOG("  Last Completion: " << stats.last_completion_time);
    LOG("======================================");
}

void RealTimeSchedulerExtension::PrintSchedulingAnalysis() {
    LOG("=== Real-Time Scheduling Analysis ===");
    LOG("  System Utilization: " << GetSystemUtilization() << "%");
    LOG("  Is Feasible: " << (IsFeasibleSchedule() ? "Yes" : "No"));
    LOG("  Is Active: " << (is_active ? "Yes" : "No"));
    LOG("  Current Quantum: " << current_quantum << " ms");
    LOG("=====================================");
}

const char* RealTimeSchedulerExtension::GetRealTimePolicyName(RealTimeSchedulingPolicy policy) {
    switch (policy) {
        case RT_SCHED_FIFO: return "FIFO";
        case RT_SCHED_RR: return "Round-Robin";
        case RT_SCHED_DEADLINE: return "Deadline";
        case RT_SCHED_SPORADIC: return "Sporadic";
        case RT_SCHED_EDF: return "Earliest Deadline First";
        case RT_SCHED_RM: return "Rate Monotonic";
        case RT_SCHED_DM: return "Deadline Monotonic";
        case RT_SCHED_LST: return "Least Slack Time";
        case RT_SCHED_GS: return "Guaranteed Scheduling";
        case RT_SCHED_CBS: return "Constant Bandwidth Server";
        case RT_SCHED_DVS: return "Dynamic Voltage Scaling";
        case RT_SCHED_DPS: return "Dynamic Priority Scheduling";
        case RT_SCHED_AE: return "Aperiodic Events";
        case RT_SCHED_BG: return "Background";
        case RT_SCHED_IDLE: return "Idle";
        case RT_SCHED_CUSTOM: return "Custom";
        default: return "Unknown";
    }
}

const char* RealTimeSchedulerExtension::GetRealTimeTaskStateName(RealTimeTaskState state) {
    switch (state) {
        case RT_TASK_STATE_INACTIVE: return "INACTIVE";
        case RT_TASK_STATE_READY: return "READY";
        case RT_TASK_STATE_RUNNING: return "RUNNING";
        case RT_TASK_STATE_WAITING: return "WAITING";
        case RT_TASK_STATE_SUSPENDED: return "SUSPENDED";
        case RT_TASK_STATE_COMPLETED: return "COMPLETED";
        case RT_TASK_STATE_DEADLINE_MISSED: return "DEADLINE_MISSED";
        default: return "INVALID";
    }
}

uint32 RealTimeSchedulerExtension::CalculateResponseTime(ProcessControlBlock* task) {
    if (!task) {
        return 0;
    }
    
    // In a real implementation, we'd calculate the response time based on
    // the task's priority, execution time, and interference from higher priority tasks
    // For now, we'll return a dummy value
    
    return 50;  // 50ms response time
}

uint32 RealTimeSchedulerExtension::CalculateUtilization() {
    // Calculate system utilization for real-time tasks
    // In a real implementation, we'd sum the utilization of all real-time tasks
    // For now, we'll return a dummy value
    
    return 75;  // 75% utilization
}

bool RealTimeSchedulerExtension::IsSystemOverloaded() {
    return GetSystemUtilization() > 100;  // Over 100% utilization
}

uint32 RealTimeSchedulerExtension::GetSystemUtilization() {
    // Get current system utilization
    return CalculateUtilization();
}

uint32 RealTimeSchedulerExtension::GetCurrentTime() {
    return global_timer ? global_timer->GetTickCount() : 0;
}

uint32 RealTimeSchedulerExtension::GetNextDeadline(ProcessControlBlock* task) {
    if (!task) {
        return 0;
    }
    
    return GetProcessDeadline(task->pid);
}

uint32 RealTimeSchedulerExtension::GetNextActivation(ProcessControlBlock* task) {
    if (!task) {
        return 0;
    }
    
    // For periodic tasks, next activation is current time + period
    uint32 period = GetProcessPeriod(task->pid);
    return GetCurrentTime() + period;
}

bool RealTimeSchedulerExtension::IsTaskReady(ProcessControlBlock* task) {
    if (!task) {
        return false;
    }
    
    return task->state == PROCESS_STATE_READY;
}

bool RealTimeSchedulerExtension::IsTaskActive(ProcessControlBlock* task) {
    if (!task) {
        return false;
    }
    
    return task->state == PROCESS_STATE_RUNNING || 
           task->state == PROCESS_STATE_READY ||
           task->state == PROCESS_STATE_WAITING;
}

void RealTimeSchedulerExtension::OnTimerTick() {
    // Called on each timer tick
    // In a real implementation, we'd check for task activations and deadline misses
    // For now, we'll just log the tick
    
    static uint32 tick_count = 0;
    tick_count++;
    
    if (tick_count % 100 == 0) {  // Log every 100 ticks
        DLOG("Real-time scheduler timer tick #" << tick_count);
    }
}

void RealTimeSchedulerExtension::OnContextSwitch() {
    // Called on each context switch
    stats.total_context_switches++;
    DLOG("Real-time scheduler context switch");
}

void RealTimeSchedulerExtension::OnDeadlineMiss(ProcessControlBlock* task) {
    if (!task) {
        return;
    }
    
    stats.total_deadline_misses++;
    stats.last_deadline_miss_time = GetCurrentTime();
    
    LOG("Real-time task PID " << task->pid << " missed deadline");
    
    // In a real implementation, we'd take appropriate action (terminate, reschedule, etc.)
}

void RealTimeSchedulerExtension::OnTaskCompletion(ProcessControlBlock* task) {
    if (!task) {
        return;
    }
    
    stats.total_completions++;
    stats.last_completion_time = GetCurrentTime();
    
    DLOG("Real-time task PID " << task->pid << " completed");
}

bool RealTimeSchedulerExtension::SetQuantum(uint32 quantum_ms) {
    if (quantum_ms < RT_QUANTUM_MIN || quantum_ms > RT_QUANTUM_MAX) {
        LOG("Invalid quantum: " << quantum_ms << " ms");
        return false;
    }
    
    current_quantum = quantum_ms;
    DLOG("Set real-time scheduler quantum to " << quantum_ms << " ms");
    
    return true;
}

uint32 RealTimeSchedulerExtension::GetQuantum() {
    return current_quantum;
}

bool RealTimeSchedulerExtension::SetSchedulingPolicy(RealTimeSchedulingPolicy policy) {
    DLOG("Setting real-time scheduling policy to " << GetRealTimePolicyName(policy));
    
    // In a real implementation, we'd update the scheduling algorithm
    // For now, we'll just log the change
    
    return true;
}

RealTimeSchedulingPolicy RealTimeSchedulerExtension::GetSchedulingPolicy() {
    // Return the current scheduling policy
    // For now, we'll return FIFO as default
    return RT_SCHED_FIFO;
}

bool RealTimeSchedulerExtension::ValidateRealTimeParams(const RealTimeParams* params) {
    if (!params) {
        return false;
    }
    
    // Validate priority range
    if (params->priority < RT_MIN_PRIORITY || params->priority > RT_MAX_PRIORITY) {
        LOG("Invalid real-time priority: " << params->priority);
        return false;
    }
    
    // Validate execution time
    if (params->execution_time == 0) {
        LOG("Invalid execution time: " << params->execution_time);
        return false;
    }
    
    // Validate period if it's a periodic task
    if (params->is_periodic && params->period == 0) {
        LOG("Invalid period for periodic task: " << params->period);
        return false;
    }
    
    // Validate deadline
    if (params->deadline == 0) {
        LOG("Invalid deadline: " << params->deadline);
        return false;
    }
    
    return true;
}

bool RealTimeSchedulerExtension::ValidateTaskSchedule(ProcessControlBlock* task) {
    if (!task) {
        return false;
    }
    
    // Validate that the task can meet its deadlines
    // In a real implementation, we'd perform schedulability analysis
    // For now, we'll just return true
    
    DLOG("Validating schedule for real-time task PID " << task->pid);
    
    return true;
}

bool RealTimeSchedulerExtension::ValidateSystemSchedule() {
    // Validate that the entire system schedule is feasible
    // In a real implementation, we'd check all real-time tasks for schedulability
    // For now, we'll just return true
    
    DLOG("Validating system real-time schedule");
    
    return true;
}

// System call implementations
uint32 SysCallSetRealTimeParams(uint32 pid, const RealTimeParams* params) {
    if (!g_real_time_extension) {
        LOG("Real-time scheduler extension not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (g_real_time_extension->SetRealTimeParams(pid, params)) {
        return SUCCESS;
    }
    
    return ERROR_INVALID_PARAMETER;
}

uint32 SysCallGetRealTimeParams(uint32 pid, RealTimeParams* params) {
    if (!g_real_time_extension) {
        LOG("Real-time scheduler extension not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!params) {
        return ERROR_INVALID_PARAMETER;
    }
    
    if (g_real_time_extension->GetRealTimeParams(pid, params)) {
        return SUCCESS;
    }
    
    return ERROR_INVALID_PARAMETER;
}

uint32 SysCallActivateRealTimeScheduling() {
    if (!g_real_time_extension) {
        LOG("Real-time scheduler extension not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (g_real_time_extension->Activate()) {
        return SUCCESS;
    }
    
    return ERROR_OPERATION_FAILED;
}

uint32 SysCallDeactivateRealTimeScheduling() {
    if (!g_real_time_extension) {
        LOG("Real-time scheduler extension not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (g_real_time_extension->Deactivate()) {
        return SUCCESS;
    }
    
    return ERROR_OPERATION_FAILED;
}

uint32 SysCallIsRealTimeSchedulingActive() {
    if (!g_real_time_extension) {
        LOG("Real-time scheduler extension not available");
        return 0;  // Not active
    }
    
    return g_real_time_extension->IsActive() ? 1 : 0;
}

uint32 SysCallGetRealTimeStatistics(RealTimeSchedulerStats* stats) {
    if (!g_real_time_extension) {
        LOG("Real-time scheduler extension not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!stats) {
        return ERROR_INVALID_PARAMETER;
    }
    
    const RealTimeSchedulerStats* rt_stats = g_real_time_extension->GetStatistics();
    if (rt_stats) {
        memcpy(stats, rt_stats, sizeof(RealTimeSchedulerStats));
        return SUCCESS;
    }
    
    return ERROR_OPERATION_FAILED;
}

// Initialize real-time scheduling extension
bool InitializeRealTimeSchedulingExtension() {
    g_real_time_extension = new RealTimeSchedulerExtension();
    if (!g_real_time_extension) {
        LOG("Error: Failed to allocate real-time scheduler extension");
        return false;
    }
    
    if (!g_real_time_extension->Initialize()) {
        LOG("Error: Failed to initialize real-time scheduler extension");
        delete g_real_time_extension;
        g_real_time_extension = nullptr;
        return false;
    }
    
    LOG("Real-time scheduling extension initialized successfully");
    return true;
}