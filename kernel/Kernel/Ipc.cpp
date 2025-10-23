#include "Ipc.h"
#include "Logging.h"
#include "Global.h"
#include "MemoryManager.h"
#include "ProcessControlBlock.h"

// Global IPC manager instance
IpcManager* ipc_manager = nullptr;

// Pipe constructor
Pipe::Pipe(uint32 buffer_size, bool blocking) {
    size = buffer_size;
    buffer = (char*)malloc(size);
    if (!buffer) {
        LOG("Failed to allocate buffer for pipe");
        size = 0;
        return;
    }
    
    read_pos = 0;
    write_pos = 0;
    data_count = 0;
    is_blocking = blocking;
    readers_waiting = nullptr;
    writers_waiting = nullptr;
}

// Pipe destructor
Pipe::~Pipe() {
    if (buffer) {
        free(buffer);
        buffer = nullptr;
    }
    size = 0;
}

// SharedMemory constructor
SharedMemory::SharedMemory(void* addr, uint32 sz, uint32 owner) {
    address = addr;
    size = sz;
    ref_count = 1;  // At least the creator is using it
    owner_pid = owner;
    permissions = 0x7;  // Default to read/write/execute
}

// Signal constructor
Signal::Signal(uint32 num, void (*h)(uint32)) {
    signal_number = num;
    handler = h;
}

// IpcManager constructor
IpcManager::IpcManager() {
    LOG("IPC manager initialized");
}

// IpcManager destructor
IpcManager::~IpcManager() {
    LOG("IPC manager destroyed");
}

// Pipe operations
Pipe* IpcManager::CreatePipe(uint32 size, bool blocking) {
    Pipe* pipe = new Pipe(size, blocking);
    if (!pipe || !pipe->buffer) {
        LOG("Failed to create pipe with size: " << size);
        if (pipe) {
            delete pipe;
        }
        return nullptr;
    }
    
    LOG("Created pipe with size: " << size << ", blocking: " << blocking);
    return pipe;
}

bool IpcManager::DestroyPipe(Pipe* pipe) {
    if (!pipe) {
        return false;
    }
    
    // Wake up any waiting processes
    // In a real implementation, we would wake up processes in the waiting lists
    
    delete pipe;
    LOG("Destroyed pipe");
    return true;
}

int32 IpcManager::PipeRead(Pipe* pipe, void* buffer, uint32 count) {
    if (!pipe || !buffer || count == 0) {
        LOG("Invalid parameters for pipe read");
        return -1;
    }
    
    ProcessControlBlock* current_process = process_manager->GetCurrentProcess();
    if (!current_process) {
        LOG("No current process for pipe read");
        return -1;
    }
    
    // If no data and blocking, wait for data
    if (pipe->data_count == 0) {
        if (pipe->is_blocking) {
            // Add to readers waiting list
            ProcessControlBlock* current = process_manager->GetCurrentProcess();
            current->next = pipe->readers_waiting;
            pipe->readers_waiting = current;
            
            // Set state to waiting
            process_manager->SetProcessState(current_process->pid, PROCESS_STATE_WAITING);
            
            // Yield processor
            process_manager->YieldCurrentProcess();
            
            // When process resumes, try read again (or handle any interruptions)
        } else {
            // Non-blocking - return immediately
            return 0;
        }
    }
    
    // Perform the read operation
    uint32 bytes_to_read = (count < pipe->data_count) ? count : pipe->data_count;
    char* buf = (char*)buffer;
    
    for (uint32 i = 0; i < bytes_to_read; i++) {
        buf[i] = pipe->buffer[pipe->read_pos];
        pipe->read_pos = (pipe->read_pos + 1) % pipe->size;
    }
    
    pipe->data_count -= bytes_to_read;
    
    // If there are writers waiting, wake them up
    if (pipe->writers_waiting) {
        ProcessControlBlock* next_writer = pipe->writers_waiting->next;
        process_manager->SetProcessState(pipe->writers_waiting->pid, PROCESS_STATE_READY);
        pipe->writers_waiting = next_writer;
    }
    
    LOG("Read " << bytes_to_read << " bytes from pipe");
    return bytes_to_read;
}

int32 IpcManager::PipeWrite(Pipe* pipe, const void* buffer, uint32 count) {
    if (!pipe || !buffer || count == 0) {
        LOG("Invalid parameters for pipe write");
        return -1;
    }
    
    ProcessControlBlock* current_process = process_manager->GetCurrentProcess();
    if (!current_process) {
        LOG("No current process for pipe write");
        return -1;
    }
    
    // If not enough space and blocking, wait for space
    uint32 free_space = pipe->size - pipe->data_count;
    if (count > free_space) {
        if (pipe->is_blocking) {
            // Add to writers waiting list
            ProcessControlBlock* current = process_manager->GetCurrentProcess();
            current->next = pipe->writers_waiting;
            pipe->writers_waiting = current;
            
            // Set state to waiting
            process_manager->SetProcessState(current_process->pid, PROCESS_STATE_WAITING);
            
            // Yield processor
            process_manager->YieldCurrentProcess();
            
            // When process resumes, try write again
            free_space = pipe->size - pipe->data_count;
            if (count > free_space) {
                // Still not enough space, return error
                return -1;
            }
        } else {
            // Non-blocking - write what we can
            count = free_space;
        }
    }
    
    // Perform the write operation
    const char* buf = (const char*)buffer;
    uint32 bytes_to_write = (count < free_space) ? count : free_space;
    
    for (uint32 i = 0; i < bytes_to_write; i++) {
        pipe->buffer[pipe->write_pos] = buf[i];
        pipe->write_pos = (pipe->write_pos + 1) % pipe->size;
    }
    
    pipe->data_count += bytes_to_write;
    
    // If there are readers waiting, wake them up
    if (pipe->readers_waiting) {
        ProcessControlBlock* next_reader = pipe->readers_waiting->next;
        process_manager->SetProcessState(pipe->readers_waiting->pid, PROCESS_STATE_READY);
        pipe->readers_waiting = next_reader;
    }
    
    LOG("Wrote " << bytes_to_write << " bytes to pipe");
    return bytes_to_write;
}

// Shared memory operations
SharedMemory* IpcManager::CreateSharedMemory(uint32 size, uint32 permissions) {
    // Allocate memory for the shared region
    void* addr = malloc(size);
    if (!addr) {
        LOG("Failed to allocate shared memory of size: " << size);
        return nullptr;
    }
    
    ProcessControlBlock* current_process = process_manager->GetCurrentProcess();
    if (!current_process) {
        LOG("No current process to create shared memory");
        free(addr);
        return nullptr;
    }
    
    SharedMemory* shm = new SharedMemory(addr, size, current_process->pid);
    if (!shm) {
        LOG("Failed to create SharedMemory structure");
        free(addr);
        return nullptr;
    }
    
    shm->permissions = permissions;
    
    LOG("Created shared memory of size: " << size << " at address: " << (uint32)addr);
    return shm;
}

bool IpcManager::AttachSharedMemory(SharedMemory* shm, uint32 pid) {
    if (!shm) {
        LOG("Invalid shared memory to attach");
        return false;
    }
    
    // In a real implementation, we would set up proper virtual memory mappings
    // for the process to access the shared memory
    
    shm->ref_count++;
    LOG("Attached shared memory to process " << pid << ", ref count now: " << shm->ref_count);
    return true;
}

bool IpcManager::DetachSharedMemory(SharedMemory* shm, uint32 pid) {
    if (!shm) {
        LOG("Invalid shared memory to detach");
        return false;
    }
    
    if (shm->ref_count > 0) {
        shm->ref_count--;
        LOG("Detached shared memory from process " << pid << ", ref count now: " << shm->ref_count);
        
        if (shm->ref_count == 0) {
            // No more references, can destroy the shared memory
            LOG("Last reference to shared memory removed, destroying");
            DestroySharedMemory(shm);
        }
        return true;
    }
    
    LOG("Attempted to detach non-referenced shared memory");
    return false;
}

bool IpcManager::DestroySharedMemory(SharedMemory* shm) {
    if (!shm) {
        return false;
    }
    
    // Free the shared memory block
    if (shm->address) {
        free(shm->address);
    }
    
    delete shm;
    LOG("Destroyed shared memory block");
    return true;
}

// Signal operations
bool IpcManager::SendSignal(uint32 pid, uint32 signal_num) {
    ProcessControlBlock* target_process = process_manager->GetProcessById(pid);
    if (!target_process) {
        LOG("Attempted to send signal " << signal_num << " to non-existent process " << pid);
        return false;
    }
    
    // In a real implementation, we would place the signal in the target's signal queue
    // and potentially interrupt the process to deliver the signal
    
    LOG("Queued signal " << signal_num << " for process " << pid);
    return true;
}

bool IpcManager::RegisterSignalHandler(uint32 signal_num, void (*handler)(uint32)) {
    // In a real implementation, we would register the signal handler for the current process
    LOG("Registered signal handler for signal " << signal_num);
    return true;
}

bool IpcManager::DeliverSignal(uint32 pid, uint32 signal_num) {
    ProcessControlBlock* target_process = process_manager->GetProcessById(pid);
    if (!target_process) {
        LOG("Attempted to deliver signal " << signal_num << " to non-existent process " << pid);
        return false;
    }
    
    // In a real implementation, we would deliver the signal by either:
    // 1. Interrupting the process and calling its signal handler
    // 2. Queuing the signal for later delivery
    
    LOG("Delivered signal " << signal_num << " to process " << pid);
    return true;
}