#ifndef _Kernel_BootProcess_h_
#define _Kernel_BootProcess_h_

#include "Defs.h"
#include "Multiboot.h"

// Boot information structure to hold information from multiboot
struct BootInfo {
    struct Multiboot* multiboot_ptr;      // Pointer to multiboot information
    uint32 memory_lower;                // Lower memory from multiboot
    uint32 memory_upper;                // Upper memory from multiboot
    uint32 cmdline_addr;                // Command line address
    uint32 boot_device;                 // Boot device
    uint32 initrd_count;               // Number of initrd modules
    uint32* initrd_addr;               // Address of initrd modules
    char* cmdline;                       // Command line string
    bool vbe_mode;                       // Whether VBE information is available
    uint16_t vbe_mode_info[32];          // VBE mode information (simplified)
};

// Enhanced boot process function
int EnhancedBootProcess(struct Multiboot* mboot_ptr, uint32 magic);

// Function to extract and parse boot parameters
BootInfo* ParseBootInfo(struct Multiboot* mboot_ptr, uint32 magic);

// Function to load configuration from command line parameters
bool LoadConfigFromCommandLine(const char* cmdline);

// Function to validate boot parameters
bool ValidateBootParameters(BootInfo* boot_info);

// Function to detect and initialize hardware based on boot info
bool InitializeHardwareFromBoot(BootInfo* boot_info);

#endif