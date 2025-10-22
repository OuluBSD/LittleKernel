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
    
    // Enable interrupts
    global->descriptor_table->interrupt_manager.Enable();
    LOG("Interrupts enabled");
    
    // Initialize paging
    global->memory_manager->InitializePaging();
    LOG("Paging initialized");
    
    // Initialize other subsystems as needed
    LOG("Kernel initialization complete");
    
    // Set up timer interrupt handler
    global->descriptor_table->interrupt_manager.SetHandler(IRQ0, TimerIrqHandler);
    
    // Set up keyboard interrupt handler
    global->descriptor_table->interrupt_manager.SetHandler(IRQ1, KeyboardIrqHandler);
    
    // Main kernel loop
    while (true) {
        // In a real kernel, this would schedule processes or handle system tasks
        // For now, just print a dot every few seconds to show the system is alive
        global->timer->Sleep(5000); // Sleep for 5 seconds
        LOG("Kernel alive...");
    }
    
    return 0xDEADBEEF; // Should never reach here
}