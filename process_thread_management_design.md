# Process and Thread Management Design

## Overview
This document outlines the design of process and thread management system that supports both cooperative and preemptive scheduling. The system will follow Ultimate++ and Windows naming conventions while maintaining compatibility with Windows 98 features and modern OS concepts.

## Naming Conventions
- Class names use PascalCase (e.g., ProcessManager, ThreadScheduler)
- Function names follow Windows/Ultimate++ style (e.g., CreateProcess, CreateThread)
- Variable names use lowercase with underscores (e.g., process_id, thread_state)
- Base classes use Base suffix (e.g., SchedulerBase, ProcessBase)
- Macros use UPPER_CASE (e.g., MAX_PROCESS_COUNT, KERNEL_STACK_SIZE)

## Architecture Overview

### Core Components
```
+----------------------+
|   Process Manager    |
|   (Process Control)  |
+----------------------+
|   Thread Scheduler   |
|   (Scheduling)       |
+----------------------+
|   Task Manager       |
|   (Context Switch)   |
+----------------------+
|   IPC Manager        |
|   (Communication)    |
+----------------------+
```

## 1. Process Management

### Process Structure
```cpp
class Process {
public:
    int32 pid;                           // Process ID
    int32 parent_pid;                    // Parent Process ID
    ProcessState state;                  // Running, Waiting, Suspended, etc.
    uint32 esp, ebp;                    // Stack pointers
    uint32 eip;                         // Instruction pointer
    uint32 kernel_stack;                // Kernel stack location
    PageDirectory* page_directory;      // Virtual memory space
    ProcessType type;                   // User, Kernel, System, DOS, Win16
    Process* next, *prev;               // Linked list pointers
    ProcessContext context;             // CPU registers save area
    Vector<Thread*> threads;            // Associated threads
    Vector<Handle> handles;             // Open handles to resources
    SecurityContext security;           // Security tokens and privileges
    ProcessFlags flags;                 // Various process flags
    uint32 exit_code;                   // Exit status
    Process* children_list;             // List of child processes
    Process* sibling_next;              // Next sibling in children list
    
    Process();
    ~Process();
    
    bool Initialize(uint32 entry_point, uint32 stack_size);
    void Terminate(uint32 exit_status);
    Process* Fork();
    bool Exec(const char* filename, char* argv[], char* envp[]);
};
```

### Process Manager
```cpp
class ProcessManager {
private:
    volatile Process* current_process;   // Currently running process
    volatile Process* ready_queue;       // Ready to run processes
    volatile Process* wait_queue;        // Waiting processes
    volatile Process* zombie_queue;      // Terminated but not cleaned up
    int32 next_pid;                     // Next available process ID
    int32 process_count;                // Total number of processes
    spinlock lock;                      // For thread-safe access
    
public:
    Process* CreateProcess(uint32 entry_point, uint32 stack_size, 
                          ProcessType type);
    Process* CreateProcessFromBinary(const char* filename, 
                                    char* argv[], char* envp[]);
    bool DestroyProcess(Process* process);
    Process* GetProcessById(int32 pid);
    Process* GetCurrentProcess();
    int32 GetNextPid();
    void ScheduleNext();
    void BlockProcess(Process* process);
    void WakeupProcess(Process* process);
    void ExitProcess(uint32 exit_code);
    void InitializeProcessManager();
    Process* ForkCurrentProcess();
    int32 WaitChildProcess(int32 pid, uint32* exit_status, int32 options);
};
```

## 2. Thread Management

### Thread Structure
```cpp
class Thread {
public:
    int32 tid;                          // Thread ID
    int32 process_id;                   // Owning process ID
    ThreadState state;                  // Running, Waiting, Suspended, etc.
    uint32 esp, ebp;                   // Stack pointers
    uint32 eip;                        // Instruction pointer
    uint32 user_stack;                 // User stack location
    uint32 kernel_stack;               // Kernel stack location
    ThreadPriority priority;           // Scheduling priority
    uint32 remaining_quantum;          // Remaining time slice
    Thread* next, *prev;               // Linked list pointers
    ThreadContext context;             // CPU registers save area
    SecurityContext security;          // Thread security context
    ThreadFlags flags;                 // Various thread flags
    uint32 exit_code;                  // Exit status
    
    Thread();
    ~Thread();
    
    bool Initialize(uint32 start_routine, void* param, 
                   ThreadPriority priority);
    void Terminate(uint32 exit_status);
    void Suspend();
    void Resume();
    void Yield();
};
```

### Thread Manager
```cpp
class ThreadManager {
private:
    volatile Thread* current_thread;     // Currently running thread
    volatile Thread* ready_queue;        // Ready to run threads
    volatile Thread* wait_queue;         // Waiting threads
    volatile Thread* zombie_queue;       // Terminated but not cleaned up
    int32 next_tid;                     // Next available thread ID
    int32 thread_count;                 // Total number of threads
    spinlock lock;                      // For thread-safe access
    
public:
    Thread* CreateThread(uint32 start_routine, void* param, 
                        ThreadPriority priority, Process* owner);
    bool DestroyThread(Thread* thread);
    Thread* GetThreadById(int32 tid);
    Thread* GetCurrentThread();
    int32 GetNextTid();
    void ScheduleNext();
    void BlockThread(Thread* thread);
    void WakeupThread(Thread* thread);
    void ExitThread(uint32 exit_status);
    void InitializeThreadManager();
    void YieldCurrentThread();
    void JoinThread(int32 tid);
};
```

## 3. Scheduling System

### Scheduler Base Class
```cpp
class SchedulerBase {
public:
    virtual Thread* SelectNextThread() = 0;
    virtual void ThreadReady(Thread* thread) = 0;
    virtual void ThreadBlocked(Thread* thread) = 0;
    virtual void UpdateQuantum(Thread* thread) = 0;
    virtual ~SchedulerBase();
};

class CooperativeScheduler : public SchedulerBase {
public:
    Thread* SelectNextThread() override;
    void ThreadReady(Thread* thread) override;
    void ThreadBlocked(Thread* thread) override;
    void UpdateQuantum(Thread* thread) override;
    void YieldToNext();
};

class PreemptiveScheduler : public SchedulerBase {
private:
    int32 time_slice_ms;               // Default time slice
    Vector<Thread*> priority_queues[8]; // Priority-based queues
    
public:
    Thread* SelectNextThread() override;
    void ThreadReady(Thread* thread) override;
    void ThreadBlocked(Thread* thread) override;
    void UpdateQuantum(Thread* thread) override;
    void SetTimeSlice(int32 ms);
    void AdjustThreadPriority(Thread* thread, ThreadPriority new_priority);
};

class Win16CompatibleScheduler : public SchedulerBase {
private:
    Process* win16_process;             // Currently running Win16 process
    
public:
    Thread* SelectNextThread() override;
    void ThreadReady(Thread* thread) override;
    void ThreadBlocked(Thread* thread) override;
    void UpdateQuantum(Thread* thread) override;
    void EnterWin16Mode(Process* process);
    void LeaveWin16Mode();
};
```

## 4. Task Management (Context Switching)

### Task Manager
```cpp
class TaskManager {
private:
    SchedulerBase* active_scheduler;     // Currently active scheduler
    CooperativeScheduler* coop_sched;    // Cooperative scheduler
    PreemptiveScheduler* prep_sched;     // Preemptive scheduler
    Win16CompatibleScheduler* win16_sched; // Win16-compatible scheduler
    spinlock lock;                      // For thread-safe access
    
public:
    void InitializeTasking();
    void TaskSwitch();
    void SwitchToScheduler(SchedulerType type);
    void ScheduleTimerTick();
    uint32 ReadEip();                   // Read current instruction pointer
    void MoveStack(void* new_stack_start, uint32 size);
    void SwitchToUserMode();
    void SetCooperativeMode();
    void SetPreemptiveMode();
    void SetWin16CompatibleMode();
    void HandleTimerInterrupt();
};
```

## 5. DOS/Windows 98 Compatibility

### DOS Process Management
```cpp
class DosProcessManager {
private:
    bool is_16bit_mode;                 // Running in 16-bit mode
    uint32* segment_table;              // Segment allocation table
    uint16 current_psp;                 // Current Program Segment Prefix
    uint16 current_dta;                 // Current Disk Transfer Address
    
public:
    Process* CreateDosProcess(const char* filename, 
                            char* cmdline, uint16 env_seg);
    bool ExecuteDosApiCall(uint8 api_function, uint16* registers);
    uint16 AllocateDosMemory(uint16 paragraphs);
    bool FreeDosMemory(uint16 segment);
    void EnterDosMode();
    void ExitDosMode();
    uint16 GetDosCurrentPsp();
    bool SetDosCurrentPsp(uint16 psp_segment);
};
```

### Windows 98 Process Compatibility
```cpp
class Win98ProcessManager {
private:
    bool is_win16_compat;               // Windows 16-bit compatibility
    Process* vmm_process;               // Virtual Memory Manager
    Process* vxd_loader;                // Virtual Device Driver loader
    
public:
    Process* CreateWin16Process(const char* exe_path, 
                              char* cmdline, uint32 creation_flags);
    bool ExecuteWin16SystemCall(uint32 call_number, uint32* params);
    void EnableWin16Compatibility();
    void DisableWin16Compatibility();
    bool LoadVirtualDeviceDriver(const char* vxd_name);
    bool UnloadVirtualDeviceDriver(const char* vxd_name);
};
```

## 6. Inter-Process Communication (IPC)

### IPC Manager
```cpp
class IpcManager {
private:
    Vector<SharedMemoryBlock> shared_blocks;
    Vector<Pipe> named_pipes;
    Vector<MessageQueue> msg_queues;
    spinlock lock;
    
public:
    int32 CreateSharedMemory(uint32 size, uint32 permissions);
    void* MapSharedMemoryToProcess(Process* proc, int32 shm_id, 
                                  uint32 virtual_addr);
    bool UnmapSharedMemoryFromProcess(Process* proc, void* addr);
    bool DestroySharedMemory(int32 shm_id);
    
    int32 CreatePipe(bool is_named = false, const char* name = nullptr);
    bool WriteToPipe(int32 pipe_id, const void* data, uint32 size);
    bool ReadFromPipe(int32 pipe_id, void* buffer, uint32 size);
    bool ClosePipe(int32 pipe_id);
    
    int32 CreateMessageQueue(const char* name, uint32 max_msg_size, 
                            uint32 max_msgs);
    bool SendMessage(int32 queue_id, const void* msg, uint32 size);
    bool ReceiveMessage(int32 queue_id, void* buffer, uint32* size);
    bool DestroyMessageQueue(int32 queue_id);
};
```

## 7. System Calls Interface

### Process-related System Calls
```cpp
// Windows-style function names for system calls
int32 SyscallCreateProcess(uint32 entry_point, uint32 stack_size, 
                          ProcessType type);
int32 SyscallGetCurrentProcessId();
int32 SyscallGetCurrentThreadId();
int32 SyscallWaitForProcess(int32 pid, uint32* exit_code, int32 timeout);
bool SyscallTerminateProcess(int32 pid, uint32 exit_code);
bool SyscallKillProcess(int32 pid, int32 signal);

// Thread-related system calls
int32 SyscallCreateThread(uint32 start_routine, void* param, 
                         ThreadPriority priority);
bool SyscallTerminateThread(int32 tid, uint32 exit_code);
bool SyscallSuspendThread(int32 tid);
bool SyscallResumeThread(int32 tid);
int32 SyscallWaitForThread(int32 tid, uint32* exit_code, int32 timeout);

// For Linux compatibility layer (internal naming)
int32 SyscallFork();
int32 SyscallClone(uint32 flags, void* child_stack, int32* ptid, 
                  int32* ctid, uint32 newtls);
int32 SyscallWait4(int32 pid, uint32* status, int32 options, 
                  struct rusage* rusage);
int32 SyscallExit(int32 status);
```

## 8. Process and Thread States

### Process States
```cpp
enum ProcessState {
    PROCESS_CREATED = 0,
    PROCESS_READY = 1,
    PROCESS_RUNNING = 2,
    PROCESS_WAITING = 3,
    PROCESS_SUSPENDED = 4,
    PROCESS_ZOMBIE = 5,
    PROCESS_TERMINATED = 6
};

enum ThreadState {
    THREAD_CREATED = 0,
    THREAD_READY = 1,
    THREAD_RUNNING = 2,
    THREAD_WAITING = 3,
    THREAD_SUSPENDED = 4,
    THREAD_ZOMBIE = 5,
    THREAD_TERMINATED = 6
};

enum ProcessType {
    PROCESS_USER = 0,
    PROCESS_KERNEL = 1,
    PROCESS_SYSTEM = 2,
    PROCESS_DOS = 3,
    PROCESS_WIN16 = 4
};

enum ThreadPriority {
    PRIORITY_IDLE = 0,
    PRIORITY_LOW = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_HIGH = 3,
    PRIORITY_REALTIME = 4
};
```

## 9. Process and Thread Creation Functions

### High-level Interface Functions
```cpp
// Process creation and management
Process* CreateProcess(uint32 entry_point, uint32 stack_size, 
                     ProcessType type);
Process* CreateProcessFromExecutable(const char* filename, 
                                   char* argv[], char* envp[]);
bool DestroyProcess(Process* proc);
Process* GetCurrentProcess();
int32 GetProcessId(Process* proc);

// Thread creation and management
Thread* CreateThread(uint32 start_routine, void* param, 
                   ThreadPriority priority, Process* owner);
bool DestroyThread(Thread* thread);
Thread* GetCurrentThread();
int32 GetThreadId(Thread* thread);

// Scheduling control
void SetCooperativeScheduling();
void SetPreemptiveScheduling();
void SetWin16CompatibleScheduling();
bool YieldCurrentThread();
void Sleep(uint32 milliseconds);
```

## Implementation Strategy

### Phase 1: Basic Process Management
1. Implement Process structure and ProcessManager
2. Add basic process creation and destruction
3. Implement simple round-robin scheduler
4. Add context switching functionality
5. Test with basic process creation

### Phase 2: Thread Support
1. Implement Thread structure and ThreadManager
2. Add thread creation and destruction
3. Extend scheduler to handle threads
4. Add thread synchronization primitives
5. Test with multi-threaded programs

### Phase 3: Advanced Scheduling
1. Implement cooperative scheduler
2. Implement preemptive scheduler
3. Add priority-based scheduling
4. Add time slicing functionality
5. Test with real-time requirements

### Phase 4: Compatibility Features
1. Implement DOS process compatibility
2. Add Windows 98 compatibility layer
3. Implement Win16 scheduler mode
4. Test with legacy applications
5. Verify compatibility

### Phase 5: IPC and Communication
1. Implement shared memory
2. Add pipes and message queues
3. Add synchronization primitives (semaphores, mutexes)
4. Test inter-process communication
5. Verify data integrity across processes

### Phase 6: Integration and Testing
1. Connect with memory management system
2. Integrate with system call interface
3. Comprehensive testing with various applications
4. Performance optimization
5. Stability and reliability testing