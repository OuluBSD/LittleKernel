#ifndef _Kernel_Ipc_h_
#define _Kernel_Ipc_h_

#include "Defs.h"
#include "ProcessControlBlock.h"

// Forward declarations
struct ProcessControlBlock;

// Structure for a pipe
struct Pipe {
    char* buffer;               // Buffer for pipe data
    uint32 size;               // Size of the buffer
    uint32 read_pos;           // Current read position
    uint32 write_pos;          // Current write position
    uint32 data_count;         // Number of bytes currently in the pipe
    bool is_blocking;          // Whether read/write operations block
    
    // Process queues for blocking operations
    ProcessControlBlock* readers_waiting;  // Processes waiting to read
    ProcessControlBlock* writers_waiting;  // Processes waiting to write
    
    // Constructor
    Pipe(uint32 buffer_size, bool blocking = true);
    ~Pipe();
};

// Structure for shared memory region
struct SharedMemory {
    void* address;              // Address of shared memory block
    uint32 size;               // Size of the shared memory block
    uint32 ref_count;          // Number of processes using this block
    uint32 owner_pid;          // PID of the process that created it
    uint32 permissions;        // Access permissions (read/write)
    
    // Constructor
    SharedMemory(void* addr, uint32 sz, uint32 owner);
};

// Structure for a signal
struct Signal {
    uint32 signal_number;      // Signal identifier
    void (*handler)(uint32);   // Signal handler function pointer
    
    // Constructor
    Signal(uint32 num, void (*h)(uint32));
};

// Class for IPC manager
class IpcManager {
private:
    // These would be lists of IPC objects in a real implementation
    
public:
    IpcManager();
    ~IpcManager();
    
    // Pipe operations
    Pipe* CreatePipe(uint32 size, bool blocking = true);
    bool DestroyPipe(Pipe* pipe);
    int32 PipeRead(Pipe* pipe, void* buffer, uint32 count);
    int32 PipeWrite(Pipe* pipe, const void* buffer, uint32 count);
    
    // Shared memory operations
    SharedMemory* CreateSharedMemory(uint32 size, uint32 permissions = 0x7); // rwx by default
    bool AttachSharedMemory(SharedMemory* shm, uint32 pid);
    bool DetachSharedMemory(SharedMemory* shm, uint32 pid);
    bool DestroySharedMemory(SharedMemory* shm);
    
    // Signal operations
    bool SendSignal(uint32 pid, uint32 signal_num);
    bool RegisterSignalHandler(uint32 signal_num, void (*handler)(uint32));
    bool DeliverSignal(uint32 pid, uint32 signal_num);
};

extern IpcManager* ipc_manager;

#endif