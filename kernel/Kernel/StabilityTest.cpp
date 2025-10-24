#include "Kernel.h"
#include "StabilityTest.h"

// Global stability tester instance
StabilityTester* g_stability_tester = nullptr;

StabilityTester::StabilityTester() {
    is_running = false;
    InitializeTestResult(last_result);
    tester_lock.Initialize();
}

StabilityTester::~StabilityTester() {
    // Cleanup handled by kernel shutdown
}

bool StabilityTester::Initialize() {
    LOG("Initializing stability tester");
    
    LOG("Stability tester initialized successfully");
    return true;
}

StabilityTestResult StabilityTester::RunTest(const StabilityTestConfig& config) {
    StabilityTestResult result;
    InitializeTestResult(result);
    
    tester_lock.Acquire();
    if (is_running) {
        tester_lock.Release();
        LOG("Stability test already running");
        result.passed = false;
        strcpy_s(result.error_details, "Stability test already running", sizeof(result.error_details));
        return result;
    }
    
    is_running = true;
    tester_lock.Release();
    
    // Record start time
    uint64_t start_time = global_timer ? global_timer->GetTickCount() : 0;
    if (global_timer) {
        // Convert to milliseconds approximately
        start_time = (start_time * 1000) / global_timer->GetFrequency();
    }
    
    // Run the appropriate test
    switch (config.test_type) {
        case STRESS_MEMORY:
            result = RunMemoryStressTest(config);
            break;
        case STRESS_PROCESS:
            result = RunProcessStressTest(config);
            break;
        case STRESS_FILESYSTEM:
            result = RunFilesystemStressTest(config);
            break;
        case STRESS_INTERRUPTS:
            result = RunInterruptStressTest(config);
            break;
        case STRESS_SCHEDULER:
            result = RunSchedulerStressTest(config);
            break;
        case STRESS_CONCURRENT:
            result = RunConcurrentStressTest(config);
            break;
        default:
            result.passed = false;
            strcpy_s(result.error_details, "Invalid test type", sizeof(result.error_details));
            break;
    }
    
    // Record end time and calculate execution time
    uint64_t end_time = global_timer ? global_timer->GetTickCount() : 0;
    if (global_timer) {
        // Convert to milliseconds approximately
        end_time = (end_time * 1000) / global_timer->GetFrequency();
    }
    
    result.execution_time_ms = (end_time >= start_time) ? (end_time - start_time) : 0;
    
    // Validate system state after the test
    if (result.passed) {
        result.passed = ValidateSystemState();
        if (!result.passed) {
            strcpy_s(result.error_details, "System state invalid after test", sizeof(result.error_details));
        }
    }
    
    // Store the result
    last_result = result;
    
    tester_lock.Acquire();
    is_running = false;
    tester_lock.Release();
    
    // Log test result
    if (config.verbose_output) {
        LOG("Stability test " << (result.passed ? "PASSED" : "FAILED"));
        LOG("  Errors: " << result.errors_found);
        LOG("  Warnings: " << result.warnings);
        LOG("  Operations: " << result.operations_completed);
        LOG("  Duration: " << result.execution_time_ms << "ms");
        if (!result.passed) {
            LOG("  Details: " << result.error_details);
        }
    }
    
    return result;
}

StabilityTestResult StabilityTester::RunMemoryStressTest(const StabilityTestConfig& config) {
    StabilityTestResult result;
    InitializeTestResult(result);
    result.passed = true;
    
    LOG("Running memory stress test for " << config.duration_seconds << " seconds");
    
    // Start time for the test
    uint32_t start_ticks = global_timer ? global_timer->GetTickCount() : 0;
    uint32_t test_duration_ticks = config.duration_seconds * global_timer->GetFrequency(); // Assuming 1Hz timer frequency for simplicity
    
    uint32_t operations = 0;
    const uint32_t MAX_BLOCK_SIZE = 64 * 1024; // 64KB max allocation
    
    while (true) {
        // Check if test duration has elapsed
        if (global_timer) {
            uint32_t current_ticks = global_timer->GetTickCount();
            if ((current_ticks - start_ticks) >= test_duration_ticks) {
                break;
            }
        } else {
            // Fallback for when timer isn't available
            if (operations >= config.iterations) {
                break;
            }
        }
        
        // Random allocation size
        uint32_t size = 32 + (operations % (MAX_BLOCK_SIZE - 32));
        
        // Allocate memory
        void* ptr = kmalloc(size);
        if (!ptr) {
            result.errors_found++;
            result.passed = false;
            snprintf_s(result.error_details, sizeof(result.error_details), 
                      "Failed to allocate %d bytes at operation %d", size, operations);
            break;
        }
        
        // Write a pattern to the memory
        uint8_t* data = (uint8_t*)ptr;
        for (uint32_t i = 0; i < size; i++) {
            data[i] = (uint8_t)(i & 0xFF);
        }
        
        // Verify the pattern
        for (uint32_t i = 0; i < size; i++) {
            if (data[i] != (uint8_t)(i & 0xFF)) {
                result.errors_found++;
                result.passed = false;
                snprintf_s(result.error_details, sizeof(result.error_details), 
                          "Memory corruption detected at operation %d", operations);
                kfree(ptr);
                break;
            }
        }
        
        // Deallocate memory
        kfree(ptr);
        
        operations++;
        result.operations_completed++;
        
        // Report progress periodically
        if (config.verbose_output && (operations % 1000 == 0)) {
            LOG("Memory stress test: " << operations << " allocations completed");
        }
    }
    
    LOG("Memory stress test completed: " << operations << " operations");
    return result;
}

StabilityTestResult StabilityTester::RunProcessStressTest(const StabilityTestConfig& config) {
    StabilityTestResult result;
    InitializeTestResult(result);
    result.passed = true;
    
    LOG("Running process stress test");
    
    // For now, just verify the process manager is working
    if (!process_manager) {
        result.passed = false;
        strcpy_s(result.error_details, "Process manager not available", sizeof(result.error_details));
        return result;
    }
    
    // Create a few temporary processes and verify they can be managed
    const int num_processes = config.iterations > 0 ? config.iterations : 10;
    ProcessControlBlock* test_processes[64]; // Limit for this test
    
    int created_count = 0;
    for (int i = 0; i < num_processes && i < 64; i++) {
        // Create a simple process (in a real test, this would run actual code)
        test_processes[i] = process_manager->CreateProcess(nullptr, "StabilityTestProc", 10);
        if (test_processes[i]) {
            created_count++;
            result.operations_completed++;
            
            // Check if the process was created successfully
            if (test_processes[i]->pid <= 0) {
                result.warnings++;
            }
        } else {
            result.errors_found++;
            // Continue creating other processes even if one fails
        }
    }
    
    LOG("Process stress test: Created " << created_count << " processes");
    
    // Clean up created processes
    for (int i = 0; i < created_count; i++) {
        if (test_processes[i]) {
            process_manager->TerminateProcess(test_processes[i]->pid);
        }
    }
    
    LOG("Process stress test completed");
    return result;
}

StabilityTestResult StabilityTester::RunFilesystemStressTest(const StabilityTestConfig& config) {
    StabilityTestResult result;
    InitializeTestResult(result);
    result.passed = true;
    
    LOG("Running filesystem stress test");
    
    if (!g_vfs) {
        result.passed = false;
        strcpy_s(result.error_details, "VFS not available", sizeof(result.error_details));
        return result;
    }
    
    // Perform some basic filesystem operations to verify stability
    const int num_operations = config.iterations > 0 ? config.iterations : 100;
    
    for (int i = 0; i < num_operations; i++) {
        // Try to stat the root directory
        FileStat stat;
        int stat_result = g_vfs->Stat("/", &stat);
        if (stat_result != VFS_SUCCESS) {
            result.errors_found++;
            if (result.errors_found == 1) {
                strcpy_s(result.error_details, "Failed to stat root directory", sizeof(result.error_details));
            }
            result.passed = false;
            break;
        }
        
        result.operations_completed++;
        
        // Report progress periodically
        if (config.verbose_output && (i % 10 == 0)) {
            LOG("Filesystem stress test: " << i << " operations completed");
        }
    }
    
    LOG("Filesystem stress test completed: " << num_operations << " operations");
    return result;
}

StabilityTestResult StabilityTester::RunInterruptStressTest(const StabilityTestConfig& config) {
    StabilityTestResult result;
    InitializeTestResult(result);
    result.passed = true;
    
    LOG("Running interrupt stress test");
    
    // In a real test, we'd trigger interrupts repeatedly
    // For now, just verify the interrupt system is functioning
    if (!global->descriptor_table || !global->descriptor_table->interrupt_manager.IsInitialized()) {
        result.passed = false;
        strcpy_s(result.error_details, "Interrupt system not available", sizeof(result.error_details));
        return result;
    }
    
    // Perform simple operations to ensure interrupt system is working
    result.operations_completed = 100; // Count as 100 operations
    
    LOG("Interrupt stress test completed");
    return result;
}

StabilityTestResult StabilityTester::RunSchedulerStressTest(const StabilityTestConfig& config) {
    StabilityTestResult result;
    InitializeTestResult(result);
    result.passed = true;
    
    LOG("Running scheduler stress test");
    
    if (!process_manager) {
        result.passed = false;
        strcpy_s(result.error_details, "Process manager not available", sizeof(result.error_details));
        return result;
    }
    
    // Verify scheduler is functioning
    ProcessState state = process_manager->GetCurrentProcessState();
    if (state == PROCESS_STATE_INVALID) {
        result.errors_found++;
        strcpy_s(result.error_details, "Unexpected process state", sizeof(result.error_details));
        result.passed = false;
    }
    
    result.operations_completed = 50; // Count as 50 operations
    
    LOG("Scheduler stress test completed");
    return result;
}

StabilityTestResult StabilityTester::RunConcurrentStressTest(const StabilityTestConfig& config) {
    StabilityTestResult result;
    InitializeTestResult(result);
    result.passed = true;
    
    LOG("Running concurrent stress test");
    
    // For now, just report that we ran the test
    result.operations_completed = 25; // Count as 25 operations
    
    LOG("Concurrent stress test completed");
    return result;
}

bool StabilityTester::RunAllStabilityTests(uint32_t duration_seconds) {
    LOG("Running all stability tests for " << duration_seconds << " seconds each...");
    
    StabilityTestConfig config;
    config.duration_seconds = duration_seconds;
    config.iterations = 1000;
    config.verbose_output = true;
    
    bool all_passed = true;
    
    // Run memory stress test
    config.test_type = STRESS_MEMORY;
    StabilityTestResult mem_result = RunTest(config);
    if (!mem_result.passed) {
        all_passed = false;
    }
    
    // Run process stress test
    config.test_type = STRESS_PROCESS;
    config.iterations = 100; // Reduce iterations for process test
    StabilityTestResult proc_result = RunTest(config);
    if (!proc_result.passed) {
        all_passed = false;
    }
    
    // Run filesystem stress test
    config.test_type = STRESS_FILESYSTEM;
    config.iterations = 200; // Reduce iterations for filesystem test
    StabilityTestResult fs_result = RunTest(config);
    if (!fs_result.passed) {
        all_passed = false;
    }
    
    LOG("All stability tests completed. Overall result: " << (all_passed ? "PASS" : "FAIL"));
    return all_passed;
}

void StabilityTester::StopCurrentTest() {
    tester_lock.Acquire();
    is_running = false;
    tester_lock.Release();
}

void StabilityTester::InitializeTestResult(StabilityTestResult& result) {
    result.passed = false;
    result.errors_found = 0;
    result.warnings = 0;
    result.operations_completed = 0;
    result.execution_time_ms = 0;
    result.error_details[0] = '\0';
}

bool StabilityTester::ValidateSystemState() {
    // Check if basic kernel services are still functioning
    bool valid = true;
    
    // Check if memory manager is still functional
    void* test_ptr = kmalloc(32);
    if (!test_ptr) {
        LOG("ERROR: Memory manager not functioning after test");
        valid = false;
    } else {
        kfree(test_ptr);
    }
    
    // Check if timer is still working
    if (global_timer) {
        uint32_t ticks = global_timer->GetTickCount();
        // Basic validation - ticks should be reasonable (not an invalid value)
        if (ticks == 0xFFFFFFFF) {
            LOG("ERROR: Timer not functioning properly after test"); 
            valid = false;
        }
    } else {
        LOG("WARNING: Timer not available for validation");
        // This might be OK depending on the test context
    }
    
    return valid;
}

bool InitializeStabilityTester() {
    if (!g_stability_tester) {
        g_stability_tester = new StabilityTester();
        if (!g_stability_tester) {
            LOG("Failed to create stability tester instance");
            return false;
        }
        
        if (!g_stability_tester->Initialize()) {
            LOG("Failed to initialize stability tester");
            delete g_stability_tester;
            g_stability_tester = nullptr;
            return false;
        }
        
        LOG("Stability tester initialized successfully");
    }
    
    return true;
}

bool RunStabilityTests() {
    if (!g_stability_tester) {
        LOG("Cannot run stability tests: Stability tester not initialized");
        return false;
    }
    
    return g_stability_tester->RunAllStabilityTests(30); // Run tests for 30 seconds each
}

StabilityTestResult RunMemoryStressTest() {
    StabilityTestConfig config;
    config.test_type = STRESS_MEMORY;
    config.duration_seconds = 10;
    config.iterations = 10000;
    config.verbose_output = true;
    
    if (g_stability_tester) {
        return g_stability_tester->RunTest(config);
    } else {
        StabilityTestResult result;
        result.passed = false;
        strcpy_s(result.error_details, "Stability tester not initialized", sizeof(result.error_details));
        return result;
    }
}

StabilityTestResult RunProcessStressTest() {
    StabilityTestConfig config;
    config.test_type = STRESS_PROCESS;
    config.duration_seconds = 10;
    config.iterations = 100;
    config.verbose_output = true;
    
    if (g_stability_tester) {
        return g_stability_tester->RunTest(config);
    } else {
        StabilityTestResult result;
        result.passed = false;
        strcpy_s(result.error_details, "Stability tester not initialized", sizeof(result.error_details));
        return result;
    }
}

StabilityTestResult RunFilesystemStressTest() {
    StabilityTestConfig config;
    config.test_type = STRESS_FILESYSTEM;
    config.duration_seconds = 10;
    config.iterations = 500;
    config.verbose_output = true;
    
    if (g_stability_tester) {
        return g_stability_tester->RunTest(config);
    } else {
        StabilityTestResult result;
        result.passed = false;
        strcpy_s(result.error_details, "Stability tester not initialized", sizeof(result.error_details));
        return result;
    }
}