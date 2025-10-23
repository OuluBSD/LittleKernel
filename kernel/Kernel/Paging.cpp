#include "Kernel.h"
#include "Paging.h"
#include "Logging.h"
#include "Common.h"

PagingManager::PagingManager() {
    kernel_directory = nullptr;
    current_directory = nullptr;
}

PagingManager::~PagingManager() {
    // Cleanup would happen here if needed
}

bool PagingManager::Initialize() {
    LOG("Initializing Paging Manager...");
    
    // Create the kernel page directory
    kernel_directory = (PageDirectory*)malloc(sizeof(PageDirectory));
    if (!kernel_directory) {
        LOG("Failed to allocate kernel page directory");
        return false;
    }
    
    // Zero out the page directory
    memset(kernel_directory, 0, sizeof(PageDirectory));
    
    // Identity map the first 1MB of memory (for kernel code/data)
    for (uint32 addr = 0; addr < 0x100000; addr += PAGE_SIZE) {
        MapPage(addr, addr, PAGE_PRESENT | PAGE_WRITABLE, kernel_directory);
    }
    
    // Map kernel heap area
    for (uint32 addr = MemoryManager::HEAP_START; 
         addr < MemoryManager::HEAP_START + MemoryManager::HEAP_SIZE; 
         addr += PAGE_SIZE) {
        MapPage(addr, addr, PAGE_PRESENT | PAGE_WRITABLE, kernel_directory);
    }
    
    // Set current directory to kernel directory
    current_directory = kernel_directory;
    
    LOG("Paging Manager initialized successfully");
    return true;
}

PageDirectory* PagingManager::CreatePageDirectory() {
    // Allocate a new page directory
    PageDirectory* new_dir = (PageDirectory*)malloc(sizeof(PageDirectory));
    if (!new_dir) {
        LOG("Failed to allocate new page directory");
        return nullptr;
    }
    
    // Zero out the page directory
    memset(new_dir, 0, sizeof(PageDirectory));
    
    // Copy kernel mappings from the kernel directory to preserve kernel access
    for (int i = 0; i < 768; i++) {  // First 768 entries are for user space (0-3GB)
        new_dir->entries[i] = kernel_directory->entries[i];
    }
    
    // Calculate the physical address of the new directory
    new_dir->physical_address = (uint32)VirtualToPhysical((void*)new_dir);
    
    DLOG("Created new page directory at virtual: 0x" << (uint32)new_dir 
          << ", physical: 0x" << new_dir->physical_address);
    
    return new_dir;
}

void PagingManager::SwitchPageDirectory(PageDirectory* new_dir) {
    if (!new_dir) {
        LOG("Attempted to switch to null page directory");
        return;
    }
    
    current_directory = new_dir;
    
    // Load the page directory address into CR3 register
    asm volatile("mov %0, %%cr3" : : "r" (new_dir->physical_address));
    
    DLOG("Switched page directory to physical address: 0x" << new_dir->physical_address);
}

bool PagingManager::MapPage(uint32 virtual_addr, uint32 physical_addr, uint32 flags, PageDirectory* dir) {
    if (!dir) {
        dir = current_directory;
    }
    
    // Align addresses to page boundaries
    virtual_addr &= PAGE_MASK;
    physical_addr &= PAGE_MASK;
    
    // Get the page directory index and page table index
    uint32 dir_idx = virtual_addr >> 22;  // Top 10 bits
    uint32 table_idx = (virtual_addr >> 12) & 0x3FF;  // Middle 10 bits
    
    // Get or create the page table
    PageTable* table = GetPageTable(virtual_addr, true, dir);
    if (!table) {
        LOG("Failed to get/create page table for virtual address 0x" << virtual_addr);
        return false;
    }
    
    // Set up the page table entry
    PageTableEntry* entry = &table->entries[table_idx];
    entry->present = flags & PAGE_PRESENT ? 1 : 0;
    entry->writable = flags & PAGE_WRITABLE ? 1 : 0;
    entry->user = flags & PAGE_USER ? 1 : 0;
    entry->writethrough = 0;  // Not using write-through
    entry->cache_disabled = (flags & PAGE_CACHED) ? 0 : 1;
    entry->accessed = 0;  // Will be set by CPU when accessed
    entry->dirty = 0;     // Will be set by CPU when written
    entry->pat = 0;       // Page attribute table
    entry->global = 0;    // Not a global page
    entry->unused = 0;    // Unused bits
    entry->frame_address = physical_addr >> 12;  // Store page frame address (shifted)
    
    // Invalidate the TLB entry for this virtual address
    asm volatile("invlpg (%0)" : : "r" (virtual_addr) : "memory");
    
    DLOG("Mapped virtual 0x" << virtual_addr << " to physical 0x" << physical_addr 
          << " in directory at 0x" << (uint32)dir);
    
    return true;
}

bool PagingManager::UnmapPage(uint32 virtual_addr, PageDirectory* dir) {
    if (!dir) {
        dir = current_directory;
    }
    
    // Align address to page boundary
    virtual_addr &= PAGE_MASK;
    
    // Get the page directory index and page table index
    uint32 dir_idx = virtual_addr >> 22;  // Top 10 bits
    uint32 table_idx = (virtual_addr >> 12) & 0x3FF;  // Middle 10 bits
    
    // Get the page table (don't create if it doesn't exist)
    PageTable* table = GetPageTable(virtual_addr, false, dir);
    if (!table) {
        // If the page table doesn't exist, the page isn't mapped
        return true;  // Unmapping a non-existent page is successful
    }
    
    // Clear the page table entry
    PageTableEntry* entry = &table->entries[table_idx];
    entry->present = 0;
    
    // Invalidate the TLB entry for this virtual address
    asm volatile("invlpg (%0)" : : "r" (virtual_addr) : "memory");
    
    DLOG("Unmapped virtual address: 0x" << virtual_addr);
    
    return true;
}

uint32 PagingManager::GetPhysicalAddress(uint32 virtual_addr, PageDirectory* dir) {
    if (!dir) {
        dir = current_directory;
    }
    
    // Align address to page boundary
    virtual_addr &= PAGE_MASK;
    
    // Get the page directory index and page table index
    uint32 dir_idx = virtual_addr >> 22;  // Top 10 bits
    uint32 table_idx = (virtual_addr >> 12) & 0x3FF;  // Middle 10 bits
    
    // Get the page table (don't create if it doesn't exist)
    PageTable* table = GetPageTable(virtual_addr, false, dir);
    if (!table) {
        return 0;  // Page not mapped
    }
    
    // Get the page table entry
    PageTableEntry* entry = &table->entries[table_idx];
    if (!entry->present) {
        return 0;  // Page not present
    }
    
    // Calculate physical address
    uint32 frame_addr = entry->frame_address << 12;
    return frame_addr | (virtual_addr & 0xFFF);  // Add offset within page
}

bool PagingManager::IsPageMapped(uint32 virtual_addr, PageDirectory* dir) {
    if (!dir) {
        dir = current_directory;
    }
    
    // Align address to page boundary
    virtual_addr &= PAGE_MASK;
    
    // Get the page directory index and page table index
    uint32 dir_idx = virtual_addr >> 22;  // Top 10 bits
    uint32 table_idx = (virtual_addr >> 12) & 0x3FF;  // Middle 10 bits
    
    // Get the page table (don't create if it doesn't exist)
    PageTable* table = GetPageTable(virtual_addr, false, dir);
    if (!table) {
        return false;  // Page table doesn't exist, so page isn't mapped
    }
    
    // Check if the page table entry is present
    return table->entries[table_idx].present != 0;
}

PageDirectory* PagingManager::CopyPageDirectory(PageDirectory* original) {
    if (!original) {
        return nullptr;
    }
    
    // Create a new page directory
    PageDirectory* new_dir = CreatePageDirectory();
    if (!new_dir) {
        return nullptr;
    }
    
    // Copy non-kernel entries (first 768 entries are user space)
    for (int i = 0; i < 768; i++) {
        if (original->entries[i].present) {
            // Get the original page table
            uint32 table_addr = original->entries[i].table_address << 12;
            PageTable* orig_table = (PageTable*)PhysicalToVirtual((void*)table_addr);
            
            // Create a new page table for the copy
            PageTable* new_table = (PageTable*)malloc(sizeof(PageTable));
            if (!new_table) {
                LOG("Failed to allocate page table during copy");
                // TODO: Clean up previously allocated tables
                return nullptr;
            }
            
            // Copy the page table entries
            memcpy(new_table, orig_table, sizeof(PageTable));
            
            // Update the new directory's entry to point to our new table
            new_dir->entries[i] = original->entries[i];  // Copy the entry
            new_dir->entries[i].table_address = (uint32)VirtualToPhysical(new_table) >> 12;
            
            // Increment reference counts for the pages being copied
            for (int j = 0; j < 1024; j++) {
                if (orig_table->entries[j].present) {
                    // In a real implementation, increment reference count here
                }
            }
        }
    }
    
    DLOG("Copied page directory successfully");
    return new_dir;
}

void PagingManager::EnablePaging() {
    // Enable paging by setting the PG bit in CR0 register
    asm volatile("mov %%cr0, %%eax\n"
                 "or $0x80000000, %%eax\n"  // Set PG bit (bit 31)
                 "mov %%eax, %%cr0" : : : "eax");
    
    LOG("Paging enabled");
}

PageDirectory* PagingManager::GetCurrentDirectory() {
    return current_directory;
}

PageTable* PagingManager::GetPageTable(uint32 virtual_addr, bool create, PageDirectory* dir) {
    if (!dir) {
        dir = current_directory;
    }
    
    uint32 dir_idx = virtual_addr >> 22;  // Top 10 bits
    
    // Check if the page directory entry exists
    if (!dir->entries[dir_idx].present) {
        if (create) {
            // Need to create a new page table
            PageTable* new_table = (PageTable*)malloc(sizeof(PageTable));
            if (!new_table) {
                LOG("Failed to allocate page table");
                return nullptr;
            }
            
            // Zero out the new page table
            memset(new_table, 0, sizeof(PageTable));
            
            // Calculate the physical address of the new table
            uint32 physical_addr = (uint32)VirtualToPhysical((void*)new_table);
            
            // Set up the page directory entry
            dir->entries[dir_idx].present = 1;
            dir->entries[dir_idx].writable = 1;
            dir->entries[dir_idx].user = 1;  // User accessible
            dir->entries[dir_idx].writethrough = 0;
            dir->entries[dir_idx].cache_disabled = 0;
            dir->entries[dir_idx].accessed = 0;  // Will be set by CPU
            dir->entries[dir_idx].reserved = 0;  // Must be zero
            dir->entries[dir_idx].size = 0;      // 4KB pages
            dir->entries[dir_idx].global = 0;
            dir->entries[dir_idx].unused = 0;
            dir->entries[dir_idx].table_address = physical_addr >> 12;
            
            DLOG("Created new page table for directory index " << dir_idx 
                  << ", virtual addr: 0x" << virtual_addr);
        } else {
            // Page table doesn't exist and we're not supposed to create it
            return nullptr;
        }
    }
    
    // Get the physical address of the page table
    uint32 table_physical_addr = dir->entries[dir_idx].table_address << 12;
    
    // Convert to virtual address and return
    return (PageTable*)PhysicalToVirtual((void*)table_physical_addr);
}

// Helper function to convert virtual address to physical
uint32 VirtualToPhysical(void* virtual_addr) {
    // In a real implementation, this would use the paging system to translate
    // For now, we'll assume identity mapping for simplicity where virtual=physical
    return (uint32)virtual_addr;
}

// Helper function to convert physical address to virtual
void* PhysicalToVirtual(void* physical_addr) {
    // In a real implementation, this would use the paging system to translate
    // For now, we'll assume identity mapping for simplicity where virtual=physical
    return physical_addr;
}