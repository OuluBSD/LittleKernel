#include "Kernel.h"
#include "MemoryMappedFile.h"
#include "Logging.h"
#include "ProcessControlBlock.h"

MemoryMappingManager::MemoryMappingManager() {
    mapping_list = nullptr;
    next_mapping_id = 1;  // Start with ID 1, 0 is invalid
}

MemoryMappingManager::~MemoryMappingManager() {
    // Clean up all memory mappings
    MemoryMappedFile* current = mapping_list;
    while (current) {
        MemoryMappedFile* next = current->next;
        
        // Remove the mapping from the process's address space
        if (global && global->paging_manager && current->page_dir) {
            // Unmap the pages from the process's page directory
            uint32 page_count = (current->size + KERNEL_PAGE_SIZE - 1) / KERNEL_PAGE_SIZE;
            for (uint32 i = 0; i < page_count; i++) {
                uint32 virt_addr = (uint32)current->virtual_address + i * KERNEL_PAGE_SIZE;
                global->paging_manager->UnmapPage(virt_addr, current->page_dir);
            }
        }
        
        free(current);
        current = next;
    }
    
    mapping_list = nullptr;
}

MemoryMappedFile* MemoryMappingManager::CreateMapFile(void* file_handle, 
                                                       uint32 offset, 
                                                       uint32 size, 
                                                       uint32 flags,
                                                       ProcessControlBlock* pcb,
                                                       void* desired_vaddr) {
    if (!file_handle || !pcb || size == 0) {
        LOG("Invalid parameters to CreateMapFile");
        return nullptr;
    }
    
    // Allocate and initialize a new memory mapping
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
    mapping->flags = flags;
    mapping->pid = pcb->pid;
    mapping->page_dir = pcb->page_directory;
    mapping->next = nullptr;
    
    // Determine virtual address to use
    if (desired_vaddr) {
        mapping->virtual_address = desired_vaddr;
    } else {
        // Allocate a virtual address in user space (e.g., starting from 0x50000000)
        static uint32 next_vaddr = 0x50000000;
        mapping->virtual_address = (void*)next_vaddr;
        next_vaddr += (size + KERNEL_PAGE_SIZE - 1) & ~(KERNEL_PAGE_SIZE - 1);  // Align to page boundary
    }
    
    // For now, we'll implement a simple approach where we read the file into memory
    // In a real implementation, this would use demand paging to load pages on demand
    uint32 page_count = (size + KERNEL_PAGE_SIZE - 1) / KERNEL_PAGE_SIZE;
    
    // Allocate physical pages and map them to the virtual address
    if (global && global->paging_manager && pcb->page_directory) {
        for (uint32 i = 0; i < page_count; i++) {
            void* page = global->memory_manager->AllocatePage();
            if (!page) {
                LOG("Failed to allocate physical page for memory mapping");
                // Clean up already allocated pages
                for (uint32 j = 0; j < i; j++) {
                    uint32 virt_addr = (uint32)mapping->virtual_address + j * KERNEL_PAGE_SIZE;
                    void* page_to_free = (void*)global->paging_manager->GetPhysicalAddress(virt_addr, pcb->page_directory);
                    if (page_to_free) {
                        global->memory_manager->FreePage(page_to_free);
                        global->paging_manager->UnmapPage(virt_addr, pcb->page_directory);
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
    
    LOG("Memory mapping not found in global list");
    return false;
}

bool MemoryMappingManager::UnmapFileById(uint32 id, uint32 pid) {
    MemoryMappedFile* mapping = FindMappingById(id);
    if (!mapping || mapping->pid != pid) {
        LOG("Memory mapping ID " << id << " not found for process " << pid);
        return false;
    }
    
    return UnmapFile(mapping);
}

MemoryMappedFile* MemoryMappingManager::GetMappingById(uint32 id) {
    return FindMappingById(id);
}

MemoryMappedFile* MemoryMappingManager::GetMappingByProcessAndAddr(uint32 pid, void* vaddr) {
    return FindMappingByProcessAndAddr(pid, vaddr);
}

bool MemoryMappingManager::SyncMapping(MemoryMappedFile* mapping) {
    if (!mapping) {
        return false;
    }
    
    // In a real implementation, this would write the mapped memory back to the file
    // For now, this is a placeholder
    DLOG("Syncing memory mapping ID " << mapping->id << " to file");
    
    // TODO: Implement actual sync to file
    // This would involve iterating through mapped pages and writing changes back to file
    
    return true;
}

uint32 MemoryMappingManager::GetMappingSize(uint32 id) {
    MemoryMappedFile* mapping = FindMappingById(id);
    return mapping ? mapping->size : 0;
}

uint32 MemoryMappingManager::GetMappingFlags(uint32 id) {
    MemoryMappedFile* mapping = FindMappingById(id);
    return mapping ? mapping->flags : 0;
}

// Private helper functions

MemoryMappedFile* MemoryMappingManager::FindMappingById(uint32 id) {
    MemoryMappedFile* current = mapping_list;
    while (current) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

MemoryMappedFile* MemoryMappingManager::FindMappingByProcessAndAddr(uint32 pid, void* vaddr) {
    MemoryMappedFile* current = mapping_list;
    while (current) {
        if (current->pid == pid && current->virtual_address == vaddr) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

bool MemoryMappingManager::AddToProcessMappingList(MemoryMappedFile* mapping) {
    // In a more complete implementation, process-specific mapping lists would be needed
    // For now, we just add to the global list
    mapping->next = mapping_list;
    mapping_list = mapping;
    return true;
}

bool MemoryMappingManager::RemoveFromProcessMappingList(MemoryMappedFile* mapping) {
    // In a more complete implementation, process-specific mapping lists would be needed
    // For now, we just remove from the global list (same as UnmapFile for global list)
    return UnmapFile(mapping);
}