#ifndef _Kernel_Linuxulator_h_
#define _Kernel_Linuxulator_h_

#include "Common.h"
#include "Defs.h"
#include "Vfs.h"
#include "ProcessControlBlock.h"

// Linuxulator constants
#define LINUXULATOR_MAGIC 0x4C494E55  // "LINU" in hex
#define MAX_LINUX_PROCESSES 1024
#define LINUX_PAGE_SIZE 4096

// File access flags (matching Linux values)
#define O_RDONLY 0x001
#define O_WRONLY 0x002
#define O_RDWR 0x003
#define O_CREAT 0x040
#define O_EXCL 0x080
#define O_TRUNC 0x200
#define O_APPEND 0x400

// Linux system call numbers (some common ones)
#define SYS_LINUX_READ 0
#define SYS_LINUX_WRITE 1
#define SYS_LINUX_OPEN 2
#define SYS_LINUX_CLOSE 3
#define SYS_LINUX_STAT 4
#define SYS_LINUX_FSTAT 5
#define SYS_LINUX_LSTAT 6
#define SYS_LINUX_POLL 7
#define SYS_LINUX_LSEEK 8
#define SYS_LINUX_MMAP 9
#define SYS_LINUX_MPROTECT 10
#define SYS_LINUX_MUNMAP 11
#define SYS_LINUX_BRK 12
#define SYS_LINUX_RT_SIGACTION 13
#define SYS_LINUX_RT_SIGPROCMASK 14
#define SYS_LINUX_RT_SIGRETURN 15
#define SYS_LINUX_IOCTL 16
#define SYS_LINUX_PREAD64 17
#define SYS_LINUX_PWRITE64 18
#define SYS_LINUX_READV 19
#define SYS_LINUX_WRITEV 20
#define SYS_LINUX_ACCESS 21
#define SYS_LINUX_PIPE 22
#define SYS_LINUX_SELECT 23
#define SYS_LINUX_SCHED_YIELD 24
#define SYS_LINUX_MREMAP 25
#define SYS_LINUX_MSYNC 26
#define SYS_LINUX_MINCORE 27
#define SYS_LINUX_MADVISE 28
#define SYS_LINUX_SHMGET 29
#define SYS_LINUX_SHMAT 30
#define SYS_LINUX_SHMCTL 31
#define SYS_LINUX_DUP 32
#define SYS_LINUX_DUP2 33
#define SYS_LINUX_PAUSE 34
#define SYS_LINUX_NANOSLEEP 35
#define SYS_LINUX_GETITIMER 36
#define SYS_LINUX_ALARM 37
#define SYS_LINUX_SETITIMER 38
#define SYS_LINUX_GETPID 39
#define SYS_LINUX_SENDFILE 40
#define SYS_LINUX_SOCKET 41
#define SYS_LINUX_CONNECT 42
#define SYS_LINUX_ACCEPT 43
#define SYS_LINUX_SENDTO 44
#define SYS_LINUX_RECVFROM 45
#define SYS_LINUX_SENDMSG 46
#define SYS_LINUX_RECVMSG 47
#define SYS_LINUX_SHUTDOWN 48
#define SYS_LINUX_BIND 49
#define SYS_LINUX_LISTEN 50
#define SYS_LINUX_GETSOCKNAME 51
#define SYS_LINUX_GETPEERNAME 52
#define SYS_LINUX_SOCKETPAIR 53
#define SYS_LINUX_SETSOCKOPT 54
#define SYS_LINUX_GETSOCKOPT 55
#define SYS_LINUX_CLONE 56
#define SYS_LINUX_FORK 57
#define SYS_LINUX_VFORK 58
#define SYS_LINUX_EXECVE 59
#define SYS_LINUX_EXIT 60
#define SYS_LINUX_WAIT4 61
#define SYS_LINUX_KILL 62
#define SYS_LINUX_UNAME 63
#define SYS_LINUX_SEMGET 64
#define SYS_LINUX_SEMOP 65
#define SYS_LINUX_SEMCTL 66
#define SYS_LINUX_SHMDT 67
#define SYS_LINUX_MSGGET 68
#define SYS_LINUX_MSGSND 69
#define SYS_LINUX_MSGRCV 70
#define SYS_LINUX_MSGCTL 71
#define SYS_LINUX_FCNTL 72
#define SYS_LINUX_FLOCK 73
#define SYS_LINUX_FSYNC 74
#define SYS_LINUX_FDATASYNC 75
#define SYS_LINUX_TRUNCATE 76
#define SYS_LINUX_FTRUNCATE 77
#define SYS_LINUX_GETDENTS 78
#define SYS_LINUX_GETCWD 79
#define SYS_LINUX_CHDIR 80
#define SYS_LINUX_FCHDIR 81
#define SYS_LINUX_RENAME 82
#define SYS_LINUX_MKDIR 83
#define SYS_LINUX_RMDIR 84
#define SYS_LINUX_CREAT 85
#define SYS_LINUX_LINK 86
#define SYS_LINUX_UNLINK 87
#define SYS_LINUX_SYMLINK 88
#define SYS_LINUX_READLINK 89
#define SYS_LINUX_CHMOD 90
#define SYS_LINUX_FCHMOD 91
#define SYS_LINUX_CHOWN 92
#define SYS_LINUX_FCHOWN 93
#define SYS_LINUX_LCHOWN 94
#define SYS_LINUX_UMASK 95
#define SYS_LINUX_GETTIMEOFDAY 96
#define SYS_LINUX_GETRLIMIT 97
#define SYS_LINUX_GETRUSAGE 98
#define SYS_LINUX_SYSINFO 99
#define SYS_LINUX_TIMES 100
#define SYS_LINUX_PTRACE 101
#define SYS_LINUX_GETUID 102
#define SYS_LINUX_SYSLOG 103
#define SYS_LINUX_GETGID 104
#define SYS_LINUX_SETUID 105
#define SYS_LINUX_SETGID 106
#define SYS_LINUX_GETEUID 107
#define SYS_LINUX_GETEGID 108
#define SYS_LINUX_SETPGID 109
#define SYS_LINUX_GETPPID 110
#define SYS_LINUX_GETPGRP 111
#define SYS_LINUX_SETSID 112
#define SYS_LINUX_SETREUID 113
#define SYS_LINUX_SETREGID 114
#define SYS_LINUX_GETGROUPS 115
#define SYS_LINUX_SETGROUPS 116
#define SYS_LINUX_SETRESUID 117
#define SYS_LINUX_GETRESUID 118
#define SYS_LINUX_SETRESGID 119
#define SYS_LINUX_GETRESGID 120
#define SYS_LINUX_GETPGID 121
#define SYS_LINUX_SETFSUID 122
#define SYS_LINUX_SETFSGID 123
#define SYS_LINUX_GETSID 124
#define SYS_LINUX_CAPGET 125
#define SYS_LINUX_CAPSET 126
#define SYS_LINUX_RT_SIGPENDING 127
#define SYS_LINUX_RT_SIGTIMEDWAIT 128
#define SYS_LINUX_RT_SIGQUEUEINFO 129
#define SYS_LINUX_RT_SIGSUSPEND 130
#define SYS_LINUX_SIGALTSTACK 131
#define SYS_LINUX_UTIME 132
#define SYS_LINUX_MKNOD 133
#define SYS_LINUX_USELIB 134
#define SYS_LINUX_PERSONALITY 135
#define SYS_LINUX_USTAT 136
#define SYS_LINUX_STATFS 137
#define SYS_LINUX_FSTATFS 138
#define SYS_LINUX_SYSFS 139
#define SYS_LINUX_GETPRIORITY 140
#define SYS_LINUX_SETPRIORITY 141
#define SYS_LINUX_SCHED_SETPARAM 142
#define SYS_LINUX_SCHED_GETPARAM 143
#define SYS_LINUX_SCHED_SETSCHEDULER 144
#define SYS_LINUX_SCHED_GETSCHEDULER 145
#define SYS_LINUX_SCHED_GET_PRIORITY_MAX 146
#define SYS_LINUX_SCHED_GET_PRIORITY_MIN 147
#define SYS_LINUX_SCHED_RR_GET_INTERVAL 148
#define SYS_LINUX_MLOCK 149
#define SYS_LINUX_MUNLOCK 150
#define SYS_LINUX_MLOCKALL 151
#define SYS_LINUX_MUNLOCKALL 152
#define SYS_LINUX_VHANGUP 153
#define SYS_LINUX_MODIFY_LDT 154
#define SYS_LINUX_PIVOT_ROOT 155
#define SYS_LINUX_SYSCTL 156
#define SYS_LINUX_PRCTL 157
#define SYS_LINUX_ARCH_PRCTL 158
#define SYS_LINUX_ADJTIMEX 159
#define SYS_LINUX_SETRLIMIT 160
#define SYS_LINUX_CHROOT 161
#define SYS_LINUX_SYNC 162
#define SYS_LINUX_ACCT 163
#define SYS_LINUX_SETTIMEOFDAY 164
#define SYS_LINUX_MOUNT 165
#define SYS_LINUX_UMOUNT2 166
#define SYS_LINUX_SWAPON 167
#define SYS_LINUX_SWAPOFF 168
#define SYS_LINUX_REBOOT 169
#define SYS_LINUX_SETHOSTNAME 170
#define SYS_LINUX_SETDOMAINNAME 171
#define SYS_LINUX_IOPL 172
#define SYS_LINUX_IOPERM 173
#define SYS_LINUX_CREATE_MODULE 174
#define SYS_LINUX_INIT_MODULE 175
#define SYS_LINUX_DELETE_MODULE 176
#define SYS_LINUX_GET_KERNEL_SYMS 177
#define SYS_LINUX_QUERY_MODULE 178
#define SYS_LINUX_QUOTACTL 179
#define SYS_LINUX_NFSSERVCTL 180
#define SYS_LINUX_GETPMSG 181
#define SYS_LINUX_PUTPMSG 182
#define SYS_LINUX_AFS 183
#define SYS_LINUX_TUXCALL 184
#define SYS_LINUX_SECURITY 185
#define SYS_LINUX_GETTID 186
#define SYS_LINUX_READAHEAD 187
#define SYS_LINUX_SETXATTR 188
#define SYS_LINUX_LSETXATTR 189
#define SYS_LINUX_FSETXATTR 190
#define SYS_LINUX_GETXATTR 191
#define SYS_LINUX_LGETXATTR 192
#define SYS_LINUX_FGETXATTR 193
#define SYS_LINUX_LISTXATTR 194
#define SYS_LINUX_LLISTXATTR 195
#define SYS_LINUX_FLISTXATTR 196
#define SYS_LINUX_REMOVEXATTR 197
#define SYS_LINUX_LREMOVEXATTR 198
#define SYS_LINUX_FREMOVEXATTR 199
#define SYS_LINUX_TKILL 200
#define SYS_LINUX_TIME 201
#define SYS_LINUX_FUTEX 202
#define SYS_LINUX_SCHED_SETAFFINITY 203
#define SYS_LINUX_SCHED_GETAFFINITY 204
#define SYS_LINUX_SET_THREAD_AREA 205
#define SYS_LINUX_GET_THREAD_AREA 206
#define SYS_LINUX_IO_SETUP 207
#define SYS_LINUX_IO_DESTROY 208
#define SYS_LINUX_IO_GETEVENTS 209
#define SYS_LINUX_IO_SUBMIT 210
#define SYS_LINUX_IO_CANCEL 211
#define SYS_LINUX_GET_THREAD_ID 212
#define SYS_LINUX_LOOKUP_DCOOKIE 213
#define SYS_LINUX_EPOLL_CREATE 214
#define SYS_LINUX_EPOLL_CTL_OLD 215
#define SYS_LINUX_EPOLL_WAIT_OLD 216
#define SYS_LINUX_REMAP_FILE_PAGES 217
#define SYS_LINUX_GETDENTS64 218
#define SYS_LINUX_SET_TID_ADDRESS 219
#define SYS_LINUX_RESTART_SYSCALL 220
#define SYS_LINUX_SEMTIMEDOP 221
#define SYS_LINUX_FADVISE64 222
#define SYS_LINUX_TIMER_CREATE 223
#define SYS_LINUX_TIMER_SETTIME 224
#define SYS_LINUX_TIMER_GETTIME 225
#define SYS_LINUX_TIMER_GETOVERRUN 226
#define SYS_LINUX_TIMER_DELETE 227
#define SYS_LINUX_CLOCK_SETTIME 228
#define SYS_LINUX_CLOCK_GETTIME 229
#define SYS_LINUX_CLOCK_GETRES 230
#define SYS_LINUX_CLOCK_NANOSLEEP 231
#define SYS_LINUX_EXIT_GROUP 232
#define SYS_LINUX_EPOLL_WAIT 233
#define SYS_LINUX_EPOLL_CTL 234
#define SYS_LINUX_TGKILL 235
#define SYS_LINUX_UTIMES 236
#define SYS_LINUX_VSERVER 237
#define SYS_LINUX_MBIND 238
#define SYS_LINUX_SET_MEMPOLICY 239
#define SYS_LINUX_GET_MEMPOLICY 240
#define SYS_LINUX_MQ_OPEN 241
#define SYS_LINUX_MQ_UNLINK 242
#define SYS_LINUX_MQ_TIMEDSEND 243
#define SYS_LINUX_MQ_TIMEDRECEIVE 244
#define SYS_LINUX_MQ_NOTIFY 245
#define SYS_LINUX_MQ_GETSETATTR 246
#define SYS_LINUX_KEXEC_LOAD 247
#define SYS_LINUX_WAITID 248
#define SYS_LINUX_ADD_KEY 249
#define SYS_LINUX_REQUEST_KEY 250
#define SYS_LINUX_KEYCTL 251
#define SYS_LINUX_IOPRIO_SET 252
#define SYS_LINUX_IOPRIO_GET 253
#define SYS_LINUX_INOTIFY_INIT 254
#define SYS_LINUX_INOTIFY_ADD_WATCH 255
#define SYS_LINUX_INOTIFY_RM_WATCH 256
#define SYS_LINUX_MIGRATE_PAGES 257
#define SYS_LINUX_OPENAT 258
#define SYS_LINUX_MKDIRAT 259
#define SYS_LINUX_MKNODAT 260
#define SYS_LINUX_FCHOWNAT 261
#define SYS_LINUX_FUTIMESAT 262
#define SYS_LINUX_NEWFSTATAT 263
#define SYS_LINUX_UNLINKAT 264
#define SYS_LINUX_RENAMEAT 265
#define SYS_LINUX_LINKAT 266
#define SYS_LINUX_SYMLINKAT 267
#define SYS_LINUX_READLINKAT 268
#define SYS_LINUX_FCHMODAT 269
#define SYS_LINUX_FACCESSAT 270
#define SYS_LINUX_PSELECT6 271
#define SYS_LINUX_PPOLL 272
#define SYS_LINUX_UNSHARE 273
#define SYS_LINUX_SET_ROBUST_LIST 274
#define SYS_LINUX_GET_ROBUST_LIST 275
#define SYS_LINUX_SPLICE 276
#define SYS_LINUX_TEE 277
#define SYS_LINUX_SYNC_FILE_RANGE 278
#define SYS_LINUX_VMSPLICE 279
#define SYS_LINUX_MOVE_PAGES 280
#define SYS_LINUX_UTIMENSAT 281
#define SYS_LINUX_EPOLL_PWAIT 282
#define SYS_LINUX_SIGNALFD 283
#define SYS_LINUX_TIMERFD_CREATE 284
#define SYS_LINUX_EVENTFD 285
#define SYS_LINUX_FALLOCATE 286
#define SYS_LINUX_TIMERFD_SETTIME 287
#define SYS_LINUX_TIMERFD_GETTIME 288
#define SYS_LINUX_ACCEPT4 289
#define SYS_LINUX_SIGNALFD4 290
#define SYS_LINUX_EVENTFD2 291
#define SYS_LINUX_EPOLL_CREATE1 292
#define SYS_LINUX_DUP3 293
#define SYS_LINUX_PIPE2 294
#define SYS_LINUX_INOTIFY_INIT1 295
#define SYS_LINUX_PREADV 296
#define SYS_LINUX_PWRITEV 297
#define SYS_LINUX_RT_TGSIGQUEUEINFO 298
#define SYS_LINUX_PERF_EVENT_OPEN 299
#define SYS_LINUX_RECVMMSG 300
#define SYS_LINUX_FANOTIFY_INIT 301
#define SYS_LINUX_FANOTIFY_MARK 302
#define SYS_LINUX_PRLIMIT64 303
#define SYS_LINUX_NAME_TO_HANDLE_AT 304
#define SYS_LINUX_OPEN_BY_HANDLE_AT 305
#define SYS_LINUX_CLOCK_ADJTIME 306
#define SYS_LINUX_SYNCFS 307
#define SYS_LINUX_SENDMMSG 308
#define SYS_LINUX_SETNS 309
#define SYS_LINUX_GETCPU 310
#define SYS_LINUX_PROCESS_VM_READV 311
#define SYS_LINUX_PROCESS_VM_WRITEV 312
#define SYS_LINUX_KCMP 313
#define SYS_LINUX_FINIT_MODULE 314
#define SYS_LINUX_SCHED_SETATTR 315
#define SYS_LINUX_SCHED_GETATTR 316
#define SYS_LINUX_RENAMEAT2 317
#define SYS_LINUX_SECCOMP 318
#define SYS_LINUX_GETRANDOM 319
#define SYS_LINUX_MEMFD_CREATE 320
#define SYS_LINUX_KEXEC_FILE_LOAD 321
#define SYS_LINUX_BPF 322
#define SYS_LINUX_EXECVEAT 323
#define SYS_LINUX_USERFAULTFD 324
#define SYS_LINUX_MEMBARRIER 325
#define SYS_LINUX_MLOCK2 326
#define SYS_LINUX_COPY_FILE_RANGE 327
#define SYS_LINUX_PREADV2 328
#define SYS_LINUX_PWRITEV2 329
#define SYS_LINUX_PKEY_MPROTECT 330
#define SYS_LINUX_PKEY_ALLOC 331
#define SYS_LINUX_PKEY_FREE 332
#define SYS_LINUX_STATX 333
#define SYS_LINUX_IO_PGETEVENTS 334
#define SYS_LINUX_RSEQ 335
#define SYS_LINUX_PIDFD_SEND_SIGNAL 424
#define SYS_LINUX_IO_URING_SETUP 425
#define SYS_LINUX_IO_URING_ENTER 426
#define SYS_LINUX_IO_URING_REGISTER 427
#define SYS_LINUX_OPEN_TREE 428
#define SYS_LINUX_MOVE_MOUNT 429
#define SYS_LINUX_FSOPEN 430
#define SYS_LINUX_FSCONFIG 431
#define SYS_LINUX_FSMOUNT 432
#define SYS_LINUX_FSPICK 433
#define SYS_LINUX_PIDFD_OPEN 434
#define SYS_LINUX_CLONE3 435
#define SYS_LINUX_CLOSE_RANGE 436
#define SYS_LINUX_OPENAT2 437
#define SYS_LINUX_PIDFD_GETFD 438
#define SYS_LINUX_FACCESSAT2 439
#define SYS_LINUX_PROCESS_MADVISE 440
#define SYS_LINUX_EPOLL_PWAIT2 441
#define SYS_LINUX_MOUNT_SETATTR 442
#define SYS_LINUX_QUOTACTL_FD 443
#define SYS_LINUX_LANDLOCK_CREATE_RULESET 444
#define SYS_LINUX_LANDLOCK_ADD_RULE 445
#define SYS_LINUX_LANDLOCK_RESTRICT_SELF 446
#define SYS_LINUX_MEMFD_SECRET 447
#define SYS_LINUX_PROCESS_MRELEASE 448
#define SYS_LINUX_FUTEX_WAITV 449
#define SYS_LINUX_SET_MEMPOLICY_HOME_NODE 450

// Linux process structure
struct LinuxProcess {
    uint32 pid;                   // Linux process ID
    uint32 ppid;                  // Parent process ID
    uint32 uid;                    // User ID
    uint32 gid;                   // Group ID
    uint32 euid;                  // Effective user ID
    uint32 egid;                  // Effective group ID
    uint32 suid;                  // Saved user ID
    uint32 sgid;                  // Saved group ID
    uint32 fsuid;                 // File system user ID
    uint32 fsgid;                 // File system group ID
    uint32 start_time;            // Process start time
    uint32 utime;                 // User time
    uint32 stime;                 // System time
    uint32 cutime;                // Children user time
    uint32 cstime;                // Children system time
    uint32 priority;              // Process priority
    uint32 nice;                  // Nice value
    uint32 num_threads;           // Number of threads
    uint32 vsize;                 // Virtual memory size
    uint32 rss;                    // Resident set size
    uint32 rsslim;                // RSS limit
    uint32 startcode;             // Start of code segment
    uint32 endcode;               // End of code segment
    uint32 startstack;            // Start of stack
    uint32 kstkesp;               // Kernel stack pointer
    uint32 kstkeip;               // Kernel instruction pointer
    uint32 signal;                // Signal bitmap
    uint32 blocked;                // Blocked signal bitmap
    uint32 sigignore;             // Ignored signal bitmap
    uint32 sigcatch;              // Caught signal bitmap
    uint32 wchan;                 // Wait channel
    uint32 nswap;                 // Number of pages swapped
    uint32 cnswap;                // Cumulative nswap for children
    int32_t exit_signal;           // Signal to be sent at exit
    int32_t processor;               // CPU number last executed on
    uint32 rt_priority;           // Real-time priority
    uint32 policy;               // Scheduling policy
    uint32 delayacct_blkio_ticks; // Delay accounting block io ticks
    uint32 guest_time;            // Guest time of the process
    uint32 cguest_time;           // Guest time of the process's children
    uint32 start_data;            // Start of data segment
    uint32 end_data;               // End of data segment
    uint32 start_brk;             // Start of heap
    uint32 arg_start;             // Start of command line arguments
    uint32 arg_end;               // End of command line arguments
    uint32 env_start;             // Start of environment variables
    uint32 env_end;               // End of environment variables
    int32_t exit_code;             // Exit code of the process
};

// Linux ELF header structure
struct LinuxElfHeader {
    uint8 e_ident[16];            // ELF identification
    uint16_t e_type;                // Object file type
    uint16_t e_machine;             // Machine type
    uint32 e_version;             // Object file version
    uint32 e_entry;               // Entry point address
    uint32 e_phoff;               // Program header offset
    uint32 e_shoff;               // Section header offset
    uint32 e_flags;               // Processor-specific flags
    uint16_t e_ehsize;              // ELF header size
    uint16_t e_phentsize;           // Size of program header entry
    uint16_t e_phnum;               // Number of program header entries
    uint16_t e_shentsize;           // Size of section header entry
    uint16_t e_shnum;               // Number of section header entries
    uint16_t e_shstrndx;            // Section name string table index
};

// Linux system call context
struct LinuxSyscallContext {
    uint32 syscall_number;        // System call number
    uint32 arg1;                  // First argument
    uint32 arg2;                  // Second argument
    uint32 arg3;                  // Third argument
    uint32 arg4;                  // Fourth argument
    uint32 arg5;                  // Fifth argument
    uint32 arg6;                  // Sixth argument
    uint32 return_value;          // Return value
    uint32 error_code;             // Error code (if any)
};

// Linuxulator class
class Linuxulator {
private:
    LinuxProcess linux_processes[MAX_LINUX_PROCESSES];
    uint32 process_count;
    Spinlock linuxulator_lock;      // Lock for thread safety
    
public:
    Linuxulator();
    ~Linuxulator();
    
    // Initialize the Linuxulator
    bool Initialize();
    
    // Load and execute a Linux binary
    bool LoadLinuxBinary(const char* filename, char* const argv[], char* const envp[]);
    
    // Handle Linux system calls
    int HandleSyscall(LinuxSyscallContext* context);
    
    // Process management
    LinuxProcess* CreateLinuxProcess(const char* filename, char* const argv[], char* const envp[]);
    bool DestroyLinuxProcess(uint32 pid);
    LinuxProcess* GetLinuxProcess(uint32 pid);
    uint32 GetLinuxProcessCount();
    
    // ELF loading
    bool LoadElfFile(const char* filename, LinuxElfHeader* elf_header);
    bool VerifyElfHeader(const LinuxElfHeader* elf_header);
    bool MapElfSegments(const LinuxElfHeader* elf_header, const char* filename);
    
    // System call implementations
    int LinuxRead(int fd, void* buf, size_t count);
    int LinuxWrite(int fd, const void* buf, size_t count);
    int LinuxOpen(const char* pathname, int flags, mode_t mode);
    int LinuxClose(int fd);
    int LinuxStat(const char* pathname, struct FileStat* statbuf);
    int LinuxFstat(int fd, struct FileStat* statbuf);
    int LinuxLstat(const char* pathname, struct FileStat* statbuf);
    int LinuxLseek(int fd, off_t offset, int whence);
    int LinuxMmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
    int LinuxMunmap(void* addr, size_t length);
    int LinuxBrk(void* addr);
    int LinuxClone(uint32 flags, void* stack, int* parent_tid, int* child_tid, uint32 tls);
    int LinuxFork();
    int LinuxVFork();
    int LinuxExecve(const char* filename, char* const argv[], char* const envp[]);
    int LinuxExit(int status);
    int LinuxWait4(pid_t pid, int* status, int options, struct rusage* rusage);
    int LinuxKill(pid_t pid, int sig);
    int LinuxUname(struct utsname* buf);
    int LinuxGetPid();
    int LinuxGetPpid();
    int LinuxGetUid();
    int LinuxGetGid();
    int LinuxGetEuid();
    int LinuxGetEgid();
    int LinuxSetUid(uid_t uid);
    int LinuxSetGid(gid_t gid);
    int LinuxChdir(const char* path);
    int LinuxGetCwd(char* buf, size_t size);
    int LinuxAccess(const char* pathname, int mode);
    int LinuxPipe(int pipefd[2]);
    int LinuxDup(int oldfd);
    int LinuxDup2(int oldfd, int newfd);
    int LinuxMkdir(const char* pathname, mode_t mode);
    int LinuxRmdir(const char* pathname);
    int LinuxUnlink(const char* pathname);
    int LinuxRename(const char* oldpath, const char* newpath);
    int LinuxChmod(const char* pathname, mode_t mode);
    int LinuxFchmod(int fd, mode_t mode);
    int LinuxChown(const char* pathname, uid_t owner, gid_t group);
    int LinuxFchown(int fd, uid_t owner, gid_t group);
    int LinuxGetTimeOfDay(struct timeval* tv, struct timezone* tz);
    int LinuxNanosleep(const struct timespec* req, struct timespec* rem);
    int LinuxSelect(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout);
    
    // Signal handling
    int LinuxSignal(int signum, void (*handler)(int));
    int LinuxSigaction(int signum, const struct sigaction* act, struct sigaction* oldact);
    int LinuxSigprocmask(int how, const sigset_t* set, sigset_t* oldset);
    int LinuxSigreturn(void* ucontext);
    int LinuxSigsuspend(const sigset_t* mask);
    int LinuxSigpending(sigset_t* set);
    int LinuxSigtimedwait(const sigset_t* set, siginfo_t* info, const struct timespec* timeout);
    int LinuxSigqueueinfo(pid_t tgid, int sig, siginfo_t* uinfo);
    int LinuxSigaltstack(const stack_t* ss, stack_t* oss);
    
    // Memory management
    int LinuxMprotect(void* addr, size_t len, int prot);
    int LinuxMremap(void* old_address, size_t old_size, size_t new_size, int flags, void* new_address);
    int LinuxMsync(void* addr, size_t length, int flags);
    int LinuxMincore(void* addr, size_t length, unsigned char* vec);
    int LinuxMadvise(void* addr, size_t length, int advice);
    int LinuxMlock(const void* addr, size_t len);
    int LinuxMunlock(const void* addr, size_t len);
    int LinuxMlockall(int flags);
    int LinuxMunlockall(void);
    
    // File system operations
    int LinuxStatfs(const char* path, struct statfs* buf);
    int LinuxFstatfs(int fd, struct statfs* buf);
    int LinuxTruncate(const char* path, off_t length);
    int LinuxFtruncate(int fd, off_t length);
    int LinuxGetdents(unsigned int fd, struct linux_dirent* dirp, unsigned int count);
    int LinuxGetdents64(unsigned int fd, struct linux_dirent64* dirp, unsigned int count);
    int LinuxSymlink(const char* target, const char* linkpath);
    int LinuxReadlink(const char* pathname, char* buf, size_t bufsiz);
    int LinuxLink(const char* oldpath, const char* newpath);
    int LinuxMount(const char* source, const char* target, const char* filesystemtype, 
                   unsigned long mountflags, const void* data);
    int LinuxUmount(const char* target);
    int LinuxUmount2(const char* target, int flags);
    
    // Process scheduling
    int LinuxSchedYield();
    int LinuxSchedSetparam(pid_t pid, const struct sched_param* param);
    int LinuxSchedGetparam(pid_t pid, struct sched_param* param);
    int LinuxSchedSetscheduler(pid_t pid, int policy, const struct sched_param* param);
    int LinuxSchedGetscheduler(pid_t pid);
    int LinuxSchedGetPriorityMax(int policy);
    int LinuxSchedGetPriorityMin(int policy);
    int LinuxSchedRrGetInterval(pid_t pid, struct timespec* tp);
    int LinuxSchedSetaffinity(pid_t pid, size_t cpusetsize, const cpu_set_t* mask);
    int LinuxSchedGetaffinity(pid_t pid, size_t cpusetsize, cpu_set_t* mask);
    
    // IPC operations
    int LinuxSemget(key_t key, int nsems, int semflg);
    int LinuxSemop(int semid, struct sembuf* sops, size_t nsops);
    int LinuxSemctl(int semid, int semnum, int cmd, ...);
    int LinuxMsgget(key_t key, int msgflg);
    int LinuxMsgsnd(int msqid, const void* msgp, size_t msgsz, int msgflg);
    int LinuxMsgrcv(int msqid, void* msgp, size_t msgsz, long msgtyp, int msgflg);
    int LinuxMsgctl(int msqid, int cmd, struct msqid_ds* buf);
    int LinuxShmget(key_t key, size_t size, int shmflg);
    int LinuxShmat(int shmid, const void* shmaddr, int shmflg);
    int LinuxShmdt(const void* shmaddr);
    int LinuxShmctl(int shmid, int cmd, struct shmid_ds* buf);
    
    // Socket operations
    int LinuxSocket(int domain, int type, int protocol);
    int LinuxConnect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    int LinuxAccept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    int LinuxBind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    int LinuxListen(int sockfd, int backlog);
    int LinuxSend(int sockfd, const void* buf, size_t len, int flags);
    int LinuxRecv(int sockfd, void* buf, size_t len, int flags);
    int LinuxSendto(int sockfd, const void* buf, size_t len, int flags,
                    const struct sockaddr* dest_addr, socklen_t addrlen);
    int LinuxRecvfrom(int sockfd, void* buf, size_t len, int flags,
                      struct sockaddr* src_addr, socklen_t* addrlen);
    int LinuxSendmsg(int sockfd, const struct msghdr* msg, int flags);
    int LinuxRecvmsg(int sockfd, struct msghdr* msg, int flags);
    int LinuxGetsockopt(int sockfd, int level, int optname, void* optval, socklen_t* optlen);
    int LinuxSetsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen);
    int LinuxGetsockname(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    int LinuxGetpeername(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    int LinuxShutdown(int sockfd, int how);
    int LinuxSocketpair(int domain, int type, int protocol, int sv[2]);
    
    // Process and thread management
    int LinuxClone3(struct clone_args* cl_args, size_t size);
    int LinuxTkill(int tid, int sig);
    int LinuxTgkill(int tgid, int tid, int sig);
    int LinuxGettid();
    int LinuxSetns(int fd, int nstype);
    int LinuxUnshare(int flags);
    int LinuxSetpgid(pid_t pid, pid_t pgid);
    int LinuxGetpgid(pid_t pid);
    int LinuxGetsid(pid_t pid);
    int LinuxSetsid();
    int LinuxSetuid(uid_t uid);
    int LinuxSetgid(gid_t gid);
    int LinuxSetreuid(uid_t ruid, uid_t euid);
    int LinuxSetregid(gid_t rgid, gid_t egid);
    int LinuxSetresuid(uid_t ruid, uid_t euid, uid_t suid);
    int LinuxSetresgid(gid_t rgid, gid_t egid, gid_t sgid);
    int LinuxSetfsuid(uid_t fsuid);
    int LinuxSetfsgid(uid_t fsgid);
    int LinuxGetgroups(int size, gid_t list[]);
    int LinuxSetgroups(size_t size, const gid_t* list);
    int LinuxGetrlimit(int resource, struct rlimit* rlim);
    int LinuxSetrlimit(int resource, const struct rlimit* rlim);
    int LinuxGetrusage(int who, struct rusage* usage);
    int LinuxPrlimit(pid_t pid, int resource, const struct rlimit* new_limit, struct rlimit* old_limit);
    int LinuxPrctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);
    int LinuxArchPrctl(int code, unsigned long addr);
    int LinuxPersonality(unsigned long persona);
    int LinuxCapget(cap_user_header_t hdrp, cap_user_data_t datap);
    int LinuxCapset(cap_user_header_t hdrp, const cap_user_data_t datap);
    int LinuxIopl(int level);
    int LinuxIoperm(unsigned long from, unsigned long num, int turn_on);
    
    // File descriptor operations
    int LinuxFcntl(int fd, int cmd, ...);
    int LinuxFlock(int fd, int operation);
    int LinuxFsync(int fd);
    int LinuxFdatasync(int fd);
    // Note: LinuxFtruncate, LinuxFchmod, and LinuxFchown are already declared above in the basic file operations
    
    int LinuxFchdir(int fd);
    int LinuxFgetxattr(int fd, const char* name, void* value, size_t size);
    int LinuxFsetxattr(int fd, const char* name, const void* value, size_t size, int flags);
    int LinuxFlistxattr(int fd, char* list, size_t size);
    int LinuxFremovexattr(int fd, const char* name);
    int LinuxFaccessat(int dirfd, const char* pathname, int mode, int flags);
    int LinuxFchmodat(int dirfd, const char* pathname, mode_t mode, int flags);
    int LinuxFchownat(int dirfd, const char* pathname, uid_t owner, gid_t group, int flags);
    int LinuxFstatat(int dirfd, const char* pathname, struct stat* statbuf, int flags);
    int LinuxFutimesat(int dirfd, const char* pathname, const struct timeval times[2]);
    int LinuxUtimensat(int dirfd, const char* pathname, const struct timespec times[2], int flags);
    int LinuxOpenat(int dirfd, const char* pathname, int flags, mode_t mode);
    int LinuxMkdirat(int dirfd, const char* pathname, mode_t mode);
    int LinuxMknodat(int dirfd, const char* pathname, mode_t mode, dev_t dev);
    int LinuxUnlinkat(int dirfd, const char* pathname, int flags);
    int LinuxRenameat(int olddirfd, const char* oldpath, int newdirfd, const char* newpath);
    int LinuxLinkat(int olddirfd, const char* oldpath, int newdirfd, const char* newpath, int flags);
    int LinuxSymlinkat(const char* target, int newdirfd, const char* linkpath);
    int LinuxReadlinkat(int dirfd, const char* pathname, char* buf, size_t bufsiz);
    
    // Advanced I/O operations
    int LinuxReadahead(int fd, off64_t offset, size_t count);
    int LinuxSplice(int fd_in, loff_t* off_in, int fd_out, loff_t* off_out, size_t len, unsigned int flags);
    int LinuxVmsplice(int fd, const struct iovec* iov, unsigned long nr_segs, unsigned int flags);
    int LinuxTee(int fd_in, int fd_out, size_t len, unsigned int flags);
    int LinuxSyncFileRange(int fd, off64_t offset, off64_t nbytes, unsigned int flags);
    int LinuxIoSetup(unsigned nr_events, aio_context_t* ctx);
    int LinuxIoDestroy(aio_context_t ctx);
    int LinuxIoSubmit(aio_context_t ctx, long nr, struct iocb** iocbpp);
    int LinuxIoCancel(aio_context_t ctx, struct iocb* iocb, struct io_event* result);
    int LinuxIoGetEvents(aio_context_t ctx, long min_nr, long nr, struct io_event* events, struct timespec* timeout);
    int LinuxIoPgetevents(aio_context_t ctx, long min_nr, long nr, struct io_event* events, struct timespec* timeout, const struct __aio_sigset* sigmask);
    
    // Asynchronous I/O operations
    int LinuxIoUringSetup(unsigned entries, struct io_uring_params* p);
    int LinuxIoUringEnter(unsigned int fd, unsigned int to_submit, unsigned int min_complete, unsigned int flags, sigset_t* sig);
    int LinuxIoUringRegister(unsigned int fd, unsigned int opcode, void* arg, unsigned int nr_args);
    
    // Process control
    int LinuxPtrace(long request, pid_t pid, void* addr, void* data);
    int LinuxWaitid(idtype_t idtype, id_t id, siginfo_t* infop, int options);
    // Note: LinuxWait4, LinuxKill, and LinuxTgkill are already declared above
    int LinuxTkil(int tid, int sig);
    
    // Memory mapping operations
    int LinuxMmap2(unsigned long addr, unsigned long len, unsigned long prot, 
                   unsigned long flags, unsigned long fd, unsigned long pgoff);
    int LinuxRemapFilePages(void* addr, size_t size, int prot, size_t pgoff, int flags);
    int LinuxMbind(void* addr, unsigned long len, int mode, const unsigned long* nodemask,
                   unsigned long maxnode, unsigned flags);
    int LinuxSetMempolicy(int mode, const unsigned long* nodemask, unsigned long maxnode);
    int LinuxGetMempolicy(int* mode, unsigned long* nodemask, unsigned long maxnode, void* addr, unsigned long flags);
    int LinuxMigratePages(int pid, unsigned long maxnode, const unsigned long* old_nodes, const unsigned long* new_nodes);
    int LinuxMovePages(int pid, unsigned long count, void** pages, const int* nodes, int* status, int flags);
    
    // Key management
    int LinuxAddKey(const char* type, const char* description, const void* payload, size_t plen, int ringid);
    int LinuxRequestKey(const char* type, const char* description, const char* callout_info, int destringid);
    int LinuxKeyctl(int cmd, ...);
    
    // Security operations
    int LinuxSeccomp(unsigned int operation, unsigned int flags, void* args);
    int LinuxLandlockCreateRuleset(const struct landlock_ruleset_attr* attr, size_t size, __u32 flags);
    int LinuxLandlockAddRule(int ruleset_fd, enum landlock_rule_type rule_type, const void* rule_attr, __u32 flags);
    int LinuxLandlockRestrictSelf(int ruleset_fd, __u32 flags);
    
    // Performance monitoring
    int LinuxPerfEventOpen(struct perf_event_attr* attr, pid_t pid, int cpu, int group_fd, unsigned long flags);
    
    // Miscellaneous
    int LinuxSysinfo(struct sysinfo* info);
    int LinuxSyslog(int type, char* bufp, int len);
    int LinuxAdjtimex(struct timex* buf);
    int LinuxSettimeofday(const struct timeval* tv, const struct timezone* tz);
    int LinuxClockSettime(clockid_t clk_id, const struct timespec* tp);
    int LinuxClockGettime(clockid_t clk_id, struct timespec* tp);
    int LinuxClockGetres(clockid_t clk_id, struct timespec* res);
    int LinuxClockNanosleep(clockid_t clock_id, int flags, const struct timespec* request, struct timespec* remain);
    // int LinuxNanosleep(const struct timespec* req, struct timespec* rem);
    int LinuxGetrandom(void* buf, size_t buflen, unsigned int flags);
    int LinuxMemfdCreate(const char* name, unsigned int flags);
    int LinuxKexecLoad(unsigned long entry, unsigned long nr_segments, struct kexec_segment* segments, unsigned long flags);
    int LinuxKexecFileLoad(int kernel_fd, int initrd_fd, unsigned long cmdline_len, const char* cmdline, unsigned long flags);
    int LinuxReboot(int magic1, int magic2, unsigned int cmd, void* arg);
    int LinuxSethostname(const char* name, size_t len);
    int LinuxSetdomainname(const char* name, size_t len);
    // int LinuxUname(struct utsname* buf);
    int LinuxUstat(dev_t dev, struct ustat* ubuf);
    int LinuxUtime(const char* filename, const struct utimbuf* times);
    int LinuxUtimes(const char* filename, const struct timeval times[2]);
    int LinuxFutimes(int fd, const struct timeval times[2]);
    int LinuxFutimens(int fd, const struct timespec times[2]);
    // int LinuxUtimensat(int dirfd, const char* pathname, const struct timespec times[2], int flags);
    int LinuxAcct(const char* filename);
    int LinuxSwapon(const char* path, int swapflags);
    int LinuxSwapoff(const char* path);
    int LinuxQuotactl(int cmd, const char* special, int id, caddr_t addr);
    int LinuxNfsservctl(int cmd, struct nfsctl_arg* arg, union nfsctl_res* res);
    int LinuxBpf(int cmd, union bpf_attr* attr, unsigned int size);
    int LinuxFinitModule(int fd, const char* param_values, int flags);
    int LinuxInitModule(void* module_image, unsigned long len, const char* param_values);
    int LinuxDeleteModule(const char* name, unsigned int flags);
    int LinuxCreateModule(const char* name, size_t size);
    int LinuxQueryModule(const char* name, int which, void* buf, size_t bufsize, size_t* ret);
    int LinuxGetKernelSyms(struct kernel_sym* table);
    int LinuxLookupDcookie(uint64_t cookie64, char* buf, size_t len);
    int LinuxKcmp(pid_t pid1, pid_t pid2, int type, unsigned long idx1, unsigned long idx2);
    int LinuxProcessVmReadv(pid_t pid, const struct iovec* liov, unsigned long liovcnt,
                           const struct iovec* riov, unsigned long riovcnt, unsigned long flags);
    int LinuxProcessVmWritev(pid_t pid, const struct iovec* liov, unsigned long liovcnt,
                            const struct iovec* riov, unsigned long riovcnt, unsigned long flags);
    int LinuxPkeyMprotect(void* addr, size_t len, int prot, int pkey);
    int LinuxPkeyAlloc(unsigned long flags, unsigned long access_rights);
    int LinuxPkeyFree(int pkey);
    int LinuxStatx(int dirfd, const char* pathname, int flags, unsigned int mask, struct statx* statxbuf);
    // int LinuxIoPgetevents(aio_context_t ctx_id, long min_nr, long nr, struct io_event* events,
    //                      const struct timespec* timeout, const struct __aio_sigset* sevp);
    // int LinuxRseq(struct rseq* rseq, uint32 rseq_len, int flags, uint32 sig);
    // int LinuxPidfdSendSignal(int pidfd, int sig, siginfo_t* info, unsigned int flags);
    // int LinuxOpenTree(int dfd, const char* pathname, unsigned int flags);
    // int LinuxMoveMount(int from_dfd, const char* from_pathname, int to_dfd, const char* to_pathname, unsigned int flags);
    // int LinuxFsopen(const char* fs_name, unsigned int flags);
    // int LinuxFsconfig(int fs_fd, unsigned int cmd, const char* key, const void* value, int aux);
    // int LinuxFsmount(int fs_fd, unsigned int flags, unsigned int mount_attrs);
    // int LinuxFspick(int dfd, const char* path, unsigned int flags);
    // int LinuxPidfdOpen(pid_t pid, unsigned int flags);
    // int LinuxClone3(struct clone_args* cl_args, size_t size);
    // int LinuxCloseRange(unsigned int fd, unsigned int max_fd, unsigned int flags);
    // int LinuxOpenat2(int dirfd, const char* pathname, struct open_how* how, size_t size);
    // int LinuxPidfdGetfd(int pidfd, int targetfd, unsigned int flags);
    // int LinuxFaccessat2(int dirfd, const char* pathname, int mode, int flags);
    // int LinuxProcessMadvise(int pidfd, const struct iovec* iov, size_t iovcnt, int advice, unsigned long flags);
    // int LinuxEpollPwait2(int epfd, struct epoll_event* events, int maxevents, const struct timespec* timeout,
    //                     const sigset_t* sigmask, size_t sigsetsize);
    // int LinuxMountSetattr(int dfd, const char* path, unsigned int flags, struct mount_attr* uattr, size_t usize);
    // int LinuxQuotactlFd(unsigned int fd, unsigned int cmd, int id, void* addr);
    // int LinuxLandlockCreateRuleset(const struct landlock_ruleset_attr* attr, size_t size, __u32 flags);
    // int LinuxLandlockAddRule(int ruleset_fd, enum landlock_rule_type rule_type, const void* rule_attr, __u32 flags);
    // int LinuxLandlockRestrictSelf(int ruleset_fd, __u32 flags);
    int LinuxMemfdSecret(unsigned int flags);
    int LinuxProcessMrelease(int pidfd, unsigned int flags);
    int LinuxFutexWaitv(struct futex_waitv* waiters, unsigned int nr_futexes, unsigned int flags,
                       struct timespec* timeout, clockid_t clockid);
    int LinuxSetMempolicyHomeNode(unsigned long start, unsigned long len, unsigned long home_node, unsigned long flags);
    
    // Utility functions
    const char* GetSyscallName(uint32 syscall_number);
    bool IsSyscallImplemented(uint32 syscall_number);
    void PrintLinuxProcessInfo(LinuxProcess* process);
    void PrintLinuxProcesses();
    
private:
    // Internal helper functions
    int DispatchSyscall(LinuxSyscallContext* context);
    LinuxProcess* FindFreeLinuxProcessSlot();
    bool ValidateLinuxProcess(LinuxProcess* process);
    void InitializeLinuxProcess(LinuxProcess* process);
    void CleanupLinuxProcess(LinuxProcess* process);
    int TranslateErrno(int linux_errno);
    int TranslateSignal(int linux_signal);
    int TranslateOpenFlags(int linux_flags);
    int TranslateProtFlags(int linux_prot);
    int TranslateMmapFlags(int linux_flags);
    int TranslateAccessMode(int linux_mode);
    int TranslateWhence(int linux_whence);
    int TranslateFcntlCmd(int linux_cmd);
    int TranslateCloneFlags(int linux_flags);
    int TranslateResource(int linux_resource);
    int TranslateWho(int linux_who);
    int TranslateOption(int linux_option);
    int TranslatePolicy(int linux_policy);
    int TranslateAdvice(int linux_advice);
    int TranslateMode(int linux_mode);
    int TranslateCmd(int linux_cmd);
    int TranslateRuleType(enum landlock_rule_type linux_rule_type);
    int TranslateKeyctlCmd(int linux_cmd);
    int TranslateBpfCmd(int linux_cmd);
    int TranslateSeccompOp(unsigned int linux_operation);
    int TranslateMountFlags(unsigned long linux_flags);
    int TranslateUmountFlags(int linux_flags);
    int TranslateSwapFlags(int linux_flags);
    int TranslateQuotaCmd(int linux_cmd);
    int TranslateIoprioClass(int linux_class);
    int TranslateIoprioData(int linux_data);
    int TranslateMqAttr(int linux_attr);
    int TranslateTimerFlags(int linux_flags);
    int TranslateClockId(clockid_t linux_clk_id);
    int TranslateRandomFlags(unsigned int linux_flags);
    int TranslateMemfdFlags(unsigned int linux_flags);
    int TranslateKexecFlags(unsigned long linux_flags);
    int TranslateRebootCmd(unsigned int linux_cmd);
    int TranslateHostnameFlags(int linux_flags);
    int TranslateDomainnameFlags(int linux_flags);
    int TranslateUtimeFlags(int linux_flags);
    int TranslateFutimensFlags(int linux_flags);
    int TranslateAcctFlags(int linux_flags);
    int TranslateSwaponFlags(int linux_flags);
    int TranslateSwapoffFlags(int linux_flags);
    int TranslateQuotactlFlags(int linux_cmd);
    int TranslateNfsservctlCmd(int linux_cmd);
    int TranslateBpfAttr(int linux_attr);
    int TranslateFinitModuleFlags(int linux_flags);
    int TranslateInitModuleFlags(int linux_flags);
    int TranslateDeleteModuleFlags(int linux_flags);
    int TranslateCreateModuleFlags(int linux_flags);
    int TranslateQueryModuleWhich(int linux_which);
    int TranslateLookupDcookieFlags(uint64_t linux_cookie64);
    int TranslateKcmpType(int linux_type);
    int TranslateProcessVmFlags(unsigned long linux_flags);
    int TranslatePkeyProtectFlags(int linux_flags);
    int TranslatePkeyAllocFlags(unsigned long linux_flags);
    int TranslateStatxMask(unsigned int linux_mask);
    int TranslateIoPgeteventsFlags(const struct __aio_sigset* linux_sevp);
    int TranslateRseqFlags(int linux_flags);
    int TranslatePidfdSendSignalFlags(unsigned int linux_flags);
    int TranslateOpenTreeFlags(unsigned int linux_flags);
    int TranslateMoveMountFlags(unsigned int linux_flags);
    int TranslateFsopenFlags(unsigned int linux_flags);
    int TranslateFsconfigCmd(unsigned int linux_cmd);
    int TranslateFsmountFlags(unsigned int linux_flags);
    int TranslateMountAttrs(unsigned int linux_mount_attrs);
    int TranslateFspickFlags(unsigned int linux_flags);
    int TranslatePidfdOpenFlags(unsigned int linux_flags);
    int TranslateClone3Args(struct clone_args* linux_cl_args);
    int TranslateCloseRangeFlags(unsigned int linux_flags);
    int TranslateOpenat2How(struct open_how* linux_how);
    int TranslatePidfdGetfdFlags(unsigned int linux_flags);
    int TranslateFaccessat2Flags(int linux_flags);
    int TranslateProcessMadviseFlags(unsigned long linux_flags);
    int TranslateEpollPwait2Flags(unsigned int linux_flags);
    int TranslateMountSetattrFlags(unsigned int linux_flags);
    int TranslateQuotactlFdCmd(unsigned int linux_cmd);
    int TranslateLandlockRulesetAttr(const struct landlock_ruleset_attr* linux_attr);
    int TranslateLandlockRuleType(enum landlock_rule_type linux_rule_type);
    int TranslateLandlockRestrictSelfFlags(__u32 linux_flags);
    int TranslateMemfdSecretFlags(unsigned int linux_flags);
    int TranslateProcessMreleaseFlags(unsigned int linux_flags);
    int TranslateFutexWaitvFlags(unsigned int linux_flags);
    int TranslateSetMempolicyHomeNodeFlags(unsigned long linux_flags);
};

// Global Linuxulator instance
extern Linuxulator* g_linuxulator;

// Initialize the Linuxulator
bool InitializeLinuxulator();

// Handle Linux system calls from the kernel
extern "C" int HandleLinuxSyscall(uint32 syscall_number, uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4, uint32 arg5, uint32 arg6);

// Load and run a Linux executable
bool RunLinuxExecutable(const char* filename, char* const argv[], char* const envp[]);

// Load a Linux executable and create a process - for ABI multiplexer
ProcessControlBlock* LoadLinuxExecutable(const char* filename, char* const argv[], char* const envp[]);

#endif