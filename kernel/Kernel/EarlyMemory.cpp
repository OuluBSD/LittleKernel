#include "EarlyMemory.h"
#include "Kernel.h"

// Static buffer for early memory management
uint8_t EarlyMemoryManager::early_memory_buffer[EARLY_MEMORY_SIZE];

// Global early memory manager instance
EarlyMemoryManager* g_early_memory_manager = nullptr;

// Static instance of the early memory manager
EarlyMemoryManager early_memory_manager_instance;

EarlyMemoryManager::EarlyMemoryManager() : free_list(nullptr), initialized_size(0) {
}

EarlyMemoryManager::~EarlyMemoryManager() {
    // Clean up memory regions if needed
    MemoryRegion* current = free_list;
    while (current) {
        MemoryRegion* next = current->next;
        // In this implementation, we're using the static buffer,
        // so we don't actually free the MemoryRegion structures
        current = next;
    }
}

bool EarlyMemoryManager::Initialize(uint32_t kernel_end_address) {
    // Initialize the early memory manager with the provided end address
    if (kernel_end_address == 0) {
        LOG("Error: Invalid kernel end address for early memory initialization");
        return false;
    }
    
    // Set up the initial memory region starting right after the kernel
    free_list = (MemoryRegion*)kernel_end_address;
    
    // Initialize the first memory region
    free_list->start = (void*)(kernel_end_address + sizeof(MemoryRegion));
    free_list->size = EARLY_MEMORY_SIZE - (uint32_t)sizeof(MemoryRegion) - 
                     (kernel_end_address - (uint32_t)early_memory_buffer);
    free_list->used = false;
    free_list->next = nullptr;
    
    initialized_size = EARLY_MEMORY_SIZE;
    
    LOG("Early memory manager initialized with " << free_list->size << " bytes available starting at 0x" << 
        (uint32_t)free_list->start);
    
    return true;
}

void* EarlyMemoryManager::Allocate(uint32_t size, uint32_t alignment) {
    if (size == 0) return nullptr;
    
    // Add space for possible alignment padding
    uint32_t required_size = size + alignment - 1;
    
    // Find a suitable free region
    MemoryRegion* current = free_list;
    while (current) {
        if (!current->used && current->size >= required_size) {
            // Align the start address
            uint32_t addr = (uint32_t)current->start;
            uint32_t aligned_addr = (addr + alignment - 1) & ~(alignment - 1);
            
            // Check if the aligned region fits
            if ((aligned_addr + size) <= ((uint32_t)current->start + current->size)) {
                // Mark as used
                current->used = true;
                
                // If there's remaining space after this allocation, create a new free region
                uint32_t remaining_size = ((uint32_t)current->start + current->size) - (aligned_addr + size);
                if (remaining_size > sizeof(MemoryRegion)) {
                    // Create a new region for the remaining space
                    MemoryRegion* new_region = (MemoryRegion*)(aligned_addr + size);
                    new_region->start = (void*)(aligned_addr + size + sizeof(MemoryRegion));
                    new_region->size = remaining_size - sizeof(MemoryRegion);
                    new_region->used = false;
                    new_region->next = current->next;
                    current->next = new_region;
                    
                    // Update current region size
                    current->size = size + ((uint32_t)new_region - (uint32_t)current->start);
                } else {
                    // Just reduce the size of the current region
                    current->size = (aligned_addr + size) - (uint32_t)current->start;
                }
                
                LOG("Early memory allocated: " << size << " bytes at 0x" << (void*)aligned_addr);
                return (void*)aligned_addr;
            }
        }
        current = current->next;
    }
    
    LOG("Early memory allocation failed for " << size << " bytes");
    return nullptr;
}

void EarlyMemoryManager::Free(void* ptr) {
    if (!ptr) return;
    
    // Find the region that contains this pointer
    MemoryRegion* current = free_list;
    while (current) {
        uint32_t addr = (uint32_t)ptr;
        uint32_t region_start = (uint32_t)current->start;
        uint32_t region_end = region_start + current->size;
        
        if (addr >= region_start && addr < region_end && current->used) {
            current->used = false;
            LOG("Early memory freed: 0x" << ptr);
            return;
        }
        current = current->next;
    }
    
    LOG("Warning: Attempt to free invalid early memory address: 0x" << ptr);
}

uint32_t EarlyMemoryManager::GetAvailableMemory() {
    uint32_t available = 0;
    MemoryRegion* current = free_list;
    while (current) {
        if (!current->used) {
            available += current->size;
        }
        current = current->next;
    }
    return available;
}

uint32_t EarlyMemoryManager::GetUsedMemory() {
    uint32_t used = 0;
    MemoryRegion* current = free_list;
    while (current) {
        if (current->used) {
            used += current->size;
        }
        current = current->next;
    }
    return used;
}

void EarlyMemoryManager::PrintMemoryMap() {
    LOG("=== Early Memory Map ===");
    MemoryRegion* current = free_list;
    uint32_t region_num = 0;
    
    while (current) {
        LOG("Region " << region_num << ": 0x" << current->start << 
            " - 0x" << (void*)((uint32_t)current->start + current->size) << 
            ", Size: " << current->size << " bytes, " << 
            (current->used ? "USED" : "FREE"));
        current = current->next;
        region_num++;
    }
    
    LOG("Available: " << GetAvailableMemory() << " bytes");
    LOG("Used: " << GetUsedMemory() << " bytes");
    LOG("========================");
}

bool EarlyMemoryManager::InitializeFromMultiboot(struct Multiboot* mboot_ptr) {
    if (!mboot_ptr || !(mboot_ptr->flags & 0x01)) {
        LOG("No multiboot memory information available");
        // Use a default approach if no multiboot info
        uint32_t kernel_end = 0x100000; // Default assumption
        return Initialize(kernel_end);
    }
    
    // Calculate how much conventional and extended memory is available
    uint32_t total_memory = (mboot_ptr->mem_lower + mboot_ptr->mem_upper) * 1024;
    uint32_t kernel_end = 0x100000; // Start at 1MB after 0
    uint32_t early_memory_end = kernel_end + EARLY_MEMORY_SIZE;
    
    // Make sure we don't exceed available memory
    if (early_memory_end > total_memory) {
        LOG("Warning: Not enough memory, reducing early memory area");
        early_memory_end = total_memory - (total_memory / 4); // Use upper 3/4 for early memory
    }
    
    LOG("Initializing early memory from multiboot, start: 0x" << kernel_end);
    LOG("Available memory: " << total_memory / 1024 << " KB");
    
    return Initialize(kernel_end);
}

bool EarlyMemoryManager::ReserveRegion(uint32_t start_addr, uint32_t size) {
    // Find the region that contains this address range
    MemoryRegion* current = free_list;
    
    // This is a simplified implementation - in a complete implementation,
    // we'd split regions as needed to reserve the requested area
    while (current) {
        uint32_t region_start = (uint32_t)current->start;
        uint32_t region_end = region_start + current->size;
        
        // Check if the requested region overlaps with this free region
        if (start_addr < region_end && (start_addr + size) > region_start && !current->used) {
            // Mark as used
            current->used = true;
            LOG("Reserved early memory region: 0x" << (void*)start_addr << " for " << size << " bytes");
            return true;
        }
        current = current->next;
    }
    
    LOG("Could not reserve region: 0x" << (void*)start_addr << " for " << size << " bytes");
    return false;
}

bool EarlyMemoryManager::IsValidAddress(uint32_t addr) {
    MemoryRegion* current = free_list;
    while (current) {
        uint32_t region_start = (uint32_t)current->start;
        uint32_t region_end = region_start + current->size;
        
        if (addr >= region_start && addr < region_end) {
            return true;
        }
        current = current->next;
    }
    return false;
}

uint32_t EarlyMemoryManager::GetTotalMemory() {
    return initialized_size;
}

// Initialize early memory management system
bool InitializeEarlyMemory(struct Multiboot* mboot_ptr) {
    g_early_memory_manager = &early_memory_manager_instance;
    
    // Initialize from multiboot information if available
    if (mboot_ptr) {
        if (!g_early_memory_manager->InitializeFromMultiboot(mboot_ptr)) {
            LOG("Error: Failed to initialize early memory from multiboot info");
            return false;
        }
    } else {
        // Fallback initialization
        uint32_t kernel_end = 0x100000; // 1MB - typical location after kernel
        if (!g_early_memory_manager->Initialize(kernel_end)) {
            LOG("Error: Failed to initialize early memory manager");
            return false;
        }
    }
    
    LOG("Early memory management system initialized successfully");
    return true;
}

// Helper functions that work before the main heap is initialized
void* EarlyMalloc(uint32_t size) {
    if (!g_early_memory_manager) {
        LOG("Error: Early memory manager not initialized");
        return nullptr;
    }
    
    return g_early_memory_manager->Allocate(size);
}

void EarlyFree(void* ptr) {
    if (!g_early_memory_manager) {
        LOG("Error: Early memory manager not initialized");
        return;
    }
    
    g_early_memory_manager->Free(ptr);
}

void* EarlyCalloc(uint32_t count, uint32_t size) {
    uint32_t total_size = count * size;
    void* ptr = EarlyMalloc(total_size);
    
    if (ptr) {
        // Zero out the allocated memory
        memset(ptr, 0, total_size);
    }
    
    return ptr;
}