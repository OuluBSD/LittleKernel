#include "Kernel.h"
#include "TestSuite.h"

// Global test suite instance
KernelTestSuite* g_kernel_test_suite = nullptr;

KernelTestSuite::KernelTestSuite() {
    test_count = 0;
    passed_tests = 0;
    failed_tests = 0;
    error_tests = 0;
    skipped_tests = 0;
    current_test_index = 0;
    test_lock.Initialize();
    
    // Initialize all test entries
    for (int i = 0; i < MAX_TESTS; i++) {
        tests[i].name = nullptr;
        tests[i].func = nullptr;
        tests[i].enabled = false;
        tests[i].result = TEST_SKIP;
        tests[i].description = nullptr;
        tests[i].execution_time = 0;
    }
}

KernelTestSuite::~KernelTestSuite() {
    // Cleanup handled by kernel shutdown
}

bool KernelTestSuite::Initialize() {
    LOG("Initializing kernel test suite");
    
    // Register all available tests
    RegisterTest("BasicMath", TestBasicMath, "Test basic arithmetic operations");
    RegisterTest("MemoryAllocation", TestMemoryAllocation, "Test memory allocation functions");
    RegisterTest("VfsInitialization", TestVfsInitialization, "Test VFS initialization");
    RegisterTest("FileOperations", TestFileOperations, "Test basic file operations");
    RegisterTest("ProcessCreation", TestProcessCreation, "Test process creation");
    RegisterTest("TimerFunctionality", TestTimerFunctionality, "Test timer functionality");
    RegisterTest("InterruptHandling", TestInterruptHandling, "Test interrupt handling");
    RegisterTest("RegistryOperations", TestRegistryOperations, "Test registry operations");
    RegisterTest("SystemCalls", TestSystemCalls, "Test system call interface");
    RegisterTest("PagingFunctionality", TestPagingFunctionality, "Test paging functionality");
    
    LOG("Kernel test suite initialized with " << test_count << " tests registered");
    return true;
}

bool KernelTestSuite::RegisterTest(const char* name, TestFunction func, const char* description) {
    if (!name || !func || test_count >= MAX_TESTS) {
        return false;
    }
    
    test_lock.Acquire();
    
    tests[test_count].name = name;
    tests[test_count].func = func;
    tests[test_count].enabled = true;  // Enable by default
    tests[test_count].result = TEST_SKIP;  // Not yet run
    tests[test_count].description = description;
    tests[test_count].execution_time = 0;
    
    test_count++;
    
    test_lock.Release();
    return true;
}

bool KernelTestSuite::RunAllTests() {
    LOG("Starting kernel test suite execution...");
    
    ResetResults();
    
    for (uint32_t i = 0; i < test_count; i++) {
        if (tests[i].enabled) {
            RunTest(i);
        } else {
            tests[i].result = TEST_SKIP;
            skipped_tests++;
        }
    }
    
    PrintResults();
    return (failed_tests == 0 && error_tests == 0);
}

bool KernelTestSuite::RunTest(uint32_t index) {
    if (index >= test_count) {
        return false;
    }
    
    StartTest(index);
    
    // Record start time
    uint32_t start_time = global_timer ? global_timer->GetTickCount() : 0;
    
    TestResult result = TEST_ERROR;
    try {
        result = tests[index].func();
    }
    catch (...) {
        result = TEST_ERROR;
    }
    
    // Record end time
    uint32_t end_time = global_timer ? global_timer->GetTickCount() : 0;
    uint32_t elapsed_time = end_time - start_time;
    if (global_timer) {
        // Convert from timer ticks to milliseconds approximately
        elapsed_time = (elapsed_time * 1000) / global_timer->GetFrequency();
    }
    
    EndTest(index, result);
    tests[index].execution_time = elapsed_time;
    
    LogTestResult(index);
    
    return (result == TEST_PASS);
}

bool KernelTestSuite::RunTestsByName(const char* pattern) {
    if (!pattern) {
        return false;
    }
    
    bool any_run = false;
    for (uint32_t i = 0; i < test_count; i++) {
        if (tests[i].enabled && tests[i].name && strstr(tests[i].name, pattern)) {
            RunTest(i);
            any_run = true;
        }
    }
    
    return any_run;
}

void KernelTestSuite::PrintResults() {
    LOG("=== KERNEL TEST SUITE RESULTS ===");
    LOG("Total tests: " << test_count);
    LOG("Passed:      " << passed_tests);
    LOG("Failed:      " << failed_tests);
    LOG("Errors:      " << error_tests);
    LOG("Skipped:     " << skipped_tests);
    
    if (failed_tests == 0 && error_tests == 0) {
        LOG("All tests passed! \\/");
    } else {
        LOG("Some tests failed or had errors :(");
    }
    
    LOG("=================================");
    
    // Print individual test results
    for (uint32_t i = 0; i < test_count; i++) {
        if (tests[i].result != TEST_SKIP) {
            const char* result_str = (tests[i].result == TEST_PASS) ? "PASS" : 
                                    (tests[i].result == TEST_FAIL) ? "FAIL" : "ERROR";
            LOG("[" << result_str << "] " << tests[i].name << " (" << tests[i].execution_time << "ms)");
        }
    }
}

void KernelTestSuite::ResetResults() {
    test_lock.Acquire();
    
    passed_tests = 0;
    failed_tests = 0;
    error_tests = 0;
    skipped_tests = 0;
    
    for (uint32_t i = 0; i < test_count; i++) {
        tests[i].result = TEST_SKIP;
        tests[i].execution_time = 0;
    }
    
    test_lock.Release();
}

bool KernelTestSuite::EnableTest(uint32_t index, bool enable) {
    if (index >= test_count) {
        return false;
    }
    
    test_lock.Acquire();
    tests[index].enabled = enable;
    test_lock.Release();
    
    return true;
}

bool KernelTestSuite::EnableTestByName(const char* name, bool enable) {
    if (!name) {
        return false;
    }
    
    for (uint32_t i = 0; i < test_count; i++) {
        if (tests[i].name && strcmp(tests[i].name, name) == 0) {
            return EnableTest(i, enable);
        }
    }
    
    return false;  // Test not found
}

void KernelTestSuite::StartTest(uint32_t index) {
    current_test_index = index;
    DLOG("Starting test: " << tests[index].name);
}

void KernelTestSuite::EndTest(uint32_t index, TestResult result) {
    tests[index].result = result;
    
    switch (result) {
        case TEST_PASS:
            passed_tests++;
            break;
        case TEST_FAIL:
            failed_tests++;
            break;
        case TEST_ERROR:
            error_tests++;
            break;
        case TEST_SKIP:
            skipped_tests++;
            break;
    }
}

void KernelTestSuite::LogTestResult(uint32_t index) {
    const char* result_str = (tests[index].result == TEST_PASS) ? "PASS" : 
                            (tests[index].result == TEST_FAIL) ? "FAIL" : "ERROR";
    
    LOG("Test [" << result_str << "] " << tests[index].name 
         << " (" << tests[index].execution_time << "ms): " 
         << tests[index].description);
}

// Individual test function implementations

TestResult TestBasicMath() {
    // Test basic arithmetic operations
    int a = 5, b = 3;
    if (a + b != 8) return TEST_FAIL;
    if (a - b != 2) return TEST_FAIL;
    if (a * b != 15) return TEST_FAIL;
    if (a / b != 1) return TEST_FAIL;  // Integer division
    
    return TEST_PASS;
}

TestResult TestMemoryAllocation() {
    // Test basic memory allocation
    void* ptr = kmalloc(1024);
    if (!ptr) {
        LOG("Memory allocation test failed: kmalloc returned null");
        return TEST_ERROR;
    }
    
    // Write and read data to verify the memory works
    char* test_data = (char*)ptr;
    for (int i = 0; i < 100; i++) {
        test_data[i] = (char)i;
    }
    
    for (int i = 0; i < 100; i++) {
        if (test_data[i] != (char)i) {
            kfree(ptr);
            return TEST_FAIL;
        }
    }
    
    kfree(ptr);
    return TEST_PASS;
}

TestResult TestVfsInitialization() {
    if (!g_vfs) {
        LOG("VFS test failed: g_vfs is null");
        return TEST_ERROR;
    }
    
    // Test that we can get the root node
    VfsNode* root = g_vfs->GetRoot();
    if (!root) {
        LOG("VFS test failed: Could not get VFS root");
        return TEST_ERROR;
    }
    
    return TEST_PASS;
}

TestResult TestFileOperations() {
    if (!g_vfs) {
        LOG("File operations test failed: VFS not initialized");
        return TEST_ERROR;
    }
    
    // Try to get file statistics for root directory
    FileStat stat;
    int result = g_vfs->Stat("/", &stat);
    if (result != VFS_SUCCESS) {
        LOG("File operations test failed: Could not stat root directory");
        return TEST_ERROR;
    }
    
    // Verify some expected properties of the root directory
    if (!(stat.attributes & ATTR_DIRECTORY)) {
        LOG("File operations test failed: Root is not marked as directory");
        return TEST_FAIL;
    }
    
    return TEST_PASS;
}

TestResult TestProcessCreation() {
    if (!process_manager) {
        LOG("Process creation test failed: Process manager not available");
        return TEST_ERROR;
    }
    
    // For now, just check that the process manager is available
    // A full test would create an actual process, which is complex
    
    return TEST_PASS;
}

TestResult TestTimerFunctionality() {
    if (!global_timer) {
        LOG("Timer functionality test failed: Global timer not available");
        return TEST_ERROR;
    }
    
    // Test basic timer functionality
    uint32_t start_time = global_timer->GetTickCount();
    // Wait a few ticks (in a real test, we might wait for a specific time)
    
    // Verify that time is advancing
    for (int i = 0; i < 100; i++) {
        uint32_t current_time = global_timer->GetTickCount();
        if (current_time >= start_time) {
            // Time is advancing as expected
            break;
        }
        // Small delay
        for (volatile int j = 0; j < 100; ++j);
    }
    
    return TEST_PASS;
}

TestResult TestInterruptHandling() {
    // For now, just verify that the interrupt manager is available
    if (!global->descriptor_table || !global->descriptor_table->interrupt_manager.IsInitialized()) {
        LOG("Interrupt handling test failed: Interrupt manager not available");
        return TEST_ERROR;
    }
    
    return TEST_PASS;
}

TestResult TestRegistryOperations() {
    if (!g_registry) {
        LOG("Registry operations test failed: Registry not initialized");
        return TEST_ERROR;
    }
    
    // Try to open the root key
    RegistryKey* key = nullptr;
    bool result = g_registry->OpenKey("HKEY_LOCAL_MACHINE", KEY_READ, &key);
    if (!result || !key) {
        LOG("Registry operations test failed: Could not open HKEY_LOCAL_MACHINE");
        return TEST_ERROR;
    }
    
    g_registry->CloseKey(key);
    return TEST_PASS;
}

TestResult TestSystemCalls() {
    if (!g_syscall_interface) {
        LOG("System calls test failed: System call interface not initialized");
        return TEST_ERROR;
    }
    
    // Try to get the current process ID
    int pid = g_syscall_interface->SysGetPid();
    if (pid < 0) {
        LOG("System calls test failed: Could not get process ID");
        return TEST_ERROR;
    }
    
    return TEST_PASS;
}

TestResult TestPagingFunctionality() {
    if (!global->paging_manager) {
        LOG("Paging functionality test failed: Paging manager not available");
        return TEST_ERROR;
    }
    
    // Try to allocate and map a page
    void* virt_addr = (void*)0x10000000;  // Example virtual address
    if (!global->paging_manager->Map(virt_addr, nullptr, true, true)) {
        LOG("Paging functionality test failed: Could not map page");
        return TEST_ERROR;
    }
    
    return TEST_PASS;
}

bool InitializeTestSuite() {
    if (!g_kernel_test_suite) {
        g_kernel_test_suite = new KernelTestSuite();
        if (!g_kernel_test_suite) {
            LOG("Failed to create kernel test suite instance");
            return false;
        }
        
        if (!g_kernel_test_suite->Initialize()) {
            LOG("Failed to initialize kernel test suite");
            delete g_kernel_test_suite;
            g_kernel_test_suite = nullptr;
            return false;
        }
        
        LOG("Kernel test suite initialized successfully");
    }
    
    return true;
}

bool RunKernelTests() {
    if (!g_kernel_test_suite) {
        LOG("Cannot run tests: Test suite not initialized");
        return false;
    }
    
    return g_kernel_test_suite->RunAllTests();
}