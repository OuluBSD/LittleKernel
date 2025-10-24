#ifndef _Kernel_TestSuite_h_
#define _Kernel_TestSuite_h_

#include "Common.h"
#include "Defs.h"
#include "Logging.h"
#include "Vfs.h"
#include "Syscalls.h"

// Test result codes
enum TestResult {
    TEST_PASS = 0,
    TEST_FAIL = 1,
    TEST_ERROR = 2,
    TEST_SKIP = 3
};

// Test function type
typedef TestResult (*TestFunction)();

// Maximum number of tests in the suite
#define MAX_TESTS 256

// Test information structure
struct TestInfo {
    const char* name;           // Name of the test
    TestFunction func;          // Function to execute the test
    bool enabled;               // Whether the test is enabled
    TestResult result;          // Result of the test
    const char* description;    // Description of what the test does
    uint32_t execution_time;    // Time taken to execute (in milliseconds)
};

// Kernel test suite class
class KernelTestSuite {
private:
    TestInfo tests[MAX_TESTS];
    uint32_t test_count;
    uint32_t passed_tests;
    uint32_t failed_tests;
    uint32_t error_tests;
    uint32_t skipped_tests;
    uint32_t current_test_index;
    Spinlock test_lock;        // Lock for thread safety during testing

public:
    KernelTestSuite();
    ~KernelTestSuite();
    
    // Initialize the test suite
    bool Initialize();
    
    // Register a test
    bool RegisterTest(const char* name, TestFunction func, const char* description);
    
    // Run all tests
    bool RunAllTests();
    
    // Run a specific test by index
    bool RunTest(uint32_t index);
    
    // Run tests by name (substring matching)
    bool RunTestsByName(const char* pattern);
    
    // Print test results
    void PrintResults();
    
    // Get test statistics
    uint32_t GetTotalTests() { return test_count; }
    uint32_t GetPassedTests() { return passed_tests; }
    uint32_t GetFailedTests() { return failed_tests; }
    uint32_t GetErrorTests() { return error_tests; }
    uint32_t GetSkippedTests() { return skipped_tests; }
    
    // Reset test results
    void ResetResults();
    
    // Enable/disable a test
    bool EnableTest(uint32_t index, bool enable);
    bool EnableTestByName(const char* name, bool enable);

private:
    // Internal helper functions
    void StartTest(uint32_t index);
    void EndTest(uint32_t index, TestResult result);
    void LogTestResult(uint32_t index);
};

// Individual test functions
TestResult TestBasicMath();
TestResult TestMemoryAllocation();
TestResult TestVfsInitialization();
TestResult TestFileOperations();
TestResult TestProcessCreation();
TestResult TestTimerFunctionality();
TestResult TestInterruptHandling();
TestResult TestRegistryOperations();
TestResult TestSystemCalls();
TestResult TestPagingFunctionality();

// Global test suite instance
extern KernelTestSuite* g_kernel_test_suite;

// Initialize the kernel test suite
bool InitializeTestSuite();

// Run the kernel test suite
bool RunKernelTests();

#endif