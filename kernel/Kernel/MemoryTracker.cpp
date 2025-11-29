#include "Kernel.h"

MemoryTracker::MemoryTracker() {
    // Initialize the memory tracking system
    allocation_list = nullptr;
    total_allocated = 0;
    allocation_count = 0;

    // Initialize the lock for thread-safe operations
    lock.Initialize();

    DLOG("Memory tracker initialized");
}

MemoryTracker::~MemoryTracker() {
    // Clean up any remaining allocations if needed
    // In a real system, this might log warnings about memory leaks
    DLOG("Memory tracker destroyed. Total allocated: " << total_allocated
          << ", Allocation count: " << allocation_count);
}

void MemoryTracker::TrackAllocation(void* ptr, uint32 size, const char* file, uint32 line) {
    if (!ptr) return;

    lock.Acquire();

    // Create a new allocation record
    MemoryAllocation* alloc = (MemoryAllocation*)malloc(sizeof(MemoryAllocation));
    if (!alloc) {
        LOG("Failed to allocate memory for tracking record");
        lock.Release();
        return;
    }

    alloc->address = ptr;
    alloc->size = size;
    alloc->file = file;
    alloc->line = line;
    alloc->next = allocation_list;
    alloc->timestamp = 0; // Using 0 for now since timer_ticks doesn't exist

    allocation_list = alloc;

    // Update statistics
    total_allocated += size;
    allocation_count++;

    lock.Release();
}

void MemoryTracker::TrackDeallocation(void* ptr, const char* file, uint32 line) {
    if (!ptr) return;

    lock.Acquire();

    // Find the allocation in the list
    MemoryAllocation* current = allocation_list;
    MemoryAllocation* prev = nullptr;

    while (current) {
        if (current->address == ptr) {
            // Update statistics
            total_allocated -= current->size;
            if (allocation_count > 0) allocation_count--;

            // Remove from the list
            if (prev) {
                prev->next = current->next;
            } else {
                allocation_list = current->next;
            }

            // Free the tracking record
            free(current);

            lock.Release();
            return;
        }
        prev = current;
        current = current->next;
    }

    LOG("Memory tracker: Attempted to free untracked pointer: 0x" << (uint32)ptr
          << " from " << (file ? file : "unknown") << ":" << line);
    lock.Release();
}

void MemoryTracker::ReportLeaks() {
    lock.Acquire();

    MemoryAllocation* current = allocation_list;
    if (current) {
        LOG("Memory leaks detected:");
        int leak_count = 0;
        while (current) {
            LOG("  Leak at 0x" << (uint32)current->address << " of size " << current->size
                  << " from " << (current->file ? current->file : "unknown") << ":" << current->line);
            current = current->next;
            leak_count++;
        }
        LOG("Total " << leak_count << " leaks reported.");
    } else {
        DLOG("No memory leaks detected");
    }

    lock.Release();
}

bool MemoryTracker::IsTracked(void* ptr) const {
    // Cast away const to acquire lock; this is safe as we're only using the lock for thread safety
    reinterpret_cast<MemoryTracker*>(const_cast<MemoryTracker*>(this))->lock.Acquire();

    MemoryAllocation* current = allocation_list;
    while (current) {
        if (current->address == ptr) {
            reinterpret_cast<MemoryTracker*>(const_cast<MemoryTracker*>(this))->lock.Release();
            return true;
        }
        current = current->next;
    }

    reinterpret_cast<MemoryTracker*>(const_cast<MemoryTracker*>(this))->lock.Release();
    return false;
}

MemoryAllocation* MemoryTracker::GetAllocationDetails(void* ptr) const {
    // Cast away const to acquire lock; this is safe as we're only using the lock for thread safety
    reinterpret_cast<MemoryTracker*>(const_cast<MemoryTracker*>(this))->lock.Acquire();

    MemoryAllocation* current = allocation_list;
    while (current) {
        if (current->address == ptr) {
            reinterpret_cast<MemoryTracker*>(const_cast<MemoryTracker*>(this))->lock.Release();
            return current;
        }
        current = current->next;
    }

    reinterpret_cast<MemoryTracker*>(const_cast<MemoryTracker*>(this))->lock.Release();
    return nullptr;
}

void MemoryTracker::VerifyAllocations() {
    lock.Acquire();

    MemoryAllocation* current = allocation_list;
    while (current) {
        // In a real implementation, this would check if the memory is still valid
        // For now, just ensuring it's not nullptr
        if (current->address == nullptr) {
            LOG("Memory tracker: Found invalid allocation (nullptr address)");
        }
        current = current->next;
    }

    lock.Release();
}