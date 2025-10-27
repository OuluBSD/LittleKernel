#ifndef _Kernel_Debugging_h_
#define _Kernel_Debugging_h_

#include "Common.h"
#include "Defs.h"
#include "Logging.h"
#include "ProcessControlBlock.h"
#include "Vfs.h"

// Debugging flags
#define DEBUG_FLAG_NONE           0x0000
#define DEBUG_FLAG_INTERRUPTS     0x0001
#define DEBUG_FLAG_MEMORY         0x0002
#define DEBUG_FLAG_PROCESS        0x0004
#define DEBUG_FLAG_FILESYSTEM     0x0008
#define DEBUG_FLAG_DRIVER         0x0010
#define DEBUG_FLAG_SCHEDULING     0x0020
#define DEBUG_FLAG_ALL            0xFFFF

// Maximum number of breakpoints
#define MAX_BREAKPOINTS 64

// Breakpoint types
enum BreakpointType {
    BP_EXECUTION = 0,    // Execute breakpoint
    BP_WRITE,            // Write breakpoint
    BP_READ,             // Read breakpoint
    BP_ACCESS            // Read/Write breakpoint
};

// Breakpoint structure
struct Breakpoint {
    void* address;                    // Address for the breakpoint
    BreakpointType type;              // Type of breakpoint
    uint32 length;                  // Length for watchpoints (1, 2, 4, or 8 bytes)
    bool enabled;                     // Whether the breakpoint is active
    uint32 hit_count;               // Number of times the breakpoint was hit
    const char* description;          // Description of the breakpoint
};

// Memory dump flags
enum MemoryDumpFlags {
    MEM_DUMP_ASCII = 0x01,
    MEM_DUMP_HEX = 0x02,
    MEM_DUMP_BOTH = 0x03
};

// Stack trace entry
struct StackFrame {
    void* return_address;
    void* frame_pointer;
    void* function_start;
    const char* function_name;
};

// Kernel debugger class
class KernelDebugger {
private:
    Breakpoint breakpoints[MAX_BREAKPOINTS];
    uint32 breakpoint_count;
    uint32 active_debug_flags;
    bool debugger_enabled;
    Spinlock debugger_lock;           // Lock for debugger operations
    
public:
    KernelDebugger();
    ~KernelDebugger();
    
    // Initialize the kernel debugger
    bool Initialize();
    
    // Enable/disable the debugger
    void Enable(bool enable);
    bool IsEnabled() { return debugger_enabled; }
    
    // Set debugging flags
    void SetDebugFlags(uint32 flags);
    uint32 GetDebugFlags() { return active_debug_flags; }
    void AddDebugFlag(uint32 flag);
    void RemoveDebugFlag(uint32 flag);
    
    // Breakpoint management
    int SetBreakpoint(void* address, BreakpointType type = BP_EXECUTION, uint32 length = 1, const char* description = nullptr);
    bool RemoveBreakpoint(int bp_id);
    bool RemoveBreakpointAtAddress(void* address);
    bool EnableBreakpoint(int bp_id);
    bool DisableBreakpoint(int bp_id);
    Breakpoint* GetBreakpoint(int bp_id);
    int FindBreakpoint(void* address);
    
    // Check if address has a breakpoint
    bool HasBreakpoint(void* address);
    
    // Memory inspection
    bool ReadMemory(void* address, void* buffer, uint32 size);
    bool WriteMemory(void* address, const void* buffer, uint32 size);
    void DumpMemory(void* address, uint32 size, MemoryDumpFlags flags = MEM_DUMP_BOTH);
    
    // Register inspection
    void DumpRegisters();
    
    // Stack trace
    uint32 GetStackTrace(StackFrame* frames, uint32 max_frames);
    void PrintStackTrace();
    
    // Process debugging
    void DumpProcessList();
    void DumpProcessInfo(ProcessControlBlock* pcb);
    void DumpCurrentProcess();
    
    // System state dump
    void DumpSystemState();
    void DumpMemoryLayout();
    void DumpFilesystemState();
    void DumpDriverState();
    
    // Breakpoint hit handler
    bool HandleBreakpoint(void* address);
    
    // Debugging helpers
    void BreakpointTrapHandler();
    void PrintDebugInfo();
    
    // Kernel panic and crash handling
    void Panic(const char* message, const char* file = nullptr, uint32 line = 0);
    void CrashDump();

private:
    // Internal helper functions
    int FindFreeBreakpointSlot();
    void ExecuteBreakpointAction(int bp_id);
    void LogBreakpointHit(int bp_id, void* address);
    void SanitizeString(char* str, uint32 max_len);
};

// Global debugger instance
extern KernelDebugger* g_kernel_debugger;

// Initialize the kernel debugger
bool InitializeDebugger();

// Debugging macros
#define DEBUG_BREAK() do { if (g_kernel_debugger && g_kernel_debugger->IsEnabled()) g_kernel_debugger->BreakpointTrapHandler(); } while(0)
#define DEBUG_PRINT(...) do { if (g_kernel_debugger && g_kernel_debugger->IsEnabled()) LOG(__VA_ARGS__); } while(0)
#define DEBUG_LOG_IF(flag, ...) do { if (g_kernel_debugger && g_kernel_debugger->IsEnabled() && (g_kernel_debugger->GetDebugFlags() & flag)) LOG(__VA_ARGS__); } while(0)

// Panic macro
#define KERNEL_PANIC(msg) g_kernel_debugger->Panic(msg, __FILE__, __LINE__)

#endif