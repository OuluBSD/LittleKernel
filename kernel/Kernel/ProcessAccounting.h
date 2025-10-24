#ifndef _Kernel_ProcessAccounting_h_
#define _Kernel_ProcessAccounting_h_

#include "Defs.h"
#include "ProcessControlBlock.h"
#include "RealTimeScheduling.h"

// Real-time scheduling policies (duplicate from RealTimeScheduling.h for completeness)
enum RealTimeSchedulingPolicy {
    RT_SCHED_FIFO = 0,     // First-In-First-Out real-time scheduling
    RT_SCHED_RR,           // Round-Robin real-time scheduling
    RT_SCHED_DEADLINE,     // Earliest Deadline First scheduling
    RT_SCHED_SPORADIC,    // Sporadic server scheduling
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

// Process accounting record structure
struct ProcessAccountingRecord {
    uint32 pid;                    // Process ID
    uint32 parent_pid;            // Parent process ID
    uint32 uid;                   // User ID
    uint32 gid;                   // Group ID
    char command[16];              // Command name (truncated to 16 chars)
    uint32 start_time;            // Process start time (ticks since boot)
    uint32 end_time;              // Process end time (ticks since boot)
    uint32 cpu_time;              // Total CPU time used (in ticks)
    uint32 user_time;             // User mode CPU time (in ticks)
    uint32 system_time;           // System mode CPU time (in ticks)
    uint32 wait_time;             // Time spent waiting (in ticks)
    uint32 read_bytes;           // Bytes read from storage
    uint32 write_bytes;           // Bytes written to storage
    uint32 read_operations;       // Number of read operations
    uint32 write_operations;      // Number of write operations
    uint32 memory_max;            // Peak memory usage (bytes)
    uint32 memory_avg;            // Average memory usage (bytes)
    uint32 context_switches;     // Number of context switches
    uint32 voluntary_switches;    // Voluntary context switches
    uint32 involuntary_switches;  // Involuntary context switches
    uint32 page_faults;           // Number of page faults
    uint32 page_ins;              // Pages read in from storage
    uint32 page_outs;             // Pages written out to storage
    uint32 signals_delivered;     // Number of signals delivered
    uint32 exit_status;           // Process exit status
    uint32 priority;              // Process priority
    uint32 nice_value;            // Nice value (Unix compatibility)
    uint32 session_id;            // Session ID
    uint32 process_group_id;       // Process group ID
    uint32 terminal_id;           // Controlling terminal ID
    uint32 flags;                 // Process flags
    uint32 minor_faults;          // Minor page faults
    uint32 major_faults;           // Major page faults
    uint32 swaps;                 // Number of swaps
    uint32 ipc_sent;              // IPC messages sent
    uint32 ipc_received;           // IPC messages received
    uint32 socket_in;             // Socket bytes received
    uint32 socket_out;            // Socket bytes sent
    uint32 characters_read;       // Characters read from terminals
    uint32 characters_written;     // Characters written to terminals
    uint32 creation_time;         // Time when accounting record was created
    
    // Real-time scheduling fields
    RealTimeSchedulingPolicy rt_policy;      // Real-time scheduling policy
    uint32 rt_priority;                      // Real-time priority
    uint32 rt_execution_time;               // Worst-case execution time (WCET)
    uint32 rt_period;                        // Period for periodic tasks (in ticks)
    uint32 rt_deadline;                      // Absolute deadline (in ticks)
    uint32 rt_release_time;                  // Time when task becomes ready (in ticks)
    uint32 rt_deadline_misses;               // Number of missed deadlines
    uint32 rt_completions;                   // Number of successful completions
    uint32 rt_budget;                        // CPU time budget for this task
    uint32 rt_budget_used;                   // CPU time used in current period
    uint32 rt_budget_period;                 // Budget replenishment period
    bool rt_is_periodic;                     // Whether this is a periodic task
    bool rt_is_soft_realtime;                // Whether this is soft real-time (misses allowed)
    bool rt_is_critical;                     // Whether this is a critical real-time task
    uint32 rt_jitter_tolerance;              // Maximum acceptable jitter (in ticks)
    uint32 rt_phase_offset;                  // Phase offset for periodic tasks (in ticks)
    uint32 rt_relative_deadline;             // Relative deadline (in ticks)
    uint32 rt_criticality_level;             // Task criticality level (0-10, 10=highest)
    uint32 rt_importance_factor;             // Task importance factor (0-100)
    uint32 rt_resource_requirements;         // Required resources bitmask
    uint32 rt_affinity_mask;                 // CPU affinity mask
};

// Process resource usage statistics
struct ProcessResourceUsage {
    uint32 cpu_time;              // CPU time used (in ticks)
    uint32 user_time;             // User mode CPU time (in ticks)
    uint32 system_time;           // System mode CPU time (in ticks)
    uint32 memory_current;        // Current memory usage (bytes)
    uint32 memory_peak;           // Peak memory usage (bytes)
    uint32 memory_average;        // Average memory usage (bytes)
    uint32 disk_reads;            // Disk read operations
    uint32 disk_writes;           // Disk write operations
    uint32 network_in;            // Network bytes received
    uint32 network_out;           // Network bytes sent
    uint32 page_faults;           // Page faults
    uint32 context_switches;     // Context switches
    uint32 signals_received;       // Signals received
    uint32 file_descriptors;      // Open file descriptors
    uint32 threads;               // Number of threads
    uint32 child_processes;       // Number of child processes
    uint64 total_io_bytes;        // Total I/O bytes
    uint32 io_operations;         // I/O operations count
    uint32 interrupts_handled;    // Interrupts handled
    uint32 system_calls;          // System calls made
};

// Process accounting flags
const uint32 ACCOUNTING_FLAG_ENABLED = 0x00000001;     // Accounting is enabled
const uint32 ACCOUNTING_FLAG_DETAILED = 0x00000002;   // Detailed accounting
const uint32 ACCOUNTING_FLAG_PER_PROCESS = 0x00000004; // Per-process accounting
const uint32 ACCOUNTING_FLAG_SYSTEM_WIDE = 0x00000008; // System-wide accounting
const uint32 ACCOUNTING_FLAG_TO_FILE = 0x00000010;    // Write to file
const uint32 ACCOUNTING_FLAG_TO_BUFFER = 0x00000020;  // Write to buffer
const uint32 ACCOUNTING_FLAG_REALTIME = 0x00000040;    // Real-time updates
const uint32 ACCOUNTING_FLAG_COMPRESSED = 0x00000080; // Compressed records

// Process accounting configuration
struct ProcessAccountingConfig {
    uint32 flags;                 // Accounting flags
    uint32 update_interval;       // Update interval in ticks (0 = immediate)
    uint32 buffer_size;           // Buffer size for records
    uint32 max_records;           // Maximum records to keep
    char log_file[256];           // Log file path
    bool auto_rotate;             // Automatically rotate log files
    uint32 rotate_size;           // Rotate when file reaches this size
    uint32 retention_days;        // Retain records for this many days
    bool compress_old;            // Compress old records
    uint32 compression_threshold; // Compress records older than this
};

// Process accounting statistics
struct ProcessAccountingStats {
    uint32 total_processes;        // Total processes accounted
    uint32 active_processes;       // Currently active processes
    uint32 terminated_processes;  // Terminated processes
    uint64 total_cpu_time;        // Total CPU time for all processes
    uint64 total_user_time;       // Total user time for all processes
    uint64 total_system_time;     // Total system time for all processes
    uint64 total_wait_time;       // Total wait time for all processes
    uint64 total_read_bytes;      // Total bytes read
    uint64 total_write_bytes;     // Total bytes written
    uint32 total_page_faults;     // Total page faults
    uint32 total_context_switches; // Total context switches
    uint32 total_signals;        // Total signals delivered
    uint32 accounting_errors;     // Accounting errors
    uint32 buffer_overflows;     // Buffer overflow events
    uint32 disk_writes;           // Disk writes performed
    uint32 log_rotations;         // Log file rotations
    uint32 compressed_records;    // Compressed records
};

// Process accounting buffer
struct ProcessAccountingBuffer {
    ProcessAccountingRecord* records;  // Array of records
    uint32 capacity;                   // Buffer capacity
    uint32 count;                      // Number of records in buffer
    uint32 head;                       // Index of oldest record
    uint32 tail;                       // Index of newest record
    bool is_full;                      // Whether buffer is full
    uint32* timestamps;                // Timestamps for each record
};

// Process accounting manager
class ProcessAccountingManager {
private:
    ProcessAccountingConfig config;        // Accounting configuration
    ProcessAccountingStats stats;          // Accounting statistics
    ProcessAccountingBuffer buffer;        // Accounting buffer
    uint32 next_record_id;                 // Next record ID
    bool is_initialized;                   // Whether manager is initialized
    uint32 last_update_time;              // Last update time
    ProcessControlBlock* monitored_processes; // List of monitored processes
    
public:
    ProcessAccountingManager();
    ~ProcessAccountingManager();
    
    // Initialization and configuration
    bool Initialize(const ProcessAccountingConfig* config = nullptr);
    bool Configure(const ProcessAccountingConfig* config);
    bool IsInitialized() const;
    bool IsEnabled() const;
    bool Enable();
    bool Disable();
    void Reset();
    
    // Process accounting management
    bool StartAccounting(uint32 pid);
    bool StopAccounting(uint32 pid);
    bool IsAccountingEnabled(uint32 pid);
    bool UpdateAccounting(uint32 pid);
    bool ForceUpdateAll();
    
    // Process accounting data collection
    bool CollectProcessData(uint32 pid, ProcessAccountingRecord* record);
    bool CollectResourceUsage(uint32 pid, ProcessResourceUsage* usage);
    bool UpdateProcessStatistics(uint32 pid);
    bool SnapshotAllProcesses();
    
    // Process accounting record management
    bool AddRecord(const ProcessAccountingRecord* record);
    bool GetRecord(uint32 record_id, ProcessAccountingRecord* record);
    bool RemoveRecord(uint32 record_id);
    bool ClearRecords();
    uint32 GetRecordCount();
    uint32 GetBufferCapacity();
    
    // Process accounting I/O operations
    bool WriteRecordToFile(const ProcessAccountingRecord* record);
    bool WriteAllRecordsToFile();
    bool ReadRecordsFromFile();
    bool RotateLogFile();
    bool CompressOldRecords();
    
    // Process accounting querying and filtering
    uint32 QueryRecordsByPID(uint32 pid, ProcessAccountingRecord* records, uint32 max_records);
    uint32 QueryRecordsByUser(uint32 uid, ProcessAccountingRecord* records, uint32 max_records);
    uint32 QueryRecordsByTimeRange(uint32 start_time, uint32 end_time, 
                                  ProcessAccountingRecord* records, uint32 max_records);
    uint32 QueryRecordsByResourceUsage(uint32 min_cpu_time, ProcessAccountingRecord* records, uint32 max_records);
    uint32 QueryActiveProcesses(ProcessAccountingRecord* records, uint32 max_records);
    
    // Process accounting aggregation and reporting
    bool GenerateSummaryReport();
    bool GenerateUserReport(uint32 uid);
    bool GenerateProcessGroupReport(uint32 pgid);
    bool GenerateSessionReport(uint32 sid);
    bool GenerateSystemLoadReport();
    bool GenerateResourceUsageReport();
    bool GeneratePerformanceReport();
    
    // Process accounting statistics
    const ProcessAccountingStats* GetStatistics();
    void ResetStatistics();
    void UpdateStatistics();
    uint64 GetTotalCPUTime();
    uint64 GetTotalIOTime();
    uint32 GetAverageProcessLifetime();
    uint32 GetPeakProcessCount();
    uint32 GetProcessCreationRate();
    
    // Process accounting utilities
    const char* GetProcessCommand(uint32 pid);
    uint32 GetProcessStartTime(uint32 pid);
    uint32 GetProcessEndTime(uint32 pid);
    uint32 GetProcessCPUTime(uint32 pid);
    uint32 GetProcessMemoryUsage(uint32 pid);
    uint32 GetProcessIOBytes(uint32 pid);
    uint32 GetProcessPageFaults(uint32 pid);
    uint32 GetProcessContextSwitches(uint32 pid);
    
    // Process accounting monitoring
    bool MonitorProcess(uint32 pid);
    bool UnmonitorProcess(uint32 pid);
    bool IsProcessMonitored(uint32 pid);
    uint32 GetMonitoredProcessCount();
    void MonitorAllProcesses();
    void UnmonitorAllProcesses();
    
    // Process accounting real-time updates
    void OnProcessCreate(uint32 pid);
    void OnProcessTerminate(uint32 pid);
    void OnProcessSwitch(uint32 old_pid, uint32 new_pid);
    void OnSystemCall(uint32 pid, uint32 syscall_number);
    void OnPageFault(uint32 pid);
    void OnContextSwitch(uint32 pid);
    void OnTimerTick();
    void OnIOPerformed(uint32 pid, uint32 bytes_read, uint32 bytes_written);
    void OnSignalDelivered(uint32 pid, uint32 signal);
    void OnResourceLimitExceeded(uint32 pid, uint32 resource);
    
    // Process accounting buffer management
    bool ResizeBuffer(uint32 new_capacity);
    bool FlushBuffer();
    bool IsBufferFull();
    uint32 GetBufferUsage();
    uint32 GetBufferFreeSpace();
    
    // Process accounting debugging and diagnostics
    void PrintAccountingSummary();
    void PrintProcessAccounting(uint32 pid);
    void PrintAllProcessAccounting();
    void PrintAccountingStatistics();
    void PrintAccountingConfiguration();
    void PrintBufferStatus();
    void DumpAccountingData();
    void ValidateAccountingData();
    
    // Process accounting export and import
    bool ExportToCSV(const char* filename);
    bool ExportToJSON(const char* filename);
    bool ExportToXML(const char* filename);
    bool ImportFromCSV(const char* filename);
    bool ImportFromJSON(const char* filename);
    bool ImportFromXML(const char* filename);
    
    // Process accounting filtering and sorting
    void SortRecordsByCPUTime(ProcessAccountingRecord* records, uint32 count);
    void SortRecordsByMemoryUsage(ProcessAccountingRecord* records, uint32 count);
    void SortRecordsByStartTime(ProcessAccountingRecord* records, uint32 count);
    void FilterRecordsByCommand(const char* command, ProcessAccountingRecord* records, uint32 count);
    void FilterRecordsByExitStatus(uint32 exit_status, ProcessAccountingRecord* records, uint32 count);
    
    // Process accounting thresholds and alerts
    bool SetCPUThreshold(uint32 pid, uint32 threshold);
    bool SetMemoryThreshold(uint32 pid, uint32 threshold);
    bool SetIOTreshold(uint32 pid, uint32 threshold);
    bool CheckThresholds(uint32 pid);
    void OnThresholdExceeded(uint32 pid, uint32 resource, uint32 value);
    bool IsThresholdExceeded(uint32 pid, uint32 resource);
    
    // Process accounting cleanup
    bool CleanupOldRecords();
    bool CleanupTerminatedProcesses();
    bool PurgeAllRecords();
    uint32 GetCleanupCount();
};

// Process accounting system calls
uint32 SysCallEnableProcessAccounting();
uint32 SysCallDisableProcessAccounting();
uint32 SysCallGetProcessAccounting(uint32 pid, ProcessAccountingRecord* record);
uint32 SysCallGetProcessResourceUsage(uint32 pid, ProcessResourceUsage* usage);
uint32 SysCallSetAccountingConfig(const ProcessAccountingConfig* config);
uint32 SysCallGetAccountingConfig(ProcessAccountingConfig* config);
uint32 SysCallGetAccountingStatistics(ProcessAccountingStats* stats);
uint32 SysCallResetAccounting();
uint32 SysCallExportAccountingData(const char* filename, uint32 format);

// Global process accounting manager instance
extern ProcessAccountingManager* g_process_accounting_manager;

#endif // _Kernel_ProcessAccounting_h_