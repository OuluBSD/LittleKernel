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
    // Fork system call - not fully implemented yet
    LOG("Fork system call called");
    return 0; // Placeholder: return 0 in child, process ID in parent
}