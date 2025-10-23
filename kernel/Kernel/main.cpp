#include "Kernel.h"

// Function to simulate a simple process
void TestProcessFunction1() {
    for(int i = 0; i < 100; i++) {
        LOG("Test Process 1 running iteration: " << i);
        // In a real implementation, this would yield to other processes
        if (process_manager) {
            process_manager->YieldCurrentProcess(); // Yield for cooperative scheduling
        }
        
        // Busy wait to slow down execution
        for (volatile int j = 0; j < 10000; ++j) { }
    }
    
    LOG("Test Process 1 finished");
    // In a real implementation, we would terminate the process
    while(true) { /* Process should be terminated by scheduler */ }
}

void TestProcessFunction2() {
    for(int i = 0; i < 100; i++) {
        LOG("Test Process 2 running iteration: " << i);
        // In a real implementation, this would yield to other processes
        if (process_manager) {
            process_manager->YieldCurrentProcess(); // Yield for cooperative scheduling
        }
        
        // Busy wait to slow down execution
        for (volatile int j = 0; j < 10000; ++j) { }
    }
    
    LOG("Test Process 2 finished");
    // In a real implementation, we would terminate the process
    while(true) { /* Process should be terminated by scheduler */ }
}

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
    
    // Initialize paging manager before enabling interrupts (required for proper operation)
    global->paging_manager->Initialize();
    LOG("Paging manager initialized");
    
    // Initialize memory manager's paging functionality
    global->memory_manager->InitializePaging();
    LOG("Paging enabled");
    
    // Set up timer interrupt handler BEFORE enabling interrupts
    global->descriptor_table->interrupt_manager.SetHandler(IRQ0, TimerIrqHandler);
    
    // Set up timer interrupt handler BEFORE enabling interrupts
    global->descriptor_table->interrupt_manager.SetHandler(IRQ0, TimerIrqHandler);
    
    // Set up keyboard interrupt handler
    global->descriptor_table->interrupt_manager.SetHandler(IRQ1, KeyboardIrqHandler);
    
    // Set up page fault handler (interrupt 14) - this must be done after IDT is loaded
    global->descriptor_table->interrupt_manager.SetHandler(14, PageFaultHandler);
    
    // Enable interrupts AFTER setting up handlers
    global->descriptor_table->interrupt_manager.Enable();
    LOG("Interrupts enabled");
    
    // Initialize process management
    process_manager = new ProcessManager();
    LOG("Process manager initialized");
    
    // Initialize synchronization manager
    sync_manager = new SyncManager();
    LOG("Synchronization manager initialized");
    
    // Initialize IPC manager
    ipc_manager = new IpcManager();
    LOG("IPC manager initialized");
    
    // Set scheduling mode to test both cooperative and preemptive
    LOG("Setting scheduling mode to PREEMPTIVE");
    process_manager->SetSchedulingMode(SCHEDULING_MODE_PREEMPTIVE);
    
    // Create some IPC mechanisms for testing
    Pipe* test_pipe = ipc_manager->CreatePipe(1024, true);  // 1KB pipe, blocking
    if (test_pipe) {
        LOG("Created test pipe successfully");
    } else {
        LOG("Failed to create test pipe");
    }
    
    // Create some shared memory regions for testing
    if (global && global->shared_memory_manager) {
        SharedMemoryRegion* test_shm = global->shared_memory_manager->CreateSharedMemory(4096);  // 4KB shared memory
        if (test_shm) {
            LOG("Created test shared memory region ID: " << test_shm->id);
        } else {
            LOG("Failed to create test shared memory region");
        }
    } else {
        LOG("Shared memory manager not available");
    }
    
    // Create some test processes
    ProcessControlBlock* pcb1 = process_manager->CreateProcess(
        (void*)TestProcessFunction1, "TestProcess1", 10);  // Lower priority number = higher priority
    ProcessControlBlock* pcb2 = process_manager->CreateProcess(
        (void*)TestProcessFunction2, "TestProcess2", 20);  // Lower priority number = higher priority
        
    if (pcb1) {
        LOG("Created process 1 with PID: " << pcb1->pid);
    } else {
        LOG("Failed to create process 1");
    }
    
    if (pcb2) {
        LOG("Created process 2 with PID: " << pcb2->pid);
    } else {
        LOG("Failed to create process 2");
    }
    
    // Print process list for debugging
    process_manager->PrintProcessList();
    
    LOG("Kernel initialization complete");
    
    // Main kernel loop
    while (true) {
        // In a real kernel, this would handle system tasks
        // Since we have processes, the scheduler will handle switching between them
        LOG("Kernel alive...");
        
        // Add a small delay to avoid overwhelming the output
        for (volatile int i = 0; i < 1000000; ++i) { 
            // Busy wait to slow down output
        }
        
        // Switch to different scheduling modes to test
        static int mode_counter = 0;
        mode_counter++;
        if (mode_counter == 10) {  // Switch mode every 10 iterations
            if (process_manager->GetSchedulingMode() == SCHEDULING_MODE_PREEMPTIVE) {
                LOG("Switching to COOPERATIVE mode");
                process_manager->SetSchedulingMode(SCHEDULING_MODE_COOPERATIVE);
            } else if (process_manager->GetSchedulingMode() == SCHEDULING_MODE_COOPERATIVE) {
                LOG("Switching to ROUND_ROBIN mode");
                process_manager->SetSchedulingMode(SCHEDULING_MODE_ROUND_ROBIN);
            } else {
                LOG("Switching to PREEMPTIVE mode");
                process_manager->SetSchedulingMode(SCHEDULING_MODE_PREEMPTIVE);
            }
            mode_counter = 0;
        }
    }
    
    return 0xDEADBEEF; // Should never reach here
}