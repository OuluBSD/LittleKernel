#include "Kernel.h"

PagingManager::PagingManager() {
    kernel_directory = nullptr;
    current_directory = nullptr;
}

PagingManager::~PagingManager() {
    // Clean up any allocated page directories/tables
}

bool PagingManager::Initialize() {
    // Allocate and initialize the kernel's page directory
    kernel_directory = (PageDirectory*)malloc(sizeof(PageDirectory));
    if (!kernel_directory) {
        LOG("Failed to allocate kernel page directory");
        return false;
    }
    
    // Zero out the page directory
    memset(kernel_directory, 0, sizeof(PageDirectory));
    
    // Identity map the first 1MB of memory (for kernel code/data)
    for (uint32 addr = 0; addr < 0x100000; addr += KERNEL_PAGE_SIZE) {
        MapPage(addr, addr, PAGE_PRESENT | PAGE_WRITABLE, kernel_directory);
    }
    
    // Map kernel heap area
    for (uint32 addr = MemoryManager::HEAP_START; 
         addr < MemoryManager::HEAP_START + MemoryManager::HEAP_SIZE; 
         addr += KERNEL_PAGE_SIZE) {
        MapPage(addr, addr, PAGE_PRESENT | PAGE_WRITABLE, kernel_directory);
    }
    
    // Set current directory to kernel directory
    current_directory = kernel_directory;
    
    DLOG("Paging manager initialized");
    return true;
}

PageDirectory* PagingManager::CreatePageDirectory() {
    // Allocate a new page directory
    PageDirectory* new_dir = (PageDirectory*)malloc(sizeof(PageDirectory));
    if (!new_dir) {
        LOG("Failed to allocate page directory");
        return nullptr;
    }
    
    // Initialize it by copying the kernel's page directory entries
    memcpy(new_dir, kernel_directory, sizeof(PageDirectory));
    
    // Set up physical address
    new_dir->physical_address = VirtualToPhysical(new_dir);
    
    DLOG("Created new page directory at: 0x" << (uint32)new_dir);
    return new_dir;
}

void PagingManager::SwitchPageDirectory(PageDirectory* new_dir) {
    if (!new_dir) {
        LOG("Attempted to switch to null page directory");
        return;
    }
    
    current_directory = new_dir;
    
    // In a real implementation, we would update CR3 register here
    // asm volatile("mov %0, %%cr3" : : "r" (new_dir->physical_address));
    
    DLOG("Switched page directory to: 0x" << (uint32)new_dir);
}

bool PagingManager::MapPage(uint32 virtual_addr, uint32 physical_addr, uint32 flags, PageDirectory* dir) {
    if (!dir) {
        dir = current_directory;
    }
    
    if (!dir) {
        LOG("No page directory available for mapping");
        return false;
    }
    
    uint32 directory_idx = virtual_addr >> 22;  // Top 10 bits
    uint32 table_idx = (virtual_addr >> 12) & 0x3FF;  // Middle 10 bits
    
    PageDirectoryEntry* directory_entry = &dir->entries[directory_idx];
    
    PageTable* table;
    if (!(directory_entry->present)) {
        // Need to create a new page table
        table = (PageTable*)malloc(sizeof(PageTable));
        if (!table) {
            LOG("Failed to allocate page table");
            return false;
        }
        
        // Initialize the table
        memset(table, 0, sizeof(PageTable));
        
        // Set up the directory entry to point to this table
        directory_entry->present = 1;
        directory_entry->writable = (flags & PAGE_WRITABLE) ? 1 : 0;
        directory_entry->user = (flags & PAGE_USER) ? 1 : 0;
        directory_entry->table_address = (uint32)VirtualToPhysical(table) >> 12;
        
        table->physical_address = VirtualToPhysical(table);
    } else {
        // Get existing table
        table = (PageTable*)((directory_entry->table_address << 12) - KERNEL_VIRTUAL_BASE);
    }
    
    // Set up the page table entry
    PageTableEntry* table_entry = &table->entries[table_idx];
    table_entry->present = 1;
    table_entry->writable = (flags & PAGE_WRITABLE) ? 1 : 0;
    table_entry->user = (flags & PAGE_USER) ? 1 : 0;
    table_entry->frame_address = physical_addr >> 12;
    
    return true;
}

bool PagingManager::UnmapPage(uint32 virtual_addr, PageDirectory* dir) {
    if (!dir) {
        dir = current_directory;
    }
    
    if (!dir) {
        LOG("No page directory available for unmapping");
        return false;
    }
    
    uint32 directory_idx = virtual_addr >> 22;
    uint32 table_idx = (virtual_addr >> 12) & 0x3FF;
    
    PageDirectoryEntry* directory_entry = &dir->entries[directory_idx];
    if (!directory_entry->present) {
        // Page table doesn't exist, so page isn't mapped
        return true;
    }
    
    PageTable* table = (PageTable*)((directory_entry->table_address << 12) - KERNEL_VIRTUAL_BASE);
    if (!table) {
        return false;
    }
    
    PageTableEntry* table_entry = &table->entries[table_idx];
    table_entry->present = 0;
    
    return true;
}

uint32 PagingManager::GetPhysicalAddress(uint32 virtual_addr, PageDirectory* dir) {
    if (!dir) {
        dir = current_directory;
    }
    
    if (!dir) {
        return 0;
    }
    
    uint32 directory_idx = virtual_addr >> 22;
    uint32 table_idx = (virtual_addr >> 12) & 0x3FF;
    
    PageDirectoryEntry* directory_entry = &dir->entries[directory_idx];
    if (!directory_entry->present) {
        return 0;
    }
    
    PageTable* table = (PageTable*)((directory_entry->table_address << 12) - KERNEL_VIRTUAL_BASE);
    if (!table) {
        return 0;
    }
    
    PageTableEntry* table_entry = &table->entries[table_idx];
    if (!table_entry->present) {
        return 0;
    }
    
    return (table_entry->frame_address << 12) | (virtual_addr & 0xFFF);
}

bool PagingManager::IsPageMapped(uint32 virtual_addr, PageDirectory* dir) {
    if (!dir) {
        dir = current_directory;
    }
    
    if (!dir) {
        return false;
    }
    
    uint32 directory_idx = virtual_addr >> 22;
    uint32 table_idx = (virtual_addr >> 12) & 0x3FF;
    
    PageDirectoryEntry* directory_entry = &dir->entries[directory_idx];
    if (!directory_entry->present) {
        return false;
    }
    
    PageTable* table = (PageTable*)((directory_entry->table_address << 12) - KERNEL_VIRTUAL_BASE);
    if (!table) {
        return false;
    }
    
    PageTableEntry* table_entry = &table->entries[table_idx];
    return table_entry->present == 1;
}

PageDirectory* PagingManager::CopyPageDirectory(PageDirectory* original) {
    if (!original) {
        return nullptr;
    }
    
    PageDirectory* new_dir = (PageDirectory*)malloc(sizeof(PageDirectory));
    if (!new_dir) {
        LOG("Failed to allocate new page directory for copy");
        return nullptr;
    }
    
    // Copy the directory structure
    memcpy(new_dir, original, sizeof(PageDirectory));
    new_dir->physical_address = VirtualToPhysical(new_dir);
    
    // Copy each page table that's present
    for (int i = 0; i < 1024; i++) {
        if (original->entries[i].present) {
            PageTable* original_table = (PageTable*)((original->entries[i].table_address << 12) - KERNEL_VIRTUAL_BASE);
            
            // Allocate a new page table
            PageTable* new_table = (PageTable*)malloc(sizeof(PageTable));
            if (!new_table) {
                LOG("Failed to allocate page table during copy");
                free(new_dir);
                return nullptr;
            }
            
            // Copy the table contents
            memcpy(new_table, original_table, sizeof(PageTable));
            new_table->physical_address = VirtualToPhysical(new_table);
            
            // Update the directory entry to point to the new table
            new_dir->entries[i].table_address = (uint32)VirtualToPhysical(new_table) >> 12;
        }
    }
    
    return new_dir;
}

void PagingManager::EnablePaging() {
    // Enable paging by setting the PG bit in CR0
    // In a real implementation, this would be done with inline assembly:
    // asm volatile("mov %cr0, %eax; orl $0x80000000, %eax; mov %eax, %cr0");
    
    DLOG("Paging enabled");
}

PageDirectory* PagingManager::GetCurrentDirectory() {
    return current_directory;
}