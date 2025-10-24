#ifndef _Kernel_Common_h_
#define _Kernel_Common_h_

// Don't include other headers in this file - only the package header should include other headers

// Common structures and functions following Ultimate++ conventions

// Include stdarg for variable argument functions
#include <stdarg.h>

struct Registers {
    uint32 ds;                  // Data segment selector
    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha
    uint32 int_no, err_code;    // Interrupt number and error code
    uint32 eip, cs, eflags, useresp, ss; // Pushed by the processor
};

// Function prototypes for common utility functions
#ifdef __cplusplus
extern "C" {
#endif

void* memcpy(void* dest, const void* src, uint32 len);
void* memset(void* dest, char val, uint32 len);
uint16* memsetw(uint16* dest, uint16 val, uint32 count);
int strlen(const char* str);
char* strcpy_safe(char* dest, const char* src, uint32 dest_size);
char* strcat_s(char* dest, const char* src, uint32 dest_size);
int strcmp(const char* str1, const char* str2);
char* strncpy_s(char* dest, const char* src, uint32 count, uint32 dest_size);
int snprintf_s(char* buffer, uint32 buffer_size, const char* format, ...);
int vsnprintf(char* buffer, uint32 buffer_size, const char* format, va_list args);

#ifdef __cplusplus
}

// C++ operator new and delete declarations for freestanding kernel
void* operator new(uint32 size);
void* operator new[](uint32 size);
void operator delete(void* ptr);
void operator delete[](void* ptr);
void operator delete(void* ptr, uint32 size);
void operator delete[](void* ptr, uint32 size);

#endif

// Spinlock implementation
struct Spinlock {
    volatile uint32 lock;
    
    void Initialize() { lock = 0; }
    void Acquire();
    void Release();
    bool TryAcquire();
};

// Inline assembly functions
static inline uint8 inportb(uint16 port) {
    uint8 ret;
    __asm__ volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

static inline void outportb(uint16 port, uint8 data) {
    __asm__ volatile("outb %1, %0" : : "dN" (port), "a" (data));
}

static inline uint16 inportw(uint16 port) {
    uint16 ret;
    __asm__ volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

static inline void outportw(uint16 port, uint16 data) {
    __asm__ volatile("outw %1, %0" : : "dN" (port), "a" (data));
}

static inline uint32 inportl(uint16 port) {
    uint32 ret;
    __asm__ volatile("inl %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

static inline void outportl(uint16 port, uint32 data) {
    __asm__ volatile("outl %1, %0" : : "dN" (port), "a" (data));
}

// Helper functions for address translation
uint32 VirtualToPhysical(void* virtual_addr);
void* PhysicalToVirtual(void* physical_addr);

// Helper functions for debugging
static inline void* get_frame_pointer() {
    void* ebp;
    __asm__ volatile("mov %%ebp, %0" : "=r" (ebp));
    return ebp;
}

static inline uint32 get_instruction_pointer() {
    uint32 eip;
    __asm__ volatile("call 1f\n1: pop %0" : "=r" (eip));
    return eip;
}

static inline bool is_kernel_address(void* addr) {
    uint32 address = (uint32)addr;
    return address >= 0x00100000 && address < 0xC0000000;
}

#endif