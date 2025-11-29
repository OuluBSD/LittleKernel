#include "Kernel.h"

void* MemoryTrackerManager::TrackedMalloc(uint32 size, const char* file, uint32 line) {
    void* ptr = malloc(size);
    if (global && global->memory_tracker) {
        global->memory_tracker->TrackAllocation(ptr, size, file, line);
    }
    return ptr;
}

void MemoryTrackerManager::TrackedFree(void* ptr, const char* file, uint32 line) {
    if (global && global->memory_tracker) {
        global->memory_tracker->TrackDeallocation(ptr, file, line);
    }
    free(ptr);
}

void* MemoryTrackerManager::TrackedCalloc(uint32 num, uint32 size, const char* file, uint32 line) {
    uint32 total_size = num * size;
    void* ptr = calloc(num, size);
    if (global && global->memory_tracker) {
        global->memory_tracker->TrackAllocation(ptr, total_size, file, line);
    }
    return ptr;
}

void* MemoryTrackerManager::TrackedRealloc(void* ptr, uint32 size, const char* file, uint32 line) {
    if (global && global->memory_tracker) {
        global->memory_tracker->TrackDeallocation(ptr, file, line);
    }
    void* new_ptr = realloc(ptr, size);
    if (global && global->memory_tracker) {
        global->memory_tracker->TrackAllocation(new_ptr, size, file, line);
    }
    return new_ptr;
}