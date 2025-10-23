#ifndef _Kernel_ArchAbstraction_h_
#define _Kernel_ArchAbstraction_h_

#include "Defs.h"

/*
 * Architecture Abstraction Layer
 * Provides architecture-specific implementations while maintaining a common interface
 */

// Architecture-specific initialization
#if defined(TARGET_AMIGA_500PLUS)
    #include "arch/m68k/arch_defines.h"
    #define ARCH_NAME "Amiga 500+"
    #define ARCH_BITS 32
    #define ARCH_ENDIAN LITTLE_ENDIAN
    
#elif defined(TARGET_PPC_G4) || defined(TARGET_PPC64_G5)
    #include "arch/ppc/arch_defines.h"
    #define ARCH_NAME "PowerPC"
    #if defined(TARGET_PPC64_G5)
        #define ARCH_BITS 64
    #else
        #define ARCH_BITS 32
    #endif
    #define ARCH_ENDIAN BIG_ENDIAN
    
#elif defined(TARGET_8088_PC_CLONE) || defined(TARGET_286_TOSHIBA_T3200) || defined(TARGET_X86)
    #include "arch/x86/arch_defines.h"
    #define ARCH_NAME "x86"
    #if defined(TARGET_AMD64)
        #define ARCH_BITS 64
    #else
        #define ARCH_BITS 32
    #endif
    #define ARCH_ENDIAN LITTLE_ENDIAN
    
#elif defined(TARGET_AMD64)
    #include "arch/x86/arch_defines.h"
    #define ARCH_NAME "AMD64"
    #define ARCH_BITS 64
    #define ARCH_ENDIAN LITTLE_ENDIAN

#else
    // Default to x86 for compatibility
    #include "arch/x86/arch_defines.h"
    #define ARCH_NAME "x86 (default)"
    #define ARCH_BITS 32
    #define ARCH_ENDIAN LITTLE_ENDIAN
#endif

// Common architecture abstractions
typedef struct {
    uint32_t eax, ebx, ecx, edx;  // General purpose registers
    uint32_t esi, edi, ebp;       // More general purpose
    uint32_t esp, eip;            // Stack and instruction pointers
    uint32_t eflags;              // Flags register
    #if ARCH_BITS == 64
    uint32_t r8, r9, r10, r11;    // Additional 64-bit registers
    uint32_t r12, r13, r14, r15;
    #endif
} CpuContext;

// Architecture-specific functions to be implemented in respective arch directories
void ArchInitialize(void);
void ArchHalt(void);
void ArchEnableInterrupts(void);
void ArchDisableInterrupts(void);
uint32_t ArchGetTickCount(void);
void* ArchAllocatePage(void);
void ArchFreePage(void* page);
void ArchSwitchToTask(CpuContext* context);
void ArchInvalidateTLB(void);

// Memory barrier operations
void ArchMemoryBarrier(void);
void ArchReadBarrier(void);
void ArchWriteBarrier(void);

// Architecture-specific timer setup
void ArchSetupTimer(uint32_t frequency);

// Platform-specific initialization
void PlatformInitialize(void);

#endif // _Kernel_ArchAbstraction_h_