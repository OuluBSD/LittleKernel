#ifndef _Kernel_Profiling_h_
#define _Kernel_Profiling_h_

#include "Defs.h"
#include "HAL.h"

// Profiling sample structure
struct ProfileSample {
    uint64_t timestamp;        // Time when sample was taken
    uint32_t cpu_usage;        // CPU utilization percentage
    uint32_t memory_usage;     // Memory utilization percentage
    uint32_t process_count;    // Number of running processes
    uint64_t total_processes;  // Total processes created
    uint64_t total_switches;   // Total context switches
    uint64_t total_syscalls;   // Total system calls made
    uint32_t page_faults;      // Page faults since last sample
    uint32_t interrupts;       // Interrupts since last sample
    uint32_t ready_queue_size; // Number of processes ready to run
};

// Profiling data for a specific function or region
struct FunctionProfile {
    const char* name;          // Name of the function/procedure
    uint64_t total_time;       // Total time spent in function (in nanoseconds)
    uint64_t call_count;       // Number of times function was called
    uint64_t min_time;         // Minimum time for a single call
    uint64_t max_time;         // Maximum time for a single call
    uint64_t avg_time;         // Average time per call
    uint64_t last_start_time;  // Time when last call started
    bool in_progress;          // Whether function is currently executing
};

// Profiling statistics structure
struct ProfileStats {
    uint64_t total_kernel_time;    // Total time kernel has been running
    uint64_t total_idle_time;      // Total time spent in idle state
    uint64_t total_process_time;   // Total time spent in processes
    uint32_t avg_context_switch_time; // Average context switch time (nanoseconds)
    uint32_t avg_syscall_time;     // Average system call time (nanoseconds)
    uint32_t avg_interrupt_time;   // Average interrupt handling time (nanoseconds)
};

// Profiling types
enum class ProfileType {
    FUNCTION,     // Function-level profiling
    SYSTEM,       // System-level profiling
    MEMORY,       // Memory usage profiling
    PROCESS,      // Process scheduling profiling
    INTERRUPT     // Interrupt handling profiling
};

// Kernel profiling manager
class KernelProfiler {
private:
    static const uint32_t MAX_PROFILED_FUNCTIONS = 256;
    static const uint32_t MAX_SAMPLES = 1024;
    static const uint32_t MAX_REGIONS = 64;
    
    FunctionProfile function_profiles[MAX_PROFILED_FUNCTIONS];
    ProfileSample samples[MAX_SAMPLES];
    ProfileStats stats;
    
    uint32_t function_count;
    uint32_t sample_count;
    uint32_t sample_index;
    
    bool profiling_enabled;
    ProfileType current_profile_type;
    uint64_t profiling_start_time;
    
    // Profiling regions for code sections
    struct ProfileRegion {
        const char* name;
        uint64_t start_time;
        uint32_t active;
    };
    
    ProfileRegion profile_regions[MAX_REGIONS];
    uint32_t region_count;
    
public:
    KernelProfiler();
    ~KernelProfiler();
    
    // Initialize the profiling system
    bool Initialize();
    
    // Enable/disable profiling
    void EnableProfiling();
    void DisableProfiling();
    bool IsProfilingEnabled() const;
    
    // Function-level profiling
    void StartFunctionProfile(const char* name);
    void EndFunctionProfile(const char* name);
    
    // Profiling regions (for code sections)
    void BeginRegion(const char* name);
    void EndRegion(const char* name);
    
    // Take a system profile sample
    void TakeSample();
    
    // Get profiling statistics
    const ProfileStats& GetStats() const;
    
    // Get profile data for a specific function
    const FunctionProfile* GetFunctionProfile(const char* name);
    
    // Get all function profiles
    const FunctionProfile* GetFunctionProfiles(uint32_t* count);
    
    // Get profiling samples
    const ProfileSample* GetSamples(uint32_t* count);
    
    // Reset profiling data
    void Reset();
    
    // Print profiling report
    void PrintReport();
    
    // Calculate specific metrics
    uint64_t GetAverageFunctionTime(const char* name);
    uint32_t GetFunctionCallCount(const char* name);
    
    // Get system utilization
    uint32_t GetCpuUtilization();
    uint32_t GetMemoryUtilization();
    
    // Update statistics
    void UpdateStats();
    
    // Profile a specific code block using RAII
    class ProfileBlock {
    private:
        const char* name;
        KernelProfiler* profiler;
    public:
        ProfileBlock(const char* func_name, KernelProfiler* prof);
        ~ProfileBlock();
    };
};

// Helper macros for profiling
#define PROFILE_FUNCTION() KernelProfiler::ProfileBlock _profile_block(__FUNCTION__, g_kernel_profiler)
#define PROFILE_REGION(name) KernelProfiler::ProfileBlock _profile_block(name, g_kernel_profiler)

// Global profiler instance
extern KernelProfiler* g_kernel_profiler;

// Initialize kernel profiling infrastructure
bool InitializeKernelProfiling();

// Helper functions for profiling
void StartProfilingFunction(const char* name);
void EndProfilingFunction(const char* name);

#endif // _Kernel_Profiling_h_