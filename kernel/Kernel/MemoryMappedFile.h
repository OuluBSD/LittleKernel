#ifndef _Kernel_MemoryMappedFile_h_
#define _Kernel_MemoryMappedFile_h_

#include "Common.h"
#include "Paging.h"
#include "FileSystem.h"  // Forward declaration should be enough

// Structure to represent a memory-mapped file
struct MemoryMappedFile {
    uint32 id;                    // Unique identifier for this mapping
    void* virtual_address;        // Virtual address in the process
    uint32 file_offset;           // Offset in the file where mapping starts
    uint32 size;                  // Size of the mapped region
    uint32 file_size;             // Total size of the file
    uint32 flags;                 // Mapping flags (read-only, read-write, etc.)
    
    // Reference to the actual file
    void* file_handle;            // Handle to the file in the file system
    
    // Process-specific mapping information
    uint32 pid;                   // Process ID that owns this mapping
    PageDirectory* page_dir;      // Page directory of the process
    
    MemoryMappedFile* next;       // Next mapping in the process or global list
};

enum MemoryMapFlags {
    MAP_READ = 1,
    MAP_WRITE = 2,
    MAP_EXECUTE = 4,
    MAP_PRIVATE = 8,      // Changes are private to the process
    MAP_SHARED = 16,      // Changes are shared with other processes
    MAP_FIXED = 32        // Use exact address given
};

// Memory mapping manager class
class MemoryMappingManager {
private:
    MemoryMappedFile* mapping_list;
    uint32 next_mapping_id;       // Next mapping ID to assign
    
public:
    MemoryMappingManager();
    ~MemoryMappingManager();
    
    // Create a new memory mapping for a file
    MemoryMappedFile* CreateMapFile(void* file_handle, 
                                    uint32 offset, 
                                    uint32 size, 
                                    uint32 flags,
                                    ProcessControlBlock* pcb,
                                    void* desired_vaddr = nullptr);
    
    // Remove a memory mapping
    bool UnmapFile(MemoryMappedFile* mapping);
    bool UnmapFileById(uint32 id, uint32 pid);
    
    // Get a specific mapping
    MemoryMappedFile* GetMappingById(uint32 id);
    MemoryMappedFile* GetMappingByProcessAndAddr(uint32 pid, void* vaddr);
    
    // Synchronize mapped file with disk
    bool SyncMapping(MemoryMappedFile* mapping);
    
    // Get information about a mapping
    uint32 GetMappingSize(uint32 id);
    uint32 GetMappingFlags(uint32 id);
    
private:
    // Internal helper functions
    MemoryMappedFile* FindMappingById(uint32 id);
    MemoryMappedFile* FindMappingByProcessAndAddr(uint32 pid, void* vaddr);
    bool AddToProcessMappingList(MemoryMappedFile* mapping);
    bool RemoveFromProcessMappingList(MemoryMappedFile* mapping);
};

#endif