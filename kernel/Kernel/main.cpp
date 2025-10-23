#include "Kernel.h"

// Kernel entry point
extern "C" int multiboot_main(struct Multiboot* mboot_ptr) {
    // Initialize the global structure with essential systems
    global = (Global*)malloc(sizeof(Global));
    global->Initialize();
    
    LOG("LittleKernel starting...");
    DLOG("Version: 2.0 (Complete Rewrite)");
    
    // Initialize serial port for logging
    InitializeSerial();
    LOG("Serial port initialized");
    
    // Load kernel configuration from multiboot info or use defaults
    LoadKernelConfig(mboot_ptr);
    
    // Validate the loaded configuration
    if (!ValidateKernelConfig()) {
        LOG("Kernel configuration validation failed - using emergency defaults");
        // If config validation fails, we can still try to continue with basic defaults
    } else {
        LOG("Kernel configuration loaded and validated");
    }
    
    // Initialize timer with configured frequency
    global_timer = global->timer;
    global->timer->Initialize(g_kernel_config->timer_frequency);
    LOG("Timer initialized with frequency: " << g_kernel_config->timer_frequency << " Hz");
    
    // Initialize paging before enabling interrupts (required for proper operation)
    global->memory_manager->InitializePaging();
    LOG("Paging initialized");
    
    // Set up timer interrupt handler BEFORE enabling interrupts
    global->descriptor_table->interrupt_manager.SetHandler(IRQ0, TimerIrqHandler);
    
    // Set up keyboard interrupt handler
    global->descriptor_table->interrupt_manager.SetHandler(IRQ1, KeyboardIrqHandler);
    
    // Enable interrupts AFTER setting up handlers
    global->descriptor_table->interrupt_manager.Enable();
    LOG("Interrupts enabled");
    
    // Initialize process management
    process_manager = new ProcessManager();
    LOG("Process manager initialized");
    
    // Initialize other subsystems as needed
    LOG("Kernel initialization complete");
    
    // Main kernel loop
    while (true) {
        // In a real kernel, this would schedule processes or handle system tasks
        // For now, just print a message to show the system is alive
        LOG("Kernel alive...");
        // Add a small delay to avoid overwhelming the output
        for (volatile int i = 0; i < 1000000; ++i) { 
            // Busy wait to slow down output
        }
    }
    
    return 0xDEADBEEF; // Should never reach here
}