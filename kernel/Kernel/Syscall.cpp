#include "Kernel.h"

SyscallManager::SyscallManager() {
    // Initialize all handlers to null
    for (int i = 0; i < 256; i++) {
        handlers[i] = nullptr;
    }
}

void SyscallManager::Initialize() {
    // Register system call handlers
    RegisterHandler(SYSCALL_WRITE, SyscallWrite);
    RegisterHandler(SYSCALL_GETPID, SyscallGetpid);
    RegisterHandler(SYSCALL_FORK, SyscallFork);
    RegisterHandler(SYSCALL_EXECVE, SyscallExecve);
    RegisterHandler(SYSCALL_YIELD, SyscallYield);
    
    LOG("System call manager initialized with basic handlers");
}

void SyscallManager::RegisterHandler(uint32 syscall_num, SyscallHandler handler) {
    if (syscall_num < 256) {
        handlers[syscall_num] = handler;
    }
}

uint32 SyscallManager::HandleSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5) {
    if (syscall_num < 256 && handlers[syscall_num] != nullptr) {
        return handlers[syscall_num](arg1, arg2, arg3, arg4, arg5);
    }
    
    LOG("Unknown system call: " << syscall_num);
    return (uint32)-1; // Invalid system call
}

uint32 SyscallManager::SyscallWrite(uint32 fd, uint32 buf, uint32 count, uint32 arg4, uint32 arg5) {
    if (fd == 1 || fd == 2) { // stdout or stderr
        // Convert the buffer to a string and print it
        const char* str = (const char*)buf;
        uint32 i = 0;
        
        // Print up to count characters
        for (i = 0; i < count && str[i] != '\0'; i++) {
            global->monitor->WriteChar(str[i]);
        }
        
        return i; // Return number of characters written
    }
    
    return 0; // For other file descriptors, return 0 for now
}

uint32 SyscallManager::SyscallGetpid(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5) {
    // Return the current process ID
    // For now, return 1 as a placeholder
    return 1;
}

uint32 SyscallManager::SyscallFork(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5) {
    // Fork system call implementation
    LOG("Fork system call called by PID: " << 
        (process_manager->GetCurrentProcess() ? process_manager->GetCurrentProcess()->pid : 0));
    
    if (!process_manager) {
        LOG("ERROR: Process manager not initialized");
        return (uint32)-1;
    }
    
    ProcessControlBlock* parent_pcb = process_manager->GetCurrentProcess();
    if (!parent_pcb) {
        LOG("ERROR: No current process to fork");
        return (uint32)-1;
    }
    
    // In a real implementation, we would:
    // 1. Create a copy of the current process's memory space
    // 2. Create a new PCB with the same content as the parent
    // 3. Return 0 to the child process and the child PID to the parent
    
    // For now, create a new process with the same entry point (which is the current instruction pointer)
    // This is a simplified implementation - in reality, both parent and child continue from fork()
    ProcessControlBlock* child_pcb = process_manager->CreateProcess(
        (void*)parent_pcb->instruction_pointer,  // Same entry point
        parent_pcb->name,                       // Same name for now
        parent_pcb->priority                    // Same priority
    );
    
    if (!child_pcb) {
        LOG("ERROR: Failed to create child process");
        return (uint32)-1;
    }
    
    // Copy parent's memory mappings and other state
    child_pcb->page_directory = parent_pcb->page_directory;  // Copy page directory reference
    child_pcb->heap_start = parent_pcb->heap_start;
    child_pcb->heap_end = parent_pcb->heap_end;
    child_pcb->stack_pointer = parent_pcb->stack_pointer;
    child_pcb->stack_start = parent_pcb->stack_start;
    
    // Set parent PID correctly
    child_pcb->parent_pid = parent_pcb->pid;
    
    // Set initial state to ready
    process_manager->SetProcessState(child_pcb->pid, PROCESS_STATE_READY);
    
    LOG("Created child process with PID: " << child_pcb->pid 
         << ", parent PID: " << child_pcb->parent_pid);
    
    // In a real fork implementation, both parent and child would continue execution,
    // with 0 returned to the child and the child's PID returned to the parent.
    // For this simplified implementation, we'll return different values:
    // For now, just return the child PID to the parent process
    return child_pcb->pid;
}

// Implementation of execve system call
uint32 SyscallManager::SyscallExecve(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5) {
    // Get the current process
    ProcessControlBlock* current_pcb = process_manager->GetCurrentProcess();
    if (!current_pcb) {
        LOG("ERROR: No current process for execve");
        return (uint32)-1;
    }
    
    LOG("Execve system call called by PID: " << current_pcb->pid);
    
    // Parameters:
    // arg1 = filename (char*)
    // arg2 = argv (char** array)
    // arg3 = envp (char** array)
    
    const char* filename = (const char*)arg1;
    if (!filename) {
        LOG("ERROR: Filename is null for execve");
        return (uint32)-1;
    }
    
    LOG("Attempting to execute file: " << filename);
    
    // In a real implementation, we would:
    // 1. Load the executable from the file system
    // 2. Set up a new memory layout for the process
    // 3. Set the instruction pointer to the program entry point
    // 4. Set up initial stack with command line arguments
    
    // For this implementation, we'll just log the operation
    // and return an error since we don't have a file system yet
    
    LOG("Execve not fully implemented - would load executable: " << filename);
    
    // For now, just return an error
    return (uint32)-1; // Not implemented yet
}

// Implementation of vfork system call
uint32 SyscallManager::SyscallVfork(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5) {
    // vfork is similar to fork but with different semantics
    // In vfork, the child shares the parent's address space until exec or _exit
    LOG("Vfork system call called");
    
    if (!process_manager) {
        LOG("ERROR: Process manager not initialized");
        return (uint32)-1;
    }
    
    ProcessControlBlock* parent_pcb = process_manager->GetCurrentProcess();
    if (!parent_pcb) {
        LOG("ERROR: No current process to vfork");
        return (uint32)-1;
    }
    
    // Create a new process PCB but don't copy memory space
    // The child will share the parent's memory space until exec
    ProcessControlBlock* child_pcb = process_manager->CreateProcess(
        (void*)parent_pcb->instruction_pointer,  // Same entry point
        parent_pcb->name,                       // Same name for now
        parent_pcb->priority                    // Same priority
    );
    
    if (!child_pcb) {
        LOG("ERROR: Failed to create child process for vfork");
        return (uint32)-1;
    }
    
    // Share memory space (in a real implementation, this would be more complex)
    child_pcb->page_directory = parent_pcb->page_directory;
    child_pcb->heap_start = parent_pcb->heap_start;
    child_pcb->heap_end = parent_pcb->heap_end;
    child_pcb->stack_pointer = parent_pcb->stack_pointer;
    child_pcb->stack_start = parent_pcb->stack_start;
    
    child_pcb->parent_pid = parent_pcb->pid;
    
    // Set initial state to ready
    process_manager->SetProcessState(child_pcb->pid, PROCESS_STATE_READY);
    
    LOG("Created child process with PID: " << child_pcb->pid 
         << " via vfork, sharing parent memory space");
    
    // Return child PID to parent, 0 to child (simplified implementation)
    return child_pcb->pid;
}

// Implementation of yield system call
uint32 SyscallManager::SyscallYield(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5) {
    LOG("Yield system call called");
    
    if (!process_manager) {
        LOG("ERROR: Process manager not initialized");
        return (uint32)-1;
    }
    
    // Yield control to the scheduler
    bool result = process_manager->YieldCurrentProcess();
    
    if (result) {
        LOG("Process yielded successfully");
        return 0; // Success
    } else {
        LOG("Process yield failed");
        return (uint32)-1; // Failure
    }
}