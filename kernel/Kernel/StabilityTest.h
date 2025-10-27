#ifndef _Kernel_StabilityTest_h_
#define _Kernel_StabilityTest_h_

#include "Common.h"
#include "Defs.h"
#include "Logging.h"
#include "ProcessControlBlock.h"
#include "Vfs.h"
#include "TestSuite.h"

// Stability test types
enum StabilityTestType {
    STRESS_MEMORY = 0,      // Memory allocation/deallocation stress
    STRESS_PROCESS,         // Process creation/destruction stress
    STRESS_FILESYSTEM,      // File system operations stress
    STRESS_INTERRUPTS,      // Interrupt handling stress
    STRESS_SCHEDULER,       // Scheduler stress
    STRESS_CONCURRENT       // Concurrent operations stress
};

// Stability test configuration
struct StabilityTestConfig {
    StabilityTestType test_type;    // Type of test to run
    uint32 duration_seconds;      // Duration of test in seconds
    uint32 iterations;            // Number of iterations to perform
    uint32 thread_count;          // Number of concurrent threads (if applicable)
    bool verbose_output;            // Whether to output detailed information
    uint32 seed;                  // Random seed for randomized tests
};

// Stability test result
struct StabilityTestResult {
    bool passed;                    // Whether the test passed
    uint32 errors_found;          // Number of errors detected
    uint32 warnings;              // Number of warnings
    uint32 operations_completed;  // Number of operations completed
    uint64_t execution_time_ms;     // Execution time in milliseconds
    char error_details[512];        // Details about errors found
};

// Stability test class
class StabilityTester {
private:
    bool is_running;                // Whether a test is currently running
    StabilityTestResult last_result; // Result of the last test
    Spinlock tester_lock;           // Lock for tester operations

public:
    StabilityTester();
    ~StabilityTester();
    
    // Initialize the stability tester
    bool Initialize();
    
    // Run a stability test with the given configuration
    StabilityTestResult RunTest(const StabilityTestConfig& config);
    
    // Run a specific type of stability test
    StabilityTestResult RunMemoryStressTest(const StabilityTestConfig& config);
    StabilityTestResult RunProcessStressTest(const StabilityTestConfig& config);
    StabilityTestResult RunFilesystemStressTest(const StabilityTestConfig& config);
    StabilityTestResult RunInterruptStressTest(const StabilityTestConfig& config);
    StabilityTestResult RunSchedulerStressTest(const StabilityTestConfig& config);
    StabilityTestResult RunConcurrentStressTest(const StabilityTestConfig& config);
    
    // Run all stability tests
    bool RunAllStabilityTests(uint32 duration_seconds = 60);
    
    // Check if a test is currently running
    bool IsTestRunning() { return is_running; }
    
    // Get the result of the last test
    const StabilityTestResult& GetLastResult() { return last_result; }
    
    // Force stop any running test
    void StopCurrentTest();

private:
    // Internal helper functions
    void InitializeTestResult(StabilityTestResult& result);
    void ReportTestProgress(StabilityTestType type, uint32 completed, uint32 total);
    bool ValidateSystemState();  // Check if system is in a valid state
    bool CheckForMemoryLeaks();  // Check for memory leaks
    bool CheckForProcessLeaks(); // Check for process leaks
    bool CheckForResourceLeaks(); // Check for various resource leaks
};

// Global stability tester instance
extern StabilityTester* g_stability_tester;

// Initialize the stability tester
bool InitializeStabilityTester();

// Run comprehensive stability tests
bool RunStabilityTests();

// Run a specific type of stress test
StabilityTestResult RunMemoryStressTest();
StabilityTestResult RunProcessStressTest();
StabilityTestResult RunFilesystemStressTest();

#endif