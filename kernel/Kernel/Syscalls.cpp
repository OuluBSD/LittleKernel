#include "Kernel.h"
#include "Syscalls.h"
#include "Logging.h"
#include "Vfs.h"

// External instance of the system call interface
SyscallInterface* g_syscall_interface = nullptr;

SyscallInterface::SyscallInterface() {
    memset(&syscall_table, 0, sizeof(syscall_table));
    syscall_table.count = 334;  // Size of our syscall table
    syscall_lock.Initialize();
    
    // Initialize the syscall table
    InitializeSyscallTable();
}

SyscallInterface::~SyscallInterface() {
    // Cleanup handled by kernel shutdown
}

bool SyscallInterface::Initialize() {
    LOG("Initializing system call interface");
    
    // The table is already initialized in the constructor
    // Register all the implemented system calls
    
    // File operations
    RegisterSyscall(SYS_READ, SysReadWrapper, "read");
    RegisterSyscall(SYS_WRITE, SysWriteWrapper, "write");
    RegisterSyscall(SYS_OPEN, SysOpenWrapper, "open");
    RegisterSyscall(SYS_CLOSE, SysCloseWrapper, "close");
    RegisterSyscall(SYS_STAT, SysStatWrapper, "stat");
    RegisterSyscall(SYS_FSTAT, SysFstatWrapper, "fstat");
    RegisterSyscall(SYS_LSEEK, SysLseekWrapper, "lseek");
    
    // Process operations
    RegisterSyscall(SYS_FORK, SysForkWrapper, "fork");
    RegisterSyscall(SYS_EXECVE, SysExecveWrapper, "execve");
    RegisterSyscall(SYS_WAITPID, SysWaitPidWrapper, "waitpid");
    RegisterSyscall(SYS_GETPID, SysGetPidWrapper, "getpid");
    RegisterSyscall(SYS_EXIT, SysExitWrapper, "exit");
    RegisterSyscall(SYS_KILL, SysKillWrapper, "kill");
    
    // Directory operations
    RegisterSyscall(SYS_MKDIR, SysMkdirWrapper, "mkdir");
    RegisterSyscall(SYS_RMDIR, SysRmdirWrapper, "rmdir");
    RegisterSyscall(SYS_UNLINK, SysUnlinkWrapper, "unlink");
    RegisterSyscall(SYS_RENAME, SysRenameWrapper, "rename");
    RegisterSyscall(SYS_CHDIR, SysChdirWrapper, "chdir");
    RegisterSyscall(SYS_GETCWD, SysGetcwdWrapper, "getcwd");
    
    // Memory operations
    RegisterSyscall(SYS_BRK, SysBrkWrapper, "brk");
    RegisterSyscall(SYS_MMAP, SysMmapWrapper, "mmap");
    RegisterSyscall(SYS_MUNMAP, SysMunmapWrapper, "munmap");
    
    // IPC operations
    RegisterSyscall(SYS_PIPE, SysPipeWrapper, "pipe");
    RegisterSyscall(SYS_DUP, SysDupWrapper, "dup");
    RegisterSyscall(SYS_DUP2, SysDup2Wrapper, "dup2");
    
    // Signal operations
    RegisterSyscall(SYS_SIGNAL, SysSignalWrapper, "signal");
    RegisterSyscall(SYS_SIGACTION, SysSigactionWrapper, "sigaction");
    
    // Network operations
    RegisterSyscall(SYS_SOCKET, SysSocketWrapper, "socket");
    RegisterSyscall(SYS_BIND, SysBindWrapper, "bind");
    RegisterSyscall(SYS_CONNECT, SysConnectWrapper, "connect");
    RegisterSyscall(SYS_LISTEN, SysListenWrapper, "listen");
    RegisterSyscall(SYS_ACCEPT, SysAcceptWrapper, "accept");
    RegisterSyscall(SYS_SENDTO, SysSendtoWrapper, "sendto");
    RegisterSyscall(SYS_RECVFROM, SysRecvfromWrapper, "recvfrom");
    RegisterSyscall(SYS_SENDMSG, SysSendmsgWrapper, "sendmsg");
    RegisterSyscall(SYS_RECVMSG, SysRecvmsgWrapper, "recvmsg");
    RegisterSyscall(SYS_SHUTDOWN, SysShutdownWrapper, "shutdown");
    RegisterSyscall(SYS_SETSOCKOPT, SysSetsockoptWrapper, "setsockopt");
    RegisterSyscall(SYS_GETSOCKOPT, SysGetsockoptWrapper, "getsockopt");
    RegisterSyscall(SYS_RECV, SysRecvWrapper, "recv");
    RegisterSyscall(SYS_SEND, SysSendWrapper, "send");
    
    // System information
    RegisterSyscall(SYS_UNAME, SysUnameWrapper, "uname");
    RegisterSyscall(SYS_GETTIMEOFDAY, SysGettimeofdayWrapper, "gettimeofday");
    
    LOG("System call interface initialized with " << syscall_table.count << " entries");
    return true;
}

bool SyscallInterface::RegisterSyscall(uint32 syscall_num, syscall_func_t func, const char* name) {
    if (syscall_num >= syscall_table.count) {
        return false;
    }
    
    syscall_lock.Acquire();
    syscall_table.functions[syscall_num] = func;
    syscall_table.names[syscall_num] = name ? name : "unknown";
    syscall_lock.Release();
    
    return true;
}

int SyscallInterface::DispatchSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (syscall_num >= syscall_table.count) {
        LOG("Invalid system call number: " << syscall_num);
        return -1;
    }
    
    syscall_func_t func = syscall_table.functions[syscall_num];
    if (!func) {
        LOG("Unimplemented system call: " << syscall_num << " (" << syscall_table.names[syscall_num] << ")");
        return -1;
    }
    
    // Call the system call function with the arguments
    int result = func(arg1, arg2, arg3, arg4, arg5, arg6);
    
    return result;
}

const char* SyscallInterface::GetSyscallName(uint32 syscall_num) {
    if (syscall_num >= syscall_table.count) {
        return "invalid";
    }
    
    return syscall_table.names[syscall_num] ? syscall_table.names[syscall_num] : "unknown";
}

// Individual system call implementations
int SyscallInterface::SysRead(int fd, void* buf, size_t count) {
    if (!g_vfs) {
        return -1;
    }
    
    return g_vfs->Read(fd, buf, count);
}

int SyscallInterface::SysWrite(int fd, const void* buf, size_t count) {
    if (!g_vfs) {
        return -1;
    }
    
    return g_vfs->Write(fd, (void*)buf, count);
}

int SyscallInterface::SysOpen(const char* pathname, int flags, mode_t mode) {
    if (!pathname || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Open(pathname, flags);
}

int SyscallInterface::SysClose(int fd) {
    if (!g_vfs) {
        return -1;
    }
    
    return g_vfs->Close(fd);
}

int SyscallInterface::SysFork() {
    if (!process_manager) {
        LOG("Process manager not available for fork");
        return -1;
    }
    
    // Create a new process that's a copy of the current one
    // This is a simplified implementation
    ProcessControlBlock* parent_pcb = g_current_process;
    if (!parent_pcb) {
        LOG("No current process for fork");
        return -1;
    }
    
    // In a real implementation, we would:
    // 1. Create a new PCB
    // 2. Copy the parent's memory space
    // 3. Set up registers appropriately for the child
    // 4. Return different values in parent and child
    
    LOG("Fork system call not fully implemented yet");
    
    // For now, return -1 to indicate unimplemented
    return -1;
}

int SyscallInterface::SysExecve(const char* filename, char* const argv[], char* const envp[]) {
    if (!filename || !process_manager) {
        LOG("Invalid parameters for execve");
        return -1;
    }
    
    LOG("Execve system call not implemented yet (filename: " << filename << ")");
    
    // For now, return -1 to indicate unimplemented
    return -1;
}

int SyscallInterface::SysWaitPid(pid_t pid, int* status, int options) {
    if (!process_manager) {
        LOG("Process manager not available for waitpid");
        return -1;
    }
    
    LOG("WaitPid system call not implemented yet (pid: " << pid << ")");
    
    // For now, return -1 to indicate unimplemented
    return -1;
}

int SyscallInterface::SysGetPid() {
    // Get the current process ID
    // For now, return a dummy value; in a real implementation, 
    // this would get the PID from the current process structure
    if (g_current_process) {
        return g_current_process->pid;
    }
    return 1; // Default to PID 1 if no current process
}

int SyscallInterface::SysMmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    // Memory mapping implementation
    // For now, return an error as this requires complex implementation
    LOG("Mmap system call not implemented yet");
    return -1;
}

int SyscallInterface::SysMunmap(void* addr, size_t length) {
    // Memory unmapping implementation
    LOG("Munmap system call not implemented yet");
    return -1;
}

int SyscallInterface::SysExit(int status) {
    // Exit the current process
    LOG("Process exiting with status: " << status);
    
    // In a real implementation, this would terminate the current process
    // For now, we'll just return
    return 0;
}

int SyscallInterface::SysKill(pid_t pid, int sig) {
    // Send a signal to a process
    LOG("Kill system call not implemented yet (pid: " << pid << ", sig: " << sig << ")");
    return -1;
}

int SyscallInterface::SysStat(const char* pathname, struct FileStat* statbuf) {
    if (!pathname || !statbuf || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Stat(pathname, statbuf);
}

int SyscallInterface::SysFstat(int fd, struct FileStat* statbuf) {
    LOG("Fstat system call not implemented yet");
    return -1;
}

int SyscallInterface::SysLseek(int fd, off_t offset, int whence) {
    if (!g_vfs) {
        return -1;
    }
    
    return g_vfs->Seek(fd, offset, whence);
}

int SyscallInterface::SysMkdir(const char* pathname, mode_t mode) {
    if (!pathname || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Mkdir(pathname, mode);
}

int SyscallInterface::SysRmdir(const char* pathname) {
    if (!pathname || !g_vfs) {
        return -1;
    }
    
    // For now, use the Unlink function which should handle directories
    return g_vfs->Unlink(pathname);
}

int SyscallInterface::SysUnlink(const char* pathname) {
    if (!pathname || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Unlink(pathname);
}

int SyscallInterface::SysRename(const char* oldpath, const char* newpath) {
    LOG("Rename system call not implemented yet (old: " << oldpath << ", new: " << newpath << ")");
    return -1;
}

int SyscallInterface::SysGettimeofday(struct timeval* tv, struct timezone* tz) {
    if (!tv) {
        return -1;
    }
    
    // Get current time - in a real implementation, this would use the system timer
    if (global_timer) {
        uint64_t ticks = global_timer->GetTickCount();
        tv->tv_sec = ticks / global_timer->GetFrequency();  // Convert ticks to seconds
        // Calculate microseconds from remaining ticks
        tv->tv_usec = ((ticks % global_timer->GetFrequency()) * 1000000) / global_timer->GetFrequency();
    } else {
        tv->tv_sec = 0;
        tv->tv_usec = 0;
    }
    
    // Timezone not implemented
    if (tz) {
        tz->tz_minuteswest = 0;
        tz->tz_dsttime = 0;
    }
    
    return 0;
}

int SyscallInterface::SysBrk(void* addr) {
    LOG("Brk system call not implemented yet (addr: " << (uint32)addr << ")");
    return -1;
}

int SyscallInterface::SysSignal(int signum, void (*handler)(int)) {
    LOG("Signal system call not implemented yet (signum: " << signum << ", handler: " << (uint32)handler << ")");
    return -1;
}

int SyscallInterface::SysSigaction(int signum, const struct sigaction* act, struct sigaction* oldact) {
    LOG("Sigaction system call not implemented yet (signum: " << signum << ")");
    return -1;
}

int SyscallInterface::SysPipe(int pipefd[2]) {
    if (!pipefd) {
        return -1;
    }
    
    if (!ipc_manager) {
        return -1;
    }
    
    Pipe* pipe = ipc_manager->CreatePipe(4096, false);  // 4KB pipe, non-blocking
    if (!pipe) {
        return -1;
    }
    
    // The pipe object contains two file descriptors
    // For now, we'll return a dummy implementation
    // In a real implementation, we'd need to register the pipe with the VFS
    pipefd[0] = 0;  // Reading end
    pipefd[1] = 1;  // Writing end
    
    LOG("Pipe system call not fully implemented yet");
    return 0;
}

int SyscallInterface::SysDup(int oldfd) {
    LOG("Dup system call not implemented yet (oldfd: " << oldfd << ")");
    return -1;
}

int SyscallInterface::SysDup2(int oldfd, int newfd) {
    LOG("Dup2 system call not implemented yet (oldfd: " << oldfd << ", newfd: " << newfd << ")");
    return -1;
}

int SyscallInterface::SysChdir(const char* path) {
    if (!path || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Chdir(path);
}

int SyscallInterface::SysGetcwd(char* buf, size_t size) {
    if (!buf || size == 0 || !g_vfs) {
        return -1;
    }
    
    const char* cwd = g_vfs->GetCwd();
    if (strlen(cwd) >= size) {
        return -1;  // Buffer too small
    }
    
    strcpy_safe(buf, cwd, size);
    return 0;
}

int SyscallInterface::SysUname(struct utsname* buf) {
    if (!buf) {
        return -1;
    }
    
    // Fill in system information (Linux-compatible)
    strcpy_safe(buf->sysname, "LittleKernel", sizeof(buf->sysname));
    strcpy_safe(buf->nodename, "localhost", sizeof(buf->nodename));
    strcpy_safe(buf->release, "1.0.0", sizeof(buf->release));
    strcpy_safe(buf->version, "LittleKernel 1.0", sizeof(buf->version));
    strcpy_safe(buf->machine, "i686", sizeof(buf->machine));
    
    return 0;
}

void SyscallInterface::InitializeSyscallTable() {
    // Initialize all function pointers to the default handler
    for (int i = 0; i < 334; i++) {
        syscall_table.functions[i] = DefaultHandler;
        syscall_table.names[i] = "unimplemented";
    }
}

int SyscallInterface::DefaultHandler(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    // This is a placeholder for unimplemented system calls
    return -1;
}

bool InitializeSyscalls() {
    if (!g_syscall_interface) {
        g_syscall_interface = new SyscallInterface();
        if (!g_syscall_interface) {
            LOG("Failed to create syscall interface instance");
            return false;
        }
        
        if (!g_syscall_interface->Initialize()) {
            LOG("Failed to initialize syscall interface");
            delete g_syscall_interface;
            g_syscall_interface = nullptr;
            return false;
        }
        
        LOG("System call interface initialized successfully");
    }
    
    return true;
}

extern "C" int HandleSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) {
        return -1;
    }
    
    // In a real implementation, we'd need to make this call work with the C++ member function
    // For now, we'll call the dispatch method directly
    // This is a simplified implementation that doesn't properly handle the C++ member function call
    
    // Log the system call for debugging
    DLOG("System call: " << g_syscall_interface->GetSyscallName(syscall_num) 
         << " (num: " << syscall_num << ")");
    
    return g_syscall_interface->DispatchSyscall(syscall_num, arg1, arg2, arg3, arg4, arg5, arg6);
}

pid_t GetCurrentProcessId() {
    if (g_current_process) {
        return g_current_process->pid;
    }
    return 1;  // Default to PID 1 if no current process
}

uid_t GetCurrentUserId() {
    // For now, return a default user ID
    // In a real implementation, this would come from the process structure
    return 0;  // Root user
}

gid_t GetCurrentGroupId() {
    // For now, return a default group ID
    return 0;  // Root group
}

// Wrapper implementations for system calls
int SyscallInterface::SysReadWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysRead((int)arg1, (void*)arg2, (size_t)arg3);
}

int SyscallInterface::SysWriteWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysWrite((int)arg1, (const void*)arg2, (size_t)arg3);
}

int SyscallInterface::SysOpenWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysOpen((const char*)arg1, (int)arg2, (mode_t)arg3);
}

int SyscallInterface::SysCloseWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysClose((int)arg1);
}

int SyscallInterface::SysStatWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysStat((const char*)arg1, (struct FileStat*)arg2);
}

int SyscallInterface::SysFstatWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysFstat((int)arg1, (struct FileStat*)arg2);
}

int SyscallInterface::SysLseekWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysLseek((int)arg1, (off_t)arg2, (int)arg3);
}

int SyscallInterface::SysGetPidWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysGetPid();
}

int SyscallInterface::SysExitWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysExit((int)arg1);
}

int SyscallInterface::SysKillWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysKill((pid_t)arg1, (int)arg2);
}

int SyscallInterface::SysMkdirWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysMkdir((const char*)arg1, (mode_t)arg2);
}

int SyscallInterface::SysRmdirWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysRmdir((const char*)arg1);
}

int SyscallInterface::SysUnlinkWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysUnlink((const char*)arg1);
}

int SyscallInterface::SysRenameWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysRename((const char*)arg1, (const char*)arg2);
}

int SyscallInterface::SysChdirWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysChdir((const char*)arg1);
}

int SyscallInterface::SysGetcwdWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysGetcwd((char*)arg1, (size_t)arg2);
}

int SyscallInterface::SysBrkWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysBrk((void*)arg1);
}

int SyscallInterface::SysMmapWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysMmap((void*)arg1, (size_t)arg2, (int)arg3, (int)arg4, (int)arg5, (off_t)arg6);
}

int SyscallInterface::SysMunmapWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysMunmap((void*)arg1, (size_t)arg2);
}

int SyscallInterface::SysPipeWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysPipe((int*)arg1);
}

int SyscallInterface::SysDupWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysDup((int)arg1);
}

int SyscallInterface::SysDup2Wrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysDup2((int)arg1, (int)arg2);
}

int SyscallInterface::SysUnameWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysUname((struct utsname*)arg1);
}

int SyscallInterface::SysGettimeofdayWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysGettimeofday((struct timeval*)arg1, (struct timezone*)arg2);
}

int SyscallInterface::SysForkWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysFork();
}

int SyscallInterface::SysExecveWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysExecve((const char*)arg1, (char**)arg2, (char**)arg3);
}

int SyscallInterface::SysWaitPidWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysWaitPid((pid_t)arg1, (int*)arg2, (int)arg3);
}

int SyscallInterface::SysSignalWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysSignal((int)arg1, (void (*)(int))arg2);
}

int SyscallInterface::SysSigactionWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysSigaction((int)arg1, (const struct sigaction*)arg2, (struct sigaction*)arg3);
}

// Network system call wrappers

int SyscallInterface::SysSocketWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysSocket((int)arg1, (int)arg2, (int)arg3);
}

int SyscallInterface::SysBindWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysBind((int)arg1, (const struct sockaddr*)arg2, (socklen_t)arg3);
}

int SyscallInterface::SysConnectWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysConnect((int)arg1, (const struct sockaddr*)arg2, (socklen_t)arg3);
}

int SyscallInterface::SysListenWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysListen((int)arg1, (int)arg2);
}

int SyscallInterface::SysAcceptWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysAccept((int)arg1, (struct sockaddr*)arg2, (socklen_t*)arg3);
}

int SyscallInterface::SysSendtoWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysSendto((int)arg1, (const void*)arg2, (size_t)arg3, (int)arg4, (const struct sockaddr*)arg5, (socklen_t)arg6);
}

int SyscallInterface::SysRecvfromWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysRecvfrom((int)arg1, (void*)arg2, (size_t)arg3, (int)arg4, (struct sockaddr*)arg5, (socklen_t*)arg6);
}

int SyscallInterface::SysSendmsgWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysSendmsg((int)arg1, (const struct msghdr*)arg2, (int)arg3);
}

int SyscallInterface::SysRecvmsgWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysRecvmsg((int)arg1, (struct msghdr*)arg2, (int)arg3);
}

int SyscallInterface::SysShutdownWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysShutdown((int)arg1, (int)arg2);
}

int SyscallInterface::SysSetsockoptWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysSetsockopt((int)arg1, (int)arg2, (int)arg3, (const void*)arg4, (socklen_t)arg5);
}

int SyscallInterface::SysGetsockoptWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysGetsockopt((int)arg1, (int)arg2, (int)arg3, (void*)arg4, (socklen_t*)arg5);
}

int SyscallInterface::SysRecvWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysRecv((int)arg1, (void*)arg2, (size_t)arg3, (int)arg4);
}

int SyscallInterface::SysSendWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_syscall_interface) return -1;
    return g_syscall_interface->SysSend((int)arg1, (const void*)arg2, (size_t)arg3, (int)arg4);
}