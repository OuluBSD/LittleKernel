#ifndef _Kernel_Syscalls_h_
#define _Kernel_Syscalls_h_

#include "Common.h"
#include "Defs.h"
#include "Vfs.h"          // For file operations
#include "ProcessControlBlock.h"  // For process operations
#include "Synchronization.h"      // For synchronization operations

// System call numbers (Linux-compatible)
#define SYS_READ                3
#define SYS_WRITE               4
#define SYS_OPEN                5
#define SYS_CLOSE               6
#define SYS_WAITPID            7
#define SYS_CREAT              8
#define SYS_LINK               9
#define SYS_UNLINK            10
#define SYS_EXECVE            11
#define SYS_CHDIR             12
#define SYS_TIME              13
#define SYS_MKNOD             14
#define SYS_CHMOD             15
#define SYS_LCHOWN            16
#define SYS_STAT              18
#define SYS_LSEEK             19
#define SYS_GETPID            20
#define SYS_MOUNT             21
#define SYS_UMOUNT            22
#define SYS_SETUID            23
#define SYS_GETUID            24
#define SYS_STIME             25
#define SYS_PTRACE            26
#define SYS_ALARM             27
#define SYS_FSTAT             28
#define SYS_PAUSE             29
#define SYS_UTIME             30
#define SYS_ACCESS            33
#define SYS_NICE              34
#define SYS_SYNC              36
#define SYS_KILL              37
#define SYS_RENAME            38
#define SYS_MKDIR             39
#define SYS_RMDIR             40
#define SYS_DUP               41
#define SYS_PIPE              42
#define SYS_TIMES             43
#define SYS_BRK               45
#define SYS_SETGID            46
#define SYS_GETGID            47
#define SYS_SIGNAL            48
#define SYS_GETEUID           49
#define SYS_GETEGID           50
#define SYS_ACCT              51
#define SYS_UMOUNT2           52
#define SYS_IOCTL             54
#define SYS_FCNTL             55
#define SYS_SETPGID           57
#define SYS_UMASK             60
#define SYS_CHROOT            61
#define SYS_USTAT             62
#define SYS_DUP2              63
#define SYS_GETPPID           64
#define SYS_GETPGRP           65
#define SYS_SETSID            66
#define SYS_SIGACTION         67
#define SYS_SETREUID          70
#define SYS_SETREGID          71
#define SYS_SIGSUSPEND        72
#define SYS_SIGPENDING        73
#define SYS_SETHOSTNAME       74
#define SYS_SETRLIMIT         75
#define SYS_GETRLIMIT         76
#define SYS_GETRUSAGE         77
#define SYS_GETTIMEOFDAY      78
#define SYS_SETTIMEOFDAY      79
#define SYS_GETGROUPS         80
#define SYS_SETGROUPS         81
#define SYS_SYMLINK           83
#define SYS_READLINK          85
#define SYS_USELIB            86
#define SYS_SWAPON            87
#define SYS_REBOOT            88
#define SYS_MMAP              90
#define SYS_MUNMAP            91
#define SYS_TRUNCATE          92
#define SYS_FTRUNCATE         93
#define SYS_FCHMOD            94
#define SYS_FCHOWN            96
#define SYS_GETPRIORITY       96
#define SYS_SETPRIORITY       97
#define SYS_STATFS            99
#define SYS_FSTATFS          100
#define SYS_SOCKET           100
#define SYS_LISTEN           101
#define SYS_ACCEPT           102
#define SYS_BIND             104
#define SYS_CONNECT          105
#define SYS_SENDTO           114
#define SYS_RECVFROM         115
#define SYS_FORK             2           // Fork system call (missing from original list)
#define SYS_CLONE            120         // Clone system call
#define SYS_SENDMSG          116
#define SYS_RECVMSG          117
#define SYS_SHUTDOWN         118
#define SYS_SETSOCKOPT       119
#define SYS_GETSOCKOPT       120
#define SYS_RECV             128
#define SYS_SEND             129
#define SYS_EXIT             130
#define SYS_UNAME            122
#define SYS_LCHOWN32         123
#define SYS_GETCWD           124
#define SYS_CAPGET          125
#define SYS_CAPSET          126
#define SYS_SIGALTSTACK     127
#define SYS_MKNOD16         128
#define SYS_STATFS64        137
#define SYS_FSTATFS64       138
#define SYS_FADVISE64_64    140
#define SYS_FSTATAT64       153
#define SYS_MMAP2           154
#define SYS_FUTEX           166
#define SYS_SCHED_SETAFFINITY 167
#define SYS_SCHED_GETAFFINITY 168
#define SYS_SET_THREAD_AREA 172
#define SYS_GET_THREAD_AREA 173
#define SYS_QUOTACTL        179
#define SYS_GETTID          186
#define SYS_READAHEAD       187
#define SYS_SETXATTR        188
#define SYS_LSETXATTR       189
#define SYS_FSETXATTR       190
#define SYS_GETXATTR        191
#define SYS_LGETXATTR       192
#define SYS_FGETXATTR       193
#define SYS_LISTXATTR       194
#define SYS_LLISTXATTR      195
#define SYS_FLISTXATTR      196
#define SYS_REMOVEXATTR     197
#define SYS_LREMOVEXATTR    198
#define SYS_FREMOVEXATTR    199
#define SYS_TKILL           208
#define SYS_SENDFILE64      209
#define SYS_EXIT_GROUP      222
#define SYS_EPOLL_CREATE    223
#define SYS_EPOLL_CTL       224
#define SYS_EPOLL_WAIT      225
#define SYS_REMAP_FILE_PAGES 226
#define SYS_SET_TID_ADDRESS 227
#define SYS_TIMER_CREATE    228
#define SYS_TIMER_SETTIME   229
#define SYS_TIMER_GETTIME   230
#define SYS_TIMER_GETOVERRUN 231
#define SYS_TIMER_DELETE    232
#define SYS_CLOCK_SETTIME   233
#define SYS_CLOCK_GETTIME   234
#define SYS_CLOCK_GETRES    235
#define SYS_CLOCK_NANOSLEEP 236
#define SYS_TGKILL          239
#define SYS_UTIMES          240
#define SYS_MQ_OPEN         241
#define SYS_MQ_UNLINK       242
#define SYS_MQ_TIMEDSEND    243
#define SYS_MQ_TIMEDRECEIVE 244
#define SYS_MQ_NOTIFY       245
#define SYS_MQ_GETSETATTR   246
#define SYS_KEXEC_LOAD      247
#define SYS_WAITID          248
#define SYS_ADD_KEY         249
#define SYS_REQUEST_KEY     250
#define SYS_KEYCTL          251
#define SYS_IOPERM          252
#define SYS_INOTIFY_INIT    253
#define SYS_INOTIFY_ADD_WATCH 254
#define SYS_INOTIFY_RM_WATCH 255
#define SYS_MIGRATE_PAGES   256
#define SYS_OPENAT          257
#define SYS_MKDIRAT         258
#define SYS_MKNODAT         259
#define SYS_FCHOWNAT        260
#define SYS_FUTIMESAT       261
#define SYS_UNLINKAT        263
#define SYS_RENAMEAT        264
#define SYS_LINKAT          265
#define SYS_SYMLINKAT       266
#define SYS_READLINKAT      267
#define SYS_FCHMODAT        268
#define SYS_FACCESSAT       269
#define SYS_PSELECT6        270
#define SYS_PPOLL           271
#define SYS_UNSHARE         272
#define SYS_SET_ROBUST_LIST 273
#define SYS_GET_ROBUST_LIST 274
#define SYS_SPLICE          275
#define SYS_SYNC_FILE_RANGE 277
#define SYS_TEE             276
#define SYS_VMSPLICE        277
#define SYS_MOVE_PAGES      278
#define SYS_GETCPU          279
#define SYS_EPOLL_PWAIT     280
#define SYS_UTIMENSAT       281
#define SYS_SIGNALFD        282
#define SYS_TIMERFD_CREATE  283
#define SYS_EVENTFD         284
#define SYS_FALLOCATE       285
#define SYS_TIMERFD_SETTIME 286
#define SYS_TIMERFD_GETTIME 287
#define SYS_SIGNALFD4       289
#define SYS_EVENTFD2        290
#define SYS_EPOLL_CREATE1   291
#define SYS_DUP3            292
#define SYS_PIPE2           293
#define SYS_INOTIFY_INIT1   294
#define SYS_PREADV          295
#define SYS_PWRITEV         296
#define SYS_RT_TGSIGQUEUEINFO 297
#define SYS_PERF_EVENT_OPEN 298
#define SYS_RECVMMSG        299
#define SYS_FANOTIFY_INIT   300
#define SYS_FANOTIFY_MARK   301
#define SYS_PRLIMIT64       302
#define SYS_NAME_TO_HANDLE_AT 303
#define SYS_OPEN_BY_HANDLE_AT 304
#define SYS_CLOCK_ADJTIME   305
#define SYS_SYNCFS          306
#define SYS_SENDMMSG        307
#define SYS_SETNS           308
#define SYS_PROCESS_VM_READV 309
#define SYS_PROCESS_VM_WRITEV 310
#define SYS_KCMP            312
#define SYS_FINIT_MODULE    313
#define SYS_SCHED_SETATTR   314
#define SYS_SCHED_GETATTR   315
#define SYS_RENAMEAT2       316
#define SYS_SECCOMP         317
#define SYS_GETRANDOM       318
#define SYS_MEMFD_CREATE    319
#define SYS_BPF             321
#define SYS_EXECVEAT        322
#define SYS_USERFAULTFD     323
#define SYS_MEMBARRIER      324
#define SYS_MLOCK2          325
#define SYS_COPY_FILE_RANGE 326
#define SYS_PREADV2         327
#define SYS_PWRITEV2        328
#define SYS_PKEY_MPROTECT   329
#define SYS_PKEY_ALLOC      330
#define SYS_PKEY_FREE       331
#define SYS_STATX           332
#define SYS_ARCH_SPECIFIC_SYSCALL 333

// System call function type
typedef int (*syscall_func_t)(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);

// System call table structure
struct SyscallTable {
    syscall_func_t functions[334];  // Array of system call function pointers
    const char* names[334];        // Names of system calls (for debugging)
    uint32 count;                  // Number of system calls in the table
};

// System call result codes
#define SYSCALL_SUCCESS 0
#define SYSCALL_ERROR -1

// System call interface class
class SyscallInterface {
private:
    SyscallTable syscall_table;
    Spinlock syscall_lock;  // Lock for system call access

public:
    SyscallInterface();
    ~SyscallInterface();
    
    // Initialize the system call interface
    bool Initialize();
    
    // Register a system call
    bool RegisterSyscall(uint32 syscall_num, syscall_func_t func, const char* name);
    
    // Dispatch a system call
    int DispatchSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    
    // Get system call name for debugging
    const char* GetSyscallName(uint32 syscall_num);
    
    // Individual system call implementations
    int SysRead(int fd, void* buf, size_t count);
    int SysWrite(int fd, const void* buf, size_t count);
    int SysOpen(const char* pathname, int flags, mode_t mode);
    int SysClose(int fd);
    int SysFork();
    int SysExecve(const char* filename, char* const argv[], char* const envp[]);
    int SysWaitPid(pid_t pid, int* status, int options);
    int SysGetPid();
    int SysMmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
    int SysMunmap(void* addr, size_t length);
    int SysExit(int status);
    int SysKill(pid_t pid, int sig);
    int SysStat(const char* pathname, struct FileStat* statbuf);
    int SysFstat(int fd, struct FileStat* statbuf);
    int SysLseek(int fd, off_t offset, int whence);
    int SysMkdir(const char* pathname, mode_t mode);
    int SysRmdir(const char* pathname);
    int SysUnlink(const char* pathname);
    int SysRename(const char* oldpath, const char* newpath);
    int SysGettimeofday(struct timeval* tv, struct timezone* tz);
    int SysBrk(void* addr);
    int SysSignal(int signum, void (*handler)(int));
    int SysSigaction(int signum, const struct sigaction* act, struct sigaction* oldact);
    int SysPipe(int pipefd[2]);
    int SysDup(int oldfd);
    int SysDup2(int oldfd, int newfd);
    int SysChdir(const char* path);
    int SysGetcwd(char* buf, size_t size);
    int SysUname(struct utsname* buf);
    
    // Network system calls
    int SysSocket(int domain, int type, int protocol);
    int SysBind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    int SysConnect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    int SysListen(int sockfd, int backlog);
    int SysAccept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    int SysSendto(int sockfd, const void* buf, size_t len, int flags,
                 const struct sockaddr* dest_addr, socklen_t addrlen);
    int SysRecvfrom(int sockfd, void* buf, size_t len, int flags,
                   struct sockaddr* src_addr, socklen_t* addrlen);
    int SysSendmsg(int sockfd, const struct msghdr* msg, int flags);
    int SysRecvmsg(int sockfd, struct msghdr* msg, int flags);
    int SysShutdown(int sockfd, int how);
    int SysSetsockopt(int sockfd, int level, int optname,
                     const void* optval, socklen_t optlen);
    int SysGetsockopt(int sockfd, int level, int optname,
                     void* optval, socklen_t* optlen);
    int SysRecv(int sockfd, void* buf, size_t len, int flags);
    int SysSend(int sockfd, const void* buf, size_t len, int flags);

private:
    // Initialize the system call table with default handlers
    void InitializeSyscallTable();
    
    // Default handler for unimplemented system calls
    static int DefaultHandler(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    
    // Wrapper functions for system calls to make them compatible with function pointer interface
    static int SysReadWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysWriteWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysOpenWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysCloseWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysStatWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysFstatWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysLseekWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysGetPidWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysExitWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysKillWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysMkdirWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysRmdirWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysUnlinkWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysRenameWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysChdirWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysGetcwdWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysBrkWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysMmapWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysMunmapWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysPipeWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysDupWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysDup2Wrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysUnameWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysGettimeofdayWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysForkWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysExecveWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysWaitPidWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysSignalWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysSigactionWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    
    // Network system calls
    static int SysSocketWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysBindWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysConnectWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysListenWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysAcceptWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysSendtoWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysRecvfromWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysSendmsgWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysRecvmsgWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysShutdownWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysSetsockoptWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysGetsockoptWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysRecvWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
    static int SysSendWrapper(uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);
};

// External instance of the system call interface
extern SyscallInterface* g_syscall_interface;

// Initialize the system call interface
bool InitializeSyscalls();

// System call entry point (called from assembly)
extern "C" int HandleSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);

// Helper functions for system call implementations
pid_t GetCurrentProcessId();
uid_t GetCurrentUserId();
gid_t GetCurrentGroupId();

#endif