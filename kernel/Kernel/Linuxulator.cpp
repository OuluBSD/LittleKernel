#include "Kernel.h"
#include "Linuxulator.h"
#include "Logging.h"
#include "Vfs.h"
#include "ProcessControlBlock.h"

// Global Linuxulator instance
Linuxulator* g_linuxulator = nullptr;

Linuxulator::Linuxulator() {
    process_count = 0;
    linuxulator_lock.Initialize();
    
    // Initialize all Linux processes
    for (int i = 0; i < MAX_LINUX_PROCESSES; i++) {
        memset(&linux_processes[i], 0, sizeof(LinuxProcess));
        linux_processes[i].pid = INVALID_PID;
    }
}

Linuxulator::~Linuxulator() {
    // Cleanup all Linux processes
    for (int i = 0; i < MAX_LINUX_PROCESSES; i++) {
        if (linux_processes[i].pid != INVALID_PID) {
            CleanupLinuxProcess(&linux_processes[i]);
        }
    }
}

bool Linuxulator::Initialize() {
    LOG("Initializing Linuxulator (Linux compatibility layer)");
    
    // For now, just initialize the basic structures
    // In a real implementation, this would initialize the ELF loader,
    // system call table, and other components needed for Linux binary execution
    
    LOG("Linuxulator initialized successfully");
    return true;
}

bool Linuxulator::LoadLinuxBinary(const char* filename, char* const argv[], char* const envp[]) {
    if (!filename) {
        return false;
    }
    
    LOG("Loading Linux binary: " << filename);
    
    // Check if file exists
    if (!g_vfs) {
        LOG("VFS not available for Linux binary loading");
        return false;
    }
    
    FileStat stat_buf;
    int result = g_vfs->Stat(filename, &stat_buf);
    if (result != VFS_SUCCESS) {
        LOG("Linux binary not found: " << filename);
        return false;
    }
    
    // Load ELF header
    LinuxElfHeader elf_header;
    if (!LoadElfFile(filename, &elf_header)) {
        LOG("Failed to load ELF file: " << filename);
        return false;
    }
    
    // Verify ELF header
    if (!VerifyElfHeader(&elf_header)) {
        LOG("Invalid ELF header in file: " << filename);
        return false;
    }
    
    // Map ELF segments into memory
    if (!MapElfSegments(&elf_header, filename)) {
        LOG("Failed to map ELF segments for file: " << filename);
        return false;
    }
    
    // Create a Linux process for this binary
    LinuxProcess* process = CreateLinuxProcess(filename, argv, envp);
    if (!process) {
        LOG("Failed to create Linux process for file: " << filename);
        return false;
    }
    
    LOG("Linux binary loaded successfully: " << filename << " (PID: " << process->pid << ")");
    return true;
}

int Linuxulator::HandleSyscall(LinuxSyscallContext* context) {
    if (!context) {
        return -1;
    }
    
    linuxulator_lock.Acquire();
    
    // Log the system call for debugging
    DLOG("Linux system call: " << GetSyscallName(context->syscall_number) 
         << " (" << context->syscall_number << ")");
    
    // Handle the system call based on its number
    int result = DispatchSyscall(context);
    
    linuxulator_lock.Release();
    
    return result;
}

LinuxProcess* Linuxulator::CreateLinuxProcess(const char* filename, char* const argv[], char* const envp[]) {
    if (!filename) {
        return nullptr;
    }
    
    linuxulator_lock.Acquire();
    
    // Find a free slot for the new process
    LinuxProcess* process = FindFreeLinuxProcessSlot();
    if (!process) {
        linuxulator_lock.Release();
        LOG("No free slots for new Linux process");
        return nullptr;
    }
    
    // Initialize the process structure
    InitializeLinuxProcess(process);
    
    // Set process information
    process->pid = GetNextPID();
    process->ppid = 1; // Parent process ID (init process)
    process->uid = 0;  // Root user
    process->gid = 0;  // Root group
    process->euid = 0; // Effective user ID
    process->egid = 0; // Effective group ID
    process->suid = 0; // Saved user ID
    process->sgid = 0; // Saved group ID
    process->fsuid = 0; // File system user ID
    process->fsgid = 0; // File system group ID
    process->start_time = global_timer ? global_timer->GetTickCount() : 0;
    process->priority = 20; // Default nice value
    process->nice = 0;      // Default nice value
    
    // Set process name
    strncpy(process->name, filename, sizeof(process->name) - 1);
    process->name[sizeof(process->name) - 1] = '\0';
    
    // Update process count
    process_count++;
    
    linuxulator_lock.Release();
    
    LOG("Created Linux process: " << filename << " (PID: " << process->pid << ")");
    return process;
}

bool Linuxulator::DestroyLinuxProcess(uint32_t pid) {
    linuxulator_lock.Acquire();
    
    // Find the process with the given PID
    LinuxProcess* process = GetLinuxProcess(pid);
    if (!process) {
        linuxulator_lock.Release();
        return false;
    }
    
    // Clean up the process
    CleanupLinuxProcess(process);
    
    // Mark the process slot as free
    process->pid = INVALID_PID;
    
    // Update process count
    if (process_count > 0) {
        process_count--;
    }
    
    linuxulator_lock.Release();
    
    LOG("Destroyed Linux process (PID: " << pid << ")");
    return true;
}

LinuxProcess* Linuxulator::GetLinuxProcess(uint32_t pid) {
    for (int i = 0; i < MAX_LINUX_PROCESSES; i++) {
        if (linux_processes[i].pid == pid) {
            return &linux_processes[i];
        }
    }
    
    return nullptr;
}

uint32_t Linuxulator::GetLinuxProcessCount() {
    return process_count;
}

bool Linuxulator::LoadElfFile(const char* filename, LinuxElfHeader* elf_header) {
    if (!filename || !elf_header) {
        return false;
    }
    
    // Open the file
    int fd = g_vfs->Open(filename, O_RDONLY);
    if (fd < 0) {
        LOG("Failed to open ELF file: " << filename);
        return false;
    }
    
    // Read the ELF header
    int bytes_read = g_vfs->Read(fd, elf_header, sizeof(LinuxElfHeader));
    if (bytes_read != sizeof(LinuxElfHeader)) {
        LOG("Failed to read ELF header from file: " << filename);
        g_vfs->Close(fd);
        return false;
    }
    
    // Close the file
    g_vfs->Close(fd);
    
    return true;
}

bool Linuxulator::VerifyElfHeader(const LinuxElfHeader* elf_header) {
    if (!elf_header) {
        return false;
    }
    
    // Check ELF magic number
    if (elf_header->e_ident[0] != 0x7F || 
        elf_header->e_ident[1] != 'E' || 
        elf_header->e_ident[2] != 'L' || 
        elf_header->e_ident[3] != 'F') {
        return false;
    }
    
    // Check architecture (32-bit little endian)
    if (elf_header->e_ident[4] != 1 || elf_header->e_ident[5] != 1) {
        return false;
    }
    
    // Check machine type (Intel 386)
    if (elf_header->e_machine != 3) {
        return false;
    }
    
    return true;
}

bool Linuxulator::MapElfSegments(const LinuxElfHeader* elf_header, const char* filename) {
    if (!elf_header || !filename) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Read the program header table
    // 2. Map each segment into memory with appropriate permissions
    // 3. Set up the process's memory layout
    // 4. Prepare the process for execution
    
    LOG("Mapping ELF segments for file: " << filename);
    return true; // For now, just return true
}

// System call implementations

int Linuxulator::LinuxRead(int fd, void* buf, size_t count) {
    if (!buf || count == 0) {
        return -1;
    }
    
    // Delegate to the kernel's VFS read function
    if (g_vfs) {
        return g_vfs->Read(fd, buf, count);
    }
    
    return -1;
}

int Linuxulator::LinuxWrite(int fd, const void* buf, size_t count) {
    if (!buf || count == 0) {
        return -1;
    }
    
    // Delegate to the kernel's VFS write function
    if (g_vfs) {
        return g_vfs->Write(fd, (void*)buf, count);
    }
    
    return -1;
}

int Linuxulator::LinuxOpen(const char* pathname, int flags, mode_t mode) {
    if (!pathname) {
        return -1;
    }
    
    // Translate Linux flags to our VFS flags
    int translated_flags = TranslateOpenFlags(flags);
    
    // Delegate to the kernel's VFS open function
    if (g_vfs) {
        return g_vfs->Open(pathname, translated_flags);
    }
    
    return -1;
}

int Linuxulator::LinuxClose(int fd) {
    // Delegate to the kernel's VFS close function
    if (g_vfs) {
        return g_vfs->Close(fd);
    }
    
    return -1;
}

int Linuxulator::LinuxStat(const char* pathname, struct FileStat* statbuf) {
    if (!pathname || !statbuf) {
        return -1;
    }
    
    // Delegate to the kernel's VFS stat function
    if (g_vfs) {
        return g_vfs->Stat(pathname, statbuf);
    }
    
    return -1;
}

int Linuxulator::LinuxFstat(int fd, struct FileStat* statbuf) {
    if (!statbuf) {
        return -1;
    }
    
    // Delegate to the kernel's VFS fstat function
    if (g_vfs) {
        return g_vfs->Fstat(fd, statbuf);
    }
    
    return -1;
}

int Linuxulator::LinuxLseek(int fd, off_t offset, int whence) {
    // Translate Linux whence to our VFS whence
    int translated_whence = TranslateWhence(whence);
    
    // Delegate to the kernel's VFS lseek function
    if (g_vfs) {
        return g_vfs->Seek(fd, offset, translated_whence);
    }
    
    return -1;
}

int Linuxulator::LinuxMmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    // Translate Linux protection and flags
    int translated_prot = TranslateProtFlags(prot);
    int translated_flags = TranslateMmapFlags(flags);
    
    // Delegate to the kernel's memory management mmap function
    if (global && global->memory_manager) {
        return global->memory_manager->Mmap(addr, length, translated_prot, translated_flags, fd, offset);
    }
    
    return -1;
}

int Linuxulator::LinuxMunmap(void* addr, size_t length) {
    // Delegate to the kernel's memory management munmap function
    if (global && global->memory_manager) {
        return global->memory_manager->Munmap(addr, length);
    }
    
    return -1;
}

int Linuxulator::LinuxBrk(void* addr) {
    // Delegate to the kernel's memory management brk function
    if (global && global->memory_manager) {
        return global->memory_manager->Brk(addr);
    }
    
    return -1;
}

int Linuxulator::LinuxFork() {
    // Delegate to the kernel's process manager fork function
    if (process_manager) {
        ProcessControlBlock* new_process = process_manager->CreateProcess(
            nullptr, "LinuxProcess", 10); // Default priority
        if (new_process) {
            return new_process->pid;
        }
    }
    
    return -1;
}

int Linuxulator::LinuxExecve(const char* filename, char* const argv[], char* const envp[]) {
    if (!filename) {
        return -1;
    }
    
    // Load and execute the Linux binary
    if (!LoadLinuxBinary(filename, argv, envp)) {
        return -1;
    }
    
    // In a real implementation, this would replace the current process
    // with the new program. For now, we'll just return success.
    return 0;
}

int Linuxulator::LinuxExit(int status) {
    // Delegate to the kernel's process manager exit function
    if (process_manager) {
        process_manager->ExitCurrentProcess(status);
        return 0;
    }
    
    return -1;
}

int Linuxulator::LinuxWait4(pid_t pid, int* status, int options, struct rusage* rusage) {
    // Delegate to the kernel's process manager wait function
    if (process_manager) {
        return process_manager->WaitForProcess(pid, status, options);
    }
    
    return -1;
}

int Linuxulator::LinuxKill(pid_t pid, int sig) {
    // Translate Linux signal to our signal
    int translated_sig = TranslateSignal(sig);
    
    // Delegate to the kernel's process manager kill function
    if (process_manager) {
        return process_manager->KillProcess(pid, translated_sig);
    }
    
    return -1;
}

int Linuxulator::LinuxGetPid() {
    // Return the current Linux process ID
    if (process_manager && process_manager->GetCurrentProcess()) {
        return process_manager->GetCurrentProcess()->pid;
    }
    
    return 1; // Default to init process
}

// Utility functions

const char* Linuxulator::GetSyscallName(uint32_t syscall_number) {
    switch (syscall_number) {
        case SYS_LINUX_READ: return "read";
        case SYS_LINUX_WRITE: return "write";
        case SYS_LINUX_OPEN: return "open";
        case SYS_LINUX_CLOSE: return "close";
        case SYS_LINUX_STAT: return "stat";
        case SYS_LINUX_FSTAT: return "fstat";
        case SYS_LINUX_LSTAT: return "lstat";
        case SYS_LINUX_POLL: return "poll";
        case SYS_LINUX_LSEEK: return "lseek";
        case SYS_LINUX_MMAP: return "mmap";
        case SYS_LINUX_MPROTECT: return "mprotect";
        case SYS_LINUX_MUNMAP: return "munmap";
        case SYS_LINUX_BRK: return "brk";
        case SYS_LINUX_RT_SIGACTION: return "rt_sigaction";
        case SYS_LINUX_RT_SIGPROCMASK: return "rt_sigprocmask";
        case SYS_LINUX_RT_SIGRETURN: return "rt_sigreturn";
        case SYS_LINUX_IOCTL: return "ioctl";
        case SYS_LINUX_PREAD64: return "pread64";
        case SYS_LINUX_PWRITE64: return "pwrite64";
        case SYS_LINUX_READV: return "readv";
        case SYS_LINUX_WRITEV: return "writev";
        case SYS_LINUX_ACCESS: return "access";
        case SYS_LINUX_PIPE: return "pipe";
        case SYS_LINUX_SELECT: return "select";
        case SYS_LINUX_SCHED_YIELD: return "sched_yield";
        case SYS_LINUX_MREMAP: return "mremap";
        case SYS_LINUX_MSYNC: return "msync";
        case SYS_LINUX_MINCORE: return "mincore";
        case SYS_LINUX_MADVISE: return "madvise";
        case SYS_LINUX_SHMGET: return "shmget";
        case SYS_LINUX_SHMAT: return "shmat";
        case SYS_LINUX_SHMCTL: return "shmctl";
        case SYS_LINUX_DUP: return "dup";
        case SYS_LINUX_DUP2: return "dup2";
        case SYS_LINUX_PAUSE: return "pause";
        case SYS_LINUX_NANOSLEEP: return "nanosleep";
        case SYS_LINUX_GETITIMER: return "getitimer";
        case SYS_LINUX_ALARM: return "alarm";
        case SYS_LINUX_SETITIMER: return "setitimer";
        case SYS_LINUX_GETPID: return "getpid";
        case SYS_LINUX_SENDFILE: return "sendfile";
        case SYS_LINUX_SOCKET: return "socket";
        case SYS_LINUX_CONNECT: return "connect";
        case SYS_LINUX_ACCEPT: return "accept";
        case SYS_LINUX_SENDTO: return "sendto";
        case SYS_LINUX_RECVFROM: return "recvfrom";
        case SYS_LINUX_SENDMSG: return "sendmsg";
        case SYS_LINUX_RECVMSG: return "recvmsg";
        case SYS_LINUX_SHUTDOWN: return "shutdown";
        case SYS_LINUX_BIND: return "bind";
        case SYS_LINUX_LISTEN: return "listen";
        case SYS_LINUX_GETSOCKNAME: return "getsockname";
        case SYS_LINUX_GETPEERNAME: return "getpeername";
        case SYS_LINUX_SOCKETPAIR: return "socketpair";
        case SYS_LINUX_SETSOCKOPT: return "setsockopt";
        case SYS_LINUX_GETSOCKOPT: return "getsockopt";
        case SYS_LINUX_CLONE: return "clone";
        case SYS_LINUX_FORK: return "fork";
        case SYS_LINUX_VFORK: return "vfork";
        case SYS_LINUX_EXECVE: return "execve";
        case SYS_LINUX_EXIT: return "exit";
        case SYS_LINUX_WAIT4: return "wait4";
        case SYS_LINUX_KILL: return "kill";
        case SYS_LINUX_UNAME: return "uname";
        case SYS_LINUX_SEMGET: return "semget";
        case SYS_LINUX_SEMOP: return "semop";
        case SYS_LINUX_SEMCTL: return "semctl";
        case SYS_LINUX_SHMDT: return "shmdt";
        case SYS_LINUX_MSGGET: return "msgget";
        case SYS_LINUX_MSGSND: return "msgsnd";
        case SYS_LINUX_MSGRCV: return "msgrcv";
        case SYS_LINUX_MSGCTL: return "msgctl";
        case SYS_LINUX_FCNTL: return "fcntl";
        case SYS_LINUX_FLOCK: return "flock";
        case SYS_LINUX_FSYNC: return "fsync";
        case SYS_LINUX_FDATASYNC: return "fdatasync";
        case SYS_LINUX_TRUNCATE: return "truncate";
        case SYS_LINUX_FTRUNCATE: return "ftruncate";
        case SYS_LINUX_GETDENTS: return "getdents";
        case SYS_LINUX_GETCWD: return "getcwd";
        case SYS_LINUX_CHDIR: return "chdir";
        case SYS_LINUX_FCHDIR: return "fchdir";
        case SYS_LINUX_RENAME: return "rename";
        case SYS_LINUX_MKDIR: return "mkdir";
        case SYS_LINUX_RMDIR: return "rmdir";
        case SYS_LINUX_CREAT: return "creat";
        case SYS_LINUX_LINK: return "link";
        case SYS_LINUX_UNLINK: return "unlink";
        case SYS_LINUX_SYMLINK: return "symlink";
        case SYS_LINUX_READLINK: return "readlink";
        case SYS_LINUX_CHMOD: return "chmod";
        case SYS_LINUX_FCHMOD: return "fchmod";
        case SYS_LINUX_CHOWN: return "chown";
        case SYS_LINUX_FCHOWN: return "fchown";
        case SYS_LINUX_LCHOWN: return "lchown";
        case SYS_LINUX_UMASK: return "umask";
        case SYS_LINUX_GETTIMEOFDAY: return "gettimeofday";
        case SYS_LINUX_GETRLIMIT: return "getrlimit";
        case SYS_LINUX_GETRUSAGE: return "getrusage";
        case SYS_LINUX_SYSINFO: return "sysinfo";
        case SYS_LINUX_TIMES: return "times";
        case SYS_LINUX_PTRACE: return "ptrace";
        case SYS_LINUX_GETUID: return "getuid";
        case SYS_LINUX_SYSLOG: return "syslog";
        case SYS_LINUX_GETGID: return "getgid";
        case SYS_LINUX_SETUID: return "setuid";
        case SYS_LINUX_SETGID: return "setgid";
        case SYS_LINUX_GETEUID: return "geteuid";
        case SYS_LINUX_GETEGID: return "getegid";
        case SYS_LINUX_SETPGID: return "setpgid";
        case SYS_LINUX_GETPPID: return "getppid";
        case SYS_LINUX_GETPGRP: return "getpgrp";
        case SYS_LINUX_SETSID: return "setsid";
        case SYS_LINUX_SETREUID: return "setreuid";
        case SYS_LINUX_SETREGID: return "setregid";
        case SYS_LINUX_GETGROUPS: return "getgroups";
        case SYS_LINUX_SETGROUPS: return "setgroups";
        case SYS_LINUX_SETRESUID: return "setresuid";
        case SYS_LINUX_GETRESUID: return "getresuid";
        case SYS_LINUX_SETRESGID: return "setresgid";
        case SYS_LINUX_GETRESGID: return "getresgid";
        case SYS_LINUX_GETPGID: return "getpgid";
        case SYS_LINUX_SETFSUID: return "setfsuid";
        case SYS_LINUX_SETFSGID: return "setfsgid";
        case SYS_LINUX_GETSID: return "getsid";
        case SYS_LINUX_CAPGET: return "capget";
        case SYS_LINUX_CAPSET: return "capset";
        case SYS_LINUX_RT_SIGPENDING: return "rt_sigpending";
        case SYS_LINUX_RT_SIGTIMEDWAIT: return "rt_sigtimedwait";
        case SYS_LINUX_RT_SIGQUEUEINFO: return "rt_sigqueueinfo";
        case SYS_LINUX_RT_SIGSUSPEND: return "rt_sigsuspend";
        case SYS_LINUX_SIGALTSTACK: return "sigaltstack";
        case SYS_LINUX_UTIME: return "utime";
        case SYS_LINUX_MKNOD: return "mknod";
        case SYS_LINUX_USELIB: return "uselib";
        case SYS_LINUX_PERSONALITY: return "personality";
        case SYS_LINUX_USTAT: return "ustat";
        case SYS_LINUX_STATFS: return "statfs";
        case SYS_LINUX_FSTATFS: return "fstatfs";
        case SYS_LINUX_SYSFS: return "sysfs";
        case SYS_LINUX_GETPRIORITY: return "getpriority";
        case SYS_LINUX_SETPRIORITY: return "setpriority";
        case SYS_LINUX_SCHED_SETPARAM: return "sched_setparam";
        case SYS_LINUX_SCHED_GETPARAM: return "sched_getparam";
        case SYS_LINUX_SCHED_SETSCHEDULER: return "sched_setscheduler";
        case SYS_LINUX_SCHED_GETSCHEDULER: return "sched_getscheduler";
        case SYS_LINUX_SCHED_GET_PRIORITY_MAX: return "sched_get_priority_max";
        case SYS_LINUX_SCHED_GET_PRIORITY_MIN: return "sched_get_priority_min";
        case SYS_LINUX_SCHED_RR_GET_INTERVAL: return "sched_rr_get_interval";
        case SYS_LINUX_MLOCK: return "mlock";
        case SYS_LINUX_MUNLOCK: return "munlock";
        case SYS_LINUX_MLOCKALL: return "mlockall";
        case SYS_LINUX_MUNLOCKALL: return "munlockall";
        case SYS_LINUX_VHANGUP: return "vhangup";
        case SYS_LINUX_MODIFY_LDT: return "modify_ldt";
        case SYS_LINUX_PIVOT_ROOT: return "pivot_root";
        case SYS_LINUX_SYSCTL: return "sysctl";
        case SYS_LINUX_PRCTL: return "prctl";
        case SYS_LINUX_ARCH_PRCTL: return "arch_prctl";
        case SYS_LINUX_ADJTIMEX: return "adjtimex";
        case SYS_LINUX_SETRLIMIT: return "setrlimit";
        case SYS_LINUX_CHROOT: return "chroot";
        case SYS_LINUX_SYNC: return "sync";
        case SYS_LINUX_ACCT: return "acct";
        case SYS_LINUX_SETTIMEOFDAY: return "settimeofday";
        case SYS_LINUX_MOUNT: return "mount";
        case SYS_LINUX_UMOUNT2: return "umount2";
        case SYS_LINUX_SWAPON: return "swapon";
        case SYS_LINUX_SWAPOFF: return "swapoff";
        case SYS_LINUX_REBOOT: return "reboot";
        case SYS_LINUX_SETHOSTNAME: return "sethostname";
        case SYS_LINUX_SETDOMAINNAME: return "setdomainname";
        case SYS_LINUX_IOPL: return "iopl";
        case SYS_LINUX_IOPERM: return "ioperm";
        case SYS_LINUX_CREATE_MODULE: return "create_module";
        case SYS_LINUX_INIT_MODULE: return "init_module";
        case SYS_LINUX_DELETE_MODULE: return "delete_module";
        case SYS_LINUX_GET_KERNEL_SYMS: return "get_kernel_syms";
        case SYS_LINUX_QUERY_MODULE: return "query_module";
        case SYS_LINUX_QUOTACTL: return "quotactl";
        case SYS_LINUX_NFSSERVCTL: return "nfsservctl";
        case SYS_LINUX_GETPMSG: return "getpmsg";
        case SYS_LINUX_PUTPMSG: return "putpmsg";
        case SYS_LINUX_AFS: return "afs";
        case SYS_LINUX_TUXCALL: return "tuxcall";
        case SYS_LINUX_SECURITY: return "security";
        case SYS_LINUX_GETTID: return "gettid";
        case SYS_LINUX_READAHEAD: return "readahead";
        case SYS_LINUX_SETXATTR: return "setxattr";
        case SYS_LINUX_LSETXATTR: return "lsetxattr";
        case SYS_LINUX_FSETXATTR: return "fsetxattr";
        case SYS_LINUX_GETXATTR: return "getxattr";
        case SYS_LINUX_LGETXATTR: return "lgetxattr";
        case SYS_LINUX_FGETXATTR: return "fgetxattr";
        case SYS_LINUX_LISTXATTR: return "listxattr";
        case SYS_LINUX_LLISTXATTR: return "llistxattr";
        case SYS_LINUX_FLISTXATTR: return "flistxattr";
        case SYS_LINUX_REMOVEXATTR: return "removexattr";
        case SYS_LINUX_LREMOVEXATTR: return "lremovexattr";
        case SYS_LINUX_FREMOVEXATTR: return "fremovexattr";
        case SYS_LINUX_TKILL: return "tkill";
        case SYS_LINUX_TIME: return "time";
        case SYS_LINUX_FUTEX: return "futex";
        case SYS_LINUX_SCHED_SETAFFINITY: return "sched_setaffinity";
        case SYS_LINUX_SCHED_GETAFFINITY: return "sched_getaffinity";
        case SYS_LINUX_SET_THREAD_AREA: return "set_thread_area";
        case SYS_LINUX_GET_THREAD_AREA: return "get_thread_area";
        case SYS_LINUX_IO_SETUP: return "io_setup";
        case SYS_LINUX_IO_DESTROY: return "io_destroy";
        case SYS_LINUX_IO_GETEVENTS: return "io_getevents";
        case SYS_LINUX_IO_SUBMIT: return "io_submit";
        case SYS_LINUX_IO_CANCEL: return "io_cancel";
        case SYS_LINUX_GET_THREAD_ID: return "get_thread_id";
        case SYS_LINUX_LOOKUP_DCOOKIE: return "lookup_dcookie";
        case SYS_LINUX_EPOLL_CREATE: return "epoll_create";
        case SYS_LINUX_EPOLL_CTL_OLD: return "epoll_ctl_old";
        case SYS_LINUX_EPOLL_WAIT_OLD: return "epoll_wait_old";
        case SYS_LINUX_REMAP_FILE_PAGES: return "remap_file_pages";
        case SYS_LINUX_GETDENTS64: return "getdents64";
        case SYS_LINUX_SET_TID_ADDRESS: return "set_tid_address";
        case SYS_LINUX_RESTART_SYSCALL: return "restart_syscall";
        case SYS_LINUX_SEMTIMEDOP: return "semtimedop";
        case SYS_LINUX_FADVISE64: return "fadvise64";
        case SYS_LINUX_TIMER_CREATE: return "timer_create";
        case SYS_LINUX_TIMER_SETTIME: return "timer_settime";
        case SYS_LINUX_TIMER_GETTIME: return "timer_gettime";
        case SYS_LINUX_TIMER_GETOVERRUN: return "timer_getoverrun";
        case SYS_LINUX_TIMER_DELETE: return "timer_delete";
        case SYS_LINUX_CLOCK_SETTIME: return "clock_settime";
        case SYS_LINUX_CLOCK_GETTIME: return "clock_gettime";
        case SYS_LINUX_CLOCK_GETRES: return "clock_getres";
        case SYS_LINUX_CLOCK_NANOSLEEP: return "clock_nanosleep";
        case SYS_LINUX_EXIT_GROUP: return "exit_group";
        case SYS_LINUX_EPOLL_WAIT: return "epoll_wait";
        case SYS_LINUX_EPOLL_CTL: return "epoll_ctl";
        case SYS_LINUX_TGKILL: return "tgkill";
        case SYS_LINUX_UTIMES: return "utimes";
        case SYS_LINUX_VSERVER: return "vserver";
        case SYS_LINUX_MBIND: return "mbind";
        case SYS_LINUX_SET_MEMPOLICY: return "set_mempolicy";
        case SYS_LINUX_GET_MEMPOLICY: return "get_mempolicy";
        case SYS_LINUX_MQ_OPEN: return "mq_open";
        case SYS_LINUX_MQ_UNLINK: return "mq_unlink";
        case SYS_LINUX_MQ_TIMEDSEND: return "mq_timedsend";
        case SYS_LINUX_MQ_TIMEDRECEIVE: return "mq_timedreceive";
        case SYS_LINUX_MQ_NOTIFY: return "mq_notify";
        case SYS_LINUX_MQ_GETSETATTR: return "mq_getsetattr";
        case SYS_LINUX_KEXEC_LOAD: return "kexec_load";
        case SYS_LINUX_WAITID: return "waitid";
        case SYS_LINUX_ADD_KEY: return "add_key";
        case SYS_LINUX_REQUEST_KEY: return "request_key";
        case SYS_LINUX_KEYCTL: return "keyctl";
        case SYS_LINUX_IOPRIO_SET: return "ioprio_set";
        case SYS_LINUX_IOPRIO_GET: return "ioprio_get";
        case SYS_LINUX_INOTIFY_INIT: return "inotify_init";
        case SYS_LINUX_INOTIFY_ADD_WATCH: return "inotify_add_watch";
        case SYS_LINUX_INOTIFY_RM_WATCH: return "inotify_rm_watch";
        case SYS_LINUX_MIGRATE_PAGES: return "migrate_pages";
        case SYS_LINUX_OPENAT: return "openat";
        case SYS_LINUX_MKDIRAT: return "mkdirat";
        case SYS_LINUX_MKNODAT: return "mknodat";
        case SYS_LINUX_FCHOWNAT: return "fchownat";
        case SYS_LINUX_FUTIMESAT: return "futimesat";
        case SYS_LINUX_NEWFSTATAT: return "newfstatat";
        case SYS_LINUX_UNLINKAT: return "unlinkat";
        case SYS_LINUX_RENAMEAT: return "renameat";
        case SYS_LINUX_LINKAT: return "linkat";
        case SYS_LINUX_SYMLINKAT: return "symlinkat";
        case SYS_LINUX_READLINKAT: return "readlinkat";
        case SYS_LINUX_FCHMODAT: return "fchmodat";
        case SYS_LINUX_FACCESSAT: return "faccessat";
        case SYS_LINUX_PSELECT6: return "pselect6";
        case SYS_LINUX_PPOLL: return "ppoll";
        case SYS_LINUX_UNSHARE: return "unshare";
        case SYS_LINUX_SET_ROBUST_LIST: return "set_robust_list";
        case SYS_LINUX_GET_ROBUST_LIST: return "get_robust_list";
        case SYS_LINUX_SPLICE: return "splice";
        case SYS_LINUX_TEE: return "tee";
        case SYS_LINUX_SYNC_FILE_RANGE: return "sync_file_range";
        case SYS_LINUX_VMSPLICE: return "vmsplice";
        case SYS_LINUX_MOVE_PAGES: return "move_pages";
        case SYS_LINUX_UTIMENSAT: return "utimensat";
        case SYS_LINUX_EPOLL_PWAIT: return "epoll_pwait";
        case SYS_LINUX_SIGNALFD: return "signalfd";
        case SYS_LINUX_TIMERFD_CREATE: return "timerfd_create";
        case SYS_LINUX_EVENTFD: return "eventfd";
        case SYS_LINUX_FALLOCATE: return "fallocate";
        case SYS_LINUX_TIMERFD_SETTIME: return "timerfd_settime";
        case SYS_LINUX_TIMERFD_GETTIME: return "timerfd_gettime";
        case SYS_LINUX_ACCEPT4: return "accept4";
        case SYS_LINUX_SIGNALFD4: return "signalfd4";
        case SYS_LINUX_EVENTFD2: return "eventfd2";
        case SYS_LINUX_EPOLL_CREATE1: return "epoll_create1";
        case SYS_LINUX_DUP3: return "dup3";
        case SYS_LINUX_PIPE2: return "pipe2";
        case SYS_LINUX_INOTIFY_INIT1: return "inotify_init1";
        case SYS_LINUX_PREADV: return "preadv";
        case SYS_LINUX_PWRITEV: return "pwritev";
        case SYS_LINUX_RT_TGSIGQUEUEINFO: return "rt_tgsigqueueinfo";
        case SYS_LINUX_PERF_EVENT_OPEN: return "perf_event_open";
        case SYS_LINUX_RECVMMSG: return "recvmmsg";
        case SYS_LINUX_FANOTIFY_INIT: return "fanotify_init";
        case SYS_LINUX_FANOTIFY_MARK: return "fanotify_mark";
        case SYS_LINUX_PRLIMIT64: return "prlimit64";
        case SYS_LINUX_NAME_TO_HANDLE_AT: return "name_to_handle_at";
        case SYS_LINUX_OPEN_BY_HANDLE_AT: return "open_by_handle_at";
        case SYS_LINUX_CLOCK_ADJTIME: return "clock_adjtime";
        case SYS_LINUX_SYNCFS: return "syncfs";
        case SYS_LINUX_SENDMMSG: return "sendmmsg";
        case SYS_LINUX_SETNS: return "setns";
        case SYS_LINUX_GETCPU: return "getcpu";
        case SYS_LINUX_PROCESS_VM_READV: return "process_vm_readv";
        case SYS_LINUX_PROCESS_VM_WRITEV: return "process_vm_writev";
        case SYS_LINUX_KCMP: return "kcmp";
        case SYS_LINUX_FINIT_MODULE: return "finit_module";
        case SYS_LINUX_SCHED_SETATTR: return "sched_setattr";
        case SYS_LINUX_SCHED_GETATTR: return "sched_getattr";
        case SYS_LINUX_RENAMEAT2: return "renameat2";
        case SYS_LINUX_SECCOMP: return "seccomp";
        case SYS_LINUX_GETRANDOM: return "getrandom";
        case SYS_LINUX_MEMFD_CREATE: return "memfd_create";
        case SYS_LINUX_KEXEC_FILE_LOAD: return "kexec_file_load";
        case SYS_LINUX_BPF: return "bpf";
        case SYS_LINUX_EXECVEAT: return "execveat";
        case SYS_LINUX_USERFAULTFD: return "userfaultfd";
        case SYS_LINUX_MEMBARRIER: return "membarrier";
        case SYS_LINUX_MLOCK2: return "mlock2";
        case SYS_LINUX_COPY_FILE_RANGE: return "copy_file_range";
        case SYS_LINUX_PREADV2: return "preadv2";
        case SYS_LINUX_PWRITEV2: return "pwritev2";
        case SYS_LINUX_PKEY_MPROTECT: return "pkey_mprotect";
        case SYS_LINUX_PKEY_ALLOC: return "pkey_alloc";
        case SYS_LINUX_PKEY_FREE: return "pkey_free";
        case SYS_LINUX_STATX: return "statx";
        case SYS_LINUX_IO_PGETEVENTS: return "io_pgetevents";
        case SYS_LINUX_RSEQ: return "rseq";
        case SYS_LINUX_PIDFD_SEND_SIGNAL: return "pidfd_send_signal";
        case SYS_LINUX_IO_URING_SETUP: return "io_uring_setup";
        case SYS_LINUX_IO_URING_ENTER: return "io_uring_enter";
        case SYS_LINUX_IO_URING_REGISTER: return "io_uring_register";
        case SYS_LINUX_OPEN_TREE: return "open_tree";
        case SYS_LINUX_MOVE_MOUNT: return "move_mount";
        case SYS_LINUX_FSOPEN: return "fsopen";
        case SYS_LINUX_FSCONFIG: return "fsconfig";
        case SYS_LINUX_FSMOUNT: return "fsmount";
        case SYS_LINUX_FSPICK: return "fspick";
        case SYS_LINUX_PIDFD_OPEN: return "pidfd_open";
        case SYS_LINUX_CLONE3: return "clone3";
        case SYS_LINUX_CLOSE_RANGE: return "close_range";
        case SYS_LINUX_OPENAT2: return "openat2";
        case SYS_LINUX_PIDFD_GETFD: return "pidfd_getfd";
        case SYS_LINUX_FACCESSAT2: return "faccessat2";
        case SYS_LINUX_PROCESS_MADVISE: return "process_madvise";
        case SYS_LINUX_EPOLL_PWAIT2: return "epoll_pwait2";
        case SYS_LINUX_MOUNT_SETATTR: return "mount_setattr";
        case SYS_LINUX_QUOTACTL_FD: return "quotactl_fd";
        case SYS_LINUX_LANDLOCK_CREATE_RULESET: return "landlock_create_ruleset";
        case SYS_LINUX_LANDLOCK_ADD_RULE: return "landlock_add_rule";
        case SYS_LINUX_LANDLOCK_RESTRICT_SELF: return "landlock_restrict_self";
        case SYS_LINUX_MEMFD_SECRET: return "memfd_secret";
        case SYS_LINUX_PROCESS_MRELEASE: return "process_mrelease";
        case SYS_LINUX_FUTEX_WAITV: return "futex_waitv";
        case SYS_LINUX_SET_MEMPOLICY_HOME_NODE: return "set_mempolicy_home_node";
        default: return "unknown";
    }
}

bool Linuxulator::IsSyscallImplemented(uint32_t syscall_number) {
    // For now, we'll say all syscalls are implemented (just return -1 for unimplemented ones)
    // In a real implementation, we would check if we have an actual implementation
    return true;
}

void Linuxulator::PrintLinuxProcessInfo(LinuxProcess* process) {
    if (!process) {
        return;
    }
    
    LOG("Linux Process Info:");
    LOG("  PID: " << process->pid);
    LOG("  PPID: " << process->ppid);
    LOG("  UID: " << process->uid);
    LOG("  GID: " << process->gid);
    LOG("  EUID: " << process->euid);
    LOG("  EGID: " << process->egid);
    LOG("  Start Time: " << process->start_time);
    LOG("  Priority: " << process->priority);
    LOG("  Nice: " << process->nice);
    LOG("  Name: " << process->name);
}

void Linuxulator::PrintLinuxProcesses() {
    LOG("Linux Process List:");
    for (uint32_t i = 0; i < process_count && i < MAX_LINUX_PROCESSES; i++) {
        if (linux_processes[i].pid != INVALID_PID) {
            LOG("  PID: " << linux_processes[i].pid << ", Name: " << linux_processes[i].name);
        }
    }
}

// Internal helper functions

int Linuxulator::DispatchSyscall(LinuxSyscallContext* context) {
    if (!context) {
        return -1;
    }
    
    // Check if the syscall is implemented
    if (!IsSyscallImplemented(context->syscall_number)) {
        LOG("Unimplemented Linux system call: " << context->syscall_number);
        context->error_code = -1;
        return -1;
    }
    
    // Dispatch to the appropriate syscall handler
    switch (context->syscall_number) {
        case SYS_LINUX_READ:
            return LinuxRead(context->arg1, (void*)context->arg2, context->arg3);
        case SYS_LINUX_WRITE:
            return LinuxWrite(context->arg1, (const void*)context->arg2, context->arg3);
        case SYS_LINUX_OPEN:
            return LinuxOpen((const char*)context->arg1, context->arg2, context->arg3);
        case SYS_LINUX_CLOSE:
            return LinuxClose(context->arg1);
        case SYS_LINUX_STAT:
            return LinuxStat((const char*)context->arg1, (struct FileStat*)context->arg2);
        case SYS_LINUX_FSTAT:
            return LinuxFstat(context->arg1, (struct FileStat*)context->arg2);
        case SYS_LINUX_LSEEK:
            return LinuxLseek(context->arg1, context->arg2, context->arg3);
        case SYS_LINUX_MMAP:
            return LinuxMmap((void*)context->arg1, context->arg2, context->arg3, context->arg4, context->arg5, context->arg6);
        case SYS_LINUX_MUNMAP:
            return LinuxMunmap((void*)context->arg1, context->arg2);
        case SYS_LINUX_BRK:
            return LinuxBrk((void*)context->arg1);
        case SYS_LINUX_FORK:
            return LinuxFork();
        case SYS_LINUX_EXECVE:
            return LinuxExecve((const char*)context->arg1, (char* const*)context->arg2, (char* const*)context->arg3);
        case SYS_LINUX_EXIT:
            return LinuxExit(context->arg1);
        case SYS_LINUX_WAIT4:
            return LinuxWait4(context->arg1, (int*)context->arg2, context->arg3, (struct rusage*)context->arg4);
        case SYS_LINUX_KILL:
            return LinuxKill(context->arg1, context->arg2);
        case SYS_LINUX_GETPID:
            return LinuxGetPid();
        default:
            LOG("Unhandled Linux system call: " << context->syscall_number 
                 << " (" << GetSyscallName(context->syscall_number) << ")");
            context->error_code = -1;
            return -1;
    }
}

LinuxProcess* Linuxulator::FindFreeLinuxProcessSlot() {
    for (int i = 0; i < MAX_LINUX_PROCESSES; i++) {
        if (linux_processes[i].pid == INVALID_PID) {
            return &linux_processes[i];
        }
    }
    
    return nullptr; // No free slots
}

bool Linuxulator::ValidateLinuxProcess(LinuxProcess* process) {
    if (!process) {
        return false;
    }
    
    // A valid process should have a valid PID
    return process->pid != INVALID_PID;
}

void Linuxulator::InitializeLinuxProcess(LinuxProcess* process) {
    if (!process) {
        return;
    }
    
    // Zero out the process structure
    memset(process, 0, sizeof(LinuxProcess));
    
    // Set default values
    process->pid = INVALID_PID;
    process->ppid = 0;
    process->uid = 0;
    process->gid = 0;
    process->euid = 0;
    process->egid = 0;
    process->suid = 0;
    process->sgid = 0;
    process->fsuid = 0;
    process->fsgid = 0;
    process->start_time = 0;
    process->utime = 0;
    process->stime = 0;
    process->cutime = 0;
    process->cstime = 0;
    process->priority = 20; // Default nice value
    process->nice = 0;
    process->num_threads = 1;
    process->vsize = 0;
    process->rss = 0;
    process->rsslim = 0;
    process->startcode = 0;
    process->endcode = 0;
    process->startstack = 0;
    process->kstkesp = 0;
    process->kstkeip = 0;
    process->signal = 0;
    process->blocked = 0;
    process->sigignore = 0;
    process->sigcatch = 0;
    process->wchan = 0;
    process->nswap = 0;
    process->cnswap = 0;
    process->exit_signal = 0;
    process->processor = 0;
    process->rt_priority = 0;
    process->policy = 0;
    process->delayacct_blkio_ticks = 0;
    process->guest_time = 0;
    process->cguest_time = 0;
    process->start_data = 0;
    process->end_data = 0;
    process->start_brk = 0;
    process->arg_start = 0;
    process->arg_end = 0;
    process->env_start = 0;
    process->env_end = 0;
    process->exit_code = 0;
    
    // Initialize the process name
    process->name[0] = '\0';
}

void Linuxulator::CleanupLinuxProcess(LinuxProcess* process) {
    if (!process) {
        return;
    }
    
    // In a real implementation, we would clean up any resources
    // associated with this process (memory, file descriptors, etc.)
    
    // For now, just mark the process as invalid
    process->pid = INVALID_PID;
}

int Linuxulator::TranslateErrno(int linux_errno) {
    // In a real implementation, we would translate Linux errno values
    // to our kernel's errno values
    return linux_errno;
}

int Linuxulator::TranslateSignal(int linux_signal) {
    // In a real implementation, we would translate Linux signal values
    // to our kernel's signal values
    return linux_signal;
}

int Linuxulator::TranslateOpenFlags(int linux_flags) {
    // Translate Linux open flags to our VFS flags
    int our_flags = 0;
    
    if (linux_flags & 0x001) our_flags |= O_RDONLY;
    if (linux_flags & 0x002) our_flags |= O_WRONLY;
    if (linux_flags & 0x003) our_flags |= O_RDWR;
    if (linux_flags & 0x040) our_flags |= O_CREAT;
    if (linux_flags & 0x200) our_flags |= O_TRUNC;
    if (linux_flags & 0x400) our_flags |= O_APPEND;
    if (linux_flags & 0x800) our_flags |= O_NONBLOCK;
    
    return our_flags;
}

int Linuxulator::TranslateProtFlags(int linux_prot) {
    // Translate Linux protection flags to our memory management flags
    int our_prot = 0;
    
    if (linux_prot & 0x1) our_prot |= PROT_READ;
    if (linux_prot & 0x2) our_prot |= PROT_WRITE;
    if (linux_prot & 0x4) our_prot |= PROT_EXEC;
    
    return our_prot;
}

int Linuxulator::TranslateMmapFlags(int linux_flags) {
    // Translate Linux mmap flags to our memory management flags
    int our_flags = 0;
    
    if (linux_flags & 0x01) our_flags |= MAP_SHARED;
    if (linux_flags & 0x02) our_flags |= MAP_PRIVATE;
    if (linux_flags & 0x10) our_flags |= MAP_FIXED;
    if (linux_flags & 0x20) our_flags |= MAP_ANONYMOUS;
    
    return our_flags;
}

int Linuxulator::TranslateWhence(int linux_whence) {
    // Translate Linux whence values to our VFS whence values
    switch (linux_whence) {
        case 0: return SEEK_SET;  // SEEK_SET
        case 1: return SEEK_CUR;  // SEEK_CUR
        case 2: return SEEK_END;  // SEEK_END
        default: return SEEK_SET; // Default to SEEK_SET
    }
}

// Global functions

bool InitializeLinuxulator() {
    if (!g_linuxulator) {
        g_linuxulator = new Linuxulator();
        if (!g_linuxulator) {
            LOG("Failed to create Linuxulator instance");
            return false;
        }
        
        if (!g_linuxulator->Initialize()) {
            LOG("Failed to initialize Linuxulator");
            delete g_linuxulator;
            g_linuxulator = nullptr;
            return false;
        }
        
        LOG("Linuxulator initialized successfully");
    }
    
    return true;
}

extern "C" int HandleLinuxSyscall(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, uint32_t arg6) {
    if (!g_linuxulator) {
        return -1;
    }
    
    LinuxSyscallContext context;
    context.syscall_number = syscall_number;
    context.arg1 = arg1;
    context.arg2 = arg2;
    context.arg3 = arg3;
    context.arg4 = arg4;
    context.arg5 = arg5;
    context.arg6 = arg6;
    context.return_value = 0;
    context.error_code = 0;
    
    return g_linuxulator->HandleSyscall(&context);
}

bool RunLinuxExecutable(const char* filename, char* const argv[], char* const envp[]) {
    if (!filename || !g_linuxulator) {
        return false;
    }
    
    return g_linuxulator->LoadLinuxBinary(filename, argv, envp);
}