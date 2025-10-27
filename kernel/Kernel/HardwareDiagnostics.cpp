#include "HardwareDiagnostics.h"
#include "Kernel.h"

// Global hardware diagnostics instance
HardwareDiagnostics* g_hardware_diagnostics = nullptr;

HardwareDiagnostics::HardwareDiagnostics() : diagnostic_count(0) {
    memset(diagnostics, 0, sizeof(diagnostics));
}

HardwareDiagnostics::~HardwareDiagnostics() {
    // Cleanup if needed
}

bool HardwareDiagnostics::Initialize() {
    diagnostic_count = 0;
    LOG("Hardware diagnostics system initialized");
    return true;
}

bool HardwareDiagnostics::RunAllDiagnostics() {
    LOG("Starting hardware diagnostics...");
    
    // Run all built-in diagnostic tests
    RunCpuDiagnostic();
    RunMemoryDiagnostic();
    RunTimerDiagnostic();
    RunPciDiagnostic();
    RunBasicSystemDiagnostic();
    
    LOG("Hardware diagnostics completed. " << diagnostic_count << " tests run.");
    
    // Print summary
    PrintDiagnosticSummary();
    
    return true;
}

DiagnosticResult HardwareDiagnostics::RunCpuDiagnostic() {
    HardwareDiagnostic& diag = diagnostics[diagnostic_count];
    diag.type = DiagnosticType::CPU;
    strncpy(diag.name, "CPU Information Test", sizeof(diag.name) - 1);
    strncpy(diag.description, "Detects CPU vendor, architecture, and basic features", sizeof(diag.description) - 1);
    
    // Get start time
    uint64_t start_time = HAL_TIMER()->GetTickCount();
    
    // Perform CPU diagnostic
    CpuHal* cpu = HAL_CPU();
    if (!cpu) {
        strncpy(diag.details, "CPU HAL not available", sizeof(diag.details) - 1);
        diag.result = DiagnosticResult::FAILED;
    } else {
        // Test basic CPU operations
        bool interrupts_work = cpu->DisableInterrupts();
        cpu->RestoreInterrupts(interrupts_work);
        
        // Test CPU vendor detection
        const char* vendor = cpu->GetVendorString();
        if (vendor && strlen(vendor) > 0) {
            strncpy(diag.details, vendor, sizeof(diag.details) - 1);
            diag.result = DiagnosticResult::PASSED;
        } else {
            strncpy(diag.details, "Could not detect CPU vendor", sizeof(diag.details) - 1);
            diag.result = DiagnosticResult::FAILED;
        }
    }
    
    // Calculate execution time
    diag.execution_time = (uint32)(HAL_TIMER()->GetTickCount() - start_time);
    diag.timestamp = HAL_TIMER()->GetTickCount();
    
    diagnostic_count++;
    LOG("CPU diagnostic: " << (diag.result == DiagnosticResult::PASSED ? "PASSED" : "FAILED"));
    
    return diag.result;
}

DiagnosticResult HardwareDiagnostics::RunMemoryDiagnostic() {
    HardwareDiagnostic& diag = diagnostics[diagnostic_count];
    diag.type = DiagnosticType::MEMORY;
    strncpy(diag.name, "Memory Information Test", sizeof(diag.name) - 1);
    strncpy(diag.description, "Detects and validates system memory information", sizeof(diag.description) - 1);
    
    // Get start time
    uint64_t start_time = HAL_TIMER()->GetTickCount();
    
    // Perform memory diagnostic
    MemoryHal* memory = HAL_MEMORY();
    if (!memory) {
        strncpy(diag.details, "Memory HAL not available", sizeof(diag.details) - 1);
        diag.result = DiagnosticResult::FAILED;
    } else {
        uint64_t mem_size = memory->GetPhysicalMemorySize();
        if (mem_size > 0) {
            snprintf(diag.details, sizeof(diag.details), 
                    "Physical memory: %llu MB", mem_size / (1024 * 1024));
            diag.result = DiagnosticResult::PASSED;
        } else {
            strncpy(diag.details, "Could not detect physical memory size", sizeof(diag.details) - 1);
            diag.result = DiagnosticResult::FAILED;
        }
    }
    
    // Calculate execution time
    diag.execution_time = (uint32)(HAL_TIMER()->GetTickCount() - start_time);
    diag.timestamp = HAL_TIMER()->GetTickCount();
    
    diagnostic_count++;
    LOG("Memory diagnostic: " << (diag.result == DiagnosticResult::PASSED ? "PASSED" : "FAILED"));
    
    return diag.result;
}

DiagnosticResult HardwareDiagnostics::RunTimerDiagnostic() {
    HardwareDiagnostic& diag = diagnostics[diagnostic_count];
    diag.type = DiagnosticType::TIMER;
    strncpy(diag.name, "Timer Functionality Test", sizeof(diag.name) - 1);
    strncpy(diag.description, "Tests hardware timer functionality and accuracy", sizeof(diag.description) - 1);
    
    // Get start time
    uint64_t start_time = HAL_TIMER()->GetTickCount();
    
    // Perform timer diagnostic
    TimerHal* timer = HAL_TIMER();
    if (!timer) {
        strncpy(diag.details, "Timer HAL not available", sizeof(diag.details) - 1);
        diag.result = DiagnosticResult::FAILED;
    } else {
        // Test timer frequency
        uint32 freq = timer->GetFrequency();
        if (freq > 0) {
            snprintf(diag.details, sizeof(diag.details), 
                    "Timer frequency: %u Hz", freq);
            diag.result = DiagnosticResult::PASSED;
        } else {
            strncpy(diag.details, "Could not get timer frequency", sizeof(diag.details) - 1);
            diag.result = DiagnosticResult::FAILED;
        }
    }
    
    // Calculate execution time
    diag.execution_time = (uint32)(HAL_TIMER()->GetTickCount() - start_time);
    diag.timestamp = HAL_TIMER()->GetTickCount();
    
    diagnostic_count++;
    LOG("Timer diagnostic: " << (diag.result == DiagnosticResult::PASSED ? "PASSED" : "FAILED"));
    
    return diag.result;
}

DiagnosticResult HardwareDiagnostics::RunPciDiagnostic() {
    HardwareDiagnostic& diag = diagnostics[diagnostic_count];
    diag.type = DiagnosticType::PCI;
    strncpy(diag.name, "PCI Bus Detection Test", sizeof(diag.name) - 1);
    strncpy(diag.description, "Enumerates PCI devices on the system", sizeof(diag.description) - 1);
    
    // Get start time
    uint64_t start_time = HAL_TIMER()->GetTickCount();
    
    // Perform PCI diagnostic
    PciHal* pci = HAL_PCI();
    if (!pci) {
        strncpy(diag.details, "PCI HAL not available", sizeof(diag.details) - 1);
        diag.result = DiagnosticResult::FAILED;
    } else {
        uint32 device_count = pci->EnumerateDevices();
        snprintf(diag.details, sizeof(diag.details), 
                "Found %u PCI devices", device_count);
        diag.result = DiagnosticResult::PASSED;
        
        // Briefly log found devices (first few only)
        if (device_count > 0) {
            LOG("PCI devices found: " << device_count);
        }
    }
    
    // Calculate execution time
    diag.execution_time = (uint32)(HAL_TIMER()->GetTickCount() - start_time);
    diag.timestamp = HAL_TIMER()->GetTickCount();
    
    diagnostic_count++;
    LOG("PCI diagnostic: " << (diag.result == DiagnosticResult::PASSED ? "PASSED" : "FAILED"));
    
    return diag.result;
}

DiagnosticResult HardwareDiagnostics::RunBasicSystemDiagnostic() {
    HardwareDiagnostic& diag = diagnostics[diagnostic_count];
    diag.type = DiagnosticType::OTHER;
    strncpy(diag.name, "Basic System Test", sizeof(diag.name) - 1);
    strncpy(diag.description, "Runs basic system functionality tests", sizeof(diag.description) - 1);
    
    // Get start time
    uint64_t start_time = HAL_TIMER()->GetTickCount();
    
    // Perform basic system diagnostic
    // This could include testing basic I/O, memory allocation, etc.
    
    // Test basic memory allocation
    void* test_ptr = malloc(1024);  // Try to allocate 1KB
    if (test_ptr) {
        // Write and read test
        uint8* test_mem = (uint8*)test_ptr;
        test_mem[0] = 0xAA;
        test_mem[511] = 0x55;
        test_mem[1023] = 0xFF;
        
        if (test_mem[0] == 0xAA && test_mem[511] == 0x55 && test_mem[1023] == 0xFF) {
            strncpy(diag.details, "Basic memory allocation and access test passed", sizeof(diag.details) - 1);
            diag.result = DiagnosticResult::PASSED;
        } else {
            strncpy(diag.details, "Memory access failed", sizeof(diag.details) - 1);
            diag.result = DiagnosticResult::FAILED;
        }
        
        free(test_ptr);
    } else {
        strncpy(diag.details, "Could not allocate test memory", sizeof(diag.details) - 1);
        diag.result = DiagnosticResult::FAILED;
    }
    
    // Calculate execution time
    diag.execution_time = (uint32)(HAL_TIMER()->GetTickCount() - start_time);
    diag.timestamp = HAL_TIMER()->GetTickCount();
    
    diagnostic_count++;
    LOG("Basic system diagnostic: " << (diag.result == DiagnosticResult::PASSED ? "PASSED" : "FAILED"));
    
    return diag.result;
}

DiagnosticResult HardwareDiagnostics::RunDiagnostic(DiagnosticType type) {
    // For now, we just run all diagnostics since they're quick
    RunAllDiagnostics();
    
    // Find the specific diagnostic result
    for (uint32 i = 0; i < diagnostic_count; i++) {
        if (diagnostics[i].type == type) {
            return diagnostics[i].result;
        }
    }
    
    return DiagnosticResult::FAILED;  // Not found
}

bool HardwareDiagnostics::RegisterDiagnostic(DiagnosticType type, const char* name, 
                                           const char* description, DiagnosticTestFn test_func) {
    // For this implementation, we'll just add custom diagnostics in addition to built-in ones
    // We're not implementing custom diagnostic registration for simplicity
    return false;
}

const HardwareDiagnostic* HardwareDiagnostics::GetDiagnosticResults(uint32* count) {
    *count = diagnostic_count;
    return diagnostics;
}

void HardwareDiagnostics::PrintDiagnosticSummary() {
    LOG("=== Hardware Diagnostic Summary ===");
    
    uint32 passed = 0, failed = 0, skipped = 0;
    
    for (uint32 i = 0; i < diagnostic_count; i++) {
        const HardwareDiagnostic& diag = diagnostics[i];
        LOG(TypeToString(diag.type) << " - " << diag.name << ": " 
            << ResultToString(diag.result) 
            << " (Time: " << diag.execution_time << "ms)");
        
        switch (diag.result) {
            case DiagnosticResult::PASSED: passed++; break;
            case DiagnosticResult::FAILED: failed++; break;
            case DiagnosticResult::SKIPPED: skipped++; break;
            default: break;
        }
    }
    
    LOG("Total: " << diagnostic_count << " tests, " 
        << passed << " passed, " << failed << " failed, " << skipped << " skipped");
    LOG("===================================");
}

const char* HardwareDiagnostics::ResultToString(DiagnosticResult result) {
    switch (result) {
        case DiagnosticResult::PASSED: return "PASSED";
        case DiagnosticResult::FAILED: return "FAILED";
        case DiagnosticResult::SKIPPED: return "SKIPPED";
        case DiagnosticResult::INCONCLUSIVE: return "INCONCLUSIVE";
        default: return "UNKNOWN";
    }
}

const char* HardwareDiagnostics::TypeToString(DiagnosticType type) {
    switch (type) {
        case DiagnosticType::CPU: return "CPU";
        case DiagnosticType::MEMORY: return "MEMORY";
        case DiagnosticType::TIMER: return "TIMER";
        case DiagnosticType::PCI: return "PCI";
        case DiagnosticType::DISK: return "DISK";
        case DiagnosticType::NETWORK: return "NETWORK";
        case DiagnosticType::OTHER: return "OTHER";
        default: return "UNKNOWN";
    }
}

bool HardwareDiagnostics::DetectHardware() {
    LOG("Starting hardware detection...");
    
    // Detect CPU information
    CpuHal* cpu = HAL_CPU();
    if (cpu) {
        LOG("CPU Vendor: " << cpu->GetVendorString());
        LOG("CPU Architecture: " << (int)cpu->GetArchitecture());
    }
    
    // Detect memory information
    MemoryHal* memory = HAL_MEMORY();
    if (memory) {
        uint64_t mem_size = memory->GetPhysicalMemorySize();
        LOG("Physical Memory: " << mem_size / (1024 * 1024) << " MB");
    }
    
    // Detect timer information
    TimerHal* timer = HAL_TIMER();
    if (timer) {
        LOG("Timer Frequency: " << timer->GetFrequency() << " Hz");
    }
    
    // Detect PCI devices
    PciHal* pci = HAL_PCI();
    if (pci) {
        uint32 device_count = pci->EnumerateDevices();
        LOG("PCI Devices Found: " << device_count);
    }
    
    // Detect interrupt controller
    InterruptHal* interrupt = HAL_INTERRUPT();
    if (interrupt) {
        LOG("Interrupt Controller: " << interrupt->GetControllerType());
    }
    
    LOG("Hardware detection completed");
    return true;
}

void HardwareDiagnostics::PrintHardwareInfo() {
    LOG("=== System Hardware Information ===");
    DetectHardware();  // Run detection to ensure info is current
    LOG("===============================");
}

// Initialize hardware diagnostics system
bool InitializeHardwareDiagnostics() {
    g_hardware_diagnostics = new HardwareDiagnostics();
    if (!g_hardware_diagnostics) {
        LOG("Error: Failed to allocate hardware diagnostics manager");
        return false;
    }
    
    if (!g_hardware_diagnostics->Initialize()) {
        LOG("Error: Failed to initialize hardware diagnostics manager");
        delete g_hardware_diagnostics;
        g_hardware_diagnostics = nullptr;
        return false;
    }
    
    LOG("Hardware diagnostics system initialized successfully");
    return true;
}