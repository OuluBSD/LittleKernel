#include "Profiler.h"
#include "Kernel.h"

// Global profiler instance
KernelProfiler* g_kernel_profiler = nullptr;

KernelProfiler::KernelProfiler() 
    : function_count(0), sample_count(0), sample_index(0), 
      profiling_enabled(false), current_profile_type(ProfileType::SYSTEM), 
      profiling_start_time(0), region_count(0) {
    memset(function_profiles, 0, sizeof(function_profiles));
    memset(samples, 0, sizeof(samples));
    memset(profile_regions, 0, sizeof(profile_regions));
    memset(&stats, 0, sizeof(stats));
}

KernelProfiler::~KernelProfiler() {
    // Clean up if needed
}

bool KernelProfiler::Initialize() {
    Reset();
    profiling_enabled = true;
    
    LOG("Kernel profiling infrastructure initialized");
    return true;
}

void KernelProfiler::EnableProfiling() {
    profiling_enabled = true;
    profiling_start_time = HAL_TIMER()->GetHighResolutionTime();
    LOG("Kernel profiling enabled");
}

void KernelProfiler::DisableProfiling() {
    profiling_enabled = false;
    LOG("Kernel profiling disabled");
}

bool KernelProfiler::IsProfilingEnabled() const {
    return profiling_enabled;
}

void KernelProfiler::StartFunctionProfile(const char* name) {
    if (!profiling_enabled || !name) return;
    
    // Find existing profile or create new one
    FunctionProfile* profile = nullptr;
    for (uint32_t i = 0; i < function_count; i++) {
        if (strcmp(function_profiles[i].name, name) == 0) {
            profile = &function_profiles[i];
            break;
        }
    }
    
    // If not found, create new profile
    if (!profile && function_count < MAX_PROFILED_FUNCTIONS) {
        profile = &function_profiles[function_count];
        profile->name = name;
        profile->total_time = 0;
        profile->call_count = 0;
        profile->min_time = UINT64_MAX;
        profile->max_time = 0;
        profile->in_progress = false;
        function_count++;
    }
    
    if (profile && !profile->in_progress) {
        profile->last_start_time = HAL_TIMER()->GetHighResolutionTime();
        profile->in_progress = true;
    }
}

void KernelProfiler::EndFunctionProfile(const char* name) {
    if (!profiling_enabled || !name) return;
    
    // Find the profile
    FunctionProfile* profile = nullptr;
    for (uint32_t i = 0; i < function_count; i++) {
        if (strcmp(function_profiles[i].name, name) == 0) {
            profile = &function_profiles[i];
            break;
        }
    }
    
    if (profile && profile->in_progress) {
        uint64_t end_time = HAL_TIMER()->GetHighResolutionTime();
        uint64_t elapsed = end_time - profile->last_start_time;
        
        profile->total_time += elapsed;
        profile->call_count++;
        
        // Update min/max times
        if (elapsed < profile->min_time) {
            profile->min_time = elapsed;
        }
        if (elapsed > profile->max_time) {
            profile->max_time = elapsed;
        }
        
        // Update average (calculated each time for running average)
        profile->avg_time = profile->call_count > 0 ? 
                           profile->total_time / profile->call_count : 0;
        
        profile->in_progress = false;
    }
}

void KernelProfiler::BeginRegion(const char* name) {
    if (!profiling_enabled || !name) return;
    
    // Look for an available profile region or create new one
    ProfileRegion* region = nullptr;
    for (uint32_t i = 0; i < region_count; i++) {
        if (strcmp(profile_regions[i].name, name) == 0) {
            region = &profile_regions[i];
            break;
        }
    }
    
    // If not found, create new region
    if (!region && region_count < MAX_REGIONS) {
        region = &profile_regions[region_count];
        region->name = name;
        region_count++;
    }
    
    if (region) {
        region->start_time = HAL_TIMER()->GetHighResolutionTime();
        region->active++;
    }
}

void KernelProfiler::EndRegion(const char* name) {
    if (!profiling_enabled || !name) return;
    
    // Find the region
    for (uint32_t i = 0; i < region_count; i++) {
        if (strcmp(profile_regions[i].name, name) == 0 && profile_regions[i].active > 0) {
            uint64_t end_time = HAL_TIMER()->GetHighResolutionTime();
            uint64_t elapsed = end_time - profile_regions[i].start_time;
            
            // Add to function profile
            StartFunctionProfile(name);  // Create profile if needed
            // Manually update the timing since we're not using the standard Start/End
            FunctionProfile* profile = nullptr;
            for (uint32_t j = 0; j < function_count; j++) {
                if (strcmp(function_profiles[j].name, name) == 0) {
                    profile = &function_profiles[j];
                    break;
                }
            }
            
            if (profile) {
                profile->total_time += elapsed;
                profile->call_count++;
                
                if (elapsed < profile->min_time) {
                    profile->min_time = elapsed;
                }
                if (elapsed > profile->max_time) {
                    profile->max_time = elapsed;
                }
                
                profile->avg_time = profile->call_count > 0 ? 
                                   profile->total_time / profile->call_count : 0;
            }
            
            profile_regions[i].active--;
            break;
        }
    }
}

void KernelProfiler::TakeSample() {
    if (!profiling_enabled) return;
    
    ProfileSample& sample = samples[sample_index];
    
    // Fill in sample data
    sample.timestamp = HAL_TIMER()->GetTickCount();
    
    // Get system stats as available (simplified implementation)
    sample.cpu_usage = GetCpuUtilization();
    sample.memory_usage = GetMemoryUtilization();
    sample.process_count = process_manager ? process_manager->GetProcessCount() : 0;
    sample.total_processes = process_manager ? process_manager->GetTotalProcessCount() : 0;
    sample.total_switches = process_manager ? process_manager->GetTotalContextSwitches() : 0;
    sample.total_syscalls = 0; // Would need to track system calls separately
    sample.page_faults = 0;    // Would need to track page faults
    sample.interrupts = 0;     // Would need to track interrupts
    sample.ready_queue_size = 0; // Would need to track ready queue size
    
    // Update sample tracking
    sample_index = (sample_index + 1) % MAX_SAMPLES;
    if (sample_count < MAX_SAMPLES) {
        sample_count++;
    }
}

const ProfileStats& KernelProfiler::GetStats() const {
    return stats;
}

const FunctionProfile* KernelProfiler::GetFunctionProfile(const char* name) {
    if (!name) return nullptr;
    
    for (uint32_t i = 0; i < function_count; i++) {
        if (strcmp(function_profiles[i].name, name) == 0) {
            return &function_profiles[i];
        }
    }
    return nullptr;
}

const FunctionProfile* KernelProfiler::GetFunctionProfiles(uint32_t* count) {
    *count = function_count;
    return function_profiles;
}

const ProfileSample* KernelProfiler::GetSamples(uint32_t* count) {
    *count = sample_count;
    return samples;
}

void KernelProfiler::Reset() {
    function_count = 0;
    sample_count = 0;
    sample_index = 0;
    region_count = 0;
    
    memset(function_profiles, 0, sizeof(function_profiles));
    memset(samples, 0, sizeof(samples));
    memset(profile_regions, 0, sizeof(profile_regions));
    memset(&stats, 0, sizeof(stats));
    
    LOG("Kernel profiler reset");
}

void KernelProfiler::PrintReport() {
    LOG("=== Kernel Profiling Report ===");
    
    // Print function profiling data
    LOG("Function Profiling Data:");
    for (uint32_t i = 0; i < function_count; i++) {
        const FunctionProfile& profile = function_profiles[i];
        LOG("  " << profile.name << ":");
        LOG("    Calls: " << profile.call_count);
        LOG("    Total Time: " << profile.total_time << " ns");
        LOG("    Avg Time: " << profile.avg_time << " ns");
        LOG("    Min Time: " << profile.min_time << " ns");
        LOG("    Max Time: " << profile.max_time << " ns");
    }
    
    // Print system statistics
    LOG("System Statistics:");
    LOG("  CPU Utilization: " << GetCpuUtilization() << "%");
    LOG("  Memory Utilization: " << GetMemoryUtilization() << "%");
    LOG("  Active Processes: " << (process_manager ? process_manager->GetProcessCount() : 0));
    
    LOG("===============================");
}

uint64_t KernelProfiler::GetAverageFunctionTime(const char* name) {
    const FunctionProfile* profile = GetFunctionProfile(name);
    return profile ? profile->avg_time : 0;
}

uint32_t KernelProfiler::GetFunctionCallCount(const char* name) {
    const FunctionProfile* profile = GetFunctionProfile(name);
    return profile ? profile->call_count : 0;
}

uint32_t KernelProfiler::GetCpuUtilization() {
    // Simplified calculation - in a real implementation, this would track
    // actual CPU busy/idle time
    return 50; // Return a placeholder value
}

uint32_t KernelProfiler::GetMemoryUtilization() {
    // Simplified calculation - in a real implementation, this would track
    // actual memory usage vs total available
    if (global && global->memory_manager) {
        uint32_t used = global->memory_manager->GetUsedMemory();
        uint32_t total = global->memory_manager->GetTotalMemory();
        if (total > 0) {
            return (used * 100) / total;
        }
    }
    return 0;
}

void KernelProfiler::UpdateStats() {
    // Update statistics based on current system state
    stats.total_kernel_time = HAL_TIMER()->GetTickCount();
    
    // More detailed statistics would be calculated here
    // This is a simplified implementation
}

// ProfileBlock RAII implementation
KernelProfiler::ProfileBlock::ProfileBlock(const char* func_name, KernelProfiler* prof) 
    : name(func_name), profiler(prof) {
    if (profiler && profiler->IsProfilingEnabled()) {
        profiler->StartFunctionProfile(name);
    }
}

KernelProfiler::ProfileBlock::~ProfileBlock() {
    if (profiler && profiler->IsProfilingEnabled()) {
        profiler->EndFunctionProfile(name);
    }
}

// Initialize kernel profiling infrastructure
bool InitializeKernelProfiling() {
    g_kernel_profiler = new KernelProfiler();
    if (!g_kernel_profiler) {
        LOG("Error: Failed to allocate kernel profiler");
        return false;
    }
    
    if (!g_kernel_profiler->Initialize()) {
        LOG("Error: Failed to initialize kernel profiler");
        delete g_kernel_profiler;
        g_kernel_profiler = nullptr;
        return false;
    }
    
    LOG("Kernel profiling infrastructure initialized successfully");
    return true;
}

// Helper functions for profiling
void StartProfilingFunction(const char* name) {
    if (g_kernel_profiler) {
        g_kernel_profiler->StartFunctionProfile(name);
    }
}

void EndProfilingFunction(const char* name) {
    if (g_kernel_profiler) {
        g_kernel_profiler->EndFunctionProfile(name);
    }
}