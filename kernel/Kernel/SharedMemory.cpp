#include "Kernel.h"
#include "SharedMemory.h"
#include "Logging.h"
#include "ProcessControlBlock.h"

SharedMemoryManager::SharedMemoryManager() {
    region_list = nullptr;
    next_shmid = 1;  // Start with ID 1, 0 is invalid
}

SharedMemoryManager::~SharedMemoryManager() {
    // Clean up all shared memory regions
    SharedMemoryRegion* current = region_list;
    while (current) {
        SharedMemoryRegion* next = current->next;

        // Free the actual shared memory
        if (current->virtual_address) {
            free(current->virtual_address);
        }

        // Free process mappings
        SharedMemoryRegion::ProcessMapping* mapping = current->mappings;
        while (mapping) {
            SharedMemoryRegion::ProcessMapping* next_mapping = mapping->next;
            free(mapping);
            mapping = next_mapping;
        }

        free(current);
        current = next;
    }
}

SharedMemoryRegion* SharedMemoryManager::CreateSharedMemory(uint32 size) {
    if (size == 0) {
        LOG("Cannot create shared memory region with zero size");
        return nullptr;
    }

    // Create a new shared memory region structure
    SharedMemoryRegion* region = (SharedMemoryRegion*)malloc(sizeof(SharedMemoryRegion));
    if (!region) {
        LOG("Failed to allocate shared memory region structure");
        return nullptr;
    }

    // Initialize the region
    region->id = next_shmid++;
    region->size = size;
    region->ref_count = 0;
    region->attach_count = 0;
    region->is_deleted = false;
    region->mappings = nullptr;
    region->next = nullptr;

    // Allocate the actual shared memory (aligned to page boundaries for efficiency)
    uint32 page_aligned_size = (size + KERNEL_PAGE_SIZE - 1) & ~(KERNEL_PAGE_SIZE - 1);
    region->virtual_address = malloc(page_aligned_size);
    if (!region->virtual_address) {
        LOG("Failed to allocate shared memory block of size " << size);
        free(region);
        return nullptr;
    }
    
    // Get the physical address for the shared memory
    region->physical_address = VirtualToPhysical(region->virtual_address);

    // Initialize all bytes to zero
    memset(region->virtual_address, 0, size);

    // Add to the global list of shared memory regions
    region->next = region_list;
    region_list = region;

    DLOG("Created shared memory region ID " << region->id << " of size " << size 
          << ", virtual address: 0x" << (uint32)region->virtual_address 
          << ", physical address: 0x" << region->physical_address);

    return region;
}

SharedMemoryRegion* SharedMemoryManager::GetSharedMemory(uint32 id) {
    SharedMemoryRegion* current = region_list;
    while (current) {
        if (current->id == id && !current->is_deleted) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void* SharedMemoryManager::MapSharedMemoryToProcess(SharedMemoryRegion* region,
                                                    ProcessControlBlock* pcb,
                                                    void* desired_vaddr) {
    if (!region || !pcb) {
        LOG("Invalid region or process control block");
        return nullptr;
    }

    // If no desired address is specified, try to find a suitable one
    void* target_vaddr = desired_vaddr;
    if (!target_vaddr) {
        // For now, we'll use a simple approach: start looking from 0x70000000
        // In a real implementation, you'd have a more sophisticated virtual address allocator
        static uint32 next_addr = 0x70000000;
        target_vaddr = (void*)next_addr;
        next_addr += (region->size + KERNEL_PAGE_SIZE - 1) & ~(KERNEL_PAGE_SIZE - 1);  // Align to page boundary
    }

    // Map the physical page to the process's virtual address
    if (global && global->paging_manager) {
        // Map the shared memory pages to the process's address space
        uint32 page_count = (region->size + KERNEL_PAGE_SIZE - 1) / KERNEL_PAGE_SIZE;
        uint32 shared_phys_addr = region->physical_address;
        
        for (uint32 i = 0; i < page_count; i++) {
            uint32 virt_addr = (uint32)target_vaddr + i * KERNEL_PAGE_SIZE;
            uint32 phys_addr = shared_phys_addr + i * KERNEL_PAGE_SIZE;
            
            // Map the page in the process's page directory
            bool success = global->paging_manager->MapPage(
                virt_addr, 
                phys_addr, 
                PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER,
                pcb->page_directory
            );
            
            if (!success) {
                LOG("Failed to map shared memory page to process");
                // Unmap any pages already mapped
                for (uint32 j = 0; j < i; j++) {
                    uint32 undo_virt_addr = (uint32)target_vaddr + j * KERNEL_PAGE_SIZE;
                    global->paging_manager->UnmapPage(undo_virt_addr, pcb->page_directory);
                }
                return nullptr;
            }
        }
    } else {
        LOG("Paging manager not available for shared memory mapping");
        return nullptr;
    }
    
    // Check if this process already has a mapping for this region
    bool already_mapped = false;
    SharedMemoryRegion::ProcessMapping* existing_mapping = region->mappings;
    while (existing_mapping) {
        if (existing_mapping->pid == pcb->pid) {
            already_mapped = true;
            break;
        }
        existing_mapping = existing_mapping->next;
    }
    
    // Add the process mapping to the region's mapping list
    if (!AddProcessMapping(region, pcb->pid, target_vaddr, pcb->page_directory)) {
        LOG("Failed to add process mapping for shared memory");
        // Unmap the pages we just mapped
        uint32 page_count = (region->size + KERNEL_PAGE_SIZE - 1) / KERNEL_PAGE_SIZE;
        for (uint32 i = 0; i < page_count; i++) {
            uint32 virt_addr = (uint32)target_vaddr + i * KERNEL_PAGE_SIZE;
            global->paging_manager->UnmapPage(virt_addr, pcb->page_directory);
        }
        return nullptr;
    }
    
    // Increment reference count only if this is a new process mapping (not just another attachment)
    if (!already_mapped) {
        region->ref_count++;
    }
    
    // Increment attachment count for this mapping
    region->attach_count++;
    
    DLOG("Mapped shared memory ID " << region->id << " to process " << pcb->pid 
          << " at virtual address: 0x" << (uint32)target_vaddr);
    
    return target_vaddr;
}

bool SharedMemoryManager::UnmapSharedMemoryFromProcess(SharedMemoryRegion* region, 
                                                       ProcessControlBlock* pcb) {
    if (!region || !pcb) {
        return false;
    }
    
    // Find the existing mapping for this process
    SharedMemoryRegion::ProcessMapping* mapping = region->mappings;
    while (mapping) {
        if (mapping->pid == pcb->pid) {
            break;
        }
        mapping = mapping->next;
    }
    
    if (!mapping) {
        LOG("Process " << pcb->pid << " not mapped to shared memory region " << region->id);
        return false;
    }
    
    // Unmap the pages in the process's address space
    uint32 page_count = (region->size + KERNEL_PAGE_SIZE - 1) / KERNEL_PAGE_SIZE;
    for (uint32 i = 0; i < page_count; i++) {
        uint32 virt_addr = (uint32)mapping->process_vaddr + i * KERNEL_PAGE_SIZE;
        global->paging_manager->UnmapPage(virt_addr, pcb->page_directory);
    }
    
    // Remove the mapping from the region's list
    RemoveProcessMapping(region, pcb->pid);
    
    // Decrement attachment count
    if (region->attach_count > 0) {
        region->attach_count--;
    }
    
    // If the region is marked for deletion and there are no more attachments, delete it
    if (region->is_deleted && region->attach_count == 0) {
        DeleteSharedMemory(region->id);
    }
    
    DLOG("Unmapped shared memory ID " << region->id << " from process " << pcb->pid);
    return true;
}

void* SharedMemoryManager::AttachSharedMemory(uint32 id, ProcessControlBlock* pcb) {
    if (!pcb) {
        LOG("Cannot attach shared memory to null process control block");
        return nullptr;
    }
    
    // Find the shared memory region
    SharedMemoryRegion* region = FindRegionById(id);
    if (!region) {
        LOG("Shared memory region ID " << id << " not found for attachment");
        return nullptr;
    }
    
    if (region->is_deleted) {
        LOG("Cannot attach to shared memory region " << id << " - marked for deletion");
        return nullptr;
    }
    
    // Check if this process already has this region attached
    SharedMemoryRegion::ProcessMapping* current_mapping = region->mappings;
    while (current_mapping) {
        if (current_mapping->pid == pcb->pid) {
            // Process already has this region attached, return the existing address
            DLOG("Process " << pcb->pid << " already attached to shared memory " << id 
                  << " at address 0x" << (uint32)current_mapping->process_vaddr);
            return current_mapping->process_vaddr;
        }
        current_mapping = current_mapping->next;
    }
    
    // Map the shared memory to this process - this will also add the process mapping
    void* mapped_address = MapSharedMemoryToProcess(region, pcb);
    if (!mapped_address) {
        LOG("Failed to map shared memory ID " << id << " to process " << pcb->pid);
        return nullptr;
    }
    
    DLOG("Attached shared memory ID " << id << " to process " << pcb->pid 
          << " at address 0x" << (uint32)mapped_address);
    
    return mapped_address;
}

bool SharedMemoryManager::DetachSharedMemory(uint32 id, ProcessControlBlock* pcb) {
    if (!pcb) {
        LOG("Cannot detach shared memory from null process control block");
        return false;
    }
    
    // Find the shared memory region
    SharedMemoryRegion* region = FindRegionById(id);
    if (!region) {
        LOG("Shared memory region ID " << id << " not found for detachment");
        return false;
    }
    
    // Find the mapping for this process
    SharedMemoryRegion::ProcessMapping* mapping = region->mappings;
    while (mapping) {
        if (mapping->pid == pcb->pid) {
            break;
        }
        mapping = mapping->next;
    }
    
    if (!mapping) {
        LOG("Process " << pcb->pid << " not attached to shared memory region " << id);
        return false;
    }
    
    // Unmap the pages in the process's address space
    uint32 page_count = (region->size + KERNEL_PAGE_SIZE - 1) / KERNEL_PAGE_SIZE;
    for (uint32 i = 0; i < page_count; i++) {
        uint32 virt_addr = (uint32)mapping->process_vaddr + i * KERNEL_PAGE_SIZE;
        global->paging_manager->UnmapPage(virt_addr, pcb->page_directory);
    }
    
    // Remove the mapping from the region's list
    RemoveProcessMapping(region, pcb->pid);
    
    // Decrement attachment count
    if (region->attach_count > 0) {
        region->attach_count--;
    }
    
    // Decrement reference count if this was the last attachment from this process
    bool found_other_attachments = false;
    SharedMemoryRegion::ProcessMapping* check_mapping = region->mappings;
    while (check_mapping) {
        if (check_mapping->pid == pcb->pid) {
            found_other_attachments = true;
            break;
        }
        check_mapping = check_mapping->next;
    }
    
    if (!found_other_attachments && region->ref_count > 0) {
        region->ref_count--;
    }
    
    // If the region is marked for deletion and there are no more attachments, delete it
    if (region->is_deleted && region->attach_count == 0) {
        DeleteSharedMemory(region->id);
    }
    
    DLOG("Detached shared memory ID " << id << " from process " << pcb->pid);
    return true;
}

bool SharedMemoryManager::DeleteSharedMemory(uint32 id) {
    SharedMemoryRegion* region = FindRegionById(id);
    if (!region) {
        LOG("Cannot delete shared memory region - ID " << id << " not found");
        return false;
    }
    
    // Mark the region for deletion
    region->is_deleted = true;
    
    // If there are no attachments, we can delete it immediately
    if (region->attach_count == 0) {
        // Remove from the global list
        SharedMemoryRegion* current = region_list;
        SharedMemoryRegion* prev = nullptr;
        
        while (current) {
            if (current == region) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    region_list = current->next;
                }
                
                // Free the memory and mappings
                if (region->virtual_address) {
                    free(region->virtual_address);
                }
                
                // Free mappings
                SharedMemoryRegion::ProcessMapping* mapping = region->mappings;
                while (mapping) {
                    SharedMemoryRegion::ProcessMapping* next = mapping->next;
                    free(mapping);
                    mapping = next;
                }
                
                // Free the region itself
                free(region);
                
                DLOG("Deleted shared memory region ID " << id);
                return true;
            }
            prev = current;
            current = current->next;
        }
    }
    
    DLOG("Marked shared memory region ID " << id << " for deletion");
    return true;
}

void SharedMemoryManager::CleanupDeletedRegions() {
    SharedMemoryRegion* current = region_list;
    SharedMemoryRegion* prev = nullptr;
    
    while (current) {
        if (current->is_deleted && current->attach_count == 0) {
            SharedMemoryRegion* to_delete = current;
            current = current->next;
            
            if (prev) {
                prev->next = current;
            } else {
                region_list = current;
            }
            
            // Free the memory and mappings
            if (to_delete->virtual_address) {
                free(to_delete->virtual_address);
            }
            
            // Free mappings
            SharedMemoryRegion::ProcessMapping* mapping = to_delete->mappings;
            while (mapping) {
                SharedMemoryRegion::ProcessMapping* next = mapping->next;
                free(mapping);
                mapping = next;
            }
            
            // Free the region itself
            free(to_delete);
            
            DLOG("Cleaned up deleted shared memory region");
        } else {
            prev = current;
            current = current->next;
        }
    }
}

uint32 SharedMemoryManager::GetSharedMemorySize(uint32 id) {
    SharedMemoryRegion* region = FindRegionById(id);
    return region ? region->size : 0;
}

uint32 SharedMemoryManager::GetSharedMemoryRefCount(uint32 id) {
    SharedMemoryRegion* region = FindRegionById(id);
    return region ? region->ref_count : 0;
}

uint32 SharedMemoryManager::GetSharedMemoryAttachCount(uint32 id) {
    SharedMemoryRegion* region = FindRegionById(id);
    return region ? region->attach_count : 0;
}

bool SharedMemoryManager::IsSharedMemoryMarkedForDeletion(uint32 id) {
    SharedMemoryRegion* region = FindRegionById(id);
    return region ? region->is_deleted : false;
}

// Private helper functions

SharedMemoryRegion* SharedMemoryManager::FindRegionById(uint32 id) {
    SharedMemoryRegion* current = region_list;
    while (current) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

bool SharedMemoryManager::AddProcessMapping(SharedMemoryRegion* region,
                                            uint32 pid,
                                            void* process_vaddr,
                                            PageDirectory* page_dir) {
    // Create a new mapping
    SharedMemoryRegion::ProcessMapping* mapping =
        (SharedMemoryRegion::ProcessMapping*)malloc(sizeof(SharedMemoryRegion::ProcessMapping));

    if (!mapping) {
        LOG("Failed to allocate process mapping structure");
        return false;
    }

    mapping->pid = pid;
    mapping->process_vaddr = process_vaddr;
    mapping->page_dir = page_dir;
    mapping->next = region->mappings;

    // Add to the beginning of the mapping list
    region->mappings = mapping;

    return true;
}

bool SharedMemoryManager::RemoveProcessMapping(SharedMemoryRegion* region, uint32 pid) {
    SharedMemoryRegion::ProcessMapping* current = region->mappings;
    SharedMemoryRegion::ProcessMapping* prev = nullptr;

    while (current) {
        if (current->pid == pid) {
            // Remove from the list
            if (prev) {
                prev->next = current->next;
            } else {
                region->mappings = current->next;
            }

            // Free the mapping structure
            free(current);

            return true;
        }

        prev = current;
        current = current->next;
    }

    return false;  // PID not found
}