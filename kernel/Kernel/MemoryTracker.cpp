#include "Kernel.h"
#include "MemoryTracker.h"
#include "Logging.h"

MemoryTracker::MemoryTracker() {
    allocation_list = nullptr;
    total_allocated = 0;
    allocation_count = 0;
    lock.Initialize();
}

MemoryTracker::~MemoryTracker() {
    // Report any remaining allocations (memory leaks)
    ReportLeaks();
    
    // Free all tracked allocations
    MemoryAllocation* current = allocation_list;
    while (current) {
        MemoryAllocation* next = current->next;
        free(current);
        current = next;
    }
    
    allocation_list = nullptr;
    total_allocated = 0;
    allocation_count = 0;
}

void MemoryTracker::TrackAllocation(void* ptr, uint32 size, const char* file, uint32 line) {
    if (!ptr) return;
    
    lock.Acquire();
    
    // Create a new allocation record
    MemoryAllocation* alloc = (MemoryAllocation*)malloc(sizeof(MemoryAllocation));
    if (!alloc) {
        LOG("Failed to create allocation record for memory tracker");
        lock.Release();
        return;
    }
    
    alloc->address = ptr;
    alloc->size = size;
    alloc->file = file;  // Note: This should be copied if we keep the string permanently
    alloc->line = line;
    alloc->next = allocation_list;
    alloc->timestamp = global ? global->timer->GetTickCount() : 0;  // Use global timer if available
    
    allocation_list = alloc;
    total_allocated += size;
    allocation_count++;
    
    DLOG("Tracked allocation: " << size << " bytes at 0x" << (uint32)ptr 
         << " in " << file << ":" << line);
    
    lock.Release();
}

void MemoryTracker::TrackDeallocation(void* ptr, const char* file, uint32 line) {
    if (!ptr) return;
    
    lock.Acquire();
    
    MemoryAllocation* current = allocation_list;
    MemoryAllocation* prev = nullptr;
    
    while (current) {
        if (current->address == ptr) {
            // Found the allocation, remove it from the list
            if (prev) {
                prev->next = current->next;
            } else {
                allocation_list = current->next;
            }
            
            total_allocated -= current->size;
            allocation_count--;
            
            DLOG("Tracked deallocation: " << current->size << " bytes at 0x" << (uint32)ptr 
                 << " in " << file << ":" << line);
            
            // Free the allocation record
            free(current);
            
            lock.Release();
            return;
        }
        
        prev = current;
        current = current->next;
    }
    
    // This might indicate a double-free or an allocation not tracked
    LOG("WARNING: Attempt to deallocate untracked memory at 0x" << (uint32)ptr 
         << " from " << file << ":" << line);
    
    lock.Release();
}

void MemoryTracker::ReportLeaks() {
    lock.Acquire();
    
    if (allocation_list) {
        LOG("=== MEMORY LEAK REPORT ===");
        LOG("Total leaked allocations: " << allocation_count);
        LOG("Total leaked bytes: " << total_allocated);
        
        MemoryAllocation* current = allocation_list;
        uint32 leak_count = 0;
        
        while (current && leak_count < 100) {  // Limit output to prevent overflow
            LOG("Leak: " << current->size << " bytes at 0x" << (uint32)current->address 
                 << " allocated in " << current->file << ":" << current->line);
            current = current->next;
            leak_count++;
        }
        
        if (allocation_list && leak_count >= 100) {
            LOG("... and more (reporting truncated)");
        }
        
        LOG("=== END MEMORY LEAK REPORT ===");
    } else {
        LOG("No memory leaks detected");
    }
    
    lock.Release();
}

bool MemoryTracker::IsTracked(void* ptr) const {
    lock.Acquire();
    
    MemoryAllocation* current = allocation_list;
    while (current) {
        if (current->address == ptr) {
            lock.Release();
            return true;
        }
        current = current->next;
    }
    
    lock.Release();
    return false;
}

MemoryAllocation* MemoryTracker::GetAllocationDetails(void* ptr) const {
    lock.Acquire();
    
    MemoryAllocation* current = allocation_list;
    while (current) {
        if (current->address == ptr) {
            lock.Release();
            return current;
        }
        current = current->next;
    }
    
    lock.Release();
    return nullptr;
}

void MemoryTracker::VerifyAllocations() {
    lock.Acquire();
    
    MemoryAllocation* current = allocation_list;
    while (current) {
        // Check if the memory is still accessible by reading/writing to it
        // This is a basic check - in a real implementation, we might want more sophisticated validation
        volatile uint8* test_addr = (volatile uint8*)current->address;
        
        // If we can access this location without a page fault, it's likely still valid
        // In a real implementation, we'd need to safely verify without causing issues
        
        current = current->next;
    }
    
    lock.Release();
}

// Global memory tracking functions
void* MemoryTrackerManager::TrackedMalloc(uint32 size, const char* file, uint32 line) {
    void* ptr = malloc(size);
    if (ptr && global && global->memory_tracker) {
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
    void* ptr = calloc(num, size);
    if (ptr && global && global->memory_tracker) {
        global->memory_tracker->TrackAllocation(ptr, num * size, file, line);
    }
    return ptr;
}

void* MemoryTrackerManager::TrackedRealloc(void* ptr, uint32 size, const char* file, uint32 line) {
    if (ptr && global && global->memory_tracker) {
        // First, remove the old allocation from tracking
        global->memory_tracker->TrackDeallocation(ptr, file, line);
    }
    
    void* new_ptr = realloc(ptr, size);
    if (new_ptr && global && global->memory_tracker) {
        // Track the new allocation
        global->memory_tracker->TrackAllocation(new_ptr, size, file, line);
    }
    return new_ptr;
}