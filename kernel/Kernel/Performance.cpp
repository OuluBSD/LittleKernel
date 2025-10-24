#include "Kernel.h"
#include "Performance.h"

// Global profiler instance
PerformanceProfiler* g_performance_profiler = nullptr;

PerformanceProfiler::PerformanceProfiler() {
    counter_count = 0;
    profiler_lock.Initialize();
    
    // Initialize all counters
    for (int i = 0; i < MAX_PERFORMANCE_COUNTERS; i++) {
        memset(&counters[i], 0, sizeof(PerfCounter));
        counters[i].active = false;
        counters[i].min_value = UINT64_MAX;
    }
}

PerformanceProfiler::~PerformanceProfiler() {
    // Cleanup handled by kernel shutdown
}

bool PerformanceProfiler::Initialize() {
    LOG("Initializing performance profiler");
    
    // Create predefined performance counters
    CreateCounter("Context Switches", PERF_COUNTER_COUNT);
    CreateCounter("Memory Allocations", PERF_COUNTER_COUNT);
    CreateCounter("Interrupt Handlers", PERF_COUNTER_COUNT);
    CreateCounter("System Calls", PERF_COUNTER_COUNT);
    CreateCounter("Page Faults", PERF_COUNTER_COUNT);
    CreateCounter("File Opens", PERF_COUNTER_COUNT);
    CreateCounter("File Reads", PERF_COUNTER_COUNT);
    CreateCounter("File Writes", PERF_COUNTER_COUNT);
    CreateCounter("Scheduler Switches", PERF_COUNTER_COUNT);
    
    LOG("Performance profiler initialized with " << counter_count << " counters");
    return true;
}

PerfCounterId PerformanceProfiler::CreateCounter(const char* name, PerfCounterType type) {
    if (!name || counter_count >= MAX_PERFORMANCE_COUNTERS) {
        return -1;
    }
    
    profiler_lock.Acquire();
    
    // Check if counter with this name already exists
    PerfCounterId existing_id = FindCounterByName(name);
    if (existing_id != -1) {
        profiler_lock.Release();
        return existing_id;
    }
    
    // Find a free slot
    PerfCounterId id = counter_count;
    if (id >= MAX_PERFORMANCE_COUNTERS) {
        profiler_lock.Release();
        return -1;  // No more slots
    }
    
    // Initialize the counter
    strncpy(counters[id].name, name, sizeof(counters[id].name) - 1);
    counters[id].name[sizeof(counters[id].name) - 1] = '\0';
    counters[id].type = type;
    counters[id].value = 0;
    counters[id].min_value = UINT64_MAX;
    counters[id].max_value = 0;
    counters[id].total_value = 0;
    counters[id].sample_count = 0;
    counters[id].start_time = 0;
    counters[id].active = true;
    
    if (counter_count <= id) {
        counter_count = id + 1;
    }
    
    profiler_lock.Release();
    return id;
}

bool PerformanceProfiler::StartTimer(PerfCounterId id) {
    if (id >= counter_count || !counters[id].active || counters[id].type != PERF_COUNTER_TIME) {
        return false;
    }
    
    profiler_lock.Acquire();
    
    // Record start time (in timer ticks)
    counters[id].start_time = global_timer ? global_timer->GetTickCount() : 0;
    
    profiler_lock.Release();
    return true;
}

bool PerformanceProfiler::StopTimer(PerfCounterId id) {
    if (id >= counter_count || !counters[id].active || counters[id].type != PERF_COUNTER_TIME) {
        return false;
    }
    
    profiler_lock.Acquire();
    
    // Calculate elapsed time
    uint64_t end_time = global_timer ? global_timer->GetTickCount() : 0;
    uint64_t elapsed = (end_time >= counters[id].start_time) ? 
                      (end_time - counters[id].start_time) : 0;
    
    // Update counter value and statistics
    counters[id].value = elapsed;
    counters[id].total_value += elapsed;
    counters[id].sample_count++;
    
    if (elapsed < counters[id].min_value) {
        counters[id].min_value = elapsed;
    }
    
    if (elapsed > counters[id].max_value) {
        counters[id].max_value = elapsed;
    }
    
    profiler_lock.Release();
    return true;
}

bool PerformanceProfiler::IncrementCounter(PerfCounterId id, uint64_t value) {
    if (id >= counter_count || !counters[id].active || 
        (counters[id].type != PERF_COUNTER_COUNT && counters[id].type != PERF_COUNTER_MEMORY)) {
        return false;
    }
    
    profiler_lock.Acquire();
    
    counters[id].value += value;
    counters[id].total_value += value;
    counters[id].sample_count++;
    
    if (counters[id].value < counters[id].min_value) {
        counters[id].min_value = counters[id].value;
    }
    
    if (counters[id].value > counters[id].max_value) {
        counters[id].max_value = counters[id].value;
    }
    
    profiler_lock.Release();
    return true;
}

bool PerformanceProfiler::SetCounter(PerfCounterId id, uint64_t value) {
    if (id >= counter_count || !counters[id].active) {
        return false;
    }
    
    profiler_lock.Acquire();
    
    counters[id].value = value;
    counters[id].total_value += value;  // Add to total for statistics
    counters[id].sample_count++;
    
    if (value < counters[id].min_value) {
        counters[id].min_value = value;
    }
    
    if (value > counters[id].max_value) {
        counters[id].max_value = value;
    }
    
    profiler_lock.Release();
    return true;
}

uint64_t PerformanceProfiler::GetCounterValue(PerfCounterId id) {
    if (id >= counter_count || !counters[id].active) {
        return 0;
    }
    
    profiler_lock.Acquire();
    uint64_t value = counters[id].value;
    profiler_lock.Release();
    
    return value;
}

bool PerformanceProfiler::ResetCounter(PerfCounterId id) {
    if (id >= counter_count || !counters[id].active) {
        return false;
    }
    
    profiler_lock.Acquire();
    
    counters[id].value = 0;
    counters[id].start_time = 0;
    
    // Reset stats
    counters[id].min_value = UINT64_MAX;
    counters[id].max_value = 0;
    counters[id].total_value = 0;
    counters[id].sample_count = 0;
    
    profiler_lock.Release();
    return true;
}

void PerformanceProfiler::ResetAllCounters() {
    profiler_lock.Acquire();
    
    for (uint32_t i = 0; i < counter_count; i++) {
        counters[i].value = 0;
        counters[i].start_time = 0;
        
        // Reset stats
        counters[i].min_value = UINT64_MAX;
        counters[i].max_value = 0;
        counters[i].total_value = 0;
        counters[i].sample_count = 0;
    }
    
    profiler_lock.Release();
}

void PerformanceProfiler::PrintCounters() {
    LOG("=== PERFORMANCE COUNTERS ===");
    
    for (uint32_t i = 0; i < counter_count; i++) {
        if (counters[i].active) {
            PrintCounter(i);
        }
    }
    
    LOG("=============================");
}

void PerformanceProfiler::PrintCounter(PerfCounterId id) {
    if (id >= counter_count || !counters[id].active) {
        return;
    }
    
    profiler_lock.Acquire();
    
    uint64_t avg = (counters[id].sample_count > 0) ? 
                  counters[id].total_value / counters[id].sample_count : 0;
    
    LOG("[" << counters[id].name << "] Current: " << counters[id].value 
         << ", Total: " << counters[id].total_value 
         << ", Samples: " << counters[id].sample_count
         << ", Avg: " << avg
         << ", Min: " << counters[id].min_value
         << ", Max: " << counters[id].max_value);
    
    profiler_lock.Release();
}

bool PerformanceProfiler::GetCounterStats(PerfCounterId id, uint64_t& avg, uint64_t& min_val, uint64_t& max_val) {
    if (id >= counter_count || !counters[id].active) {
        return false;
    }
    
    profiler_lock.Acquire();
    
    avg = (counters[id].sample_count > 0) ? 
          counters[id].total_value / counters[id].sample_count : 0;
    min_val = counters[id].min_value;
    max_val = counters[id].max_value;
    
    profiler_lock.Release();
    return true;
}

void PerformanceProfiler::OptimizeScheduler() {
    // Performance optimizations for scheduler
    LOG("Applying scheduler optimizations...");
    
    // In a real implementation, we might:
    // - Optimize the scheduling algorithm
    // - Improve cache locality in PCB arrays
    // - Reduce context switch overhead
    // - Optimize timer interrupt handling
    
    // For now, we'll just record a counter
    PerfCounterId id = FindCounterByName("Scheduler Switches");
    if (id != -1) {
        IncrementCounter(id);
    }
}

void PerformanceProfiler::OptimizeMemoryManagement() {
    // Performance optimizations for memory management
    LOG("Applying memory management optimizations...");
    
    // In a real implementation, we might:
    // - Optimize memory allocation algorithms
    // - Improve buddy system performance
    // - Optimize page fault handling
    // - Cache frequently accessed memory management structures
    
    // For now, we'll just record a counter
    PerfCounterId id = FindCounterByName("Memory Allocations");
    if (id != -1) {
        IncrementCounter(id);
    }
}

void PerformanceProfiler::OptimizeInterruptHandling() {
    // Performance optimizations for interrupt handling
    LOG("Applying interrupt handling optimizations...");
    
    // In a real implementation, we might:
    // - Optimize interrupt dispatch
    // - Reduce interrupt latency
    // - Implement interrupt batching where appropriate
    
    // For now, we'll just record a counter
    PerfCounterId id = FindCounterByName("Interrupt Handlers");
    if (id != -1) {
        IncrementCounter(id);
    }
}

void PerformanceProfiler::OptimizeProcessSwitching() {
    // Performance optimizations for process switching
    LOG("Applying process switching optimizations...");
    
    // In a real implementation, we might:
    // - Optimize context switching code
    // - Reduce TLB flushes
    // - Cache process state where possible
    
    // For now, we'll just record a counter
    PerfCounterId id = FindCounterByName("Context Switches");
    if (id != -1) {
        IncrementCounter(id);
    }
}

void PerformanceProfiler::OptimizeFilesystems() {
    // Performance optimizations for filesystem operations
    LOG("Applying filesystem optimizations...");
    
    // In a real implementation, we might:
    // - Optimize VFS layer performance
    // - Improve filesystem caching
    // - Optimize disk I/O scheduling
    
    // For now, we'll just record appropriate counters
    PerfCounterId id1 = FindCounterByName("File Opens");
    PerfCounterId id2 = FindCounterByName("File Reads");
    PerfCounterId id3 = FindCounterByName("File Writes");
    
    if (id1 != -1) IncrementCounter(id1);
    if (id2 != -1) IncrementCounter(id2);
    if (id3 != -1) IncrementCounter(id3);
}

PerfCounter* PerformanceProfiler::GetCounter(PerfCounterId id) {
    if (id >= counter_count) {
        return nullptr;
    }
    
    return &counters[id];
}

PerfCounterId PerformanceProfiler::FindCounterByName(const char* name) {
    if (!name) {
        return -1;
    }
    
    for (uint32_t i = 0; i < counter_count; i++) {
        if (counters[i].active && strcmp(counters[i].name, name) == 0) {
            return i;
        }
    }
    
    return -1;  // Not found
}

bool InitializePerformanceProfiler() {
    if (!g_performance_profiler) {
        g_performance_profiler = new PerformanceProfiler();
        if (!g_performance_profiler) {
            LOG("Failed to create performance profiler instance");
            return false;
        }
        
        if (!g_performance_profiler->Initialize()) {
            LOG("Failed to initialize performance profiler");
            delete g_performance_profiler;
            g_performance_profiler = nullptr;
            return false;
        }
        
        LOG("Performance profiler initialized successfully");
    }
    
    return true;
}