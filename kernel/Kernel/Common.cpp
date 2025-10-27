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

int snprintf_s(char* buffer, uint32 buffer_size, const char* format, ...) {
    if (!buffer || buffer_size == 0 || !format) {
        return -1;
    }
    
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, buffer_size, format, args);
    va_end(args);
    
    // Make sure the string is null-terminated
    if (result >= 0 && (uint32)result >= buffer_size) {
        buffer[buffer_size - 1] = '\0';
    }
    
    return result;
}

int vsnprintf(char* buffer, uint32 buffer_size, const char* format, va_list args) {
    if (!buffer || buffer_size == 0 || !format) {
        return -1;
    }
    
    uint32 i = 0;
    const char* format_ptr = format;
    va_list temp_args;
    va_copy(temp_args, args);
    
    while (*format_ptr && i < buffer_size - 1) {
        if (*format_ptr == '%') {
            format_ptr++; // Move past '%'
            
            if (*format_ptr == 'd') { // Integer
                int value = va_arg(temp_args, int);
                
                // Convert integer to string (simple implementation)
                char temp[12]; // Enough for 32-bit integer
                int temp_idx = 0;
                bool negative = false;
                
                if (value < 0) {
                    negative = true;
                    value = -value;
                    if (i < buffer_size - 1) buffer[i++] = '-';
                }
                
                if (value == 0) {
                    temp[temp_idx++] = '0';
                } else {
                    while (value > 0 && temp_idx < 11) {
                        temp[temp_idx++] = '0' + (value % 10);
                        value /= 10;
                    }
                }
                
                // Reverse the digits
                for (int j = temp_idx - 1; j >= 0 && i < buffer_size - 1; j--) {
                    buffer[i++] = temp[j];
                }
            } else if (*format_ptr == 's') { // String
                char* str = va_arg(temp_args, char*);
                if (!str) str = "(null)";
                
                int j = 0;
                while (str[j] && i < buffer_size - 1) {
                    buffer[i++] = str[j++];
                }
            } else if (*format_ptr == 'x' || *format_ptr == 'X') { // Hexadecimal
                unsigned int value = va_arg(temp_args, unsigned int);
                
                // Convert to hex string
                char temp[9]; // Enough for 32-bit hex
                int temp_idx = 0;
                
                if (value == 0) {
                    temp[temp_idx++] = '0';
                } else {
                    while (value > 0 && temp_idx < 8) {
                        int digit = value % 16;
                        temp[temp_idx++] = digit < 10 ? '0' + digit : (*format_ptr == 'x' ? 'a' + digit - 10 : 'A' + digit - 10);
                        value /= 16;
                    }
                }
                
                // Reverse the digits
                for (int j = temp_idx - 1; j >= 0 && i < buffer_size - 1; j--) {
                    buffer[i++] = temp[j];
                }
            } else {
                // Unknown format specifier, just output as is
                if (i < buffer_size - 1) buffer[i++] = '%';
                if (i < buffer_size - 1) buffer[i++] = *format_ptr;
            }
        } else {
            if (i < buffer_size - 1) buffer[i++] = *format_ptr;
        }
        format_ptr++;
    }
    
    buffer[i] = '\0';
    va_end(temp_args);
    
    return i; // Return number of characters written (excluding null terminator)
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

// String compare with limit
int strncmp(const char* str1, const char* str2, uint32 n) {
    if (!str1 || !str2) return (str1 == str2) ? 0 : (str1 ? 1 : -1);
    
    for (uint32 i = 0; i < n; i++) {
        if (str1[i] != str2[i] || str1[i] == '\0') {
            return (unsigned char)str1[i] - (unsigned char)str2[i];
        }
    }
    return 0;
}

// Find first occurrence of character in string
char* strchr(const char* str, int c) {
    if (!str) return nullptr;
    
    // Special case for null terminator
    if (c == '\0') {
        // Find the end of the string
        while (*str) {
            str++;
        }
        return (char*)str;
    }
    
    // Look for the character in the string
    while (*str) {
        if (*str == (char)c) {
            return (char*)str;
        }
        str++;
    }
    
    return nullptr;
}

// String tokenizer
char* strtok(char* str, const char* delim) {
    static char* last_token = nullptr;
    
    // If str is provided, use it as the starting point, otherwise continue with last_token
    if (str) {
        last_token = str;
    } else if (!last_token) {
        return nullptr;
    }
    
    // Skip leading delimiters
    while (*last_token && strchr(delim, *last_token)) {
        last_token++;
    }
    
    // If we're at the end of the string, return nullptr
    if (*last_token == '\0') {
        return nullptr;
    }
    
    // Find the beginning of the token
    char* token_start = last_token;
    
    // Find the end of the token
    while (*last_token && !strchr(delim, *last_token)) {
        last_token++;
    }
    
    // If we found a delimiter, terminate the token and advance past the delimiter
    if (*last_token) {
        *last_token = '\0';
        last_token++;
    }
    
    return token_start;
}

// Find last occurrence of character in string
char* strrchr(const char* str, int c) {
    if (!str) return nullptr;
    
    const char* last_occurrence = nullptr;
    const char* current = str;
    
    // Check if we're looking for the null terminator
    if (c == '\0') {
        // Find the end of the string
        while (*current) {
            current++;
        }
        return (char*)current;
    }
    
    // Look for the character in the string
    while (*current) {
        if (*current == (char)c) {
            last_occurrence = current;
        }
        current++;
    }
    
    return (char*)last_occurrence;
}

// Simple snprintf implementation
int snprintf(char* buffer, uint32 buffer_size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, buffer_size, format, args);
    va_end(args);
    return result;
}

// Convert string to integer
int atoi(const char* str) {
    if (!str) return 0;
    
    int result = 0;
    int sign = 1;
    int i = 0;
    
    // Handle whitespace
    while (str[i] == ' ' || str[i] == '\t') i++;
    
    // Handle sign
    if (str[i] == '-' || str[i] == '+') {
        sign = (str[i] == '-') ? -1 : 1;
        i++;
    }
    
    // Convert digits
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    
    return result * sign;
}

// Copy string
char* strcpy(char* dest, const char* src) {
    if (!dest || !src) return dest;
    
    char* ret = dest;
    while ((*dest++ = *src++))
        ;
    return ret;
}

// Append characters from src to dest, limited to n characters
char* strncat(char* dest, const char* src, uint32 n) {
    if (!dest || !src) return dest;
    
    // Find end of dest string
    char* dest_end = dest;
    while (*dest_end) {
        dest_end++;
    }
    
    // Copy at most n characters from src
    uint32 i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest_end[i] = src[i];
    }
    
    // Null terminate
    dest_end[i] = '\0';
    
    return dest;
}

// Find first occurrence of substring in string
char* strstr(const char* haystack, const char* needle) {
    if (!haystack || !needle) return nullptr;
    
    // Empty needle matches at beginning of haystack
    if (!*needle) return (char*)haystack;
    
    const char* h = haystack;
    while (*h) {
        const char* h_it = h;
        const char* n_it = needle;
        
        // Compare characters until end of needle or mismatch
        while (*h_it && *n_it && (*h_it == *n_it)) {
            h_it++;
            n_it++;
        }
        
        // If we've reached end of needle, we found a match
        if (!*n_it) return (char*)h;
        
        h++;
    }
    
    return nullptr;  // Not found
}

// String to integer conversion
int str_to_int(const char* str) {
    if (!str) return 0;
    
    int result = 0;
    int sign = 1;
    int i = 0;
    
    // Handle whitespace
    while (str[i] == ' ' || str[i] == '\t') i++;
    
    // Handle sign
    if (str[i] == '-' || str[i] == '+') {
        sign = (str[i] == '-') ? -1 : 1;
        i++;
    }
    
    // Convert digits
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    
    return result * sign;
}