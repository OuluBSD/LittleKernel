#include "Kernel.h"

MemoryManager::MemoryManager() {
    heap_start = nullptr;
    first_block = nullptr;
    total_memory = 0;
    used_memory = 0;
    max_memory = HEAP_SIZE;
    lock.Initialize();
    
    // Initialize page tracking
    for (int i = 0; i < MAX_TRACKED_PAGES; i++) {
        tracked_pages[i] = nullptr;
        page_in_use[i] = false;
    }
    total_tracked_pages = 0;
    page_lock.Initialize();
}

void MemoryManager::Initialize() {
    heap_start = (MemoryBlock*)DEFAULT_KERNEL_HEAP_START;
    heap_start->address = DEFAULT_KERNEL_HEAP_START + sizeof(MemoryBlock);
    heap_start->size = max_memory - sizeof(MemoryBlock);
    heap_start->is_free = true;
    heap_start->next = nullptr;
    
    first_block = heap_start;
    total_memory = max_memory;
    used_memory = sizeof(MemoryBlock);
    
    LOG("Memory manager initialized with " << max_memory << " bytes");
}

void* MemoryManager::Allocate(uint32 size) {
    if (size == 0) return nullptr;
    
    // Add space for potential block structure in case we need to split
    uint32 request_size = size + sizeof(MemoryBlock);
    
    lock.Acquire();
    MemoryBlock* block = FindFreeBlock(request_size);
    
    if (block == nullptr) {
        lock.Release();
        LOG("Failed to allocate " << size << " bytes");
        return nullptr;
    }
    
    if (block->size > request_size + sizeof(MemoryBlock)) {
        // Split the block if it's much larger than needed
        SplitBlock(block, request_size);
    }
    
    block->is_free = false;
    used_memory += block->size;
    
    lock.Release();
    
    return (void*)block->address;
}

void* MemoryManager::AllocateAligned(uint32 size, uint32 alignment) {
    if (alignment & (alignment - 1)) {
        // Not a power of 2
        return nullptr;
    }
    
    lock.Acquire();
    
    // Need additional space for alignment padding
    uint32 needed_size = size + alignment + sizeof(MemoryBlock);
    MemoryBlock* block = FindFreeBlock(needed_size);
    
    if (block == nullptr) {
        lock.Release();
        return nullptr;
    }
    
    uint32 aligned_addr = ALIGN_UP(block->address + sizeof(MemoryBlock), alignment);
    uint32 offset = aligned_addr - block->address;
    
    if (offset > sizeof(MemoryBlock)) {
        // Create a new block at the aligned address
        MemoryBlock* new_block = (MemoryBlock*)block->address;
        new_block->address = aligned_addr;
        new_block->size = block->size - offset;
        new_block->is_free = false;
        new_block->next = block->next;
        
        // Update the original block to reflect the front padding
        block->size = offset - sizeof(MemoryBlock);
        block->next = new_block;
        block->is_free = true;
        
        used_memory += new_block->size;
        
        lock.Release();
        return (void*)new_block->address;
    } else {
        // Original block is already suitably aligned
        if (block->size > size + sizeof(MemoryBlock)) {
            SplitBlock(block, size);
        }
        
        block->is_free = false;
        used_memory += block->size;
        
        lock.Release();
        return (void*)block->address;
    }
}

void MemoryManager::Free(void* ptr) {
    if (ptr == nullptr) return;
    
    lock.Acquire();
    
    // Find which block this pointer belongs to
    MemoryBlock* current = first_block;
    while (current != nullptr) {
        if (current->address == (uint32)ptr) {
            current->is_free = true;
            used_memory -= current->size;
            
            // Try to merge with adjacent free blocks
            MergeFreeBlocks();
            lock.Release();
            return;
        }
        current = current->next;
    }
    
    lock.Release();
    LOG("Attempted to free unallocated pointer: 0x" << (uint32)ptr);
}

MemoryBlock* MemoryManager::FindFreeBlock(uint32 size) {
    MemoryBlock* current = first_block;
    while (current != nullptr) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void MemoryManager::SplitBlock(MemoryBlock* block, uint32 size) {
    MemoryBlock* new_block = (MemoryBlock*)(block->address + size + sizeof(MemoryBlock));
    new_block->address = block->address + size + sizeof(MemoryBlock);
    new_block->size = block->size - size - sizeof(MemoryBlock);
    new_block->is_free = true;
    new_block->next = block->next;
    
    block->size = size;
    block->next = new_block;
}

void MemoryManager::MergeFreeBlocks() {
    MemoryBlock* current = first_block;
    while (current != nullptr && current->next != nullptr) {
        if (current->is_free && current->next->is_free) {
            // Merge current block with next block
            current->size += current->next->size + sizeof(MemoryBlock);
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

// Page management
bool MemoryManager::InitializePaging() {
    // Initialize the paging system
    if (global && global->paging_manager) {
        if (!global->paging_manager->Initialize()) {
            LOG("Failed to initialize paging manager");
            return false;
        }
        
        // Enable paging
        global->paging_manager->EnablePaging();
        DLOG("Paging enabled successfully");
        return true;
    } else {
        LOG("Global paging manager not available");
        return false;
    }
}

void* MemoryManager::AllocatePage() {
    // For now, allocate a page using the basic memory allocator
    // In a more complete implementation, we'd have a dedicated page management system
    void* page = malloc(KERNEL_PAGE_SIZE);
    if (page) {
        // Track this page if we're in debug mode or tracking is enabled
        page_lock.Acquire();
        if (total_tracked_pages < MAX_TRACKED_PAGES) {
            // Find an available slot
            for (uint32 i = 0; i < MAX_TRACKED_PAGES; i++) {
                if (!page_in_use[i]) {
                    tracked_pages[i] = page;
                    page_in_use[i] = true;
                    total_tracked_pages++;
                    break;
                }
            }
        }
        page_lock.Release();
    }
    return page;
}

void MemoryManager::FreePage(void* page) {
    if (!page) return;
    
    // Untrack this page if it was tracked
    page_lock.Acquire();
    for (uint32 i = 0; i < MAX_TRACKED_PAGES; i++) {
        if (tracked_pages[i] == page && page_in_use[i]) {
            page_in_use[i] = false;
            total_tracked_pages--;
            break;
        }
    }
    page_lock.Release();
    
    // Actually free the page
    free(page);
}

void MemoryManager::RunGarbageCollection() {
    // In a real implementation, this would identify and free unused pages
    // that are allocated but no longer referenced by any process
    
    DLOG("Running garbage collection...");
    
    // For now, we'll just report the number of currently tracked pages
    LOG("Tracked pages: " << total_tracked_pages << "/" << MAX_TRACKED_PAGES);
    
    // In a more complete implementation:
    // 1. Mark all pages as potentially unused
    // 2. Walk through all active data structures that reference pages
    // 3. Mark pages that are referenced as used
    // 4. Free all pages still marked as unused
    
    // For now, we'll check if we have any invalid tracked pages
    page_lock.Acquire();
    uint32 cleaned_pages = 0;
    
    // This is a simplified version - a real system would need to determine
    // which pages are no longer in use by walking through page tables,
    // shared memory regions, etc.
    for (uint32 i = 0; i < MAX_TRACKED_PAGES; i++) {
        if (page_in_use[i] && tracked_pages[i]) {
            // In a real system, we'd check if this page is still referenced
            // For now, we'll assume it's still in use
        }
    }
    
    page_lock.Release();
    
    DLOG("Garbage collection completed. Cleaned up " << cleaned_pages << " pages.");
}

uint32 MemoryManager::GetFreePageCount() {
    // This is a simplification - in a real system, we'd have a proper
    // free page tracking system
    // For now, we'll return a placeholder value
    return 0; // Placeholder - would need real implementation
}

uint32 MemoryManager::GetUsedPageCount() {
    // Return the number of tracked pages
    return total_tracked_pages;
}

void MemoryManager::DefragmentMemory() {
    // In a real implementation, this would defragment memory
    // to consolidate free space and reduce fragmentation
    
    LOG("Memory defragmentation not implemented in this version");
}

// void* MemoryManager::AllocatePage() {
//     // For now, allocate a 4KB page
//     return Allocate(KERNEL_PAGE_SIZE);
// }

// void MemoryManager::FreePage(void* page) {
//     Free(page);
// }

PageDirectory* MemoryManager::CreatePageDirectory() {
    // In a complete implementation, this would create a proper page directory
    return (PageDirectory*)AllocatePage();
}

void MemoryManager::SwitchPageDirectory(PageDirectory* new_dir) {
    // In a complete implementation, this would switch to a new page directory
    // using the mov cr3 instruction
}

// Global allocation functions
extern "C" void* malloc(uint32 size) {
    if (global && global->memory_manager) {
        return global->memory_manager->Allocate(size);
    }
    return nullptr;
}

extern "C" void free(void* ptr) {
    if (global && global->memory_manager) {
        global->memory_manager->Free(ptr);
    }
}

extern "C" void* realloc(void* ptr, uint32 size) {
    if (ptr == nullptr) {
        return malloc(size);
    }
    
    if (size == 0) {
        free(ptr);
        return nullptr;
    }
    
    void* new_ptr = malloc(size);
    if (new_ptr == nullptr) {
        return nullptr; // Allocation failed
    }
    
    // Copy the content from old to new
    // In a real implementation, we'd need to know the old size
    // For now, we'll just copy a reasonable amount
    memcpy(new_ptr, ptr, size < 1024 ? size : 1024);
    free(ptr);
    
    return new_ptr;
}

extern "C" void* calloc(uint32 num, uint32 size) {
    uint32 total_size = num * size;
    void* ptr = malloc(total_size);
    if (ptr != nullptr) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}