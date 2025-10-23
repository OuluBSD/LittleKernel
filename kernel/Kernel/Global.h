#ifndef _Kernel_Global_h_
#define _Kernel_Global_h_

// Don't include other headers in this file - only the package header should include other headers

// Forward declarations
class Monitor;
class Timer;
class DescriptorTable;
class MemoryManager;
class ProcessManager;
class FileSystem;
class SyscallManager;
class SerialDriver;
class PagingManager;
class SharedMemoryManager;
class MemoryMappingManager;
class MemoryTracker;

// Global System Variables
struct Global {
    Monitor* monitor;
    Timer* timer;
    DescriptorTable* descriptor_table;
    MemoryManager* memory_manager;
    ProcessManager* process_manager;
    FileSystem* file_system;
    SyscallManager* syscall_manager;
    SerialDriver* serial_driver;  // Add SerialDriver to the global structure
    PagingManager* paging_manager;  // Add PagingManager to the global structure
    SharedMemoryManager* shared_memory_manager;  // Add SharedMemoryManager to the global structure
    MemoryMappingManager* memory_mapping_manager;  // Add MemoryMappingManager to the global structure
    MemoryTracker* memory_tracker;                 // Add MemoryTracker to the global structure
    
    // Boot information
    uint32 placement_address;        // Next free physical address
    uint32 initial_esp;              // Initial stack pointer
    
    // System flags
    bool initialized;
    
    // Initialize the global structure
    void Initialize();
};

// Global variable declaration
extern Global* global;

#endif