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
    
    region_list = nullptr;
}

SharedMemoryRegion* SharedMemoryManager::CreateSharedMemory(uint32 size) {
    if (size == 0) {
        LOG("Cannot create shared memory region with size 0");
        return nullptr;
    }
    
    // Allocate and initialize a new shared memory region
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
    uint32 page_aligned_size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    region->virtual_address = malloc(page_aligned_size);
    if (!region->virtual_address) {
        LOG("Failed to allocate shared memory block of size " << size);
        free(region);
        return nullptr;
    }
    
    // Get the physical address
    region->physical_address = VirtualToPhysical(region->virtual_address);
    
    // Add to the beginning of the global list
    region->next = region_list;
    region_list = region;
    
    DLOG("Created shared memory region ID " << region->id << " with size " << size 
          << " at virtual: 0x" << (uint32)region->virtual_address 
          << ", physical: 0x" << region->physical_address);
    
    return region;
}

SharedMemoryRegion* SharedMemoryManager::GetSharedMemory(uint32 id) {
    return FindRegionById(id);
}

void* SharedMemoryManager::MapSharedMemoryToProcess(SharedMemoryRegion* region, 
                                                    ProcessControlBlock* pcb, 
                                                    void* desired_vaddr) {
    if (!region || !pcb) {
        LOG("Invalid parameters to MapSharedMemoryToProcess");
        return nullptr;
    }
    
    // Find a suitable virtual address if none is provided
    void* target_vaddr = desired_vaddr ? desired_vaddr : (void*)(0x40000000 + region->id * 0x1000000); // Example: 0x40xxxxxx
    
    // Map the physical page to the process's virtual address
    if (global && global->paging_manager) {
        // Map the shared memory pages to the process's address space
        uint32 page_count = (region->size + PAGE_SIZE - 1) / PAGE_SIZE;
        uint32 shared_phys_addr = region->physical_address;
        
        for (uint32 i = 0; i < page_count; i++) {
            uint32 virt_addr = (uint32)target_vaddr + i * PAGE_SIZE;
            uint32 phys_addr = shared_phys_addr + i * PAGE_SIZE;
            
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
                    uint32 undo_virt_addr = (uint32)target_vaddr + j * PAGE_SIZE;
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
        uint32 page_count = (region->size + PAGE_SIZE - 1) / PAGE_SIZE;
        for (uint32 i = 0; i < page_count; i++) {
            uint32 virt_addr = (uint32)target_vaddr + i * PAGE_SIZE;
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
    uint32 page_count = (region->size + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint32 i = 0; i < page_count; i++) {
        uint32 virt_addr = (uint32)mapping->process_vaddr + i * PAGE_SIZE;
        global->paging_manager->UnmapPage(virt_addr, pcb->page_directory);
    }
    
    // Remove the mapping from the region's list
    if (!RemoveProcessMapping(region, pcb->pid)) {
        LOG("Failed to remove process mapping for shared memory");
        return false;
    }
    
    // Decrement reference count
    if (region->ref_count > 0) {
        region->ref_count--;
    }
    
    // Decrement attachment count
    if (region->attach_count > 0) {
        region->attach_count--;
    }
    
    // If ref count is 0 and the region is marked for deletion, delete it completely
    if (region->ref_count == 0 && region->is_deleted) {
        // Check if the region should be completely removed
        // (This could be done by a cleanup thread or in a later function call)
    }
    
    DLOG("Unmapped shared memory ID " << region->id << " from process " << pcb->pid);
    
    return true;
}

void* SharedMemoryManager::AttachSharedMemory(uint32 id, ProcessControlBlock* pcb) {
    SharedMemoryRegion* region = FindRegionById(id);
    if (!region) {
        LOG("Shared memory ID " << id << " not found");
        return nullptr;
    }
    
    if (region->is_deleted) {
        LOG("Shared memory ID " << id << " is marked for deletion");
        return nullptr;
    }
    
    // Check if this process already has this shared memory mapped
    SharedMemoryRegion::ProcessMapping* mapping = region->mappings;
    while (mapping) {
        if (mapping->pid == pcb->pid) {
            // Already mapped, return the existing address
            // Increment the attach count for this process
            region->attach_count++;
            return mapping->process_vaddr;
        }
        mapping = mapping->next;
    }
    
    // Map the shared memory to this process
    void* result = MapSharedMemoryToProcess(region, pcb);
    if (result) {
        // Increment attach count when successful
        region->attach_count++;
    }
    return result;
}

bool SharedMemoryManager::DetachSharedMemory(uint32 id, ProcessControlBlock* pcb) {
    SharedMemoryRegion* region = FindRegionById(id);
    if (!region) {
        LOG("Shared memory ID " << id << " not found");
        return false;
    }
    
    bool result = UnmapSharedMemoryFromProcess(region, pcb);
    
    // Decrement attach count if the detach was successful
    if (result && region->attach_count > 0) {
        region->attach_count--;
    }
    
    return result;
}

bool SharedMemoryManager::DeleteSharedMemory(uint32 id) {
    SharedMemoryRegion* region = FindRegionById(id);
    if (!region) {
        LOG("Shared memory ID " << id << " not found for deletion");
        return false;
    }
    
    // Check if there are still attachments to this region
    if (region->attach_count > 0) {
        LOG("Warning: Shared memory ID " << id << " still has " << region->attach_count 
             << " attachments, but marked for deletion. Region will be deleted when all attachments are removed.");
    }
    
    // Mark the region for deletion
    region->is_deleted = true;
    
    DLOG("Marked shared memory ID " << id << " for deletion");
    
    // If there are no references (processes), we can potentially remove it now
    // But we'll wait for all attachments to be removed first
    if (region->ref_count == 0) {
        CleanupDeletedRegions();
    }
    
    return true;
}

void SharedMemoryManager::CleanupDeletedRegions() {
    SharedMemoryRegion* current = region_list;
    SharedMemoryRegion* prev = nullptr;
    
    while (current) {
        if (current->is_deleted && current->ref_count == 0) {
            // Check if all attachments have been removed
            if (current->attach_count == 0) {
                SharedMemoryRegion* next = current->next;
                
                // Remove from the list
                if (prev) {
                    prev->next = current->next;
                } else {
                    region_list = current->next;
                }
                
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
                
                // Free the region structure
                free(current);
                
                current = next;
                continue;  // Continue with the next item, don't update prev
            }
        }
        
        prev = current;
        current = current->next;
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