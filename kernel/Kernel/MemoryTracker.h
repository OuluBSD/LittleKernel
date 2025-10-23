#ifndef _Kernel_MemoryTracker_h_
#define _Kernel_MemoryTracker_h_

#include "Common.h"

// Structure to track memory allocations
struct MemoryAllocation {
    void* address;              // Address of the allocation
    uint32 size;                // Size of the allocation
    const char* file;           // File where allocation occurred
    uint32 line;                // Line number where allocation occurred
    uint32 timestamp;           // Time of allocation
    MemoryAllocation* next;     // Next allocation in the list
};

// Memory tracker class to help detect memory leaks
class MemoryTracker {
private:
    MemoryAllocation* allocation_list;
    uint32 total_allocated;
    uint32 allocation_count;
    Spinlock lock;              // Lock to protect the tracker in multi-threaded scenarios

public:
    MemoryTracker();
    ~MemoryTracker();
    
    // Track a new allocation
    void TrackAllocation(void* ptr, uint32 size, const char* file, uint32 line);
    
    // Track a deallocation
    void TrackDeallocation(void* ptr, const char* file, uint32 line);
    
    // Report all outstanding allocations (potential memory leaks)
    void ReportLeaks();
    
    // Get statistics about memory usage
    uint32 GetTotalAllocated() const { return total_allocated; }
    uint32 GetAllocationCount() const { return allocation_count; }
    
    // Check if a pointer is tracked by the memory tracker
    bool IsTracked(void* ptr) const;
    
    // Get allocation details for a specific pointer
    MemoryAllocation* GetAllocationDetails(void* ptr) const;
    
    // Verify all tracked allocations are still valid
    void VerifyAllocations();
};

// Macros to wrap allocation and deallocation for tracking
#ifdef ENABLE_MEMORY_TRACKING
    #define TRACKED_MALLOC(size) global->memory_tracker->TrackedMalloc(size, __FILE__, __LINE__)
    #define TRACKED_FREE(ptr) global->memory_tracker->TrackedFree(ptr, __FILE__, __LINE__)
    #define TRACKED_CALLOC(num, size) global->memory_tracker->TrackedCalloc(num, size, __FILE__, __LINE__)
    #define TRACKED_REALLOC(ptr, size) global->memory_tracker->TrackedRealloc(ptr, size, __FILE__, __LINE__)
#else
    #define TRACKED_MALLOC(size) malloc(size)
    #define TRACKED_FREE(ptr) free(ptr)
    #define TRACKED_CALLOC(num, size) calloc(num, size)
    #define TRACKED_REALLOC(ptr, size) realloc(ptr, size)
#endif

// Global memory tracking functions
class MemoryTrackerManager {
public:
    static void* TrackedMalloc(uint32 size, const char* file, uint32 line);
    static void TrackedFree(void* ptr, const char* file, uint32 line);
    static void* TrackedCalloc(uint32 num, uint32 size, const char* file, uint32 line);
    static void* TrackedRealloc(void* ptr, uint32 size, const char* file, uint32 line);
};

#endif