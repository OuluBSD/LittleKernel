#ifndef _Kernel_Global_h_
#define _Kernel_Global_h_

// Don't include other headers in this file - only the package header should include other headers

// Define PAGE_SIZE for compatibility with code that expects it
#define PAGE_SIZE KERNEL_PAGE_SIZE

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
class DriverFramework;

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
    DriverFramework* driver_framework;             // Add DriverFramework to the global structure
    
    // Boot information
    uint32 placement_address;        // Next free physical address
    uint32 initial_esp;              // Initial stack pointer
    
    // System flags
    bool initialized;

    // Device management
    uint32 next_device_id;           // Next available device ID for assignment

    // Initialize the global structure
    void Initialize();
};

// Global variable declaration
extern Global* global;

// Current process pointer
extern ProcessControlBlock* g_current_process;

#endif