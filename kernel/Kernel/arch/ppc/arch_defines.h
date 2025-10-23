// PowerPC architecture specific definitions
#ifndef _Arch_PPC_ArchDefines_h_
#define _Arch_PPC_ArchDefines_h_

// PowerPC-specific definitions
#define PPC_NUM_GPR 32  // General purpose registers
#define PPC_NUM_SPR 1024 // Special purpose registers

// Common PowerPC SPR (Special Purpose Register) numbers
#define SPR_XER     1
#define SPR_LR      8
#define SPR_CTR     9
#define SPR_DSISR   18
#define SPR_DAR     19
#define SPR_DEC     22
#define SPR_SDR1    25
#define SPR_SRR0    26
#define SPR_SRR1    27
#define SPR_CSRR0   58
#define SPR_CSRR1   59
#define SPR_DEAR    61
#define SPR_ESR     62
#define SPR_TBL     284
#define SPR_TBU     285

// PowerPC-specific instructions via inline assembly
static inline uint32_t mftb(void) {
    uint32_t val;
    asm volatile("mftb %0" : "=r"(val));
    return val;
}

static inline uint32_t mftb_l(void) {
    uint32_t val;
    asm volatile("mftbl %0" : "=r"(val));
    return val;
}

static inline uint32_t mftb_u(void) {
    uint32_t val;
    asm volatile("mftbu %0" : "=r"(val));
    return val;
}

static inline uint32_t mfspr(uint32_t spr) {
    uint32_t val;
    asm volatile("mfspr %0, %1" : "=r"(val) : "i"(spr));
    return val;
}

static inline void mtspr(uint32_t spr, uint32_t val) {
    asm volatile("mtspr %0, %1" : : "i"(spr), "r"(val));
}

static inline void sync(void) {
    asm volatile("sync" ::: "memory");
}

static inline void lwsync(void) {
    asm volatile("lwsync" ::: "memory");
}

static inline void isync(void) {
    asm volatile("isync");
}

static inline void enable_interrupts(void) {
    asm volatile("mtmsr %0" :: "r"(mfspr(27) | 0x8000)); // Set MSR[EE]
}

static inline void disable_interrupts(void) {
    asm volatile("mtmsr %0" :: "r"(mfspr(27) & ~0x8000)); // Clear MSR[EE]
}

// PowerPC memory management
static inline void ppc_tlb_invalidate(void) {
    asm volatile("tlbie %0, 1" :: "r"(0) : "memory");
    asm volatile("sync");
    asm volatile("isync");
}

// MSR (Machine State Register) manipulation
static inline uint32_t get_msr(void) {
    uint32_t msr;
    asm volatile("mfmsr %0" : "=r"(msr));
    return msr;
}

static inline void set_msr(uint32_t msr) {
    asm volatile("mtmsr %0" : : "r"(msr) : "memory");
}

#endif // _Arch_PPC_ArchDefines_h_