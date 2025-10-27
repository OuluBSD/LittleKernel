#ifndef _Kernel_EarlyMemory_h_
#define _Kernel_EarlyMemory_h_

#include "Defs.h"

// Early memory manager for use before the main heap is initialized
class EarlyMemoryManager {
private:
    static const uint32 EARLY_MEMORY_SIZE = 1024 * 1024; // 1MB for early memory
    static const uint32 MIN_ALIGNMENT = 4;
    
    // Memory region structure for tracking allocations
    struct MemoryRegion {
        void* start;
        uint32 size;
        bool used;
        MemoryRegion* next;
    };
    
    // Use a pre-allocated buffer for our early memory needs
    static uint8 early_memory_buffer[EARLY_MEMORY_SIZE];
    MemoryRegion* free_list;
    uint32 initialized_size;
    
public:
    EarlyMemoryManager();
    ~EarlyMemoryManager();
    
    // Initialize the early memory manager
    bool Initialize(uint32 kernel_end_address);
    
    // Allocate memory in the early stage
    void* Allocate(uint32 size, uint32 alignment = MIN_ALIGNMENT);
    
    // Free memory in the early stage
    void Free(void* ptr);
    
    // Get available memory
    uint32 GetAvailableMemory();
    
    // Get used memory
    uint32 GetUsedMemory();
    
    // Print memory map for debugging
    void PrintMemoryMap();
    
    // Initialize memory manager from multiboot memory info
    bool InitializeFromMultiboot(struct Multiboot* mboot_ptr);
    
    // Reserve a specific memory region
    bool ReserveRegion(uint32 start_addr, uint32 size);
    
    // Check if memory address is valid for allocation
    bool IsValidAddress(uint32 addr);
    
    // Get total memory size
    uint32 GetTotalMemory();
};

// Global early memory manager instance (using static allocation to avoid circular dependency)
extern EarlyMemoryManager* g_early_memory_manager;

// Static instance of the early memory manager to avoid needing dynamic allocation for it
extern EarlyMemoryManager early_memory_manager_instance;

// Initialize early memory management system
bool InitializeEarlyMemory(struct Multiboot* mboot_ptr);

// Helper functions that work before the main heap is initialized
void* EarlyMalloc(uint32 size);
void EarlyFree(void* ptr);
void* EarlyCalloc(uint32 count, uint32 size);

#endif // _Kernel_EarlyMemory_h_