#include "Kernel.h"

extern "C" {

void* memcpy(void* dest, const void* src, uint32 len) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (uint32 i = 0; i < len; i++) {
        d[i] = s[i];
    }
    return dest;
}

void* memset(void* dest, char val, uint32 len) {
    char* ptr = (char*)dest;
    for (uint32 i = 0; i < len; i++) {
        ptr[i] = val;
    }
    return dest;
}

uint16* memsetw(uint16* dest, uint16 val, uint32 count) {
    for (uint32 i = 0; i < count; i++) {
        dest[i] = val;
    }
    return dest;
}

int strlen(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

char* strcpy_safe(char* dest, const char* src, uint32 dest_size) {
    if (dest == nullptr || src == nullptr || dest_size == 0) {
        return dest;
    }
    
    uint32 i = 0;
    // Copy up to dest_size-1 characters to leave space for null terminator
    while (i < dest_size - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    // Always null-terminate
    dest[i] = '\0';
    return dest;
}

char* strcat_s(char* dest, const char* src, uint32 dest_size) {
    if (dest == nullptr || src == nullptr || dest_size == 0) {
        return dest;
    }
    
    // Find the end of the destination string
    uint32 dest_len = 0;
    while (dest[dest_len] != '\0' && dest_len < dest_size) {
        dest_len++;
    }
    
    // If destination buffer is already full, nothing to do
    if (dest_len >= dest_size) {
        return dest;
    }
    
    // Append the source string up to the available space
    uint32 i = 0;
    while (src[i] != '\0' && dest_len + i < dest_size - 1) {
        dest[dest_len + i] = src[i];
        i++;
    }
    
    // Null-terminate
    dest[dest_len + i] = '\0';
    return dest;
}

int strcmp(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];
}

char* strncpy_safe(char* dest, const char* src, uint32 count, uint32 dest_size) {
    if (dest == nullptr || src == nullptr || dest_size == 0) {
        return dest;
    }
    
    // Limit count to the available destination buffer size
    uint32 safe_count = count < dest_size ? count : dest_size - 1;
    
    uint32 i;
    for (i = 0; i < safe_count && src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    
    // Pad the rest with nulls
    for (; i < safe_count; ++i) {
        dest[i] = '\0';
    }
    
    return dest;
}

} // extern "C"

void Spinlock::Acquire() {
    while (__sync_val_compare_and_swap(&lock, 0, 1) == 1) {
        // Wait until the lock is available
        while (lock == 1) {
            // Add a pause instruction to be nice to hyperthreading CPUs
            __asm__ volatile("pause");
        }
    }
}

void Spinlock::Release() {
    __sync_lock_release(&lock);
}

bool Spinlock::TryAcquire() {
    return __sync_val_compare_and_swap(&lock, 0, 1) == 0;
}

// C++ operator new and delete implementations for freestanding kernel
void* operator new(uint32 size) {
    return malloc(size);
}

void* operator new[](uint32 size) {
    return malloc(size);
}

void operator delete(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

void operator delete[](void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

void operator delete(void* ptr, uint32 size) {
    if (ptr) {
        free(ptr);
    }
}

void operator delete[](void* ptr, uint32 size) {
    if (ptr) {
        free(ptr);
    }
}

// Dummy implementations to satisfy linker requirements for exception handling
// Even with -fno-exceptions, the compiler may still generate calls to these functions
extern "C" {
    // Exception handling functions (dummy implementations)
    void __cxa_pure_virtual() {
        // Pure virtual function called - should never happen in kernel
        while(1); // Infinite loop - halt the system
    }
    
    void __cxa_atexit() {
        // Dummy implementation - do nothing
    }
    
    void __cxa_finalize() {
        // Dummy implementation - do nothing
    }
    
    void __gxx_personality_v0() {
        // Dummy implementation - do nothing
    }
    
    void __cxa_throw() {
        // Exception thrown - should never happen in kernel
        while(1); // Infinite loop - halt the system
    }
    
    void __cxa_begin_catch() {
        // Dummy implementation - do nothing
    }
    
    void __cxa_end_catch() {
        // Dummy implementation - do nothing
    }
    
    void std_terminate() {
        // std::terminate called - halt the system
        while(1); // Infinite loop - halt the system
    }
    
    // Weak symbol for std::terminate - this will be overridden if needed
    __attribute__((weak)) void _ZSt9terminatev() {
        std_terminate();
    }
    
    // Unwind functions that are causing linking issues
    void _Unwind_Resume() {
        // Should never happen in kernel
        while(1); // Infinite loop - halt the system
    }
}

// Helper functions for address translation
// In a real implementation, this would use the paging system to translate
// For now, we'll assume identity mapping for simplicity where virtual=physical
uint32 VirtualToPhysical(void* virtual_addr) {
    if (global && global->paging_manager) {
        // In a real implementation, this would traverse the page tables to find the physical address
        // For now, return the virtual address value as physical
        return (uint32)virtual_addr;
    }
    return (uint32)virtual_addr;
}

void* PhysicalToVirtual(void* physical_addr) {
    if (global && global->paging_manager) {
        // In a real implementation, this would use the paging system to translate
        // For now, return the physical address value as virtual
        return physical_addr;
    }
    return physical_addr;
}