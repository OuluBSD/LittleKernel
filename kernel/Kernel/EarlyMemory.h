#ifndef _Kernel_EarlyMemory_h_
#define _Kernel_EarlyMemory_h_

#include "Defs.h"

// Early memory manager for use before the main heap is initialized
class EarlyMemoryManager {
private:
    static const uint32_t EARLY_MEMORY_SIZE = 1024 * 1024; // 1MB for early memory
    static const uint32_t MIN_ALIGNMENT = 4;
    
    // Memory region structure for tracking allocations
    struct MemoryRegion {
        void* start;
        uint32_t size;
        bool used;
        MemoryRegion* next;
    };
    
    // Use a pre-allocated buffer for our early memory needs
    static uint8_t early_memory_buffer[EARLY_MEMORY_SIZE];
    MemoryRegion* free_list;
    uint32_t initialized_size;
    
public:
    EarlyMemoryManager();
    ~EarlyMemoryManager();
    
    // Initialize the early memory manager
    bool Initialize(uint32_t kernel_end_address);
    
    // Allocate memory in the early stage
    void* Allocate(uint32_t size, uint32_t alignment = MIN_ALIGNMENT);
    
    // Free memory in the early stage
    void Free(void* ptr);
    
    // Get available memory
    uint32_t GetAvailableMemory();
    
    // Get used memory
    uint32_t GetUsedMemory();
    
    // Print memory map for debugging
    void PrintMemoryMap();
    
    // Initialize memory manager from multiboot memory info
    bool InitializeFromMultiboot(struct Multiboot* mboot_ptr);
    
    // Reserve a specific memory region
    bool ReserveRegion(uint32_t start_addr, uint32_t size);
    
    // Check if memory address is valid for allocation
    bool IsValidAddress(uint32_t addr);
    
    // Get total memory size
    uint32_t GetTotalMemory();
};

// Global early memory manager instance (using static allocation to avoid circular dependency)
extern EarlyMemoryManager* g_early_memory_manager;

// Static instance of the early memory manager to avoid needing dynamic allocation for it
extern EarlyMemoryManager early_memory_manager_instance;

// Initialize early memory management system
bool InitializeEarlyMemory(struct Multiboot* mboot_ptr);

// Helper functions that work before the main heap is initialized
void* EarlyMalloc(uint32_t size);
void EarlyFree(void* ptr);
void* EarlyCalloc(uint32_t count, uint32_t size);

#endif // _Kernel_EarlyMemory_h_