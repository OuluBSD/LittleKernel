#ifndef _Kernel_Syscall_h_
#define _Kernel_Syscall_h_

// Forward declarations
class ProcessManager;

// Don't include other headers in this file - only the package header should include other headers

// System call numbers
#define SYSCALL_EXIT 0
#define SYSCALL_WRITE 1
#define SYSCALL_READ 2
#define SYSCALL_OPEN 3
#define SYSCALL_CLOSE 4
#define SYSCALL_FORK 5
#define SYSCALL_EXECVE 6
#define SYSCALL_GETPID 7
#define SYSCALL_YIELD 8

// System call function type
typedef int (*SyscallHandler)(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);

class SyscallManager {
private:
    SyscallHandler handlers[256];
    
public:
    SyscallManager();
    void Initialize();
    void RegisterHandler(uint32 syscall_num, SyscallHandler handler);
    int HandleSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    
    // Common system call handlers
    static int SyscallWrite(uint32 fd, uint32 buf, uint32 count, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SyscallGetpid(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SyscallFork(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SyscallExecve(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SyscallVfork(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SyscallYield(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
};

#endif