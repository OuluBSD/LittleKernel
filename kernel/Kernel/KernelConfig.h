#ifndef _Kernel_KernelConfig_h_
#define _Kernel_KernelConfig_h_

#include "Defs.h"

// Kernel configuration structure
struct KernelConfig {
    // Memory settings
    uint32 kernel_heap_size;        // Size of kernel heap in bytes
    uint32 max_processes;           // Maximum number of processes
    uint32 max_threads_per_process; // Maximum number of threads per process
    
    // Timer settings
    uint32 timer_frequency;         // Timer interrupt frequency in Hz
    uint32 scheduler_quantum_ms;    // Time slice quantum in milliseconds
    
    // Process settings
    bool enable_preemptive_scheduling; // Whether to use preemptive scheduling
    bool enable_cooperative_scheduling; // Whether to allow cooperative scheduling
    
    // Memory management settings
    uint32 page_size;               // Page size in bytes
    uint32 max_virtual_memory_per_process; // Max VM per process in bytes
    
    // Debug settings
    bool enable_kernel_debugging;     // Enable kernel debugging features
    bool enable_verbose_logging;      // Enable verbose logging
    bool enable_memory_debugging;     // Enable memory debugging (bounds checking, etc.)
    
    // I/O settings
    uint32 console_buffer_size;     // Console buffer size in bytes
    bool enable_serial_logging;       // Enable logging to serial port
    bool enable_vga_logging;          // Enable logging to VGA display
    
    // File system settings
    uint32 max_open_files;          // Maximum number of open files per process
    uint32 max_mount_points;        // Maximum number of mounted file systems
    bool enable_vfs_layer;            // Enable virtual file system layer
    
    // Network settings (for future expansion)
    bool enable_networking;           // Enable networking features
    uint32 max_network_connections; // Maximum network connections
};

// Default kernel configuration
static inline void InitializeDefaultConfig(KernelConfig& config) {
    // Memory settings
    config.kernel_heap_size = 16 * 1024 * 1024;  // 16MB
    config.max_processes = 128;
    config.max_threads_per_process = 16;
    
    // Timer settings
    config.timer_frequency = 100;  // 100Hz (10ms intervals)
    config.scheduler_quantum_ms = 10; // 10ms time slices
    
    // Process settings
    config.enable_preemptive_scheduling = true;
    config.enable_cooperative_scheduling = true;
    
    // Memory management settings
    config.page_size = 4096;  // 4KB pages
    config.max_virtual_memory_per_process = 512 * 1024 * 1024;  // 512MB
    
    // Debug settings
    config.enable_kernel_debugging = false;
    config.enable_verbose_logging = false;
    config.enable_memory_debugging = false;
    
    // I/O settings
    config.console_buffer_size = 4096;
    config.enable_serial_logging = true;
    config.enable_vga_logging = true;
    
    // File system settings
    config.max_open_files = 32;
    config.max_mount_points = 8;
    config.enable_vfs_layer = true;
    
    // Network settings
    config.enable_networking = false;
    config.max_network_connections = 16;
}

// Global kernel configuration instance
extern KernelConfig* g_kernel_config;

// Function to load configuration from multiboot info or other source
void LoadKernelConfig(struct Multiboot* mboot_ptr);

// Function to validate the configuration values
bool ValidateKernelConfig();

#endif