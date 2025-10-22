#ifndef _Kernel_Common_h_
#define _Kernel_Common_h_

// Don't include other headers in this file - only the package header should include other headers

// Common structures and functions following Ultimate++ conventions

struct Registers {
    uint32 ds;                  // Data segment selector
    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha
    uint32 int_no, err_code;    // Interrupt number and error code
    uint32 eip, cs, eflags, useresp, ss; // Pushed by the processor
};

// Function prototypes for common utility functions
void* memcpy(void* dest, const void* src, uint32 len);
void* memset(void* dest, char val, uint32 len);
uint16* memsetw(uint16* dest, uint16 val, uint32 count);
int strlen(const char* str);
char* strcpy(char* dest, const char* src);
char* strcat(char* dest, const char* src);
int strcmp(const char* str1, const char* str2);
char* strncpy(char* dest, const char* src, uint32 count);

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

// Spinlock implementation
struct Spinlock {
    volatile uint32 lock;
    
    void Initialize() { lock = 0; }
    void Acquire();
    void Release();
    bool TryAcquire();
};

#endif