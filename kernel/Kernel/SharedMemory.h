#ifndef _Kernel_SharedMemory_h_
#define _Kernel_SharedMemory_h_

#include "Common.h"
#include "Paging.h"
#include "ProcessControlBlock.h"  // Include ProcessControlBlock for SharedMemory functions

// Structure to represent a shared memory region
struct SharedMemoryRegion {
    uint32 id;                    // Unique identifier for this shared memory
    void* virtual_address;        // Virtual address in kernel space
    uint32 physical_address;      // Physical address of the shared memory
    uint32 size;                  // Size of the shared memory region in bytes
    uint32 ref_count;             // Number of processes using this shared memory
    uint32 attach_count;          // Number of attachments (can be different from ref_count if processes attach multiple times)
    bool is_deleted;              // Flag indicating if the region is marked for deletion
    
    // Process-specific mappings
    struct ProcessMapping {
        uint32 pid;               // Process ID
        void* process_vaddr;      // Virtual address in the process's address space
        PageDirectory* page_dir;  // Page directory of the process
        ProcessMapping* next;     // Next mapping in the list
    } *mappings;
    
    SharedMemoryRegion* next;     // Next shared memory region in the global list
};

// Shared memory manager class
class SharedMemoryManager {
private:
    SharedMemoryRegion* region_list;
    uint32 next_shmid;            // Next shared memory ID to assign
    
public:
    SharedMemoryManager();
    ~SharedMemoryManager();
    
    // Create a new shared memory region
    SharedMemoryRegion* CreateSharedMemory(uint32 size);
    
    // Get a shared memory region by ID
    SharedMemoryRegion* GetSharedMemory(uint32 id);
    
    // Map a shared memory region to a process's address space
    void* MapSharedMemoryToProcess(SharedMemoryRegion* region, 
                                   ProcessControlBlock* pcb, 
                                   void* desired_vaddr = nullptr);
    
    // Unmap a shared memory region from a process's address space
    bool UnmapSharedMemoryFromProcess(SharedMemoryRegion* region, 
                                      ProcessControlBlock* pcb);
    
    // Attach to an existing shared memory region
    void* AttachSharedMemory(uint32 id, ProcessControlBlock* pcb);
    
    // Detach from a shared memory region
    bool DetachSharedMemory(uint32 id, ProcessControlBlock* pcb);
    
    // Mark a shared memory region for deletion
    bool DeleteSharedMemory(uint32 id);
    
    // Clean up regions marked for deletion that have no remaining attachments
    void CleanupDeletedRegions();
    
    // Get information about a shared memory region
    uint32 GetSharedMemorySize(uint32 id);
    uint32 GetSharedMemoryRefCount(uint32 id);
    uint32 GetSharedMemoryAttachCount(uint32 id);
    bool IsSharedMemoryMarkedForDeletion(uint32 id);
    
private:
    // Internal helper functions
    SharedMemoryRegion* FindRegionById(uint32 id);
    bool AddProcessMapping(SharedMemoryRegion* region, 
                           uint32 pid, 
                           void* process_vaddr, 
                           PageDirectory* page_dir);
    bool RemoveProcessMapping(SharedMemoryRegion* region, uint32 pid);
};

#endif