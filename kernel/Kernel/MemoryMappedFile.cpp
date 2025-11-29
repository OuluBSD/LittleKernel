#include "Kernel.h"

MemoryMappingManager::MemoryMappingManager() {
    mapping_list = nullptr;
    next_mapping_id = 1;
}

MemoryMappingManager::~MemoryMappingManager() {
    // Clean up all memory mappings
    MemoryMappedFile* current = mapping_list;
    while (current) {
        MemoryMappedFile* next = current->next;
        
        // Unmap the pages if they were mapped
        if (global && global->paging_manager && current->page_dir) {
            // Unmap the pages from the process's address space
            uint32 page_count = (current->size + KERNEL_PAGE_SIZE - 1) / KERNEL_PAGE_SIZE;
            for (uint32 i = 0; i < page_count; i++) {
                uint32 virt_addr = (uint32)current->virtual_address + i * KERNEL_PAGE_SIZE;
                global->paging_manager->UnmapPage(virt_addr, current->page_dir);
            }
        }
        
        free(current);
        current = next;
    }
}

MemoryMappedFile* MemoryMappingManager::CreateMapFile(void* file_handle, uint32 offset, uint32 size, uint32 flags, ProcessControlBlock* pcb, void* desired_vaddr) {
    if (!file_handle || size == 0 || !pcb) {
        LOG("Invalid parameters for creating memory mapping");
        return nullptr;
    }

    // Allocate a new mapping structure
    MemoryMappedFile* mapping = (MemoryMappedFile*)malloc(sizeof(MemoryMappedFile));
    if (!mapping) {
        LOG("Failed to allocate memory mapping structure");
        return nullptr;
    }

    // Initialize the mapping
    mapping->id = next_mapping_id++;
    mapping->file_handle = file_handle;
    mapping->file_offset = offset;
    mapping->size = size;
    mapping->file_size = size;  // Simplified - should get actual file size
    mapping->flags = flags;
    mapping->pid = pcb->pid;
    mapping->page_dir = pcb->page_directory;
    mapping->next = nullptr;

    // Determine where to map in the process's address space
    if (flags & MAP_FIXED) {
        mapping->virtual_address = desired_vaddr;  // Use the desired address
    } else if (flags & MAP_SHARED) {
        // Map in a shared region (for now, start from a fixed address)
        static uint32 next_vaddr = 0x60000000;
        mapping->virtual_address = (void*)next_vaddr;
        next_vaddr += (size + KERNEL_PAGE_SIZE - 1) & ~(KERNEL_PAGE_SIZE - 1);  // Align to page boundary
    } else {
        // Allocate a virtual address in user space (e.g., starting from 0x50000000)
        static uint32 next_vaddr = 0x50000000;
        if (desired_vaddr) {
            mapping->virtual_address = desired_vaddr;
        } else {
            mapping->virtual_address = (void*)next_vaddr;
            next_vaddr += (size + KERNEL_PAGE_SIZE - 1) & ~(KERNEL_PAGE_SIZE - 1);  // Align to page boundary
        }
    }

    // For now, we'll implement a simple approach where we read the file into memory
    // In a real implementation, this would use demand paging to load pages on demand
    uint32 page_count = (size + KERNEL_PAGE_SIZE - 1) / KERNEL_PAGE_SIZE;
    
    // Allocate physical pages and map them to the virtual address
    if (global && global->paging_manager && pcb->page_directory) {
        for (uint32 i = 0; i < page_count; i++) {
            void* page = global->memory_manager->AllocatePage();
            if (!page) {
                LOG("Failed to allocate physical page for mapping");
                // Clean up already allocated pages
                for (uint32 j = 0; j < i; j++) {
                    uint32 undo_virt_addr = (uint32)mapping->virtual_address + j * KERNEL_PAGE_SIZE;
                    void* page_to_free = (void*)global->paging_manager->GetPhysicalAddress(undo_virt_addr, pcb->page_directory);
                    if (page_to_free) {
                        global->memory_manager->FreePage(page_to_free);
                        global->paging_manager->UnmapPage(undo_virt_addr, pcb->page_directory);
                    }
                }
                free(mapping);
                return nullptr;
            }
            
            // Map the physical page to the virtual address
            uint32 virt_addr = (uint32)mapping->virtual_address + i * KERNEL_PAGE_SIZE;
            uint32 phys_addr = VirtualToPhysical(page);
            
            // Determine page flags based on mapping flags
            uint32 page_flags = PAGE_PRESENT;
            if (flags & MAP_WRITE) {
                page_flags |= PAGE_WRITABLE;
            }
            if (flags & MAP_PRIVATE) {  // User access
                page_flags |= PAGE_USER;
            }
            
            bool success = global->paging_manager->MapPage(virt_addr, phys_addr, page_flags, pcb->page_directory);
            if (!success) {
                LOG("Failed to map page to process address space");
                global->memory_manager->FreePage(page);
                // Clean up already allocated pages
                for (uint32 j = 0; j < i; j++) {
                    uint32 undo_virt_addr = (uint32)mapping->virtual_address + j * KERNEL_PAGE_SIZE;
                    void* page_to_free = (void*)global->paging_manager->GetPhysicalAddress(undo_virt_addr, pcb->page_directory);
                    if (page_to_free) {
                        global->memory_manager->FreePage(page_to_free);
                        global->paging_manager->UnmapPage(undo_virt_addr, pcb->page_directory);
                    }
                }
                free(mapping);
                return nullptr;
            }
            
            // Read the relevant file data into this page
            // This is a simplified implementation - in reality, we'd either:
            // 1. Read the appropriate file segment into the page, or
            // 2. Set up demand paging so the page is loaded when first accessed
            memset(page, 0, KERNEL_PAGE_SIZE);  // For now, just zero the page
        }
    } else {
        LOG("Paging manager or process page directory not available");
        free(mapping);
        return nullptr;
    }
    
    // Add to the global mapping list
    mapping->next = mapping_list;
    mapping_list = mapping;
    
    DLOG("Created memory mapping ID " << mapping->id << " for file, size " << size 
          << ", virtual address: 0x" << (uint32)mapping->virtual_address 
          << ", process PID: " << pcb->pid);
    
    return mapping;
}

bool MemoryMappingManager::UnmapFile(MemoryMappedFile* mapping) {
    if (!mapping) {
        return false;
    }
    
    // Remove from the global list
    MemoryMappedFile* current = mapping_list;
    MemoryMappedFile* prev = nullptr;
    
    while (current) {
        if (current == mapping) {
            if (prev) {
                prev->next = current->next;
            } else {
                mapping_list = current->next;
            }
            
            // Unmap pages from the process's address space
            if (global && global->paging_manager && mapping->page_dir) {
                uint32 page_count = (mapping->size + KERNEL_PAGE_SIZE - 1) / KERNEL_PAGE_SIZE;
                for (uint32 i = 0; i < page_count; i++) {
                    uint32 virt_addr = (uint32)mapping->virtual_address + i * KERNEL_PAGE_SIZE;
                    uint32 phys_addr = global->paging_manager->GetPhysicalAddress(virt_addr, mapping->page_dir);
                    if (phys_addr) {
                        // Free the physical page if the mapping was private
                        if (mapping->flags & MAP_PRIVATE) {
                            global->memory_manager->FreePage((void*)phys_addr);
                        }
                        global->paging_manager->UnmapPage(virt_addr, mapping->page_dir);
                    }
                }
            }
            
            // Free the mapping structure
            free(mapping);
            
            DLOG("Unmapped memory mapping ID " << mapping->id);
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    LOG("Memory mapping not found for unmapping");
    return false;
}

MemoryMappedFile* MemoryMappingManager::GetMappingById(uint32 id) {
    MemoryMappedFile* current = mapping_list;
    while (current) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}