#include "ProcessDebugging.h"
#include "Global.h"
#include "MemoryManager.h"
#include "Logging.h"
#include "Timer.h"
#include "ProcessManager.h"

// Global process debugging manager instance
ProcessDebuggingManager* g_process_debugging_manager = nullptr;

// ProcessDebuggingManager implementation
ProcessDebuggingManager::ProcessDebuggingManager() 
    : config(), stats(), session_list_head(nullptr), session_count(0), 
      next_session_id(1), is_initialized(false), last_activity_time(0),
      debugged_processes(nullptr) {
    // Initialize configuration
    memset(&config, 0, sizeof(config));
    config.flags = DEBUG_FLAG_ENABLED;
    config.update_interval = 100;  // Update every 100 ticks
    config.buffer_size = 1024;     // 1024 records buffer
    config.max_records = 10000;    // Keep up to 10,000 records
    strcpy(config.log_file, "/var/log/process_debugging.log");
    config.auto_rotate = true;
    config.rotate_size = 10 * 1024 * 1024;  // 10MB rotate size
    config.retention_days = 30;              // Keep records for 30 days
    config.compress_old = true;
    config.compression_threshold = 7;        // Compress records older than 7 days
    
    // Initialize statistics
    memset(&stats, 0, sizeof(stats));
    
    // Initialize buffer
    memset(&buffer, 0, sizeof(buffer));
    buffer.capacity = config.buffer_size;
    buffer.records = (ProcessDebuggingRecord*)malloc(sizeof(ProcessDebuggingRecord) * buffer.capacity);
    buffer.timestamps = (uint32*)malloc(sizeof(uint32) * buffer.capacity);
    if (!buffer.records || !buffer.timestamps) {
        LOG("Failed to allocate debugging buffer");
        if (buffer.records) free(buffer.records);
        if (buffer.timestamps) free(buffer.timestamps);
        buffer.records = nullptr;
        buffer.timestamps = nullptr;
        buffer.capacity = 0;
    }
    
    // Initialize other fields
    session_list_head = nullptr;
    session_count = 0;
    next_session_id = 1;
    is_initialized = false;
    last_activity_time = 0;
    debugged_processes = nullptr;
    
    DLOG("Process debugging manager created");
}

ProcessDebuggingManager::~ProcessDebuggingManager() {
    // Clean up all debugging sessions
    CleanupAllSessions();
    
    // Clean up buffer
    if (buffer.records) {
        free(buffer.records);
        buffer.records = nullptr;
    }
    
    if (buffer.timestamps) {
        free(buffer.timestamps);
        buffer.timestamps = nullptr;
    }
    
    buffer.capacity = 0;
    buffer.count = 0;
    
    DLOG("Process debugging manager destroyed");
}

bool ProcessDebuggingManager::Initialize(const ProcessDebuggingConfig* config) {
    DLOG("Initializing process debugging manager");
    
    // Apply configuration if provided
    if (config) {
        if (!Configure(config)) {
            LOG("Failed to configure process debugging manager");
            return false;
        }
    }
    
    // Validate buffer
    if (!buffer.records || !buffer.timestamps) {
        LOG("Debugging buffer not allocated");
        return false;
    }
    
    // Clear buffer
    memset(buffer.records, 0, sizeof(ProcessDebuggingRecord) * buffer.capacity);
    memset(buffer.timestamps, 0, sizeof(uint32) * buffer.capacity);
    buffer.count = 0;
    buffer.head = 0;
    buffer.tail = 0;
    buffer.is_full = false;
    
    // Reset statistics
    ResetStatistics();
    
    // Mark as initialized
    is_initialized = true;
    
    DLOG("Process debugging manager initialized successfully");
    return true;
}

bool ProcessDebuggingManager::Configure(const ProcessDebuggingConfig* new_config) {
    if (!new_config) {
        LOG("Invalid configuration provided");
        return false;
    }
    
    // Copy configuration
    memcpy(&config, new_config, sizeof(ProcessDebuggingConfig));
    
    // Validate and adjust buffer size
    if (config.buffer_size > config.max_records) {
        config.buffer_size = config.max_records;
        LOG("Adjusted buffer size to " << config.buffer_size);
    }
    
    // Resize buffer if needed
    if (config.buffer_size != buffer.capacity) {
        ProcessDebuggingRecord* new_records = (ProcessDebuggingRecord*)realloc(
            buffer.records, sizeof(ProcessDebuggingRecord) * config.buffer_size);
        uint32* new_timestamps = (uint32*)realloc(
            buffer.timestamps, sizeof(uint32) * config.buffer_size);
        
        if (new_records && new_timestamps) {
            buffer.records = new_records;
            buffer.timestamps = new_timestamps;
            buffer.capacity = config.buffer_size;
            DLOG("Resized debugging buffer to " << config.buffer_size << " records");
        } else {
            LOG("Failed to resize debugging buffer");
            // Keep existing buffer
        }
    }
    
    DLOG("Process debugging manager configured successfully");
    return true;
}

bool ProcessDebuggingManager::IsInitialized() const {
    return is_initialized;
}

bool ProcessDebuggingManager::IsEnabled() const {
    return (config.flags & DEBUG_FLAG_ENABLED) != 0;
}

bool ProcessDebuggingManager::Enable() {
    if (!is_initialized) {
        LOG("Debugging manager not initialized");
        return false;
    }
    
    config.flags |= DEBUG_FLAG_ENABLED;
    DLOG("Process debugging enabled");
    return true;
}

bool ProcessDebuggingManager::Disable() {
    if (!is_initialized) {
        LOG("Debugging manager not initialized");
        return false;
    }
    
    config.flags &= ~DEBUG_FLAG_ENABLED;
    DLOG("Process debugging disabled");
    return true;
}

void ProcessDebuggingManager::Reset() {
    if (!is_initialized) {
        return;
    }
    
    // Clear buffer
    if (buffer.records) {
        memset(buffer.records, 0, sizeof(ProcessDebuggingRecord) * buffer.capacity);
    }
    
    if (buffer.timestamps) {
        memset(buffer.timestamps, 0, sizeof(uint32) * buffer.capacity);
    }
    
    buffer.count = 0;
    buffer.head = 0;
    buffer.tail = 0;
    buffer.is_full = false;
    
    // Reset statistics
    ResetStatistics();
    
    // Reset counters
    next_record_id = 1;
    last_activity_time = 0;
    
    DLOG("Process debugging manager reset");
}

bool ProcessDebuggingManager::StartDebugging(uint32 pid) {
    if (!is_initialized || !IsEnabled()) {
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
    
    // Mark process for debugging
    process->flags |= 0x01000000;  // Debugging flag
    
    // Add to debugged processes list
    process->next = debugged_processes;
    if (debugged_processes) {
        debugged_processes->prev = process;
    }
    debugged_processes = process;
    
    DLOG("Started debugging for process PID " << pid);
    return true;
}

bool ProcessDebuggingManager::StopDebugging(uint32 pid) {
    if (!is_initialized || !IsEnabled()) {
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
    
    // Remove debugging flag
    process->flags &= ~0x01000000;  // Remove debugging flag
    
    // Remove from debugged processes list
    ProcessControlBlock* current = debugged_processes;
    ProcessControlBlock* prev = nullptr;
    
    while (current) {
        if (current->pid == pid) {
            if (prev) {
                prev->next = current->next;
            } else {
                debugged_processes = current->next;
            }
            
            if (current->next) {
                current->next->prev = prev;
            }
            
            break;
        }
        prev = current;
        current = current->next;
    }
    
    DLOG("Stopped debugging for process PID " << pid);
    return true;
}

bool ProcessDebuggingManager::IsDebugEnabled(uint32 pid) {
    if (!process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    return (process->flags & 0x01000000) != 0;  // Check debugging flag
}

bool ProcessDebuggingManager::UpdateDebugging(uint32 pid) {
    if (!is_initialized || !IsEnabled()) {
        return false;
    }
    
    ProcessDebuggingRecord record;
    if (!CollectProcessData(pid, &record)) {
        return false;
    }
    
    return AddRecord(&record);
}

bool ProcessDebuggingManager::ForceUpdateAll() {
    if (!is_initialized || !IsEnabled()) {
        return false;
    }
    
    if (!process_manager) {
        LOG("Process manager not available");
        return false;
    }
    
    bool success = true;
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    
    while (current) {
        if (IsDebugEnabled(current->pid)) {
            if (!UpdateDebugging(current->pid)) {
                success = false;
                LOG("Failed to update debugging for process PID " << current->pid);
            } else {
                UpdateProcessStatistics(current->pid);
            }
        }
        current = current->next;
    }
    
    return success;
}

bool ProcessDebuggingManager::CollectProcessData(uint32 pid, ProcessDebuggingRecord* record) {
    if (!record || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Initialize record
    memset(record, 0, sizeof(ProcessDebuggingRecord));
    
    // Fill in process data
    record->pid = process->pid;
    record->parent_pid = process->parent_pid;
    record->uid = process->uid;
    record->gid = process->gid;
    
    // Copy command name (truncate to 16 chars)
    strncpy(record->command, process->name, 15);
    record->command[15] = '\0';
    
    // Fill in timing data
    record->start_time = process->creation_time;
    record->end_time = process->termination_time;
    record->cpu_time = process->total_cpu_time_used;
    record->user_time = process->total_cpu_time_used / 2;  // Approximation
    record->system_time = process->total_cpu_time_used / 2; // Approximation
    record->wait_time = process->wait_time;
    
    // Fill in I/O data (approximations)
    record->read_bytes = process->total_cpu_time_used * 1024;  // Dummy value
    record->write_bytes = process->total_cpu_time_used * 512;   // Dummy value
    record->read_operations = process->total_cpu_time_used / 10; // Dummy value
    record->write_operations = process->total_cpu_time_used / 20; // Dummy value
    
    // Fill in memory data
    record->memory_max = process->heap_end - process->heap_start; // Approximation
    record->memory_avg = record->memory_max / 2;                  // Approximation
    
    // Fill in context switch data
    record->context_switches = process->context_switch_count;
    record->voluntary_switches = process->voluntary_yield_count;
    record->involuntary_switches = process->preemption_count;
    
    // Fill in page fault data
    record->page_faults = process->total_cpu_time_used / 100; // Dummy value
    record->page_ins = record->page_faults / 2;               // Dummy value
    record->page_outs = record->page_faults / 4;             // Dummy value
    
    // Fill in signal data
    record->signals_delivered = 0; // Not tracked in this implementation
    
    // Fill in exit status
    record->exit_status = process->exit_code;
    
    // Fill in scheduling data
    record->priority = process->current_priority;
    record->nice_value = 0; // Not used in this implementation
    
    // Fill in session and group data
    record->session_id = process->sid;
    record->process_group_id = process->pgid;
    record->terminal_id = 0; // Not tracked in this implementation
    
    // Fill in flags
    record->flags = process->flags;
    
    // Fill in additional data
    record->minor_faults = record->page_faults;
    record->major_faults = record->page_faults / 3; // Dummy value
    record->swaps = 0; // Not tracked in this implementation
    record->ipc_sent = 0; // Not tracked in this implementation
    record->ipc_received = 0; // Not tracked in this implementation
    record->socket_in = 0; // Not tracked in this implementation
    record->socket_out = 0; // Not tracked in this implementation
    record->characters_read = 0; // Not tracked in this implementation
    record->characters_written = 0; // Not tracked in this implementation
    
    // Fill in timing data
    record->creation_time = global_timer ? global_timer->GetTickCount() : 0;
    
    return true;
}

bool ProcessDebuggingManager::CollectResourceUsage(uint32 pid, ProcessResourceUsage* usage) {
    if (!usage || !process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Initialize usage structure
    memset(usage, 0, sizeof(ProcessResourceUsage));
    
    // Fill in resource usage data
    usage->cpu_time = process->total_cpu_time_used;
    usage->user_time = process->total_cpu_time_used / 2;  // Approximation
    usage->system_time = process->total_cpu_time_used / 2;  // Approximation
    usage->memory_current = process->heap_end - process->heap_start; // Approximation
    usage->memory_peak = usage->memory_current;              // Approximation
    usage->memory_average = usage->memory_current / 2;         // Approximation
    usage->disk_reads = process->total_cpu_time_used * 10;     // Dummy value
    usage->disk_writes = process->total_cpu_time_used * 5;     // Dummy value
    usage->network_in = 0; // Not tracked in this implementation
    usage->network_out = 0; // Not tracked in this implementation
    usage->page_faults = process->total_cpu_time_used / 100;   // Dummy value
    usage->context_switches = process->context_switch_count;
    usage->signals_received = 0; // Not tracked in this implementation
    usage->file_descriptors = 0; // Not tracked in this implementation
    usage->threads = 1; // Assuming single-threaded for now
    usage->child_processes = 0; // Not tracked in this implementation
    usage->total_io_bytes = usage->disk_reads + usage->disk_writes; // Dummy value
    usage->io_operations = usage->disk_reads / 1024; // Dummy value
    usage->interrupts_handled = 0; // Not tracked in this implementation
    usage->system_calls = process->total_cpu_time_used / 50; // Dummy value
    
    return true;
}

bool ProcessDebuggingManager::UpdateProcessStatistics(uint32 pid) {
    if (!is_initialized || !IsEnabled()) {
        return false;
    }
    
    ProcessControlBlock* process = nullptr;
    if (process_manager) {
        process = process_manager->GetProcessById(pid);
    }
    
    if (!process) {
        return false;
    }
    
    // Update statistics
    stats.total_cpu_time += process->total_cpu_time_used;
    stats.total_user_time += process->total_cpu_time_used / 2;
    stats.total_system_time += process->total_cpu_time_used / 2;
    stats.total_wait_time += process->wait_time;
    stats.total_read_bytes += process->total_cpu_time_used * 1024;
    stats.total_write_bytes += process->total_cpu_time_used * 512;
    stats.total_page_faults += process->total_cpu_time_used / 100;
    stats.total_context_switches += process->context_switch_count;
    stats.total_signals += 0; // Not tracked
    
    return true;
}

bool ProcessDebuggingManager::SnapshotAllProcesses() {
    if (!is_initialized || !IsEnabled()) {
        return false;
    }
    
    if (!process_manager) {
        LOG("Process manager not available");
        return false;
    }
    
    bool success = true;
    uint32 current_time = global_timer ? global_timer->GetTickCount() : 0;
    
    // Check if it's time for an update
    if (config.update_interval > 0 && 
        (current_time - last_update_time) < config.update_interval) {
        return true; // Not time for update yet
    }
    
    last_update_time = current_time;
    
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        if (IsDebugEnabled(current->pid)) {
            if (!UpdateDebugging(current->pid)) {
                success = false;
                LOG("Failed to update debugging for process PID " << current->pid);
            } else {
                UpdateProcessStatistics(current->pid);
            }
        }
        current = current->next;
    }
    
    return success;
}

bool ProcessDebuggingManager::AddRecord(const ProcessDebuggingRecord* record) {
    if (!record || !is_initialized || !IsEnabled()) {
        return false;
    }
    
    // Check if buffer is full
    if (buffer.is_full) {
        // Handle buffer overflow
        stats.buffer_overflows++;
        if (config.flags & DEBUG_FLAG_TO_FILE) {
            // Write oldest record to file before overwriting
            WriteRecordToFile(&buffer.records[buffer.head]);
        }
        
        // Move head forward
        buffer.head = (buffer.head + 1) % buffer.capacity;
    }
    
    // Add new record at tail
    uint32 index = buffer.tail;
    memcpy(&buffer.records[index], record, sizeof(ProcessDebuggingRecord));
    buffer.timestamps[index] = global_timer ? global_timer->GetTickCount() : 0;
    
    // Update buffer state
    buffer.tail = (buffer.tail + 1) % buffer.capacity;
    if (buffer.tail == buffer.head) {
        buffer.is_full = true;
    }
    
    if (buffer.count < buffer.capacity) {
        buffer.count++;
    } else {
        stats.total_processes++; // Account for overwritten records
    }
    
    // Write to file if configured
    if (config.flags & DEBUG_FLAG_TO_FILE) {
        WriteRecordToFile(record);
    }
    
    // Update statistics
    stats.disk_writes++;
    
    return true;
}

bool ProcessDebuggingManager::GetRecord(uint32 record_id, ProcessDebuggingRecord* record) {
    if (!record || !is_initialized) {
        return false;
    }
    
    // Find record by ID (simplified implementation)
    // In a real implementation, we'd have a more efficient lookup mechanism
    for (uint32 i = 0; i < buffer.count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (buffer.records[index].pid == record_id) {
            memcpy(record, &buffer.records[index], sizeof(ProcessDebuggingRecord));
            return true;
        }
    }
    
    return false; // Config not found
}

bool ProcessDebuggingManager::RemoveRecord(uint32 record_id) {
    if (!is_initialized) {
        return false;
    }
    
    // Find and remove record by ID (simplified implementation)
    for (uint32 i = 0; i < buffer.count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (buffer.records[index].pid == record_id) {
            // Shift records to fill gap
            for (uint32 j = i; j < buffer.count - 1; j++) {
                uint32 src_index = (buffer.head + j + 1) % buffer.capacity;
                uint32 dst_index = (buffer.head + j) % buffer.capacity;
                memcpy(&buffer.records[dst_index], &buffer.records[src_index], sizeof(ProcessDebuggingRecord));
                buffer.timestamps[dst_index] = buffer.timestamps[src_index];
            }
            
            // Update buffer state
            buffer.count--;
            if (buffer.is_full) {
                buffer.is_full = false;
            } else {
                buffer.tail = (buffer.tail - 1 + buffer.capacity) % buffer.capacity;
            }
            
            return true;
        }
    }
    
    return false;
}

bool ProcessDebuggingManager::ClearRecords() {
    if (!is_initialized) {
        return false;
    }
    
    // Clear buffer
    memset(buffer.records, 0, sizeof(ProcessDebuggingRecord) * buffer.capacity);
    memset(buffer.timestamps, 0, sizeof(uint32) * buffer.capacity);
    buffer.count = 0;
    buffer.head = 0;
    buffer.tail = 0;
    buffer.is_full = false;
    
    DLOG("Cleared all debugging records");
    return true;
}

uint32 ProcessDebuggingManager::GetRecordCount() {
    return buffer.count;
}

uint32 ProcessDebuggingManager::GetBufferCapacity() {
    return buffer.capacity;
}

bool ProcessDebuggingManager::WriteRecordToFile(const ProcessDebuggingRecord* record) {
    if (!record || !is_initialized || !(config.flags & DEBUG_FLAG_TO_FILE)) {
        return false;
    }
    
    // In a real implementation, we'd write to the configured log file
    // For now, we'll just log that we would write
    
    DLOG("Writing debugging record for PID " << record->pid 
         << " to file " << config.log_file);
    
    return true;
}

bool ProcessDebuggingManager::WriteAllRecordsToFile() {
    if (!is_initialized || !(config.flags & DEBUG_FLAG_TO_FILE)) {
        return false;
    }
    
    // Write all records to file
    for (uint32 i = 0; i < buffer.count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        WriteRecordToFile(&buffer.records[index]);
    }
    
    DLOG("Wrote all " << buffer.count << " debugging records to file");
    return true;
}

bool ProcessDebuggingManager::ReadRecordsFromFile() {
    if (!is_initialized || !(config.flags & DEBUG_FLAG_TO_FILE)) {
        return false;
    }
    
    // In a real implementation, we'd read records from the configured log file
    // For now, we'll just log that we would read
    
    DLOG("Reading debugging records from file " << config.log_file);
    
    return true;
}

bool ProcessDebuggingManager::RotateLogFile() {
    if (!is_initialized || !config.auto_rotate) {
        return false;
    }
    
    // In a real implementation, we'd rotate the log file
    // For now, we'll just log that we would rotate
    
    DLOG("Rotating debugging log file " << config.log_file);
    
    stats.log_rotations++;
    return true;
}

bool ProcessDebuggingManager::CompressOldRecords() {
    if (!is_initialized || !config.compress_old) {
        return false;
    }
    
    // In a real implementation, we'd compress old records
    // For now, we'll just log that we would compress
    
    DLOG("Compressing debugging records older than " 
         << config.compression_threshold << " days");
    
    stats.compressed_records++;
    return true;
}

uint32 ProcessDebuggingManager::QueryRecordsByPID(uint32 pid, ProcessDebuggingRecord* records, uint32 max_records) {
    if (!records || !is_initialized || max_records == 0) {
        return 0;
    }
    
    uint32 count = 0;
    
    // Find records matching the PID
    for (uint32 i = 0; i < buffer.count && count < max_records; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (buffer.records[index].pid == pid) {
            memcpy(&records[count], &buffer.records[index], sizeof(ProcessDebuggingRecord));
            count++;
        }
    }
    
    return count;
}

uint32 ProcessDebuggingManager::QueryRecordsByUser(uint32 uid, ProcessDebuggingRecord* records, uint32 max_records) {
    if (!records || !is_initialized || max_records == 0) {
        return 0;
    }
    
    uint32 count = 0;
    
    // Find records matching the UID
    for (uint32 i = 0; i < buffer.count && count < max_records; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (buffer.records[index].uid == uid) {
            memcpy(&records[count], &buffer.records[index], sizeof(ProcessDebuggingRecord));
            count++;
        }
    }
    
    return count;
}

uint32 ProcessDebuggingManager::QueryRecordsByTimeRange(uint32 start_time, uint32 end_time, 
                                                        ProcessDebuggingRecord* records, uint32 max_records) {
    if (!records || !is_initialized || max_records == 0) {
        return 0;
    }
    
    uint32 count = 0;
    
    // Find records within the time range
    for (uint32 i = 0; i < buffer.count && count < max_records; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        uint32 record_time = buffer.timestamps[index];
        if (record_time >= start_time && record_time <= end_time) {
            memcpy(&records[count], &buffer.records[index], sizeof(ProcessDebuggingRecord));
            count++;
        }
    }
    
    return count;
}

uint32 ProcessDebuggingManager::QueryRecordsByResourceUsage(uint32 min_cpu_time, ProcessDebuggingRecord* records, uint32 max_records) {
    if (!records || !is_initialized || max_records == 0) {
        return 0;
    }
    
    uint32 count = 0;
    
    // Find records with CPU time >= min_cpu_time
    for (uint32 i = 0; i < buffer.count && count < max_records; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (buffer.records[index].cpu_time >= min_cpu_time) {
            memcpy(&records[count], &buffer.records[index], sizeof(ProcessDebuggingRecord));
            count++;
        }
    }
    
    return count;
}

uint32 ProcessDebuggingManager::QueryActiveProcesses(ProcessDebuggingRecord* records, uint32 max_records) {
    if (!records || !is_initialized || !process_manager || max_records == 0) {
        return 0;
    }
    
    uint32 count = 0;
    
    // Find active processes and create records for them
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current && count < max_records) {
        if (current->state != PROCESS_STATE_TERMINATED && 
            current->state != PROCESS_STATE_ZOMBIE) {
            
            // Create record for active process
            ProcessDebuggingRecord record;
            if (CollectProcessData(current->pid, &record)) {
                memcpy(&records[count], &record, sizeof(ProcessDebuggingRecord));
                count++;
            }
        }
        current = current->next;
    }
    
    return count;
}

bool ProcessDebuggingManager::GenerateSummaryReport() {
    if (!is_initialized) {
        return false;
    }
    
    UpdateStatistics();
    
    LOG("=== Process Debugging Summary Report ===");
    LOG("Total Processes Debugged: " << stats.total_processes);
    LOG("Active Processes: " << stats.active_processes);
    LOG("Terminated Processes: " << stats.terminated_processes);
    LOG("Total CPU Time: " << stats.total_cpu_time << " ticks");
    LOG("Total User Time: " << stats.total_user_time << " ticks");
    LOG("Total System Time: " << stats.total_system_time << " ticks");
    LOG("Total Wait Time: " << stats.total_wait_time << " ticks");
    LOG("Total Read Bytes: " << stats.total_read_bytes);
    LOG("Total Write Bytes: " << stats.total_write_bytes);
    LOG("Total Page Faults: " << stats.total_page_faults);
    LOG("Total Context Switches: " << stats.total_context_switches);
    LOG("Total Signals: " << stats.total_signals);
    LOG("Debugging Errors: " << stats.debugging_errors);
    LOG("Buffer Overflows: " << stats.buffer_overflows);
    LOG("Disk Writes: " << stats.disk_writes);
    LOG("Log Rotations: " << stats.log_rotations);
    LOG("Compressed Records: " << stats.compressed_records);
    LOG("========================================");
    
    return true;
}

bool ProcessDebuggingManager::GenerateUserReport(uint32 uid) {
    if (!is_initialized) {
        return false;
    }
    
    LOG("=== Process Debugging Report for UID " << uid << " ===");
    
    // Count processes for this user
    uint32 user_process_count = 0;
    uint64 user_cpu_time = 0;
    uint64 user_io_bytes = 0;
    
    for (uint32 i = 0; i < buffer.count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (buffer.records[index].uid == uid) {
            user_process_count++;
            user_cpu_time += buffer.records[index].cpu_time;
            user_io_bytes += (buffer.records[index].read_bytes + buffer.records[index].write_bytes);
        }
    }
    
    LOG("User Process Count: " << user_process_count);
    LOG("Total CPU Time: " << user_cpu_time << " ticks");
    LOG("Total I/O Bytes: " << user_io_bytes);
    LOG("========================================");
    
    return true;
}

bool ProcessDebuggingManager::GenerateProcessGroupReport(uint32 pgid) {
    if (!is_initialized) {
        return false;
    }
    
    LOG("=== Process Debugging Report for PGID " << pgid << " ===");
    
    // Count processes in this group
    uint32 group_process_count = 0;
    uint64 group_cpu_time = 0;
    uint64 group_io_bytes = 0;
    
    for (uint32 i = 0; i < buffer.count && group_process_count < 100; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (buffer.records[index].process_group_id == pgid) {
            group_process_count++;
            group_cpu_time += buffer.records[index].cpu_time;
            group_io_bytes += (buffer.records[index].read_bytes + buffer.records[index].write_bytes);
        }
    }
    
    LOG("Group Process Count: " << group_process_count);
    LOG("Total CPU Time: " << group_cpu_time << " ticks");
    LOG("Total I/O Bytes: " << group_io_bytes);
    LOG("========================================");
    
    return true;
}

bool ProcessDebuggingManager::GenerateSessionReport(uint32 sid) {
    if (!is_initialized) {
        return false;
    }
    
    LOG("=== Process Debugging Report for SID " << sid << " ===");
    
    // Count processes in this session
    uint32 session_process_count = 0;
    uint64 session_cpu_time = 0;
    uint64 session_io_bytes = 0;
    
    for (uint32 i = 0; i < buffer.count && session_process_count < 100; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (buffer.records[index].session_id == sid) {
            session_process_count++;
            session_cpu_time += buffer.records[index].cpu_time;
            session_io_bytes += (buffer.records[index].read_bytes + buffer.records[index].write_bytes);
        }
    }
    
    LOG("Session Process Count: " << session_process_count);
    LOG("Total CPU Time: " << session_cpu_time << " ticks");
    LOG("Total I/O Bytes: " << session_io_bytes);
    LOG("========================================");
    
    return true;
}

bool ProcessDebuggingManager::GenerateSystemLoadReport() {
    if (!is_initialized) {
        return false;
    }
    
    LOG("=== System Load Report ===");
    
    // Calculate system load metrics
    uint32 current_time = global_timer ? global_timer->GetTickCount() : 0;
    uint32 elapsed_time = current_time - last_update_time;
    
    if (elapsed_time > 0) {
        double processes_per_second = (double)stats.total_processes / (double)elapsed_time * 1000.0;
        double cpu_utilization = (stats.total_cpu_time > 0) ? 
            (double)stats.total_cpu_time / (double)elapsed_time * 100.0 : 0.0;
        
        LOG("Processes per Second: " << processes_per_second);
        LOG("CPU Utilization: " << cpu_utilization << "%");
        LOG("Active Processes: " << stats.active_processes);
        LOG("Terminated Processes: " << stats.terminated_processes);
    }
    
    LOG("==========================");
    
    return true;
}

bool ProcessDebuggingManager::GenerateResourceUsageReport() {
    if (!is_initialized) {
        return false;
    }
    
    LOG("=== Resource Usage Report ===");
    
    LOG("Total CPU Time: " << stats.total_cpu_time << " ticks");
    LOG("Total User Time: " << stats.total_user_time << " ticks");
    LOG("Total System Time: " << stats.total_system_time << " ticks");
    LOG("Total I/O Bytes: " << (stats.total_read_bytes + stats.total_write_bytes));
    LOG("Total Page Faults: " << stats.total_page_faults);
    LOG("Total Context Switches: " << stats.total_context_switches);
    LOG("==============================");
    
    return true;
}

bool ProcessDebuggingManager::GeneratePerformanceReport() {
    if (!is_initialized) {
        return false;
    }
    
    LOG("=== Performance Report ===");
    
    // Calculate performance metrics
    uint32 current_time = global_timer ? global_timer->GetTickCount() : 0;
    uint32 elapsed_time = current_time - last_update_time;
    
    if (elapsed_time > 0) {
        double avg_cpu_time = (stats.total_processes > 0) ? 
            (double)stats.total_cpu_time / (double)stats.total_processes : 0.0;
        double avg_context_switches = (stats.total_processes > 0) ? 
            (double)stats.total_context_switches / (double)stats.total_processes : 0.0;
        double avg_page_faults = (stats.total_processes > 0) ? 
            (double)stats.total_page_faults / (double)stats.total_processes : 0.0;
        
        LOG("Average CPU Time per Process: " << avg_cpu_time << " ticks");
        LOG("Average Context Switches per Process: " << avg_context_switches);
        LOG("Average Page Faults per Process: " << avg_page_faults);
        LOG("Buffer Usage: " << GetBufferUsage() << "%");
        LOG("Buffer Free Space: " << GetBufferFreeSpace() << " records");
    }
    
    LOG("==========================");
    
    return true;
}

const ProcessDebuggingStats* ProcessDebuggingManager::GetStatistics() {
    UpdateStatistics();
    return &stats;
}

void ProcessDebuggingManager::ResetStatistics() {
    memset(&stats, 0, sizeof(stats));
    DLOG("Process debugging statistics reset");
}

void ProcessDebuggingManager::UpdateStatistics() {
    if (!is_initialized) {
        return;
    }
    
    // Update active process count
    stats.active_processes = 0;
    stats.terminated_processes = 0;
    
    if (process_manager) {
        ProcessControlBlock* current = process_manager->GetProcessListHead();
        while (current) {
            if (current->state == PROCESS_STATE_TERMINATED || 
                current->state == PROCESS_STATE_ZOMBIE) {
                stats.terminated_processes++;
            } else {
                stats.active_processes++;
            }
            current = current->next;
        }
    }
    
    stats.total_processes = stats.active_processes + stats.terminated_processes;
    
    DLOG("Updated process debugging statistics");
}

uint64 ProcessDebuggingManager::GetTotalCPUTime() {
    return stats.total_cpu_time;
}

uint64 ProcessDebuggingManager::GetTotalIOTime() {
    return stats.total_read_bytes + stats.total_write_bytes;
}

uint32 ProcessDebuggingManager::GetAverageProcessLifetime() {
    if (stats.terminated_processes == 0) {
        return 0;
    }
    
    return (uint32)(stats.total_cpu_time / stats.terminated_processes);
}

uint32 ProcessDebuggingManager::GetPeakProcessCount() {
    return stats.total_processes;
}

uint32 ProcessDebuggingManager::GetProcessCreationRate() {
    uint32 current_time = global_timer ? global_timer->GetTickCount() : 0;
    uint32 elapsed_time = current_time - last_update_time;
    
    if (elapsed_time > 0) {
        return stats.total_processes / elapsed_time * 1000; // Per second rate
    }
    
    return 0;
}

const char* ProcessDebuggingManager::GetProcessCommand(uint32 pid) {
    if (!process_manager) {
        return nullptr;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return nullptr;
    }
    
    return process->name;
}

uint32 ProcessDebuggingManager::GetProcessStartTime(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    return process->creation_time;
}

uint32 ProcessDebuggingManager::GetProcessEndTime(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    return process->termination_time;
}

uint32 ProcessDebuggingManager::GetProcessCPUTime(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    return process->total_cpu_time_used;
}

uint32 ProcessDebuggingManager::GetProcessMemoryUsage(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    return process->heap_end - process->heap_start;
}

uint32 ProcessDebuggingManager::GetProcessIOBytes(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    // Return approximate I/O bytes (dummy implementation)
    return process->total_cpu_time_used * 1536; // Read + Write bytes
}

uint32 ProcessDebuggingManager::GetProcessPageFaults(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    // Return approximate page faults (dummy implementation)
    return process->total_cpu_time_used / 100;
}

uint32 ProcessDebuggingManager::GetProcessContextSwitches(uint32 pid) {
    if (!process_manager) {
        return 0;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return 0;
    }
    
    return process->context_switch_count;
}

bool ProcessDebuggingManager::MonitorProcess(uint32 pid) {
    return StartDebugging(pid);
}

bool ProcessDebuggingManager::UnmonitorProcess(uint32 pid) {
    return StopDebugging(pid);
}

bool ProcessDebuggingManager::IsProcessMonitored(uint32 pid) {
    return IsDebugEnabled(pid);
}

uint32 ProcessDebuggingManager::GetMonitoredProcessCount() {
    if (!process_manager) {
        return 0;
    }
    
    uint32 count = 0;
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    
    while (current) {
        if (IsDebugEnabled(current->pid)) {
            count++;
        }
        current = current->next;
    }
    
    return count;
}

void ProcessDebuggingManager::MonitorAllProcesses() {
    if (!process_manager) {
        return;
    }
    
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        StartDebugging(current->pid);
        current = current->next;
    }
    
    DLOG("Monitoring all processes");
}

void ProcessDebuggingManager::UnmonitorAllProcesses() {
    if (!process_manager) {
        return;
    }
    
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        StopDebugging(current->pid);
        current = current->next;
    }
    
    DLOG("Unmonitoring all processes");
}

void ProcessDebuggingManager::OnProcessCreate(uint32 pid) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Start debugging for new process
    StartDebugging(pid);
    
    // Update statistics
    stats.total_processes++;
    stats.active_processes++;
    
    DLOG("Debugging started for new process PID " << pid);
}

void ProcessDebuggingManager::OnProcessTerminate(uint32 pid) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update statistics
    stats.terminated_processes++;
    if (stats.active_processes > 0) {
        stats.active_processes--;
    }
    
    // Create final debugging record
    ProcessDebuggingRecord record;
    if (CollectProcessData(pid, &record)) {
        record.end_time = global_timer ? global_timer->GetTickCount() : 0;
        AddRecord(&record);
    }
    
    DLOG("Debugging finalized for terminated process PID " << pid);
}

void ProcessDebuggingManager::OnProcessSwitch(uint32 old_pid, uint32 new_pid) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update debugging for both processes
    if (old_pid != INVALID_PID) {
        UpdateDebugging(old_pid);
    }
    
    if (new_pid != INVALID_PID) {
        UpdateDebugging(new_pid);
    }
    
    DLOG("Debugging updated for process switch: " << old_pid << " -> " << new_pid);
}

void ProcessDebuggingManager::OnSystemCall(uint32 pid, uint32 syscall_number) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update system call count
    stats.total_processes++; // Increment total to account for the syscall
    UpdateDebugging(pid);
    
    DLOG("Debugging updated for system call " << syscall_number << " by PID " << pid);
}

void ProcessDebuggingManager::OnPageFault(uint32 pid) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update page fault statistics
    stats.total_page_faults++;
    UpdateDebugging(pid);
    
    DLOG("Debugging updated for page fault by PID " << pid);
}

void ProcessDebuggingManager::OnContextSwitch(uint32 pid) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update context switch statistics
    stats.total_context_switches++;
    UpdateDebugging(pid);
    
    DLOG("Debugging updated for context switch by PID " << pid);
}

void ProcessDebuggingManager::OnTimerTick() {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Check if it's time for a snapshot
    uint32 current_time = global_timer ? global_timer->GetTickCount() : 0;
    
    if (config.update_interval > 0 && 
        (current_time - last_update_time) >= config.update_interval) {
        SnapshotAllProcesses();
        last_update_time = current_time;
    }
    
    // Handle log rotation if needed
    if (config.auto_rotate) {
        // In a real implementation, we'd check log file size
        // For now, we'll just log that we would check
        static uint32 rotation_check_counter = 0;
        rotation_check_counter++;
        
        if (rotation_check_counter % 1000 == 0) {  // Check every 1000 ticks
            RotateLogFile();
            rotation_check_counter = 0;
        }
    }
}

void ProcessDebuggingManager::OnIOPerformed(uint32 pid, uint32 bytes_read, uint32 bytes_written) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update I/O statistics
    stats.total_read_bytes += bytes_read;
    stats.total_write_bytes += bytes_written;
    
    DLOG("Debugging updated for I/O: PID " << pid 
         << ", Read: " << bytes_read << " bytes, Write: " << bytes_written << " bytes");
}

void ProcessDebuggingManager::OnSignalDelivered(uint32 pid, uint32 signal) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Update signal statistics
    stats.total_signals++;
    UpdateDebugging(pid);
    
    DLOG("Debugging updated for signal " << signal << " delivered to PID " << pid);
}

void ProcessDebuggingManager::OnResourceLimitExceeded(uint32 pid, uint32 resource) {
    if (!is_initialized || !IsEnabled()) {
        return;
    }
    
    // Log resource limit exceeded
    LOG("Process PID " << pid << " exceeded resource limit " << resource);
    
    // Update debugging error statistics
    stats.debugging_errors++;
    
    DLOG("Debugging error recorded for PID " << pid);
}

bool ProcessDebuggingManager::ResizeBuffer(uint32 new_capacity) {
    if (!is_initialized) {
        return false;
    }
    
    if (new_capacity == 0 || new_capacity > config.max_records) {
        LOG("Invalid buffer capacity: " << new_capacity);
        return false;
    }
    
    // Allocate new buffers
    ProcessDebuggingRecord* new_records = (ProcessDebuggingRecord*)malloc(
        sizeof(ProcessDebuggingRecord) * new_capacity);
    uint32* new_timestamps = (uint32*)malloc(sizeof(uint32) * new_capacity);
    
    if (!new_records || !new_timestamps) {
        LOG("Failed to allocate new debugging buffers");
        if (new_records) free(new_records);
        if (new_timestamps) free(new_timestamps);
        return false;
    }
    
    // Copy existing data
    uint32 copy_count = (buffer.count < new_capacity) ? buffer.count : new_capacity;
    for (uint32 i = 0; i < copy_count; i++) {
        uint32 src_index = (buffer.head + i) % buffer.capacity;
        uint32 dst_index = i;
        memcpy(&new_records[dst_index], &buffer.records[src_index], sizeof(ProcessDebuggingRecord));
        new_timestamps[dst_index] = buffer.timestamps[src_index];
    }
    
    // Replace buffers
    free(buffer.records);
    free(buffer.timestamps);
    buffer.records = new_records;
    buffer.timestamps = new_timestamps;
    buffer.capacity = new_capacity;
    buffer.count = copy_count;
    buffer.head = 0;
    buffer.tail = copy_count;
    buffer.is_full = (copy_count == new_capacity);
    
    DLOG("Resized debugging buffer to " << new_capacity << " records");
    return true;
}

bool ProcessDebuggingManager::FlushBuffer() {
    if (!is_initialized || !IsEnabled()) {
        return false;
    }
    
    // Write all records to file
    if (config.flags & DEBUG_FLAG_TO_FILE) {
        WriteAllRecordsToFile();
    }
    
    // Clear buffer
    ClearRecords();
    
    DLOG("Flushed debugging buffer");
    return true;
}

bool ProcessDebuggingManager::IsBufferFull() {
    return buffer.is_full;
}

uint32 ProcessDebuggingManager::GetBufferUsage() {
    if (buffer.capacity == 0) {
        return 0;
    }
    
    return (buffer.count * 100) / buffer.capacity;
}

uint32 ProcessDebuggingManager::GetBufferFreeSpace() {
    return buffer.capacity - buffer.count;
}

void ProcessDebuggingManager::PrintDebuggingSummary() {
    LOG("=== Process Debugging Summary ===");
    LOG("Initialized: " << (is_initialized ? "Yes" : "No"));
    LOG("Enabled: " << (IsEnabled() ? "Yes" : "No"));
    LOG("Buffer Capacity: " << buffer.capacity);
    LOG("Buffer Count: " << buffer.count);
    LOG("Buffer Usage: " << GetBufferUsage() << "%");
    LOG("Buffer Free Space: " << GetBufferFreeSpace() << " records");
    LOG("Records Processed: " << stats.total_processes);
    LOG("Active Processes: " << stats.active_processes);
    LOG("Terminated Processes: " << stats.terminated_processes);
    LOG("=================================");
}

void ProcessDebuggingManager::PrintProcessDebugging(uint32 pid) {
    ProcessDebuggingRecord record;
    if (GetRecord(pid, &record)) {
        LOG("=== Debugging for PID " << pid << " ===");
        LOG("Command: " << record.command);
        LOG("User: " << record.uid << ", Group: " << record.gid);
        LOG("Start Time: " << record.start_time);
        LOG("End Time: " << record.end_time);
        LOG("CPU Time: " << record.cpu_time << " ticks");
        LOG("User Time: " << record.user_time << " ticks");
        LOG("System Time: " << record.system_time << " ticks");
        LOG("Wait Time: " << record.wait_time << " ticks");
        LOG("Read Bytes: " << record.read_bytes);
        LOG("Write Bytes: " << record.write_bytes);
        LOG("Page Faults: " << record.page_faults);
        LOG("Context Switches: " << record.context_switches);
        LOG("===============================");
    } else {
        LOG("No debugging record found for PID " << pid);
    }
}

void ProcessDebuggingManager::PrintAllProcessDebugging() {
    LOG("=== All Process Debugging Records ===");
    LOG("Total Records: " << buffer.count);
    LOG("Buffer Capacity: " << buffer.capacity);
    LOG("Buffer Usage: " << GetBufferUsage() << "%");
    
    for (uint32 i = 0; i < buffer.count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        ProcessDebuggingRecord* record = &buffer.records[index];
        LOG("PID: " << record->pid 
            << ", Command: " << record->command
            << ", CPU Time: " << record->cpu_time << " ticks"
            << ", Memory: " << record->memory_max << " bytes"
            << ", Time: " << buffer.timestamps[index]);
    }
    
    LOG("=====================================");
}

void ProcessDebuggingManager::PrintDebuggingStatistics() {
    UpdateStatistics();
    LOG("=== Process Debugging Statistics ===");
    LOG("Total Processes: " << stats.total_processes);
    LOG("Active Processes: " << stats.active_processes);
    LOG("Terminated Processes: " << stats.terminated_processes);
    LOG("Total CPU Time: " << stats.total_cpu_time << " ticks");
    LOG("Total User Time: " << stats.total_user_time << " ticks");
    LOG("Total System Time: " << stats.total_system_time << " ticks");
    LOG("Total Wait Time: " << stats.total_wait_time << " ticks");
    LOG("Total Read Bytes: " << stats.total_read_bytes);
    LOG("Total Write Bytes: " << stats.total_write_bytes);
    LOG("Total Page Faults: " << stats.total_page_faults);
    LOG("Total Context Switches: " << stats.total_context_switches);
    LOG("Total Signals: " << stats.total_signals);
    LOG("Debugging Errors: " << stats.debugging_errors);
    LOG("Buffer Overflows: " << stats.buffer_overflows);
    LOG("Disk Writes: " << stats.disk_writes);
    LOG("Log Rotations: " << stats.log_rotations);
    LOG("Compressed Records: " << stats.compressed_records);
    LOG("===================================");
}

void ProcessDebuggingManager::PrintDebuggingConfiguration() {
    LOG("=== Process Debugging Configuration ===");
    LOG("Flags: 0x" << config.flags);
    LOG("Update Interval: " << config.update_interval << " ticks");
    LOG("Buffer Size: " << config.buffer_size << " records");
    LOG("Max Records: " << config.max_records);
    LOG("Log File: " << config.log_file);
    LOG("Auto Rotate: " << (config.auto_rotate ? "Yes" : "No"));
    LOG("Rotate Size: " << config.rotate_size << " bytes");
    LOG("Retention Days: " << config.retention_days);
    LOG("Compress Old: " << (config.compress_old ? "Yes" : "No"));
    LOG("Compression Threshold: " << config.compression_threshold << " days");
    LOG("=====================================");
}

void ProcessDebuggingManager::PrintBufferStatus() {
    LOG("=== Debugging Buffer Status ===");
    LOG("Capacity: " << buffer.capacity);
    LOG("Count: " << buffer.count);
    LOG("Head: " << buffer.head);
    LOG("Tail: " << buffer.tail);
    LOG("Is Full: " << (buffer.is_full ? "Yes" : "No"));
    LOG("Usage: " << GetBufferUsage() << "%");
    LOG("Free Space: " << GetBufferFreeSpace() << " records");
    LOG("===============================");
}

void ProcessDebuggingManager::DumpDebuggingData() {
    LOG("=== Debugging Data Dump ===");
    
    // Dump raw buffer data
    for (uint32 i = 0; i < buffer.count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        ProcessDebuggingRecord* record = &buffer.records[index];
        LOG("Index: " << i << ", PID: " << record->pid 
            << ", Command: " << record->command
            << ", CPU Time: " << record->cpu_time << " ticks"
            << ", Memory: " << record->memory_max << " bytes"
            << ", Time: " << buffer.timestamps[index]);
    }
    
    LOG("=============================");
}

void ProcessDebuggingManager::ValidateDebuggingData() {
    LOG("=== Validating Debugging Data ===");
    
    bool is_valid = true;
    
    // Validate buffer consistency
    if (buffer.count > buffer.capacity) {
        LOG("ERROR: Buffer count (" << buffer.count 
            << ") exceeds capacity (" << buffer.capacity << ")");
        is_valid = false;
    }
    
    if (buffer.is_full && buffer.count != buffer.capacity) {
        LOG("WARNING: Buffer marked as full but count (" << buffer.count 
            << ") != capacity (" << buffer.capacity << ")");
    }
    
    // Validate record data
    for (uint32 i = 0; i < buffer.count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        ProcessDebuggingRecord* record = &buffer.records[index];
        
        if (record->pid == 0) {
            LOG("WARNING: Record " << i << " has invalid PID: " << record->pid);
        }
        
        if (record->cpu_time > 1000000) {  // Arbitrary large value check
            LOG("WARNING: Record " << i << " has unusually high CPU time: " << record->cpu_time);
        }
    }
    
    LOG("Validation " << (is_valid ? "PASSED" : "FAILED"));
    LOG("===============================");
}

bool ProcessDebuggingManager::ExportToCSV(const char* filename) {
    if (!filename || !is_initialized) {
        return false;
    }
    
    LOG("Exporting debugging data to CSV file: " << filename);
    
    // In a real implementation, we'd write to the CSV file
    // For now, we'll just log that we would export
    
    return true;
}

bool ProcessDebuggingManager::ExportToJSON(const char* filename) {
    if (!filename || !is_initialized) {
        return false;
    }
    
    LOG("Exporting debugging data to JSON file: " << filename);
    
    // In a real implementation, we'd write to the JSON file
    // For now, we'll just log that we would export
    
    return true;
}

bool ProcessDebuggingManager::ExportToXML(const char* filename) {
    if (!filename || !is_initialized) {
        return false;
    }
    
    LOG("Exporting debugging data to XML file: " << filename);
    
    // In a real implementation, we'd write to the XML file
    // For now, we'll just log that we would export
    
    return true;
}

bool ProcessDebuggingManager::ImportFromCSV(const char* filename) {
    if (!filename || !is_initialized) {
        return false;
    }
    
    LOG("Importing debugging data from CSV file: " << filename);
    
    // In a real implementation, we'd read from the CSV file
    // For now, we'll just log that we would import
    
    return true;
}

bool ProcessDebuggingManager::ImportFromJSON(const char* filename) {
    if (!filename || !is_initialized) {
        return false;
    }
    
    LOG("Importing debugging data from JSON file: " << filename);
    
    // In a real implementation, we'd read from the JSON file
    // For now, we'll just log that we would import
    
    return true;
}

bool ProcessDebuggingManager::ImportFromXML(const char* filename) {
    if (!filename || !is_initialized) {
        return false;
    }
    
    LOG("Importing debugging data from XML file: " << filename);
    
    // In a real implementation, we'd read from the XML file
    // For now, we'll just log that we would import
    
    return true;
}

void ProcessDebuggingManager::SortRecordsByCPUTime(ProcessDebuggingRecord* records, uint32 count) {
    if (!records || count <= 1) {
        return;
    }
    
    // Simple bubble sort by CPU time (descending order)
    for (uint32 i = 0; i < count - 1; i++) {
        for (uint32 j = 0; j < count - i - 1; j++) {
            if (records[j].cpu_time < records[j + 1].cpu_time) {
                // Swap records
                ProcessDebuggingRecord temp = records[j];
                records[j] = records[j + 1];
                records[j + 1] = temp;
            }
        }
    }
}

void ProcessDebuggingManager::SortRecordsByMemoryUsage(ProcessDebuggingRecord* records, uint32 count) {
    if (!records || count <= 1) {
        return;
    }
    
    // Simple bubble sort by memory usage (descending order)
    for (uint32 i = 0; i < count - 1; i++) {
        for (uint32 j = 0; j < count - i - 1; j++) {
            if (records[j].memory_max < records[j + 1].memory_max) {
                // Swap records
                ProcessDebuggingRecord temp = records[j];
                records[j] = records[j + 1];
                records[j + 1] = temp;
            }
        }
    }
}

void ProcessDebuggingManager::SortRecordsByStartTime(ProcessDebuggingRecord* records, uint32 count) {
    if (!records || count <= 1) {
        return;
    }
    
    // Simple bubble sort by start time (ascending order)
    for (uint32 i = 0; i < count - 1; i++) {
        for (uint32 j = 0; j < count - i - 1; j++) {
            if (records[j].start_time > records[j + 1].start_time) {
                // Swap records
                ProcessDebuggingRecord temp = records[j];
                records[j] = records[j + 1];
                records[j + 1] = temp;
            }
        }
    }
}

void ProcessDebuggingManager::FilterRecordsByCommand(const char* command, ProcessDebuggingRecord* records, uint32 count) {
    if (!command || !records || count == 0) {
        return;
    }
    
    // Filter records by command name
    uint32 filtered_count = 0;
    
    for (uint32 i = 0; i < buffer.count && filtered_count < count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (strstr(buffer.records[index].command, command)) {
            memcpy(&records[filtered_count], &buffer.records[index], sizeof(ProcessDebuggingRecord));
            filtered_count++;
        }
    }
    
    // Clear remaining slots
    for (uint32 i = filtered_count; i < count; i++) {
        memset(&records[i], 0, sizeof(ProcessDebuggingRecord));
    }
}

void ProcessDebuggingManager::FilterRecordsByExitStatus(uint32 exit_status, ProcessDebuggingRecord* records, uint32 count) {
    if (!records || count == 0) {
        return;
    }
    
    // Filter records by exit status
    uint32 filtered_count = 0;
    
    for (uint32 i = 0; i < buffer.count && filtered_count < count; i++) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (buffer.records[index].exit_status == exit_status) {
            memcpy(&records[filtered_count], &buffer.records[index], sizeof(ProcessDebuggingRecord));
            filtered_count++;
        }
    }
    
    // Clear remaining slots
    for (uint32 i = filtered_count; i < count; i++) {
        memset(&records[i], 0, sizeof(ProcessDebuggingRecord));
    }
}

bool ProcessDebuggingManager::SetCPUThreshold(uint32 pid, uint32 threshold) {
    if (!process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Set CPU time threshold (we'll use a flag to indicate this)
    process->flags |= (threshold << 8);  // Store threshold in upper bits of flags
    
    DLOG("Set CPU threshold for PID " << pid << " to " << threshold << " ticks");
    return true;
}

bool ProcessDebuggingManager::SetMemoryThreshold(uint32 pid, uint32 threshold) {
    if (!process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Set memory usage threshold
    process->flags |= (threshold << 16);  // Store threshold in upper bits of flags
    
    DLOG("Set memory threshold for PID " << pid << " to " << threshold << " bytes");
    return true;
}

bool ProcessDebuggingManager::SetIOTreshold(uint32 pid, uint32 threshold) {
    if (!process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Set I/O threshold
    process->flags |= (threshold << 24);  // Store threshold in upper bits of flags
    
    DLOG("Set I/O threshold for PID " << pid << " to " << threshold << " bytes");
    return true;
}

bool ProcessDebuggingManager::CheckThresholds(uint32 pid) {
    if (!process_manager) {
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        return false;
    }
    
    // Check CPU threshold
    uint32 cpu_threshold = (process->flags >> 8) & 0xFF;
    if (cpu_threshold > 0 && process->total_cpu_time_used >= cpu_threshold) {
        OnThresholdExceeded(pid, 1, process->total_cpu_time_used);
        return true;
    }
    
    // Check memory threshold
    uint32 memory_threshold = (process->flags >> 16) & 0xFF;
    uint32 memory_usage = process->heap_end - process->heap_start;
    if (memory_threshold > 0 && memory_usage >= memory_threshold) {
        OnThresholdExceeded(pid, 2, memory_usage);
        return true;
    }
    
    // Check I/O threshold
    uint32 io_threshold = (process->flags >> 24) & 0xFF;
    uint32 io_bytes = process->total_cpu_time_used * 1536; // Approximate I/O bytes
    if (io_threshold > 0 && io_bytes >= io_threshold) {
        OnThresholdExceeded(pid, 3, io_bytes);
        return true;
    }
    
    return false;
}

void ProcessDebuggingManager::OnThresholdExceeded(uint32 pid, uint32 resource, uint32 value) {
    LOG("Process PID " << pid << " exceeded threshold for resource " << resource 
        << " with value " << value);
    
    // In a real implementation, we might send a signal to the process
    // or take other corrective action
}

bool ProcessDebuggingManager::IsThresholdExceeded(uint32 pid, uint32 resource) {
    // Check if a threshold has been exceeded for a specific resource
    // This is a simplified implementation
    return CheckThresholds(pid);
}

bool ProcessDebuggingManager::CleanupOldRecords() {
    if (!is_initialized) {
        return false;
    }
    
    uint32 current_time = global_timer ? global_timer->GetTickCount() : 0;
    uint32 cutoff_time = current_time - (config.retention_days * 24 * 60 * 60 * 1000); // Convert days to milliseconds
    
    uint32 cleanup_count = 0;
    
    // Remove records older than cutoff time
    for (uint32 i = 0; i < buffer.count; ) {
        uint32 index = (buffer.head + i) % buffer.capacity;
        if (buffer.timestamps[index] < cutoff_time) {
            // Remove this record
            RemoveRecord(buffer.records[index].pid);
            cleanup_count++;
            // Don't increment i since we removed a record
        } else {
            i++; // Move to next record
        }
    }
    
    if (cleanup_count > 0) {
        DLOG("Cleaned up " << cleanup_count << " old debugging records");
    }
    
    return true;
}

bool ProcessDebuggingManager::CleanupTerminatedProcesses() {
    if (!is_initialized || !process_manager) {
        return false;
    }
    
    uint32 cleanup_count = 0;
    
    // Clean up debugging for terminated processes
    ProcessControlBlock* current = process_manager->GetProcessListHead();
    while (current) {
        if (current->state == PROCESS_STATE_TERMINATED || 
            current->state == PROCESS_STATE_ZOMBIE) {
            if (IsDebugEnabled(current->pid)) {
                StopDebugging(current->pid);
                cleanup_count++;
            }
        }
        current = current->next;
    }
    
    if (cleanup_count > 0) {
        DLOG("Cleaned up debugging for " << cleanup_count << " terminated processes");
    }
    
    return true;
}

bool ProcessDebuggingManager::PurgeAllRecords() {
    if (!is_initialized) {
        return false;
    }
    
    // Clear all records
    ClearRecords();
    
    // Reset statistics
    ResetStatistics();
    
    DLOG("Purged all debugging records");
    return true;
}

uint32 ProcessDebuggingManager::GetCleanupCount() {
    // Return approximate cleanup count
    return stats.debugging_errors + stats.buffer_overflows;
}

// System call implementations
uint32 SysCallEnableProcessDebugging() {
    if (!g_process_debugging_manager) {
        LOG("Process debugging manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (g_process_debugging_manager->Enable()) {
        return SUCCESS;
    }
    
    return ERROR_OPERATION_FAILED;
}

uint32 SysCallDisableProcessDebugging() {
    if (!g_process_debugging_manager) {
        LOG("Process debugging manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (g_process_debugging_manager->Disable()) {
        return SUCCESS;
    }
    
    return ERROR_OPERATION_FAILED;
}

uint32 SysCallGetProcessDebugging(uint32 pid, ProcessDebuggingRecord* record) {
    if (!g_process_debugging_manager) {
        LOG("Process debugging manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!record) {
        return ERROR_INVALID_PARAMETER;
    }
    
    if (g_process_debugging_manager->GetRecord(pid, record)) {
        return SUCCESS;
    }
    
    return ERROR_NOT_FOUND;
}

uint32 SysCallGetProcessResourceUsage(uint32 pid, ProcessResourceUsage* usage) {
    if (!g_process_debugging_manager) {
        LOG("Process debugging manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!usage) {
        return ERROR_INVALID_PARAMETER;
    }
    
    if (g_process_debugging_manager->CollectResourceUsage(pid, usage)) {
        return SUCCESS;
    }
    
    return ERROR_NOT_FOUND;
}

uint32 SysCallSetDebuggingConfig(const ProcessDebuggingConfig* config) {
    if (!g_process_debugging_manager) {
        LOG("Process debugging manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        return ERROR_INVALID_PARAMETER;
    }
    
    if (g_process_debugging_manager->Configure(config)) {
        return SUCCESS;
    }
    
    return ERROR_INVALID_PARAMETER;
}

uint32 SysCallGetDebuggingConfig(ProcessDebuggingConfig* config) {
    if (!g_process_debugging_manager) {
        LOG("Process debugging manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        return ERROR_INVALID_PARAMETER;
    }
    
    // Copy current configuration
    memcpy(config, &g_process_debugging_manager->config, sizeof(ProcessDebuggingConfig));
    
    return SUCCESS;
}

uint32 SysCallGetDebuggingStatistics(ProcessDebuggingStats* stats) {
    if (!g_process_debugging_manager) {
        LOG("Process debugging manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!stats) {
        return ERROR_INVALID_PARAMETER;
    }
    
    const ProcessDebuggingStats* dbg_stats = g_process_debugging_manager->GetStatistics();
    if (dbg_stats) {
        memcpy(stats, dbg_stats, sizeof(ProcessDebuggingStats));
        return SUCCESS;
    }
    
    return ERROR_OPERATION_FAILED;
}

uint32 SysCallResetDebugging() {
    if (!g_process_debugging_manager) {
        LOG("Process debugging manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    g_process_debugging_manager->Reset();
    return SUCCESS;
}

uint32 SysCallExportDebuggingData(const char* filename, uint32 format) {
    if (!g_process_debugging_manager) {
        LOG("Process debugging manager not available");
        return ERROR_NOT_INITIALIZED;
    }
    
    if (!filename) {
        return ERROR_INVALID_PARAMETER;
    }
    
    bool result = false;
    
    switch (format) {
        case 0: // CSV
            result = g_process_debugging_manager->ExportToCSV(filename);
            break;
        case 1: // JSON
            result = g_process_debugging_manager->ExportToJSON(filename);
            break;
        case 2: // XML
            result = g_process_debugging_manager->ExportToXML(filename);
            break;
        default:
            LOG("Unsupported export format: " << format);
            return ERROR_INVALID_PARAMETER;
    }
    
    return result ? SUCCESS : ERROR_OPERATION_FAILED;
}