# Memory Management System Design

## Overview
This document outlines the design of the memory management system for both kernel and user space. The system will follow Ultimate++ and Windows naming conventions while maintaining compatibility with both legacy DOS applications and modern multitasking requirements.

## Naming Conventions
- Function names follow Windows/Ultimate++ style (e.g., AllocateMemory, FreeMemory)
- Variable names use lowercase with underscores (e.g., mem_size, virtual_address)
- Class names use lowercase with underscores (e.g., memory_manager, page_directory)
- Macros use UPPER_CASE (e.g., PAGE_SIZE, KERNEL_HEAP_START)
- No variable prefixes (like m_)

## Architecture Overview

### Core Components
```
+------------------------+
|   Memory Manager       |
|   (Virtual Memory)     |
+------------------------+
|   Page Manager         |
|   (Physical Memory)    |
+------------------------+
|   Heap Manager         |
|   (Dynamic Allocation) |
+------------------------+
|   Hardware Layer       |
|   (MMU, Page Tables)   |
+------------------------+
```

## 1. Physical Memory Management

### Page Manager
The Page Manager handles physical memory allocation and tracking:

```cpp
class PageManager {
private:
    uint32* frames;           // Bitset of allocated frames
    uint32 nframes;          // Number of physical frames
    spinlock lock;           // For thread-safe access
    
public:
    bool AllocFrame(PageDirectory* pd, uint32 virtual_address, bool is_kernel, bool is_writeable);
    void FreeFrame(uint32 frame_addr);
    uint32 FirstFreeFrame();
    uint32 RequestFrame(bool is_kernel, bool is_writeable);
    void UnreservePage(uint32 virtual_address);
    void CopyPage(uint32 src, uint32 dst);
    uint32 GetFrameCount();
    void InitializePhysicalMemory(uint32 mem_start, uint32 mem_end);
};
```

### Frame Management
- Uses a bitmap to track allocated/free frames
- Each frame is 4KB (PAGE_SIZE)
- Manages physical memory fragmentation
- Supports kernel vs user page allocation

## 2. Virtual Memory Management

### Page Directory and Page Table
```cpp
struct PageDirectory {
    PageTable* tables[1024];    // Array of page table pointers
    uint32 tables_physical[1024]; // Physical addresses of tables
    uint32 physical_addr;        // Physical address of directory
};

struct PageTable {
    PageFrame entries[1024];    // Page table entries
};
```

### Virtual Memory Manager
```cpp
class VirtualMemoryManager {
private:
    PageDirectory* kernel_directory;
    PageDirectory* current_directory;
    uint32 placement_address;    // For identity mapping
    PageManager* pageman;       // Physical page manager
    
public:
    PageDirectory* ClonePageDirectory(PageDirectory* src);
    void SwitchPageDirectory(PageDirectory* new_dir);
    void MapVirtualAddress(PageDirectory* dir, uint32 virtual_addr, 
                         uint32 physical_addr, bool is_kernel, bool is_writeable);
    bool UnmapVirtualAddress(PageDirectory* dir, uint32 virtual_addr);
    uint32 VirtualToPhysical(PageDirectory* dir, uint32 virtual_addr);
    PageDirectory* CreatePageDirectory();
    void DestroyPageDirectory(PageDirectory* dir);
    void InitializePaging();
    void AllocPage(PageDirectory* dir, uint32 virtual_addr, 
                  bool is_kernel, bool is_writeable);
    void FreePage(PageDirectory* dir, uint32 virtual_addr);
};
```

## 3. Kernel Memory Management

### Kernel Heap
The kernel heap manages dynamic allocation for kernel space:

```cpp
class KernelHeap {
private:
    ordered_array<header> index;  // Free block index
    uint32 start_address;         // Start of heap
    uint32 end_address;           // Current end of heap
    uint32 max_address;           // Max possible heap size
    bool supervisor;              // Access level for new pages
    bool readonly;                // Read-only flag for new pages
    spinlock lock;                // For thread-safe access
    
public:
    void* Alloc(uint32 size, bool page_align = false);
    void Free(void* p);
    int32 FindSmallestHole(uint32 size, bool page_align);
    void Expand(uint32 new_size);
    uint32 Contract(uint32 new_size);
    KernelHeap& Create(uint32 start, uint32 end, uint32 max, 
                       bool supervisor, bool readonly);
    ~KernelHeap();
};
```

### Kernel Memory Allocation Functions
```cpp
// Following Windows/Ultimate++ naming conventions
void* AllocateKernelMemory(uint32 size);
void* AllocateKernelMemoryAligned(uint32 size);
uint32 AllocateKernelPhysicalMemory(uint32 size, uint32* physical = nullptr);
uint32 AllocateKernelAlignedPhysical(uint32 size, uint32* physical = nullptr);
void FreeKernelMemory(void* addr);

// Memory pool for frequently allocated objects
class MemoryPool {
private:
    void** free_list;
    uint32 block_size;
    uint32 max_blocks;
    uint32 current_blocks;
    spinlock lock;
    
public:
    void* AllocBlock();
    void FreeBlock(void* block);
    MemoryPool Create(uint32 block_size, uint32 max_blocks);
};
```

## 4. User Space Memory Management

### Process Memory Space
```cpp
class ProcessMemorySpace {
private:
    PageDirectory* page_dir;
    uint32 heap_start;       // Start of process heap
    uint32 heap_end;         // Current end of heap
    uint32 heap_max;         // Maximum heap size
    uint32 stack_start;      // Stack start address
    uint32 stack_size;       // Stack size
    uint32 image_base;       // Base address of executable
    
public:
    void* AllocUserMemory(uint32 size);
    bool FreeUserMemory(void* addr, uint32 size);
    bool ResizeUserHeap(uint32 new_size);
    void* MapUserMemory(uint32 virtual_addr, uint32 size, 
                       uint32 flags); // flags for permissions
    bool UnmapUserMemory(uint32 virtual_addr, uint32 size);
    uint32 GetUserHeapSize();
    uint32 GetUserStackSize();
    void InitializeUserSpace();
    void DestroyUserSpace();
};
```

### User Memory Allocation Functions
```cpp
// Using Windows-style naming for user allocation
void* VirtualAllocProcessMemory(process_memory_space* space, uint32 address, 
                               uint32 size, uint32 allocation_type, uint32 protect);
bool VirtualFreeProcessMemory(process_memory_space* space, void* address, 
                             uint32 size, uint32 free_type);
uint32 VirtualQueryProcessMemory(process_memory_space* space, void* address, 
                                memory_basic_information* info);
```

## 5. Memory Mapping

### Shared Memory
```cpp
class SharedMemoryManager {
private:
    // Map of shared memory regions
    std::map<uint32, shared_memory_block> shared_blocks;
    uint32 next_shm_id;
    spinlock lock;
    
public:
    uint32 CreateSharedMemory(uint32 size, uint32 permissions);
    void* MapSharedMemoryToProcess(ProcessMemorySpace* space, 
                                 uint32 shm_id, uint32 virtual_addr);
    bool UnmapSharedMemoryFromProcess(ProcessMemorySpace* space, 
                                    void* addr);
    bool DestroySharedMemory(uint32 shm_id);
    uint32 GetSharedMemorySize(uint32 shm_id);
};
```

### Memory-Mapped Files
```cpp
class MemoryMappedFiles {
public:
    void* MapFileToMemory(fs_node* file, uint32 offset, uint32 size, 
                         uint32 protection);
    bool UnmapFileFromMemory(void* addr);
    bool SyncMappedFile(void* addr, bool invalidate = false);
};
```

## 6. DOS Memory Compatibility

### DOS Memory Management
For Windows 98 and DOS compatibility:

```cpp
class DosMemoryManager {
private:
    uint32 conventional_memory_start;  // 0x00000000 - 0x0009FFFF
    uint32 conventional_memory_end;    // First 640KB
    uint32 upper_memory_start;         // 0x000A0000 - 0x000FFFFF  
    uint32 extended_memory_start;      // Above 1MB
    uint32* umb_blocks;               // Upper Memory Blocks
    uint32* xms_handles;              // XMS memory handles
    uint32* ems_pages;                // EMS memory pages
    
public:
    uint16 AllocateDosMemory(uint16 paragraphs);  // 16-byte paragraphs
    bool FreeDosMemory(uint16 segment);
    uint32 AllocateXmsMemory(uint32 bytes);
    bool FreeXmsMemory(uint32 handle);
    uint16 AllocateEmsPages(uint16 pages);
    bool FreeEmsPages(uint16 handle);
    bool MapEmsPageToPhysical(uint8 page_num, uint16 segment);
    uint16 GetDosMemorySize();
    uint16 GetDosLargestBlock();
};
```

## 7. System Memory Calls (Following Windows Conventions)

The system calls will use Windows-style names but with the Linux system call numbers for ABI compatibility:

```cpp
// Using Windows naming conventions, not Linux
uint32 SyscallVirtualAlloc(void* address, uint32 size, uint32 allocation_type, 
                          uint32 protect);
uint32 SyscallVirtualFree(void* address, uint32 size, uint32 free_type);
uint32 SyscallGetProcessHeap();
uint32 SyscallHeapAlloc(uint32 heap, uint32 flags, uint32 size);
uint32 SyscallHeapFree(uint32 heap, uint32 flags, void* ptr);

// For Linux compatibility layer (internal naming)
uint32 SyscallMmap(uint32 addr, uint32 len, uint32 prot, 
                   uint32 flags, uint32 fd, uint32 offset);
uint32 SyscallMunmap(uint32 addr, uint32 len);
uint32 SyscallBrk(uint32 new_brk);
```

## 8. Memory Protection

### Memory Protection Flags
```cpp
#define MEM_COMMIT              0x1000
#define MEM_RESERVE             0x2000
#define MEM_DECOMMIT            0x4000
#define MEM_RELEASE             0x8000

#define PAGE_NOACCESS           0x01
#define PAGE_READONLY           0x02
#define PAGE_READWRITE          0x04
#define PAGE_EXECUTE            0x10
#define PAGE_EXECUTE_READ       0x20
#define PAGE_EXECUTE_READWRITE  0x40
```

## 9. Initialization and Management Functions

```cpp
class MemorySubsystem {
private:
    PageManager* pageman;
    VirtualMemoryManager* virmem;
    KernelHeap* kheap;
    DosMemoryManager* dosmem;
    
public:
    bool InitializeMemorySubsystem(uint32 kernel_end);
    void EnablePaging();
    PageDirectory* CreateKernelPageDirectory();
    KernelHeap* CreateKernelHeap(uint32 start, uint32 end, uint32 max);
    void* AllocateKernelStack();
    void FreeKernelStack(void* stack);
    uint32 GetAvailableMemory();
    uint32 GetUsedMemory();
    uint32 GetTotalMemory();
};
```

## 10. Memory Management Interface

### Unified Interface Functions
```cpp
// High-level memory management functions
void* AllocateMemory(uint32 size);
void* AllocateAlignedMemory(uint32 size, uint32 alignment);
void FreeMemory(void* ptr);
uint32 GetMemorySize(void* ptr);

// Memory statistics
uint32 GetTotalPhysicalMemory();
uint32 GetFreePhysicalMemory();
uint32 GetUsedPhysicalMemory();

// Page-level functions
void* GetFreePage();
void ReleasePage(void* page);
bool MapPage(void* virtual_addr, void* physical_addr, bool kernel, bool writable);
bool UnmapPage(void* virtual_addr);
```

## Implementation Strategy

### Phase 1: Core Infrastructure
1. Implement page_manager for physical memory
2. Set up basic paging and page tables
3. Create identity mapping for kernel space
4. Test basic page allocation/deallocation

### Phase 2: Kernel Heap
1. Implement kernel heap with ordered array
2. Add basic allocation and freeing functions
3. Test with kernel memory allocations
4. Add alignment support

### Phase 3: User Space Memory
1. Extend paging system to support multiple address spaces
2. Implement process memory space management
3. Add VirtualAlloc/VirtualFree equivalents
4. Test with basic user processes

### Phase 4: Special Memory Types
1. Implement shared memory manager
2. Add memory-mapped file support
3. Add DOS-compatible memory management
4. Integrate with file system

### Phase 5: Integration and Testing
1. Connect with process management system
2. Implement memory protection
3. Add system call interface
4. Comprehensive testing with applications