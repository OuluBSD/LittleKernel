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
    // Initialize error handling system first (before anything else)
    if (!InitializeErrorHandling()) {
        // We can't even log properly if this fails, so just continue
        // In a real implementation, we might need a basic error reporting mechanism
    } else {
        LOG("Error handling framework initialized successfully");
    }
    
    // Initialize kernel profiling infrastructure
    if (!InitializeKernelProfiling()) {
        LOG("Warning: Failed to initialize kernel profiling infrastructure");
        REPORT_ERROR(KernelError::ERROR_NOT_INITIALIZED, "KernelProfilingInitialization");
    } else {
        LOG("Kernel profiling infrastructure initialized successfully");
        g_kernel_profiler->EnableProfiling();
    }
    
    // Initialize module loading system
    if (!InitializeModuleLoader()) {
        LOG("Warning: Failed to initialize module loading system");
        REPORT_ERROR(KernelError::ERROR_NOT_INITIALIZED, "ModuleLoaderInitialization");
    } else {
        LOG("Module loading system initialized successfully");
        LOG("Kernel module loading framework ready");
    }
    
    // Initialize hardware components system
    g_pci_device_manager = new PCIDeviceManager();
    if (!g_pci_device_manager) {
        LOG("Warning: Failed to allocate PCI device manager");
        REPORT_ERROR(KernelError::ERROR_OUT_OF_MEMORY, "PCIDeviceManagerAllocation");
    } else if (g_pci_device_manager->Initialize() != HalResult::SUCCESS) {
        LOG("Warning: Failed to initialize PCI device manager");
        REPORT_ERROR(KernelError::ERROR_DEVICE_ERROR, "PCIDeviceManagerInitialization");
        delete g_pci_device_manager;
        g_pci_device_manager = nullptr;
    } else {
        LOG("Hardware components system (PCI Device Manager) initialized successfully");
        g_pci_device_manager->PrintDeviceList();
    }
    
    // Initialize Linux-style configuration system
    if (!InitializeConfigSystem()) {
        LOG("Warning: Failed to initialize configuration system");
        REPORT_ERROR(KernelError::ERROR_NOT_INITIALIZED, "ConfigSystemInitialization");
    } else {
        LOG("Linux-style configuration system initialized successfully");
        
        // Load the .config file
        if (!LoadKernelConfigFile(".config")) {
            LOG("Warning: Failed to load .config file, using defaults");
        } else {
            LOG("Kernel configuration loaded from .config");
            g_config_parser->PrintConfig();
            
            // Generate configuration header for build system
            if (!GenerateConfigHeader(".config", "kernel_config_defines.h")) {
                LOG("Warning: Failed to generate configuration header");
            } else {
                LOG("Configuration header generated successfully");
            }
        }
    }
    
    // Initialize early memory management system first (before anything else)
    if (!InitializeEarlyMemory(mboot_ptr)) {
        LOG("Error: Failed to initialize early memory management, attempting with standard allocation");
        REPORT_ERROR(KernelError::ERROR_DEVICE_ERROR, "EarlyMemoryInitialization");
        // Continue with standard allocation approach
    } else {
        LOG("Early memory system initialized successfully");
        g_early_memory_manager->PrintMemoryMap();
    }
    
    // Initialize the global structure with essential systems (early initialization)
    // For the enhanced boot process, we'll initialize the basic structure first
    // to enable logging and basic functionality
    global = (Global*)kmalloc(sizeof(Global));
    if (!global) {
        LOG("Fatal: Failed to allocate global structure");
        REPORT_ERROR(KernelError::ERROR_OUT_OF_MEMORY, "GlobalStructureAllocation");
        return -1;
    }
    global->Initialize();
    
    LOG("LittleKernel starting...");
    DLOG("Version: 2.0 (Complete Rewrite)");
    
    // Use the enhanced boot process to handle multiboot information and configuration
    if (EnhancedBootProcess(mboot_ptr, 0x2BADB002) != 0) {
        LOG("Warning: Enhanced boot process had issues, continuing with basic initialization");
        REPORT_ERROR(KernelError::ERROR_GENERAL, "EnhancedBootProcess");
    }
    
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
    
    // Initialize the Hardware Abstraction Layer
    g_hal_manager = new HalManager();
    if (g_hal_manager->Initialize() != HalResult::SUCCESS) {
        LOG("Error: Failed to initialize HAL Manager, continuing with reduced functionality");
    } else {
        LOG("HAL Manager initialized successfully");
    }
    
    // Initialize the runtime configuration system
    if (!InitializeRuntimeConfig()) {
        LOG("Error: Failed to initialize runtime configuration system");
    } else {
        LOG("Runtime configuration system initialized successfully");
    }
    
    // Initialize hardware diagnostics system
    if (!InitializeHardwareDiagnostics()) {
        LOG("Error: Failed to initialize hardware diagnostics system");
    } else {
        LOG("Hardware diagnostics system initialized successfully");
        
        // Run hardware detection to identify components
        g_hardware_diagnostics->DetectHardware();
        
        // Run diagnostics during boot
        g_hardware_diagnostics->RunAllDiagnostics();
    }
    
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
    
    // Initialize and register console driver
    ConsoleDriver* console_driver = new ConsoleDriver();
    if (console_driver->Initialize()) {
        if (global->driver_framework->RegisterDevice(console_driver->GetDevice())) {
            LOG("Console driver registered successfully");
        } else {
            LOG("Failed to register console driver");
        }
    } else {
        LOG("Failed to initialize console driver");
    }
    
    // Initialize driver framework
    if (global->driver_framework->InitializeAllDevices()) {
        LOG("Driver framework initialized and all devices initialized successfully");
    } else {
        LOG("Driver framework initialized but some devices failed to initialize");
    }
    
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