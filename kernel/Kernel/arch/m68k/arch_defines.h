// m68k architecture specific definitions for Amiga
#ifndef _Arch_M68k_ArchDefines_h_
#define _Arch_M68k_ArchDefines_h_

// m68k-specific definitions for Amiga
#define M68K_NUM_REGS 8  // D0-D7
#define M68K_ADDR_REGS 8 // A0-A7

// Placeholder structures for m68k-specific functions
typedef struct {
    uint32_t d[8];  // Data registers D0-D7
    uint32_t a[8];  // Address registers A0-A7
    uint32_t pc;    // Program counter
    uint16_t sr;    // Status register
} M68kCpuContext;

// Amiga-specific hardware register addresses
#define CUSTOM_BASE 0xDFF000

// CIA (Complex Interface Adapter) registers
#define CIABASE      0xBFE001
#define CIAA_BASE    0xBFE001

// Custom chip registers
#define BLTDDAT      (CUSTOM_BASE + 0x00)
#define DMACON       (CUSTOM_BASE + 0x02)
#define INTENA       (CUSTOM_BASE + 0x0E)
#define INTREQ       (CUSTOM_BASE + 0x1E)
#define POTGO        (CUSTOM_BASE + 0x34)
#define SERDAT       (CUSTOM_BASE + 0x30)
#define SERPER       (CUSTOM_BASE + 0x32)

// Memory management for Amiga
#define CHIP_RAM_BASE   0x00000000
#define FAST_RAM_BASE   0x00200000  // On Amiga with expansion
#define EXPANSION_BASE  0x00C00000

// Amiga-specific functions
static inline void amiga_custom_write(uint16_t offset, uint16_t value) {
    *(volatile uint16_t*)(CUSTOM_BASE + offset) = value;
}

static inline uint16_t amiga_custom_read(uint16_t offset) {
    return *(volatile uint16_t*)(CUSTOM_BASE + offset);
}

// Interrupt handling on Amiga
static inline void amiga_enable_int(uint16_t int_mask) {
    amiga_custom_write(INTENA, int_mask);
}

static inline void amiga_disable_int(uint16_t int_mask) {
    amiga_custom_write(INTENA, 0x8000 | int_mask); // Set high bit to disable
}

static inline void amiga_clear_int(uint16_t int_mask) {
    amiga_custom_write(INTREQ, int_mask);
}

// Basic cache control for 68020+
static inline void m68k_flush_cache(void) {
    asm volatile("nop"); // Placeholder - actual implementation would depend on CPU
}

// Supervisor/user mode transition helpers
static inline void m68k_supervisor_mode(void) {
    // Implementation would involve switching to supervisor mode
    // This is usually handled by the processor automatically on exceptions
}

#endif // _Arch_M68k_ArchDefines_h_