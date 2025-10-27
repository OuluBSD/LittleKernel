#include "Kernel.h"
#include "DosKpiV2.h"
#include "Logging.h"
#include "Vfs.h"
#include "Linuxulator.h"  // For O_* constants
#include "ProcessControlBlock.h"
#include "AbiMultiplexer.h"

// Global instance of the DOS-KPIv2 interface
DosKpiV2Interface* g_dos_kpi_v2_interface = nullptr;

DosKpiV2Interface::DosKpiV2Interface() {
    memset(&global_context, 0, sizeof(DosKpiV2Context));
    strcpy_safe(global_context.current_directory, "C:\\", sizeof(global_context.current_directory));
    global_context.current_drive = 0;  // A: drive by default
    global_context.last_error = DOS_ERROR_NONE;
    global_context.verify_flag = false;
    global_context.current_psp = nullptr;
    global_context.current_dta = nullptr;
    global_context.environment_block = nullptr;
    global_context.environment_size = 0;
    global_context.file_handles = nullptr;
    global_context.file_handle_count = 0;
    global_context.memory_blocks = nullptr;
    global_context.memory_block_count = 0;
    dos_kpi_v2_lock.Initialize();
}

DosKpiV2Interface::~DosKpiV2Interface() {
    // Cleanup handled by kernel shutdown
}

bool DosKpiV2Interface::Initialize() {
    LOG("Initializing DOS-KPIv2 interface");
    
    // Allocate and initialize core DOS-KPIv2 structures
    global_context.current_dta = CreateDta();
    if (!global_context.current_dta) {
        LOG("Failed to create initial DTA for DOS-KPIv2");
        return false;
    }
    
    // Set up default memory management
    global_context.memory_blocks = (DosMcb*)malloc(1024 * sizeof(DosMcb));  // Initial allocation for 1024 blocks
    if (global_context.memory_blocks) {
        memset(global_context.memory_blocks, 0, 1024 * sizeof(DosMcb));
        global_context.memory_block_count = 1024;
    }
    
    // Set up default file handle management
    global_context.file_handles = (uint8*)malloc(256);  // Basic file handle table
    if (global_context.file_handles) {
        memset(global_context.file_handles, 0xFF, 256);  // Mark all handles as unused
        global_context.file_handle_count = 256;
        
        // Set up standard handles (0, 1, 2)
        global_context.file_handles[0] = 0;  // stdin
        global_context.file_handles[1] = 1;  // stdout
        global_context.file_handles[2] = 2;  // stderr
    }
    
    LOG("DOS-KPIv2 interface initialized successfully");
    return true;
}

int DosKpiV2Interface::HandleSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, 
                                    uint32 arg4, uint32 arg5, uint32 arg6) {
    return DispatchSyscall(syscall_num, arg1, arg2, arg3, arg4, arg5, arg6);
}

int DosKpiV2Interface::DispatchSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, 
                                      uint32 arg4, uint32 arg5, uint32 arg6) {
    // Check if this is a DOS-KPIv2 syscall
    if (syscall_num < DOS_KPIV2_BASE_SYSCALL) {
        LOG("Invalid DOS-KPIv2 syscall number: " << syscall_num);
        return -1;
    }
    
    // Convert to DOS-KPIv2 specific syscall number
    uint32 dos_syscall = syscall_num - DOS_KPIV2_BASE_SYSCALL;
    
    switch (dos_syscall) {
        case DOS_KPIV2_SYSCALL_EXIT - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Exit(arg1);  // status
            
        case DOS_KPIV2_SYSCALL_READ - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Read(arg1, (void*)arg2, arg3);  // fd, buf, count
            
        case DOS_KPIV2_SYSCALL_WRITE - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Write(arg1, (const void*)arg2, arg3);  // fd, buf, count
            
        case DOS_KPIV2_SYSCALL_OPEN - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Open((const char*)arg1, arg2, arg3);  // pathname, flags, mode
            
        case DOS_KPIV2_SYSCALL_CLOSE - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Close(arg1);  // fd
            
        case DOS_KPIV2_SYSCALL_CREAT - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Creat((const char*)arg1, arg2);  // pathname, mode
            
        case DOS_KPIV2_SYSCALL_UNLINK - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Unlink((const char*)arg1);  // pathname
            
        case DOS_KPIV2_SYSCALL_EXEC - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Exec((const char*)arg1, (char* const*)arg2, (char* const*)arg3);  // filename, argv, envp
            
        case DOS_KPIV2_SYSCALL_FORK - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Fork();
            
        case DOS_KPIV2_SYSCALL_WAIT - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Wait((int*)arg1);  // status
            
        case DOS_KPIV2_SYSCALL_GETPID - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2GetPid();
            
        case DOS_KPIV2_SYSCALL_KILL - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Kill(arg1, arg2);  // pid, signal
            
        case DOS_KPIV2_SYSCALL_STAT - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Stat((const char*)arg1, (struct FileStat*)arg2);  // pathname, statbuf
            
        case DOS_KPIV2_SYSCALL_FSTAT - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Fstat(arg1, (struct FileStat*)arg2);  // fd, statbuf
            
        case DOS_KPIV2_SYSCALL_LSEEK - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Lseek(arg1, (int32_t)arg2, arg3);  // fd, offset, whence
            
        case DOS_KPIV2_SYSCALL_CHDIR - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Chdir((const char*)arg1);  // path
            
        case DOS_KPIV2_SYSCALL_GETCWD - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Getcwd((char*)arg1, arg2);  // buf, size
            
        case DOS_KPIV2_SYSCALL_MKDIR - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Mkdir((const char*)arg1, arg2);  // path, mode
            
        case DOS_KPIV2_SYSCALL_RMDIR - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Rmdir((const char*)arg1);  // path
            
        case DOS_KPIV2_SYSCALL_RENAME - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Rename((const char*)arg1, (const char*)arg2);  // oldpath, newpath
            
        case DOS_KPIV2_SYSCALL_ACCESS - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Access((const char*)arg1, arg2);  // path, mode
            
        case DOS_KPIV2_SYSCALL_CHMOD - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Chmod((const char*)arg1, arg2);  // path, mode
            
        case DOS_KPIV2_SYSCALL_CHOWN - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Chown((const char*)arg1, arg2, arg3);  // path, owner, group
            
        case DOS_KPIV2_SYSCALL_UTIME - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Utime((const char*)arg1, (struct utimbuf*)arg2);  // path, times
            
        case DOS_KPIV2_SYSCALL_PIPE - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Pipe((int*)arg1);  // pipefd
            
        case DOS_KPIV2_SYSCALL_DUP - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Dup(arg1);  // oldfd
            
        case DOS_KPIV2_SYSCALL_DUP2 - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Dup2(arg1, arg2);  // oldfd, newfd
            
        case DOS_KPIV2_SYSCALL_SYMLINK - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Symlink((const char*)arg1, (const char*)arg2);  // target, linkpath
            
        case DOS_KPIV2_SYSCALL_READLINK - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Readlink((const char*)arg1, (char*)arg2, arg3);  // path, buf, bufsiz
            
        case DOS_KPIV2_SYSCALL_TRUNCATE - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Truncate((const char*)arg1, arg2);  // path, length
            
        case DOS_KPIV2_SYSCALL_FTRUNCATE - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Ftruncate(arg1, arg2);  // fd, length
            
        case DOS_KPIV2_SYSCALL_GETDENTS - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Getdents(arg1, (struct dirent*)arg2, arg3);  // fd, dirp, count
            
        case DOS_KPIV2_SYSCALL_MMAP - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Mmap((void*)arg1, arg2, arg3, arg4, arg5, arg6);  // addr, length, prot, flags, fd, offset
            
        case DOS_KPIV2_SYSCALL_MUNMAP - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Munmap((void*)arg1, arg2);  // addr, length
            
        case DOS_KPIV2_SYSCALL_BRK - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Brk((void*)arg1);  // addr
            
        case DOS_KPIV2_SYSCALL_SBRK - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Sbrk(arg1);  // increment
            
        case DOS_KPIV2_SYSCALL_MPROTECT - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Mprotect((void*)arg1, arg2, arg3);  // addr, len, prot
            
        case DOS_KPIV2_SYSCALL_MSYNC - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Msync((void*)arg1, arg2, arg3);  // addr, len, flags
            
        case DOS_KPIV2_SYSCALL_MINCORE - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Mincore((void*)arg1, arg2, (unsigned char*)arg3);  // addr, length, vec
            
        case DOS_KPIV2_SYSCALL_MADVISE - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Madvise((void*)arg1, arg2, arg3);  // addr, length, advice
            
        case DOS_KPIV2_SYSCALL_MLOCK - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Mlock((const void*)arg1, arg2);  // addr, len
            
        case DOS_KPIV2_SYSCALL_MUNLOCK - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Munlock((const void*)arg1, arg2);  // addr, len
            
        case DOS_KPIV2_SYSCALL_MLOCKALL - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Mlockall(arg1);  // flags
            
        case DOS_KPIV2_SYSCALL_MUNLOCKALL - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Munlockall();
            
        case DOS_KPIV2_SYSCALL_MOUNT - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Mount((const char*)arg1, (const char*)arg2, (const char*)arg3, 
                                arg4, (const void*)arg5);  // source, target, filesystemtype, mountflags, data
            
        case DOS_KPIV2_SYSCALL_UMOUNT - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Umount((const char*)arg1);  // target
            
        case DOS_KPIV2_SYSCALL_UMOUNT2 - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Umount2((const char*)arg1, arg2);  // target, flags
            
        case DOS_KPIV2_SYSCALL_STATFS - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Statfs((const char*)arg1, (struct statfs*)arg2);  // path, buf
            
        case DOS_KPIV2_SYSCALL_FSTATFS - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Fstatfs(arg1, (struct statfs*)arg2);  // fd, buf
            
        case DOS_KPIV2_SYSCALL_USTAT - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Ustat(arg1, (struct ustat*)arg2);  // dev, ubuf
            
        case DOS_KPIV2_SYSCALL_UNAME - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Uname((struct utsname*)arg1);  // buf
            
        case DOS_KPIV2_SYSCALL_GETTIMEOFDAY - DOS_KPIV2_BASE_SYSCALL:
            return DosKpiV2Gettimeofday((struct timeval*)arg1, (struct timezone*)arg2);  // tv, tz
            
        default:
            LOG("Unsupported DOS-KPIv2 syscall: " << syscall_num << " (subcode: " << dos_syscall << ")");
            return -1;
    }
}

// DOS-KPIv2 implementation functions (similar to DOS-KPIv1 but using syscall interface)
int DosKpiV2Interface::DosKpiV2Read(uint32 fd, void* buffer, uint32 count) {
    if (!g_vfs) {
        return -1;
    }
    
    return g_vfs->Read(fd, buffer, count);
}

int DosKpiV2Interface::DosKpiV2Write(uint32 fd, const void* buffer, uint32 count) {
    if (!g_vfs) {
        return -1;
    }
    
    return g_vfs->Write(fd, (void*)buffer, count);
}

int DosKpiV2Interface::DosKpiV2Open(const char* filename, uint32 flags, uint32 mode) {
    if (!filename || !g_vfs) {
        return -1;
    }
    
    // Convert DOS-style flags to standard flags
    int std_flags = 0;
    if (flags & DOS_FILE_ACCESS_READ) {
        std_flags |= O_RDONLY;
    }
    if (flags & DOS_FILE_ACCESS_WRITE) {
        std_flags |= O_WRONLY;
    }
    if (flags & DOS_FILE_ACCESS_READ_WRITE) {
        std_flags |= O_RDWR;
    }
    
    return g_vfs->Open(filename, std_flags);
}

int DosKpiV2Interface::DosKpiV2Close(uint32 fd) {
    if (!g_vfs) {
        return -1;
    }
    
    return g_vfs->Close(fd);
}

int DosKpiV2Interface::DosKpiV2Creat(const char* filename, uint32 mode) {
    if (!filename || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Open(filename, O_CREAT | O_WRONLY | O_TRUNC);
}

int DosKpiV2Interface::DosKpiV2Unlink(const char* filename) {
    if (!filename || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Unlink(filename);
}

int DosKpiV2Interface::DosKpiV2Exec(const char* filename, char* const argv[], char* const envp[]) {
    if (!filename || !process_manager) {
        LOG("Invalid parameters for DOS-KPIv2 exec");
        return -1;
    }
    
    LOG("DOS-KPIv2 Exec system call not implemented yet (filename: " << filename << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Fork() {
    if (!process_manager) {
        LOG("Process manager not available for DOS-KPIv2 fork");
        return -1;
    }
    
    LOG("DOS-KPIv2 Fork system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Wait(int* status) {
    if (!process_manager) {
        LOG("Process manager not available for DOS-KPIv2 wait");
        return -1;
    }
    
    LOG("DOS-KPIv2 Wait system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2GetPid() {
    // Get the current process ID
    if (g_current_process) {
        return g_current_process->pid;
    }
    return 1; // Default to PID 1 if no current process
}

int DosKpiV2Interface::DosKpiV2Exit(int status) {
    // Exit the current DOS-KPIv2 process
    LOG("DOS-KPIv2 Process exiting with status: " << status);
    
    // In a real implementation, this would terminate the current process
    // For now, we'll just return
    return 0;
}

int DosKpiV2Interface::DosKpiV2Kill(int pid, int signal) {
    LOG("DOS-KPIv2 Kill system call not implemented yet (pid: " << pid << ", sig: " << signal << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Stat(const char* filename, struct FileStat* statbuf) {
    if (!filename || !statbuf || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Stat(filename, statbuf);
}

int DosKpiV2Interface::DosKpiV2Fstat(int fd, struct FileStat* statbuf) {
    LOG("DOS-KPIv2 Fstat system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Lseek(int fd, int32_t offset, int origin) {
    if (!g_vfs) {
        return -1;
    }
    
    return g_vfs->Seek(fd, offset, origin);
}

int DosKpiV2Interface::DosKpiV2Chdir(const char* path) {
    if (!path || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Chdir(path);
}

int DosKpiV2Interface::DosKpiV2Getcwd(char* buf, uint32 size) {
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

int DosKpiV2Interface::DosKpiV2Mkdir(const char* path, uint32 mode) {
    if (!path || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Mkdir(path, mode);
}

int DosKpiV2Interface::DosKpiV2Rmdir(const char* path) {
    if (!path || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Unlink(path);  // VFS Unlink should handle directories
}

int DosKpiV2Interface::DosKpiV2Rename(const char* oldpath, const char* newpath) {
    LOG("DOS-KPIv2 Rename system call not implemented yet (old: " << oldpath << ", new: " << newpath << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Access(const char* path, int mode) {
    LOG("DOS-KPIv2 Access system call not implemented yet (path: " << path << ", mode: " << mode << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Chmod(const char* path, uint32 mode) {
    LOG("DOS-KPIv2 Chmod system call not implemented yet (path: " << path << ", mode: " << mode << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Chown(const char* path, uint32 owner, uint32 group) {
    LOG("DOS-KPIv2 Chown system call not implemented yet (path: " << path << ", owner: " << owner << ", group: " << group << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Utime(const char* path, struct utimbuf* times) {
    LOG("DOS-KPIv2 Utime system call not implemented yet (path: " << path << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Dup(int oldfd) {
    LOG("DOS-KPIv2 Dup system call not implemented yet (oldfd: " << oldfd << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Dup2(int oldfd, int newfd) {
    LOG("DOS-KPIv2 Dup2 system call not implemented yet (oldfd: " << oldfd << ", newfd: " << newfd << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Pipe(int pipefd[2]) {
    if (!pipefd) {
        return -1;
    }
    
    if (!ipc_manager) {
        return -1;
    }
    
    LOG("DOS-KPIv2 Pipe system call not fully implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Symlink(const char* target, const char* linkpath) {
    LOG("DOS-KPIv2 Symlink system call not implemented yet (target: " << target << ", link: " << linkpath << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Readlink(const char* path, char* buf, uint32 bufsiz) {
    LOG("DOS-KPIv2 Readlink system call not implemented yet (path: " << path << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Truncate(const char* path, uint32 length) {
    LOG("DOS-KPIv2 Truncate system call not implemented yet (path: " << path << ", length: " << length << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Ftruncate(int fd, uint32 length) {
    LOG("DOS-KPIv2 Ftruncate system call not implemented yet (fd: " << fd << ", length: " << length << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Getdents(int fd, struct dirent* dirp, uint32 count) {
    LOG("DOS-KPIv2 Getdents system call not implemented yet (fd: " << fd << ")");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Mmap(void* addr, uint32 length, int prot, int flags, int fd, uint32 offset) {
    LOG("DOS-KPIv2 Mmap system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Munmap(void* addr, uint32 length) {
    LOG("DOS-KPIv2 Munmap system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Brk(void* addr) {
    LOG("DOS-KPIv2 Brk system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Sbrk(int32 increment) {
    LOG("DOS-KPIv2 Sbrk system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Mprotect(void* addr, uint32 len, int prot) {
    LOG("DOS-KPIv2 Mprotect system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Msync(void* addr, uint32 len, int flags) {
    LOG("DOS-KPIv2 Msync system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Mincore(void* addr, uint32 length, unsigned char* vec) {
    LOG("DOS-KPIv2 Mincore system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Madvise(void* addr, uint32 length, int advice) {
    LOG("DOS-KPIv2 Madvise system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Mlock(const void* addr, uint32 len) {
    LOG("DOS-KPIv2 Mlock system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Munlock(const void* addr, uint32 len) {
    LOG("DOS-KPIv2 Munlock system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Mlockall(int flags) {
    LOG("DOS-KPIv2 Mlockall system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Munlockall() {
    LOG("DOS-KPIv2 Munlockall system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Mount(const char* source, const char* target, const char* filesystemtype, 
                                     unsigned long mountflags, const void* data) {
    LOG("DOS-KPIv2 Mount system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Umount(const char* target) {
    LOG("DOS-KPIv2 Umount system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Umount2(const char* target, int flags) {
    LOG("DOS-KPIv2 Umount2 system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Statfs(const char* path, struct statfs* buf) {
    LOG("DOS-KPIv2 Statfs system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Fstatfs(int fd, struct statfs* buf) {
    LOG("DOS-KPIv2 Fstatfs system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Ustat(dev_t dev, struct ustat* ubuf) {
    LOG("DOS-KPIv2 Ustat system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2Uname(struct utsname* buf) {
    if (!buf) {
        return -1;
    }
    
    // Fill in system information (DOS-compatible)
    strcpy_safe(buf->sysname, "LittleKernel", sizeof(buf->sysname));
    strcpy_safe(buf->nodename, "localhost", sizeof(buf->nodename));
    strcpy_safe(buf->release, "1.0.0", sizeof(buf->release));
    strcpy_safe(buf->version, "LittleKernel DOS-KPIv2 1.0", sizeof(buf->version));
    strcpy_safe(buf->machine, "x86_64", sizeof(buf->machine));
    
    return 0;
}

int DosKpiV2Interface::DosKpiV2Gettimeofday(struct timeval* tv, struct timezone* tz) {
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

// DOS-specific functions
int DosKpiV2Interface::DosKpiV2SetCurrentDirectory(const char* path) {
    int result = DosKpiV2Chdir(path);
    if (result == 0) {
        // Update our internal current directory
        strcpy_safe(global_context.current_directory, path, sizeof(global_context.current_directory));
    }
    return result;
}

int DosKpiV2Interface::DosKpiV2GetCurrentDirectory(char* buffer, uint32 size) {
    if (!buffer || size == 0) {
        return -1;
    }
    
    if (strlen(global_context.current_directory) >= size) {
        return -1;  // Buffer too small
    }
    
    strcpy_safe(buffer, global_context.current_directory, size);
    return 0;
}

int DosKpiV2Interface::DosKpiV2SetCurrentDrive(uint8 drive) {
    if (drive < DOS_MAX_DRIVE_LETTERS) {
        global_context.current_drive = drive;
        return 0;
    }
    
    return -1;
}

uint8 DosKpiV2Interface::DosKpiV2GetCurrentDrive() {
    return global_context.current_drive;
}

int DosKpiV2Interface::DosKpiV2FindFirst(const char* filespec, uint16_t attributes, DosDirEntry* entry) {
    LOG("DOS-KPIv2 FindFirst system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2FindNext(DosDirEntry* entry) {
    LOG("DOS-KPIv2 FindNext system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2SetFileAttributes(const char* filename, uint16_t attributes) {
    LOG("DOS-KPIv2 SetFileAttributes system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2GetFileAttributes(const char* filename, uint16_t* attributes) {
    LOG("DOS-KPIv2 GetFileAttributes system call not implemented yet");
    return -1;
}

int DosKpiV2Interface::DosKpiV2AllocateMemory(uint32 paragraphs, uint16_t* segment) {
    if (!segment) {
        return -1;
    }
    
    // Allocate memory and return segment address
    uint8* memory = AllocateDosMemory(paragraphs);
    if (!memory) {
        return -1;
    }
    
    // Convert virtual address to segment:offset format (simplified)
    // In a real implementation, this would properly handle the segment conversion
    *segment = (uint16_t)((uint32)memory >> 4);
    return 0;
}

int DosKpiV2Interface::DosKpiV2FreeMemory(uint16_t segment) {
    // Convert segment back to virtual address (simplified)
    // In a real implementation, this would properly handle the conversion
    uint8* memory = (uint8*)((uint32)segment << 4);
    return FreeDosMemory(memory) ? 0 : -1;
}

int DosKpiV2Interface::DosKpiV2ResizeMemory(uint16_t segment, uint32 new_paragraphs) {
    // Convert segment back to virtual address (simplified)
    // In a real implementation, this would properly handle the conversion
    uint8* memory = (uint8*)((uint32)segment << 4);
    return ResizeDosMemory(memory, new_paragraphs) ? 0 : -1;
}

// Helper functions (similar to DOS-KPIv1)
int DosKpiV2Interface::TranslateLinuxToDosError(int linux_errno) {
    // Convert Linux errno to DOS error code
    switch (linux_errno) {
        case 0: return DOS_ERROR_NONE;
        case ENOENT: return DOS_ERROR_FILE_NOT_FOUND;
        case EACCES: return DOS_ERROR_ACCESS_DENIED;
        case ENOMEM: return DOS_ERROR_INSUFFICIENT_MEMORY;
        case EEXIST: return DOS_ERROR_CURRENT_DIRECTORY_ATTEMPT_TO_REMOVE;
        case EINVAL: return DOS_ERROR_INVALID_ACCESS_CODE;
        case EISDIR: return DOS_ERROR_ACCESS_DENIED;
        case ENOTDIR: return DOS_ERROR_PATH_NOT_FOUND;
        case ENOSPC: return DOS_ERROR_WRITE_PROTECTED;
        case EROFS: return DOS_ERROR_WRITE_PROTECTED;
        default: return DOS_ERROR_GENERAL_FAILURE;
    }
}

int DosKpiV2Interface::TranslateDosToLinuxError(int dos_error) {
    // Convert DOS error code to Linux errno
    switch (dos_error) {
        case DOS_ERROR_NONE: return 0;
        case DOS_ERROR_FILE_NOT_FOUND: return ENOENT;
        case DOS_ERROR_ACCESS_DENIED: return EACCES;
        case DOS_ERROR_INSUFFICIENT_MEMORY: return ENOMEM;
        case DOS_ERROR_PATH_NOT_FOUND: return ENOTDIR;
        case DOS_ERROR_INVALID_ACCESS_CODE: return EINVAL;
        default: return EIO;
    }
}

bool DosKpiV2Interface::IsValidDosPath(const char* path) {
    if (!path) {
        return false;
    }
    
    // Basic validation for DOS-style path
    if (strlen(path) > DOS_MAX_PATH_LENGTH) {
        return false;
    }
    
    // Check that the path doesn't contain invalid characters for DOS
    const char* invalid_chars = "<>\"|?*";
    for (int i = 0; i < strlen(path); i++) {
        if (strchr(invalid_chars, path[i])) {
            return false;  // Invalid character found
        }
    }
    
    return true;
}

bool DosKpiV2Interface::ConvertDosPathToUnix(const char* dos_path, char* unix_path, uint32 max_len) {
    return g_abi_multiplexer->ConvertDosPathToUnix(dos_path, unix_path, max_len);
}

bool DosKpiV2Interface::ConvertUnixPathToDos(const char* unix_path, char* dos_path, uint32 max_len) {
    return g_abi_multiplexer->ConvertUnixPathToDos(unix_path, dos_path, max_len);
}

uint8 DosKpiV2Interface::GetDefaultDrive() {
    return global_context.current_drive;
}

bool DosKpiV2Interface::SetDefaultDrive(uint8 drive) {
    if (drive < DOS_MAX_DRIVE_LETTERS) {
        global_context.current_drive = drive;
        return true;
    }
    return false;
}

const char* DosKpiV2Interface::GetDosDrivePath(uint8 drive_letter) {
    // For now, return a simple mapping
    // A full implementation would use the registry or a mapping table
    if (drive_letter < DOS_MAX_DRIVE_LETTERS) {
        static char temp_path[64];
        snprintf(temp_path, sizeof(temp_path), "/Drive%c", 'A' + drive_letter);
        return temp_path;
    }
    return nullptr;
}

bool DosKpiV2Interface::SetDosDrivePath(uint8 drive_letter, const char* path) {
    // For now, this is a simplified implementation
    // A full implementation would update the registry or mapping table
    return drive_letter < DOS_MAX_DRIVE_LETTERS && path != nullptr;
}

DosPsp* DosKpiV2Interface::CreatePsp(uint16_t parent_psp_segment, const char* program_name) {
    return g_dos_syscall_interface->CreatePsp(parent_psp_segment, program_name);
}

bool DosKpiV2Interface::DestroyPsp(DosPsp* psp) {
    return g_dos_syscall_interface->DestroyPsp(psp);
}

DosDta* DosKpiV2Interface::CreateDta() {
    return g_dos_syscall_interface->CreateDta();
}

bool DosKpiV2Interface::DestroyDta(DosDta* dta) {
    return g_dos_syscall_interface->DestroyDta(dta);
}

DosMcb* DosKpiV2Interface::CreateMcb(uint8 signature, uint16_t owner_psp, uint16_t size, const char* program_name) {
    return g_dos_syscall_interface->CreateMcb(signature, owner_psp, size, program_name);
}

bool DosKpiV2Interface::DestroyMcb(DosMcb* mcb) {
    return g_dos_syscall_interface->DestroyMcb(mcb);
}

uint8* DosKpiV2Interface::AllocateDosMemory(uint32 paragraphs) {
    return g_dos_syscall_interface->AllocateDosMemory(paragraphs);
}

bool DosKpiV2Interface::FreeDosMemory(uint8* address) {
    return g_dos_syscall_interface->FreeDosMemory(address);
}

bool DosKpiV2Interface::ResizeDosMemory(uint8* address, uint32 new_paragraphs) {
    return g_dos_syscall_interface->ResizeDosMemory(address, new_paragraphs);
}

uint16_t DosKpiV2Interface::GetDosMemoryBlockOwner(uint8* address) {
    return g_dos_syscall_interface->GetDosMemoryBlockOwner(address);
}

bool DosKpiV2Interface::SetDosMemoryBlockOwner(uint8* address, uint16_t owner_psp) {
    return g_dos_syscall_interface->SetDosMemoryBlockOwner(address, owner_psp);
}

uint16_t DosKpiV2Interface::GetDosMemoryBlockSize(uint8* address) {
    return g_dos_syscall_interface->GetDosMemoryBlockSize(address);
}

bool DosKpiV2Interface::SetDosMemoryBlockSize(uint8* address, uint16_t size) {
    return g_dos_syscall_interface->SetDosMemoryBlockSize(address, size);
}

bool DosKpiV2Interface::ValidateDosMemoryBlock(uint8* address) {
    return g_dos_syscall_interface->ValidateDosMemoryBlock(address);
}

bool DosKpiV2Interface::SanitizeDosMemoryBlock(uint8* address) {
    return g_dos_syscall_interface->SanitizeDosMemoryBlock(address);
}

bool DosKpiV2Interface::NormalizeDosMemoryBlock(uint8* address) {
    return g_dos_syscall_interface->NormalizeDosMemoryBlock(address);
}

int DosKpiV2Interface::CompareDosMemoryBlocks(uint8* address1, uint8* address2) {
    return g_dos_syscall_interface->CompareDosMemoryBlocks(address1, address2);
}

uint8* DosKpiV2Interface::CloneDosMemoryBlock(uint8* source) {
    return g_dos_syscall_interface->CloneDosMemoryBlock(source);
}

void DosKpiV2Interface::FreeDosMemoryBlock(uint8* address) {
    g_dos_syscall_interface->FreeDosMemoryBlock(address);
}

uint8* DosKpiV2Interface::AllocateDosMemoryBlock(uint32 size) {
    return g_dos_syscall_interface->AllocateDosMemoryBlock(size);
}

void DosKpiV2Interface::DeallocateDosMemoryBlock(uint8* address) {
    g_dos_syscall_interface->DeallocateDosMemoryBlock(address);
}

void DosKpiV2Interface::PrintDosMemoryBlock(uint8* address) {
    g_dos_syscall_interface->PrintDosMemoryBlock(address);
}

void DosKpiV2Interface::PrintDosMemoryBlocks() {
    g_dos_syscall_interface->PrintDosMemoryBlocks();
}

bool DosKpiV2Interface::PrintDosMemoryStatistics() {
    return g_dos_syscall_interface->PrintDosMemoryStatistics();
}

bool DosKpiV2Interface::PrintDosMemoryValidation() {
    return g_dos_syscall_interface->PrintDosMemoryValidation();
}

bool DosKpiV2Interface::PrintDosMemorySanitization() {
    return g_dos_syscall_interface->PrintDosMemorySanitization();
}

bool DosKpiV2Interface::PrintDosMemoryNormalization() {
    return g_dos_syscall_interface->PrintDosMemoryNormalization();
}

bool DosKpiV2Interface::PrintDosMemoryComparison(uint8* address1, uint8* address2) {
    return g_dos_syscall_interface->PrintDosMemoryComparison(address1, address2);
}

bool DosKpiV2Interface::PrintDosMemoryCloning(uint8* source) {
    return g_dos_syscall_interface->PrintDosMemoryCloning(source);
}

bool DosKpiV2Interface::PrintDosMemoryDeallocation(uint8* address) {
    return g_dos_syscall_interface->PrintDosMemoryDeallocation(address);
}

bool DosKpiV2Interface::PrintDosMemoryAllocation(uint32 size) {
    return g_dos_syscall_interface->PrintDosMemoryAllocation(size);
}

// Initialize the DOS-KPIv2 interface
bool InitializeDosKpiV2() {
    if (!g_dos_kpi_v2_interface) {
        g_dos_kpi_v2_interface = new DosKpiV2Interface();
        if (!g_dos_kpi_v2_interface) {
            LOG("Failed to create DOS-KPIv2 interface instance");
            return false;
        }
        
        if (!g_dos_kpi_v2_interface->Initialize()) {
            LOG("Failed to initialize DOS-KPIv2 interface");
            delete g_dos_kpi_v2_interface;
            g_dos_kpi_v2_interface = nullptr;
            return false;
        }
        
        LOG("DOS-KPIv2 interface initialized successfully");
    }
    
    return true;
}

// Handle DOS-KPIv2 syscalls (using syscall instruction)
extern "C" int HandleDosKpiV2Syscall(uint32 syscall_num, 
                                    uint32 arg1, uint32 arg2, uint32 arg3, 
                                    uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_dos_kpi_v2_interface) {
        return -1;
    }
    
    return g_dos_kpi_v2_interface->HandleSyscall(syscall_num, arg1, arg2, arg3, arg4, arg5, arg6);
}

// Setup DOS-KPIv2 syscall table for the ABI multiplexer
bool SetupDosKpiV2SyscallTable() {
    if (!g_abi_multiplexer) {
        LOG("ABI multiplexer not initialized for DOS-KPIv2 setup");
        return false;
    }
    
    // Create syscall table for DOS-KPIv2
    AbiSyscallTable* table = (AbiSyscallTable*)malloc(sizeof(AbiSyscallTable));
    if (!table) {
        LOG("Failed to allocate ABI syscall table for DOS-KPIv2");
        return false;
    }
    
    // We'll use 100 syscall slots for DOS-KPIv2
    const uint32 max_syscalls = 100;
    table->handlers = (SyscallHandler*)malloc(max_syscalls * sizeof(SyscallHandler));
    if (!table->handlers) {
        free(table);
        LOG("Failed to allocate syscall handlers for DOS-KPIv2");
        return false;
    }
    
    // Initialize all handlers to a default handler
    for (uint32 i = 0; i < max_syscalls; i++) {
        table->handlers[i] = nullptr;
    }
    
    // Register specific handlers for DOS-KPIv2 syscalls
    // Map them to the range [DOS_KPIV2_BASE_SYSCALL, DOS_KPIV2_BASE_SYSCALL+max_syscalls)
    table->max_syscall_num = max_syscalls;
    table->names = nullptr; // For now, no names array
    
    // Register the table with the ABI multiplexer
    bool result = g_abi_multiplexer->RegisterAbiSyscalls(DOS_KPI_V2, table);
    
    // The table will be freed by the multiplexer on shutdown
    return result;
}