#ifndef _Kernel_AbiMultiplexer_h_
#define _Kernel_AbiMultiplexer_h_

#include "Common.h"
#include "Defs.h"
#include "ProcessControlBlock.h"

// System Call Interface (SCI) types supported by the kernel
enum SciType {
    SCI_UNKNOWN = 0,      // Unknown or uninitialized SCI
    DOS_SCI_V1,          // DOS interrupt-based SCI (INT 21h, etc.)
    DOS_SCI_V2,          // DOS syscall-based SCI (SYSCALL instruction)
    LINUXULATOR,         // Linux-compatible SCI
    NATIVE,              // Native LittleKernel SCI
    MAX_SCI_TYPES        // Keep this as the last entry
};

// SCI-specific context data structure
struct SciContext {
    SciType type;           // SCI type
    void* context_data;     // SCI-specific context data
    uint32 sci_flags;       // SCI-specific flags
};

// Function pointer types for SCI-specific handlers
typedef int (*SyscallHandler)(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);

// SCI-specific syscall table
struct SciSyscallTable {
    SyscallHandler* handlers;
    uint32 max_syscall_num;
    const char** names;     // Names for debugging
};

// Main SCI multiplexer class
class SciMultiplexer {
private:
    SciSyscallTable* sci_tables[MAX_SCI_TYPES];  // Syscall tables for each SCI
    bool initialized;                            // Initialization status

public:
    SciMultiplexer();
    ~SciMultiplexer();
    
    // Initialize the multiplexer
    bool Initialize();
    
    // Register a syscall table for an SCI type
    bool RegisterSciSyscalls(SciType type, SciSyscallTable* table);
    
    // Dispatch a syscall to the appropriate SCI handler
    int DispatchSyscall(SciType type, uint32 syscall_num, 
                       uint32 arg1, uint32 arg2, uint32 arg3, 
                       uint32 arg4, uint32 arg5, uint32 arg6);
    
    // Get the current process SCI type
    SciType GetCurrentProcessSci();
    
    // Set the SCI type for a process
    bool SetProcessSci(ProcessControlBlock* pcb, SciType type);
    
    // Get the SCI type for a process
    SciType GetProcessSci(ProcessControlBlock* pcb);
    
    // Get the SCI context for a process
    SciContext* GetProcessSciContext(ProcessControlBlock* pcb);
    
    // Create an SCI context for a process
    SciContext* CreateSciContext(SciType type);
    
    // Destroy an SCI context
    void DestroySciContext(SciContext* context);
    
    // Translate DOS path to Unix path (for DOS SCI)
    bool ConvertDosPathToUnix(const char* dos_path, char* unix_path, uint32 max_len);
    
    // Translate Unix path to DOS path (for DOS SCI)
    bool ConvertUnixPathToDos(const char* unix_path, char* dos_path, uint32 max_len);
    
    // Set up default drive mappings (for DOS SCI)
    bool SetupDosDriveMappings();
    
    // Load a DOS executable
    ProcessControlBlock* LoadDosExecutable(const char* filename, char* const argv[], char* const envp[]);
    
    // Load a Linux executable
    ProcessControlBlock* LoadLinuxExecutable(const char* filename, char* const argv[], char* const envp[]);
    
    // Load a native executable
    ProcessControlBlock* LoadNativeExecutable(const char* filename, char* const argv[], char* const envp[]);
    
    // Auto-detect executable type and load appropriately
    ProcessControlBlock* LoadExecutable(const char* filename, char* const argv[], char* const envp[]);
};

// Global instance of the SCI multiplexer
extern SciMultiplexer* g_sci_multiplexer;

// Initialize the SCI multiplexer
bool InitializeSciMultiplexer();

// Main syscall dispatcher function
extern "C" int HandleMultiplexedSyscall(uint32 syscall_num, 
                                       uint32 arg1, uint32 arg2, uint32 arg3, 
                                       uint32 arg4, uint32 arg5, uint32 arg6);

// Determine SCI type from executable format
SciType DetectSciTypeFromExecutable(const char* filename);

// SCI-specific initialization functions
bool InitializeDosSciV1();
bool InitializeDosSciV2();
bool InitializeLinuxulatorSci();

#endif