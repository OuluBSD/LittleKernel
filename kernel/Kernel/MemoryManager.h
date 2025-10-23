#ifndef _Kernel_MemoryManager_h_
#define _Kernel_MemoryManager_h_

// Don't include other headers in this file - only the package header should include other headers

// Forward declarations
struct PageDirectory;

// Include necessary headers
#include "Common.h"
#include "Paging.h"

// Memory management structures
struct MemoryBlock {
    uint32 address;
    uint32 size;
    bool is_free;
    MemoryBlock* next;
};

class MemoryManager {
public:
    static const uint32 HEAP_START = 0xD0000000;
    static const uint32 HEAP_SIZE = 0x1000000; // 16MB

private:
    MemoryBlock* heap_start;
    MemoryBlock* first_block;
    uint32 total_memory;
    uint32 used_memory;
    uint32 max_memory;
    Spinlock lock;
    
public:
    MemoryManager();
    void Initialize();
    void* Allocate(uint32 size);
    void* AllocateAligned(uint32 size, uint32 alignment);
    void Free(void* ptr);
    uint32 GetUsedMemory() const { return used_memory; }
    uint32 GetTotalMemory() const { return total_memory; }
    uint32 GetFreeMemory() const { return total_memory - used_memory; }
    
    // Page management
    bool InitializePaging();
    void* AllocatePage();
    void FreePage(void* page);
    PageDirectory* CreatePageDirectory();
    void SwitchPageDirectory(PageDirectory* new_dir);
    
private:
    MemoryBlock* FindFreeBlock(uint32 size);
    void SplitBlock(MemoryBlock* block, uint32 size);
    void MergeFreeBlocks();
};

// Global memory allocation functions
extern "C" void* malloc(uint32 size);
extern "C" void free(void* ptr);
extern "C" void* realloc(void* ptr, uint32 size);
extern "C" void* calloc(uint32 num, uint32 size);

#endif