#ifndef _Kernel_Performance_h_
#define _Kernel_Performance_h_

#include "Common.h"
#include "Defs.h"
#include "Logging.h"

// Performance counter ID type
typedef uint32 PerfCounterId;

// Performance counter types
enum PerfCounterType {
    PERF_COUNTER_TIME = 0,      // Time-based counter
    PERF_COUNTER_COUNT,         // Count-based counter
    PERF_COUNTER_MEMORY,        // Memory usage counter
    PERF_COUNTER_CACHE,         // Cache performance counter
    PERF_COUNTER_BRANCH         // Branch prediction counter
};

// Performance counter structure
struct PerfCounter {
    char name[64];              // Name of the counter
    PerfCounterType type;       // Type of counter
    uint64_t value;             // Current value
    uint64_t min_value;         // Minimum value recorded
    uint64_t max_value;         // Maximum value recorded
    uint64_t total_value;       // Total accumulated value
    uint32_t sample_count;      // Number of samples taken
    uint64_t start_time;        // Start time for time-based counters
    bool active;                // Whether the counter is active
};

// Performance profiling class
class PerformanceProfiler {
private:
    static const uint32_t MAX_PERFORMANCE_COUNTERS = 128;
    PerfCounter counters[MAX_PERFORMANCE_COUNTERS];
    uint32_t counter_count;
    Spinlock profiler_lock;     // Lock for profiler operations
    
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Initialize the performance profiler
    bool Initialize();
    
    // Create a new performance counter
    PerfCounterId CreateCounter(const char* name, PerfCounterType type);
    
    // Start timing a counter (for time-based counters)
    bool StartTimer(PerfCounterId id);
    
    // Stop timing a counter (for time-based counters)
    bool StopTimer(PerfCounterId id);
    
    // Increment a counter (for count-based counters)
    bool IncrementCounter(PerfCounterId id, uint64_t value = 1);
    
    // Set a counter value
    bool SetCounter(PerfCounterId id, uint64_t value);
    
    // Get counter value
    uint64_t GetCounterValue(PerfCounterId id);
    
    // Reset a counter
    bool ResetCounter(PerfCounterId id);
    
    // Reset all counters
    void ResetAllCounters();
    
    // Print all performance counters
    void PrintCounters();
    
    // Print a specific counter
    void PrintCounter(PerfCounterId id);
    
    // Get statistics for a counter
    bool GetCounterStats(PerfCounterId id, uint64_t& avg, uint64_t& min_val, uint64_t& max_val);
    
    // Performance optimization utilities
    void OptimizeScheduler();
    void OptimizeMemoryManagement();
    void OptimizeInterruptHandling();
    void OptimizeProcessSwitching();
    void OptimizeFilesystems();
    
private:
    // Internal helper functions
    PerfCounter* GetCounter(PerfCounterId id);
    PerfCounterId FindCounterByName(const char* name);
};

// Global profiler instance
extern PerformanceProfiler* g_performance_profiler;

// Initialize the performance profiler
bool InitializePerformanceProfiler();

// Performance profiling macros
#define PERF_START(id) g_performance_profiler->StartTimer(id)
#define PERF_STOP(id) g_performance_profiler->StopTimer(id)
#define PERF_INCREMENT(id, val) g_performance_profiler->IncrementCounter(id, val)
#define PERF_SET(id, val) g_performance_profiler->SetCounter(id, val)

// Scoped performance timer
class ScopedPerfTimer {
private:
    PerfCounterId counter_id;
    
public:
    ScopedPerfTimer(PerfCounterId id) : counter_id(id) {
        if (g_performance_profiler) {
            g_performance_profiler->StartTimer(counter_id);
        }
    }
    
    ~ScopedPerfTimer() {
        if (g_performance_profiler) {
            g_performance_profiler->StopTimer(counter_id);
        }
    }
};

// Predefined performance counter IDs
enum PredefinedPerfCounters {
    PERF_SCHEDULER_SWITCHES = 0,
    PERF_MEMORY_ALLOCATIONS,
    PERF_INTERRUPT_HANDLERS,
    PERF_SYSCALLS,
    PERF_CONTEXT_SWITCHES,
    PERF_FILE_OPENS,
    PERF_FILE_READS,
    PERF_FILE_WRITES,
    PERF_PAGE_FAULTS,
    PERF_MAX_COUNTERS
};

#endif