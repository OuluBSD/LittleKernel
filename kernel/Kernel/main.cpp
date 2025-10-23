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
    
    // Initialize timer
    global_timer = global->timer;
    LOG("Timer initialized");
    
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