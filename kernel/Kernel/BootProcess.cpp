#include "BootProcess.h"
#include "Kernel.h"
#include "KernelConfig.h"

// Implement missing string functions for freestanding environment

// String comparison function
int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

// String length function
int strlen(const char* str) {
    int len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

// Find first occurrence of substring in string
char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack; // Empty needle
    
    const char* h = haystack;
    while (*h) {
        const char* h_it = h;
        const char* n_it = needle;
        
        while (*h_it && *n_it && (*h_it == *n_it)) {
            h_it++;
            n_it++;
        }
        
        if (!*n_it) return (char*)h; // Found complete match
        
        h++;
    }
    
    return nullptr; // Not found
}

// Copy string (with safe bounds)
char* strcpy_safe(char* dest, const char* src, uint32 dest_size) {
    if (!dest || !src || dest_size == 0) return dest;
    
    uint32 i;
    for (i = 0; i < dest_size - 1 && src[i] != '\\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\\0';
    return dest;
}

// Enhanced boot process function
int EnhancedBootProcess(struct Multiboot* mboot_ptr, uint32 magic) {
    // Check if multiboot magic number is correct
    if (magic != 0x2BADB002) {
        LOG("Error: Invalid multiboot magic number: 0x" << magic);
        return -1;
    }

    LOG("Enhanced boot process starting with multiboot magic: 0x" << magic);

    // Parse boot information from multiboot structure
    BootInfo* boot_info = ParseBootInfo(mboot_ptr, magic);
    if (!boot_info) {
        LOG("Error: Failed to parse boot information");
        return -1;
    }

    // Validate the parsed boot parameters
    if (!ValidateBootParameters(boot_info)) {
        LOG("Error: Boot parameters validation failed");
        return -1;
    }

    LOG("Boot parameters validated successfully");

    // Initialize the global structure with essential systems (early initialization)
    // Note: We can't use malloc yet as the heap may not be initialized
    // So we'll need to use static allocation or a different approach
    if (!global) {
        LOG("Initializing global structure in EnhancedBootProcess");
        // For now, this will be done in main.cpp before calling EnhancedBootProcess
        // This function will focus on configuration loading and validation
    }

    // Load kernel configuration based on boot information and command line
    if (boot_info->cmdline) {
        LOG("Loading configuration from command line: " << boot_info->cmdline);
        if (!LoadConfigFromCommandLine(boot_info->cmdline)) {
            LOG("Warning: Failed to load configuration from command line, using defaults");
        }
    }

    // Initialize hardware based on boot information
    if (!InitializeHardwareFromBoot(boot_info)) {
        LOG("Warning: Hardware initialization from boot info had issues");
    } else {
        LOG("Hardware initialized from boot information");
    }

    // Log memory information from boot
    LOG("Memory information from multiboot:");
    LOG("  Lower memory: " << boot_info->memory_lower << " KB");
    LOG("  Upper memory: " << boot_info->memory_upper << " KB");
    LOG("  Total memory estimate: " << (boot_info->memory_lower + boot_info->memory_upper) << " KB");

    // Clean up boot info structure (it was allocated in ParseBootInfo)
    // We need to make sure not to call free before heap is initialized
    // For now, don't free the structure to avoid issues with early boot
    
    // Store the boot info pointer globally if needed later in the boot process
    // (in a real implementation, you might want to store it temporarily somewhere)
    
    return 0; // Success
}

// Function to extract and parse boot parameters
BootInfo* ParseBootInfo(struct Multiboot* mboot_ptr, uint32 magic) {
    if (!mboot_ptr) {
        LOG("Error: Null multiboot pointer");
        return nullptr;
    }

    // Allocate boot info structure
    BootInfo* boot_info = (BootInfo*)malloc(sizeof(BootInfo));
    if (!boot_info) {
        LOG("Error: Failed to allocate boot info structure");
        return nullptr;
    }

    // Initialize structure to zero
    memset(boot_info, 0, sizeof(BootInfo));

    // Store multiboot pointer
    boot_info->multiboot_ptr = mboot_ptr;

    // Extract memory information
    if (mboot_ptr->flags & 0x01) {
        boot_info->memory_lower = mboot_ptr->mem_lower;
        boot_info->memory_upper = mboot_ptr->mem_upper;
        LOG("Memory info available from multiboot");
    } else {
        LOG("Warning: Memory info not available from multiboot");
        boot_info->memory_lower = 0;
        boot_info->memory_upper = 0;
    }

    // Extract command line if available
    if (mboot_ptr->flags & 0x02) {
        boot_info->cmdline_addr = mboot_ptr->cmdline;
        boot_info->cmdline = (char*)mboot_ptr->cmdline;
        LOG("Command line available at: 0x" << boot_info->cmdline_addr);
    } else {
        boot_info->cmdline = nullptr;
        LOG("No command line provided");
    }

    // Extract boot device if available
    if (mboot_ptr->flags & 0x04) {
        boot_info->boot_device = mboot_ptr->boot_device;
        LOG("Boot device info available: 0x" << boot_info->boot_device);
    } else {
        boot_info->boot_device = 0;
        LOG("No boot device info available");
    }

    // Extract initrd info if available
    if (mboot_ptr->flags & 0x06) {
        boot_info->initrd_count = mboot_ptr->mods_count;
        boot_info->initrd_addr = (uint32*)mboot_ptr->mods_addr;
        LOG("Initrd info: " << boot_info->initrd_count << " modules at 0x" << boot_info->initrd_addr);
    } else {
        boot_info->initrd_count = 0;
        boot_info->initrd_addr = nullptr;
        LOG("No initrd modules provided");
    }

    // Extract VBE info if available
    if (mboot_ptr->flags & 0x07) {
        boot_info->vbe_mode = true;
        LOG("VBE information available");
        // Simplified - in a real implementation we would extract full VBE info
        for (int i = 0; i < 32; i++) {
            boot_info->vbe_mode_info[i] = 0;
        }
    } else {
        boot_info->vbe_mode = false;
        LOG("No VBE information available");
    }

    return boot_info;
}

// Function to load configuration from command line parameters
bool LoadConfigFromCommandLine(const char* cmdline) {
    if (!cmdline) {
        return false;
    }

    LOG("Parsing command line for configuration: " << cmdline);

    // This is a simplified command line parser
    // In a more sophisticated implementation, we would parse key=value pairs
    if (strstr(cmdline, "debug")) {
        g_kernel_config->enable_kernel_debugging = true;
        g_kernel_config->enable_verbose_logging = true;
        LOG("Debug mode enabled from command line");
    }

    if (strstr(cmdline, "timer_freq=")) {
        // Find the value after "timer_freq="
        const char* freq_str = strstr(cmdline, "timer_freq=") + 11; // 11 is length of "timer_freq="
        int freq = 0;
        // Simple atoi implementation for extracting the number
        while (*freq_str >= '0' && *freq_str <= '9') {
            freq = freq * 10 + (*freq_str - '0');
            freq_str++;
        }
        
        if (freq > 0 && freq <= 1000) { // Reasonable upper limit
            g_kernel_config->timer_frequency = freq;
            LOG("Timer frequency set from command line: " << freq << " Hz");
        }
    }

    if (strstr(cmdline, "heap_size=")) {
        // Find the value after "heap_size="
        const char* size_str = strstr(cmdline, "heap_size=") + 10; // 10 is length of "heap_size="
        int size = 0;
        // Simple atoi implementation for extracting the number
        while (*size_str >= '0' && *size_str <= '9') {
            size = size * 10 + (*size_str - '0');
            size_str++;
        }
        
        if (size > 0) {
            g_kernel_config->kernel_heap_size = size * 1024 * 1024; // Convert MB to bytes
            LOG("Heap size set from command line: " << size << " MB");
        }
    }

    if (strstr(cmdline, "max_processes=")) {
        const char* proc_str = strstr(cmdline, "max_processes=") + 14; // 14 is length of "max_processes="
        int procs = 0;
        while (*proc_str >= '0' && *proc_str <= '9') {
            procs = procs * 10 + (*proc_str - '0');
            proc_str++;
        }
        
        if (procs > 0) {
            g_kernel_config->max_processes = procs;
            LOG("Max processes set from command line: " << procs);
        }
    }

    // Add more command line options as needed

    return true;
}

// Function to validate boot parameters
bool ValidateBootParameters(BootInfo* boot_info) {
    if (!boot_info) {
        LOG("Error: Boot info is null");
        return false;
    }

    // Validate memory information
    if (boot_info->memory_upper == 0 && boot_info->memory_lower == 0) {
        LOG("Warning: No memory information available");
        // Don't fail, but log warning - we might still proceed with estimates
    }

    // Validate command line if available
    if (boot_info->cmdline && strlen(boot_info->cmdline) > 1024) {
        LOG("Warning: Command line appears to be very long: " << strlen(boot_info->cmdline) << " chars");
        // Don't fail, but log warning
    }

    // Additional validation here as needed

    return true;
}

// Function to detect and initialize hardware based on boot info
bool InitializeHardwareFromBoot(BootInfo* boot_info) {
    if (!boot_info) {
        LOG("Error: Boot info is null for hardware initialization");
        return false;
    }

    LOG("Initializing hardware based on boot information");

    // Initialize hardware based on detected configuration
    // This is a placeholder for more sophisticated hardware detection

    // Example: Adjust configuration based on available memory
    uint32 total_memory = (boot_info->memory_lower + boot_info->memory_upper) * 1024; // Convert from KB to bytes
    if (total_memory < 32 * 1024 * 1024) { // Less than 32MB
        LOG("Low memory system detected, adjusting configuration");
        // Reduce some default values for low-memory systems
        if (g_kernel_config->kernel_heap_size > 8 * 1024 * 1024) { // More than 8MB
            g_kernel_config->kernel_heap_size = 8 * 1024 * 1024; // Set to 8MB
            LOG("Reduced heap size to 8MB for low memory system");
        }
    } else if (total_memory > 1024 * 1024 * 1024) { // More than 1GB
        LOG("High memory system detected, adjusting configuration");
        // Increase some default values for high-memory systems
        if (g_kernel_config->max_processes < 512) {
            g_kernel_config->max_processes = 512; // Increase max processes
            LOG("Increased max processes to 512 for high memory system");
        }
    }

    // Additional hardware initialization based on boot info would go here

    return true;
}