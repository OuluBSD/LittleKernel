#ifndef _Kernel_Paging_h_
#define _Kernel_Paging_h_

#include "Common.h"

// Page size constants - using pre-defined macro
const uint32 PAGE_MASK = ~(KERNEL_PAGE_SIZE - 1);  // Mask to get page-aligned address

// Page table entry flags
struct PageTableEntry {
    uint32 present : 1;          // Page is present in memory
    uint32 writable : 1;         // Page is writable
    uint32 user : 1;             // Page is user-accessible (else supervisor only)
    uint32 writethrough : 1;     // Write-through caching
    uint32 cache_disabled : 1;   // Disable caching
    uint32 accessed : 1;         // Page has been accessed
    uint32 dirty : 1;            // Page has been written to
    uint32 pat : 1;              // Page attribute table
    uint32 global : 1;           // Global page (ignored in TLB flushes)
    uint32 unused : 3;           // Free for OS use
    uint32 frame_address : 20;   // Frame address (shifted right by 12 bits)
};

// Page directory entry
struct PageDirectoryEntry {
    uint32 present : 1;          // Page table is present in memory
    uint32 writable : 1;         // Page table is writable
    uint32 user : 1;             // Page table is user-accessible
    uint32 writethrough : 1;     // Write-through caching
    uint32 cache_disabled : 1;   // Disable caching
    uint32 accessed : 1;         // Page table has been accessed
    uint32 reserved : 1;         // Reserved (zero)
    uint32 size : 1;             // Page size (0=4KB, 1=4MB)
    uint32 global : 1;           // Global page
    uint32 unused : 3;           // Free for OS use
    uint32 table_address : 20;   // Page table address (shifted right by 12 bits)
};

// Page directory structure
struct PageDirectory {
    PageDirectoryEntry entries[1024];
    uint32 physical_address;  // Physical address of the page directory
};

// Page table structure
struct PageTable {
    PageTableEntry entries[1024];
    uint32 physical_address;  // Physical address of the page table
};

// Page mapping flags
enum PageFlags {
    PAGE_PRESENT = 1,
    PAGE_WRITABLE = 2,
    PAGE_USER = 4,
    PAGE_CACHED = 8,      // If not set, page is not cached
    PAGE_ACCESSED = 16,   // Page has been accessed
    PAGE_DIRTY = 32       // Page has been written to
};

// Virtual memory management functions
class PagingManager {
private:
    PageDirectory* kernel_directory;
    PageDirectory* current_directory;
    
public:
    PagingManager();
    ~PagingManager();
    
    // Initialize the paging system
    bool Initialize();
    
    // Allocate and initialize a new page directory for a process
    PageDirectory* CreatePageDirectory();
    
    // Switch to a different page directory (context switch)
    void SwitchPageDirectory(PageDirectory* new_dir);
    
    // Map a virtual address to a physical frame
    bool MapPage(uint32 virtual_addr, uint32 physical_addr, uint32 flags, PageDirectory* dir = nullptr);
    
    // Unmap a virtual page
    bool UnmapPage(uint32 virtual_addr, PageDirectory* dir = nullptr);
    
    // Get physical address for a virtual address
    uint32 GetPhysicalAddress(uint32 virtual_addr, PageDirectory* dir = nullptr);
    
    // Check if a virtual page is mapped
    bool IsPageMapped(uint32 virtual_addr, PageDirectory* dir = nullptr);
    
    // Copy a page directory (for fork operations)
    PageDirectory* CopyPageDirectory(PageDirectory* original);
    
    // Enable paging (call this after initialization)
    void EnablePaging();
    
    // Get the current page directory
    PageDirectory* GetCurrentDirectory();
    
private:
    // Get page table for a virtual address, creating if necessary
    PageTable* GetPageTable(uint32 virtual_addr, bool create, PageDirectory* dir = nullptr);
};

#endif