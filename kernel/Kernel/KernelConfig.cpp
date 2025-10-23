#include "KernelConfig.h"
#include "Kernel.h"

// Global kernel configuration instance - using static allocation to avoid heap issues early in boot
static KernelConfig kernel_config_instance;
KernelConfig* g_kernel_config = &kernel_config_instance;

void LoadKernelConfig(struct Multiboot* mboot_ptr) {
    // Use static allocation to avoid potential heap initialization issues early in boot
    g_kernel_config = &kernel_config_instance;
    
    // Initialize with default configuration
    InitializeDefaultConfig(*g_kernel_config);
    
    // Extract configuration from multiboot info if available
    // For now, we'll just use the defaults, but this function can be extended
    // to parse command-line parameters from multiboot info
    if (mboot_ptr && mboot_ptr->flags & 0x02) { // Check if cmd line is available
        // Command line parsing would go here
        // This is a placeholder for potential command-line configuration
        LOG("Multiboot command line available at: " << mboot_ptr->cmdline);
    }
    
    // Log the configuration values for debugging
    DLOG("Kernel Configuration Loaded:");
    DLOG("  Kernel heap size: " << g_kernel_config->kernel_heap_size << " bytes");
    DLOG("  Max processes: " << g_kernel_config->max_processes);
    DLOG("  Max threads per process: " << g_kernel_config->max_threads_per_process);
    DLOG("  Timer frequency: " << g_kernel_config->timer_frequency << " Hz");
    DLOG("  Scheduler quantum: " << g_kernel_config->scheduler_quantum_ms << " ms");
    DLOG("  Page size: " << g_kernel_config->page_size << " bytes");
    DLOG("  Max VM per process: " << g_kernel_config->max_virtual_memory_per_process << " bytes");
    DLOG("  Enable preemptive scheduling: " << g_kernel_config->enable_preemptive_scheduling);
    DLOG("  Enable cooperative scheduling: " << g_kernel_config->enable_cooperative_scheduling);
    DLOG("  Enable kernel debugging: " << g_kernel_config->enable_kernel_debugging);
    DLOG("  Enable verbose logging: " << g_kernel_config->enable_verbose_logging);
    DLOG("  Console buffer size: " << g_kernel_config->console_buffer_size << " bytes");
    DLOG("  Enable serial logging: " << g_kernel_config->enable_serial_logging);
    DLOG("  Enable VGA logging: " << g_kernel_config->enable_vga_logging);
    DLOG("  Max open files: " << g_kernel_config->max_open_files);
    DLOG("  Max mount points: " << g_kernel_config->max_mount_points);
    DLOG("  Enable VFS layer: " << g_kernel_config->enable_vfs_layer);
    DLOG("  Enable networking: " << g_kernel_config->enable_networking);
    DLOG("  Max network connections: " << g_kernel_config->max_network_connections);
}

// Function to validate the configuration values
bool ValidateKernelConfig() {
    if (!g_kernel_config) {
        LOG("Error: Kernel configuration not loaded");
        return false;
    }
    
    // Validate critical values
    if (g_kernel_config->kernel_heap_size == 0) {
        LOG("Error: Kernel heap size cannot be zero");
        return false;
    }
    
    if (g_kernel_config->timer_frequency == 0) {
        LOG("Error: Timer frequency cannot be zero");
        return false;
    }
    
    if (g_kernel_config->page_size == 0 || (g_kernel_config->page_size & (g_kernel_config->page_size - 1)) != 0) {
        LOG("Error: Page size must be a non-zero power of 2");
        return false;
    }
    
    if (g_kernel_config->max_processes == 0) {
        LOG("Error: Max processes cannot be zero");
        return false;
    }
    
    return true;
}