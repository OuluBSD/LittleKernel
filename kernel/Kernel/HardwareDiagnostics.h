#ifndef _Kernel_HardwareDiagnostics_h_
#define _Kernel_HardwareDiagnostics_h_

#include "Defs.h"
#include "HAL.h"

// Diagnostic result codes
enum class DiagnosticResult {
    PASSED = 0,
    FAILED = -1,
    SKIPPED = -2,
    INCONCLUSIVE = -3
};

// Hardware diagnostic types
enum class DiagnosticType {
    CPU,
    MEMORY,
    TIMER,
    PCI,
    DISK,
    NETWORK,
    OTHER
};

// Hardware diagnostic information structure
struct HardwareDiagnostic {
    DiagnosticType type;
    char name[64];              // Name of the diagnostic test
    char description[128];      // Description of what the test does
    DiagnosticResult result;    // Result of the diagnostic
    char details[256];          // Additional details about the result
    uint32_t execution_time;    // Time taken to execute in milliseconds
    uint64_t timestamp;         // When the diagnostic was run
};

// Hardware diagnostic test function type
typedef DiagnosticResult (*DiagnosticTestFn)();

// Hardware diagnostics manager
class HardwareDiagnostics {
private:
    static const uint32_t MAX_DIAGNOSTICS = 64;
    HardwareDiagnostic diagnostics[MAX_DIAGNOSTICS];
    uint32_t diagnostic_count;
    
    // Individual diagnostic tests
    DiagnosticResult RunCpuDiagnostic();
    DiagnosticResult RunMemoryDiagnostic();
    DiagnosticResult RunTimerDiagnostic();
    DiagnosticResult RunPciDiagnostic();
    DiagnosticResult RunBasicSystemDiagnostic();
    
public:
    HardwareDiagnostics();
    ~HardwareDiagnostics();
    
    // Initialize the diagnostics system
    bool Initialize();
    
    // Run all registered diagnostics
    bool RunAllDiagnostics();
    
    // Run a specific diagnostic by type
    DiagnosticResult RunDiagnostic(DiagnosticType type);
    
    // Register a custom diagnostic test
    bool RegisterDiagnostic(DiagnosticType type, const char* name, const char* description, 
                           DiagnosticTestFn test_func);
    
    // Get diagnostic results
    const HardwareDiagnostic* GetDiagnosticResults(uint32_t* count);
    
    // Print diagnostic summary to console
    void PrintDiagnosticSummary();
    
    // Get diagnostic result as string
    const char* ResultToString(DiagnosticResult result);
    
    // Get diagnostic type as string
    const char* TypeToString(DiagnosticType type);
    
    // Perform hardware detection without running diagnostics
    bool DetectHardware();
    
    // Get detected hardware information
    void PrintHardwareInfo();
};

// Global hardware diagnostics instance
extern HardwareDiagnostics* g_hardware_diagnostics;

// Initialize hardware diagnostics system
bool InitializeHardwareDiagnostics();

#endif // _Kernel_HardwareDiagnostics_h_