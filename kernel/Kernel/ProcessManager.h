#ifndef _Kernel_ProcessManager_h_
#define _Kernel_ProcessManager_h_

#include "Common.h"
#include "ProcessControlBlock.h"
#include "Scheduling.h"

// Process creation flags
#define PROCESS_CREATE_DEFAULT 0x0000
#define PROCESS_CREATE_SUSPENDED 0x0001
#define PROCESS_CREATE_DETACHED 0x0002

// Process termination flags
#define PROCESS_TERMINATE_NORMAL 0x0000
#define PROCESS_TERMINATE_FORCE 0x0001

// Process states
enum ProcessState {
    PROCESS_STATE_NEW = 0,
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_WAITING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_SUSPENDED,
    PROCESS_STATE_ZOMBIE,
    PROCESS_STATE_TERMINATED
};

// Process scheduling policies
enum SchedulingPolicy {
    SCHED_POLICY_DEFAULT = 0,
    SCHED_POLICY_FIFO,
    SCHED_POLICY_RR,
    SCHED_POLICY_OTHER
};

// Process manager class
class ProcessManager {
private:
    ProcessControlBlock* process_list_head;
    ProcessControlBlock* current_process;
    uint32 next_pid;
    Spinlock process_lock;
    
public:
    ProcessManager();
    ~ProcessManager();
    
    // Process creation and destruction
    ProcessControlBlock* CreateProcess(void* entry_point, const char* name, uint32 priority);
    bool DestroyProcess(uint32 pid);
    bool TerminateProcess(uint32 pid, int exit_code = 0);
    
    // Process lookup
    ProcessControlBlock* GetProcessById(uint32 pid);
    ProcessControlBlock* GetCurrentProcess();
    uint32 GetNextPID();
    
    // Process state management
    bool SetProcessState(uint32 pid, ProcessState new_state);
    ProcessState GetProcessState(uint32 pid);
    bool SuspendProcess(uint32 pid);
    bool ResumeProcess(uint32 pid);
    
    // Process scheduling
    ProcessControlBlock* ScheduleNextProcess();
    void YieldCurrentProcess();
    
    // Process lifecycle
    void RunProcess(ProcessControlBlock* pcb);
    void ExitCurrentProcess(int exit_code);
    
    // Process synchronization
    int WaitForProcess(uint32 pid, int* status, uint32 options);
    bool KillProcess(uint32 pid, int signal);
    
    // Memory management for processes
    bool AllocateProcessMemory(ProcessControlBlock* pcb, uint32 size);
    bool FreeProcessMemory(ProcessControlBlock* pcb);
    
    // Utility functions
    uint32 GetProcessCount();
    void PrintProcessList();
    
    // Scheduling-related functions
    ProcessControlBlock* ScheduleNextProcessRR(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessMLFQ(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessFairShare(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessRealtime(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessFIFO(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessEDF(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessRM(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessDM(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessLST(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessGS(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessCBS(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessDVS(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessDPS(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessAE(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessBG(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessIdle(ProcessControlBlock* current);
    ProcessControlBlock* ScheduleNextProcessCustom(ProcessControlBlock* current);
    
    // Statistics and metrics
    uint32 GetTotalProcessCount();
    uint32 GetTotalContextSwitches();
    ProcessState GetCurrentProcessState();
    
    // Priority management
    bool UpdateMLFQLevels();
    bool BoostStarvingProcesses();
    bool AgeProcessPriorities();
    
    // Deadline handling
    bool HandleDeadlineMiss(ProcessControlBlock* task);
    bool ReplenishTaskBudget(ProcessControlBlock* task);
    bool IsHigherPriority(ProcessControlBlock* task1, ProcessControlBlock* task2);
};

// Global process manager instance
extern ProcessManager* process_manager;

// Initialize the process manager
bool InitializeProcessManager();

#endif