#ifndef _Kernel_DosSyscalls_h_
#define _Kernel_DosSyscalls_h_

#include "Common.h"
#include "Defs.h"
#include "Vfs.h"
#include "ProcessControlBlock.h"
#include "Syscalls.h"

// DOS system call numbers
#define DOS_SYSCALL_INT21 0x21
#define DOS_SYSCALL_INT20 0x20
#define DOS_SYSCALL_INT25 0x25
#define DOS_SYSCALL_INT26 0x26
#define DOS_SYSCALL_INT27 0x27
#define DOS_SYSCALL_INT28 0x28
#define DOS_SYSCALL_INT29 0x29
#define DOS_SYSCALL_INT2A 0x2A
#define DOS_SYSCALL_INT2B 0x2B
#define DOS_SYSCALL_INT2C 0x2C
#define DOS_SYSCALL_INT2D 0x2D
#define DOS_SYSCALL_INT2E 0x2E
#define DOS_SYSCALL_INT2F 0x2F

// DOS INT 21h function numbers
#define DOS_INT21_TERMINATE_PROGRAM 0x00
#define DOS_INT21_CHARACTER_INPUT 0x01
#define DOS_INT21_CHARACTER_OUTPUT 0x02
#define DOS_INT21_AUXILIARY_INPUT 0x03
#define DOS_INT21_AUXILIARY_OUTPUT 0x04
#define DOS_INT21_WRITE_STRING 0x09
#define DOS_INT21_BUFFERED_INPUT 0x0A
#define DOS_INT21_CHECK_STDIN_STATUS 0x0B
#define DOS_INT21_FLUSH_BUFFER_AND_READ_STDIN 0x0C
#define DOS_INT21_RESET_DRIVE 0x0D
#define DOS_INT21_SET_INTERRUPT_VECTOR 0x25
#define DOS_INT21_CREATE_PROCESS 0x26
#define DOS_INT21_TERMINATE_AND_STAY_RESIDENT 0x31
#define DOS_INT21_GET_INTERRUPT_VECTOR 0x35
#define DOS_INT21_SET_DTA 0x1A
#define DOS_INT21_GET_DTA 0x2F
#define DOS_INT21_SET_DEFAULT_DRIVE 0x0E
#define DOS_INT21_GET_DEFAULT_DRIVE 0x19
#define DOS_INT21_SET_DISK_TRANSFER_AREA 0x1A
#define DOS_INT21_GET_VERSION 0x30
#define DOS_INT21_TERMINATE_PROCESS 0x4C
#define DOS_INT21_GET_CURRENT_DATE 0x2A
#define DOS_INT21_GET_CURRENT_TIME 0x2C
#define DOS_INT21_SET_CURRENT_DATE 0x2B
#define DOS_INT21_SET_CURRENT_TIME 0x2D
#define DOS_INT21_ALLOCATE_MEMORY 0x48
#define DOS_INT21_RELEASE_MEMORY 0x49
#define DOS_INT21_RESIZE_MEMORY 0x4A
#define DOS_INT21_EXEC 0x4B
#define DOS_INT21_EXIT 0x4C
#define DOS_INT21_WAIT 0x4D
#define DOS_INT21_FIND_FIRST 0x4E
#define DOS_INT21_FIND_NEXT 0x4F
#define DOS_INT21_SET_VERIFY_FLAG 0x2E
#define DOS_INT21_GET_VERIFY_FLAG 0x54
#define DOS_INT21_CREATE_PSP 0x55
#define DOS_INT21_RENAME_FILE 0x56
#define DOS_INT21_GET_LOGIN 0x62
#define DOS_INT21_GET_TRUE_VERSION 0x63
#define DOS_INT21_EXT_COUNTRY_INFO 0x65
#define DOS_INT21_GET_EXTENDED_COUNTRY_INFO 0x66
#define DOS_INT21_GET_TRUE_VERSION_EXTENDED 0x67
#define DOS_INT21_SET_WAIT_FOR_EXTERNAL_EVENT_FLAG 0x68
#define DOS_INT21_OPEN_FILE 0x3D
#define DOS_INT21_CLOSE_FILE 0x3E
#define DOS_INT21_READ_FILE 0x3F
#define DOS_INT21_WRITE_FILE 0x40
#define DOS_INT21_DELETE_FILE 0x41
#define DOS_INT21_SET_FILE_POINTER 0x42
#define DOS_INT21_GET_FILE_SIZE 0x43
#define DOS_INT21_SET_FILE_ATTRIBUTES 0x43
#define DOS_INT21_GET_FILE_ATTRIBUTES 0x44
#define DOS_INT21_CREATE_FILE 0x45
#define DOS_INT21_RENAME_FILE_EXTENDED 0x46
#define DOS_INT21_GET_CURRENT_DIRECTORY 0x47
#define DOS_INT21_SET_CURRENT_DIRECTORY 0x48
#define DOS_INT21_CREATE_DIRECTORY 0x49
#define DOS_INT21_REMOVE_DIRECTORY 0x4A
#define DOS_INT21_SET_INTERRUPT_VECTOR_EXTENDED 0x25
#define DOS_INT21_GET_INTERRUPT_VECTOR_EXTENDED 0x35
#define DOS_INT21_CREATE_PROCESS_EXTENDED 0x26
#define DOS_INT21_TERMINATE_AND_STAY_RESIDENT_EXTENDED 0x31
#define DOS_INT21_GET_VERSION_EXTENDED 0x30
#define DOS_INT21_TERMINATE_PROCESS_EXTENDED 0x4C
#define DOS_INT21_GET_CURRENT_DATE_EXTENDED 0x2A
#define DOS_INT21_GET_CURRENT_TIME_EXTENDED 0x2C
#define DOS_INT21_SET_CURRENT_DATE_EXTENDED 0x2B
#define DOS_INT21_SET_CURRENT_TIME_EXTENDED 0x2D
#define DOS_INT21_ALLOCATE_MEMORY_EXTENDED 0x48
#define DOS_INT21_RELEASE_MEMORY_EXTENDED 0x49
#define DOS_INT21_RESIZE_MEMORY_EXTENDED 0x4A
#define DOS_INT21_EXEC_EXTENDED 0x4B
#define DOS_INT21_EXIT_EXTENDED 0x4C
#define DOS_INT21_WAIT_EXTENDED 0x4D
#define DOS_INT21_FIND_FIRST_EXTENDED 0x4E
#define DOS_INT21_FIND_NEXT_EXTENDED 0x4F
#define DOS_INT21_SET_VERIFY_FLAG_EXTENDED 0x2E
#define DOS_INT21_GET_VERIFY_FLAG_EXTENDED 0x54
#define DOS_INT21_CREATE_PSP_EXTENDED 0x55
#define DOS_INT21_RENAME_FILE_EXTENDED2 0x56
#define DOS_INT21_GET_LOGIN_EXTENDED 0x62
#define DOS_INT21_GET_TRUE_VERSION_EXTENDED2 0x63
#define DOS_INT21_EXT_COUNTRY_INFO_EXTENDED 0x65
#define DOS_INT21_GET_EXTENDED_COUNTRY_INFO_EXTENDED 0x66
#define DOS_INT21_GET_TRUE_VERSION_EXTENDED3 0x67
#define DOS_INT21_SET_WAIT_FOR_EXTERNAL_EVENT_FLAG_EXTENDED 0x68
#define DOS_INT21_OPEN_FILE_EXTENDED 0x3D
#define DOS_INT21_CLOSE_FILE_EXTENDED 0x3E
#define DOS_INT21_READ_FILE_EXTENDED 0x3F
#define DOS_INT21_WRITE_FILE_EXTENDED 0x40
#define DOS_INT21_DELETE_FILE_EXTENDED 0x41
#define DOS_INT21_SET_FILE_POINTER_EXTENDED 0x42
#define DOS_INT21_GET_FILE_SIZE_EXTENDED 0x43
#define DOS_INT21_SET_FILE_ATTRIBUTES_EXTENDED 0x43
#define DOS_INT21_GET_FILE_ATTRIBUTES_EXTENDED 0x44
#define DOS_INT21_CREATE_FILE_EXTENDED 0x45
#define DOS_INT21_RENAME_FILE_EXTENDED3 0x46
#define DOS_INT21_GET_CURRENT_DIRECTORY_EXTENDED 0x47
#define DOS_INT21_SET_CURRENT_DIRECTORY_EXTENDED 0x48
#define DOS_INT21_CREATE_DIRECTORY_EXTENDED 0x49
#define DOS_INT21_REMOVE_DIRECTORY_EXTENDED 0x4A

// DOS file attributes
#define DOS_ATTR_READ_ONLY 0x01
#define DOS_ATTR_HIDDEN 0x02
#define DOS_ATTR_SYSTEM 0x04
#define DOS_ATTR_VOLUME_ID 0x08
#define DOS_ATTR_DIRECTORY 0x10
#define DOS_ATTR_ARCHIVE 0x20
#define DOS_ATTR_LONG_NAME (DOS_ATTR_READ_ONLY | DOS_ATTR_HIDDEN | DOS_ATTR_SYSTEM | DOS_ATTR_VOLUME_ID)

// DOS file access modes
#define DOS_FILE_ACCESS_READ 0x00
#define DOS_FILE_ACCESS_WRITE 0x01
#define DOS_FILE_ACCESS_READ_WRITE 0x02
#define DOS_FILE_ACCESS_EXECUTE 0x03

// DOS file sharing modes
#define DOS_FILE_SHARE_COMPATIBLE 0x00
#define DOS_FILE_SHARE_DENY_ALL 0x10
#define DOS_FILE_SHARE_DENY_WRITE 0x20
#define DOS_FILE_SHARE_DENY_READ 0x30
#define DOS_FILE_SHARE_DENY_NONE 0x40

// DOS error codes
#define DOS_ERROR_NONE 0
#define DOS_ERROR_FUNCTION_NUMBER_INVALID 1
#define DOS_ERROR_FILE_NOT_FOUND 2
#define DOS_ERROR_PATH_NOT_FOUND 3
#define DOS_ERROR_TOO_MANY_OPEN_FILES 4
#define DOS_ERROR_ACCESS_DENIED 5
#define DOS_ERROR_INVALID_HANDLE 6
#define DOS_ERROR_MEMORY_CONTROL_BLOCKS_DESTROYED 7
#define DOS_ERROR_INSUFFICIENT_MEMORY 8
#define DOS_ERROR_INVALID_MEMORY_BLOCK_ADDRESS 9
#define DOS_ERROR_INVALID_ENVIRONMENT 10
#define DOS_ERROR_INVALID_FORMAT 11
#define DOS_ERROR_INVALID_ACCESS_CODE 12
#define DOS_ERROR_INVALID_DATA 13
#define DOS_ERROR_INVALID_DRIVE 15
#define DOS_ERROR_CURRENT_DIRECTORY_ATTEMPT_TO_REMOVE 16
#define DOS_ERROR_NOT_SAME_DEVICE 17
#define DOS_ERROR_NO_MORE_FILES 18
#define DOS_ERROR_WRITE_PROTECTED 19
#define DOS_ERROR_BAD_UNIT 20
#define DOS_ERROR_NOT_READY 21
#define DOS_ERROR_BAD_COMMAND 22
#define DOS_ERROR_CRC_ERROR 23
#define DOS_ERROR_BAD_LENGTH 24
#define DOS_ERROR_SEEK_ERROR 25
#define DOS_ERROR_NOT_DOS_DISK 26
#define DOS_ERROR_SECTOR_NOT_FOUND 27
#define DOS_ERROR_OUT_OF_PAPER 28
#define DOS_ERROR_WRITE_FAULT 29
#define DOS_ERROR_READ_FAULT 30
#define DOS_ERROR_GENERAL_FAILURE 31
#define DOS_ERROR_SHARING_VIOLATION 32
#define DOS_ERROR_LOCK_VIOLATION 33
#define DOS_ERROR_WRONG_DISK 34
#define DOS_ERROR_FCB_UNAVAILABLE 35
#define DOS_ERROR_SHARING_BUFFER_OVERFLOW 36

// DOS PSP (Program Segment Prefix) structure
struct DosPsp {
    uint16_t int_20h_instruction;     // INT 20h instruction
    uint16_t end_of_memory;           // Segment address of first byte beyond memory allocated to program
    uint8 reserved1[2];             // Reserved
    uint8 dos_dispatch[5];          // Far call to DOS dispatcher
    uint8 reserved2[10];           // Reserved
    uint8 int_22h_vector[4];        // Terminate program return address (stored as segment:offset)
    uint8 int_23h_vector[4];        // Ctrl+C handler address (stored as segment:offset)
    uint8 int_24h_vector[4];        // Critical error handler address (stored as segment:offset)
    uint16_t parent_psp_segment;      // Segment address of parent PSP
    uint8 file_handles[20];         // File handle table
    uint16_t environment_segment;     // Segment address of environment block
    uint8 reserved3[2];             // Reserved
    uint8 int_21h_return[6];        // Far return address for INT 21h calls
    uint8 reserved4[6];             // Reserved
    uint8 file_handles_extended[20]; // Extended file handle table
    uint8 reserved5[36];            // Reserved
    uint8 fcb1[16];                 // Unopened FCB 1
    uint8 fcb2[20];                 // Unopened FCB 2
    uint8 command_tail[128];        // Command tail buffer
};

// DOS file control block (FCB) structure
struct DosFcb {
    uint8 drive_number;             // Drive number (0=default, 1=A:, etc.)
    char filename[8];                 // Filename (padded with spaces)
    char extension[3];                // Extension (padded with spaces)
    uint16_t current_block;           // Current block number
    uint16_t record_size;            // Record size
    uint32 file_size;               // File size in bytes
    uint16_t date;                    // Date of last write
    uint16_t time;                    // Time of last write
    uint8 reserved[8];              // Reserved
    uint8 current_record;           // Current record within current block
    uint32 random_record;           // Random record number
};

// DOS directory entry structure
struct DosDirEntry {
    char name[11];                    // 8.3 filename (8 chars + 3 extension)
    uint8 attributes;               // File attributes
    uint8 reserved[10];             // Reserved
    uint16_t time;                    // Time of last write
    uint16_t date;                    // Date of last write
    uint16_t first_cluster;           // First cluster of file
    uint32 file_size;               // File size in bytes
};

// DOS disk transfer area (DTA) structure
struct DosDta {
    uint8 drive;                    // Drive letter (0=A:, 1=B:, etc.)
    char pattern[11];                 // Search pattern
    uint8 attributes;               // Search attributes
    uint16_t entry_count;             // Number of directory entries found
    uint16_t cluster;                 // Starting cluster of directory
    DosDirEntry entries[16];          // Directory entries
};

// DOS memory control block (MCB) structure
struct DosMcb {
    uint8 signature;                // MCB signature ('M' or 'Z')
    uint16_t owner_psp;              // PSP segment of owner process
    uint16_t size;                    // Size of block in paragraphs
    uint8 reserved[3];              // Reserved
    char program_name[8];             // Program name (if MCB is for a program)
};

// DOS system call context
struct DosSyscallContext {
    uint8 interrupt_number;         // Interrupt number (0x21, etc.)
    uint8 function_number;          // Function number within interrupt
    uint32 ax;                      // AX register value
    uint32 bx;                      // BX register value
    uint32 cx;                      // CX register value
    uint32 dx;                      // DX register value
    uint32 si;                      // SI register value
    uint32 di;                      // DI register value
    uint32 bp;                      // BP register value
    uint32 sp;                      // SP register value
    uint32 ds;                      // DS register value
    uint32 es;                      // ES register value
    uint32 flags;                   // FLAGS register value
    uint32 cs;                      // CS register value
    uint32 ip;                      // IP register value
    uint32 ss;                      // SS register value
};

// DOS system call interface class
class DosSyscallInterface {
private:
    uint8 current_drive;            // Current drive letter (0=A:, 1=B:, etc.)
    char current_directory[DOS_MAX_PATH_LENGTH]; // Current directory for current drive
    uint16_t last_error;              // Last DOS error code
    bool verify_flag;                 // Verify flag for disk operations
    DosDta* current_dta;              // Current DTA (Disk Transfer Area)
    uint8* environment_block;       // Environment variables block
    uint32 environment_size;        // Size of environment block
    uint8* file_handles;            // File handle table
    uint32 file_handle_count;       // Number of file handles
    uint8* extended_file_handles;   // Extended file handle table
    uint32 extended_file_handle_count; // Number of extended file handles
    uint8* interrupt_vectors;       // Interrupt vector table
    uint32 interrupt_vector_count;  // Number of interrupt vectors
    uint8* memory_blocks;           // Memory control blocks
    uint32 memory_block_count;      // Number of memory blocks
    uint8* file_control_blocks;     // File control blocks
    uint32 file_control_block_count; // Number of file control blocks
    uint8* disk_transfer_areas;     // Disk transfer areas
    uint32 disk_transfer_area_count; // Number of disk transfer areas
    uint8* program_segment_prefixes; // Program segment prefixes
    uint32 program_segment_prefix_count; // Number of program segment prefixes
    uint8* search_paths;            // Search paths
    uint32 search_path_count;       // Number of search paths
    Spinlock dos_syscall_lock;        // Lock for thread safety

public:
    DosSyscallInterface();
    ~DosSyscallInterface();
    
    // Initialize the DOS system call interface
    bool Initialize();
    
    // Handle DOS system calls
    int HandleSyscall(DosSyscallContext* context);
    
    // DOS system call implementations
    int DosRead(uint32 fd, void* buffer, uint32 count);
    int DosWrite(uint32 fd, const void* buffer, uint32 count);
    int DosOpen(const char* filename, uint32 flags, uint32 mode);
    int DosClose(uint32 fd);
    int DosCreat(const char* filename, uint32 mode);
    int DosUnlink(const char* filename);
    int DosExec(const char* filename, char* const argv[], char* const envp[]);
    int DosFork();
    int DosWait(int* status);
    int DosGetPid();
    int DosExit(int status);
    int DosKill(int pid, int signal);
    int DosStat(const char* filename, struct FileStat* statbuf);
    int DosFstat(int fd, struct FileStat* statbuf);
    int DosLseek(int fd, int32_t offset, int origin);
    int DosChdir(const char* path);
    int DosGetcwd(char* buf, uint32 size);
    int DosMkdir(const char* path, uint32 mode);
    int DosRmdir(const char* path);
    int DosRename(const char* oldpath, const char* newpath);
    int DosAccess(const char* path, int mode);
    int DosChmod(const char* path, uint32 mode);
    int DosChown(const char* path, uint32 owner, uint32 group);
    int DosUtime(const char* path, struct utimbuf* times);
    int DosDup(int oldfd);
    int DosDup2(int oldfd, int newfd);
    int DosPipe(int pipefd[2]);
    int DosLink(const char* oldpath, const char* newpath);
    int DosSymlink(const char* target, const char* linkpath);
    int DosReadlink(const char* path, char* buf, uint32 bufsiz);
    int DosTruncate(const char* path, uint32 length);
    int DosFtruncate(int fd, uint32 length);
    int DosGetdents(int fd, struct dirent* dirp, uint32 count);
    int DosMmap(void* addr, uint32 length, int prot, int flags, int fd, uint32 offset);
    int DosMunmap(void* addr, uint32 length);
    int DosBrk(void* addr);
    int DosSbrk(int32_t increment);
    int DosMprotect(void* addr, uint32 len, int prot);
    int DosMsync(void* addr, uint32 len, int flags);
    int DosMincore(void* addr, uint32 length, unsigned char* vec);
    int DosMadvise(void* addr, uint32 length, int advice);
    int DosMlock(const void* addr, uint32 len);
    int DosMunlock(const void* addr, uint32 len);
    int DosMlockall(int flags);
    int DosMunlockall();
    int DosMount(const char* source, const char* target, const char* filesystemtype, 
                unsigned long mountflags, const void* data);
    int DosUmount(const char* target);
    int DosUmount2(const char* target, int flags);
    int DosStatfs(const char* path, struct statfs* buf);
    int DosFstatfs(int fd, struct statfs* buf);
    int DosUstat(dev_t dev, struct ustat* ubuf);
    int DosUname(struct utsname* buf);
    int DosGettimeofday(struct timeval* tv, struct timezone* tz);
    int DosSettimeofday(const struct timeval* tv, const struct timezone* tz);
    int DosGetrlimit(int resource, struct rlimit* rlim);
    int DosSetrlimit(int resource, const struct rlimit* rlim);
    int DosGetrusage(int who, struct rusage* usage);
    int DosSysinfo(struct sysinfo* info);
    int DosTimes(struct tms* buf);
    int DosPtrace(long request, pid_t pid, void* addr, void* data);
    int DosGetuid();
    int DosGeteuid();
    int DosGetgid();
    int DosGetegid();
    int DosSetuid(uid_t uid);
    int DosSetgid(gid_t gid);
    int DosGetgroups(int size, gid_t list[]);
    int DosSetgroups(size_t size, const gid_t* list);
    int DosGetpgrp();
    int DosSetpgrp(pid_t pid, pid_t pgrp);
    int DosSetsid();
    int DosGetsid(pid_t pid);
    int DosGetpgid(pid_t pid);
    int DosSetpgid(pid_t pid, pid_t pgid);
    int DosGetppid();
    int DosSignal(int signum, void (*handler)(int));
    int DosSigaction(int signum, const struct sigaction* act, struct sigaction* oldact);
    int DosSigprocmask(int how, const sigset_t* set, sigset_t* oldset);
    int DosSigpending(sigset_t* set);
    int DosSigsuspend(const sigset_t* mask);
    int DosSigaltstack(const stack_t* ss, stack_t* oss);
    int DosKillpg(int pgrp, int sig);
    int DosAlarm(unsigned int seconds);
    int DosPause();
    int DosSleep(unsigned int seconds);
    int DosUsleep(unsigned int useconds);
    int DosNanosleep(const struct timespec* req, struct timespec* rem);
    int DosGetitimer(int which, struct itimerval* curr_value);
    int DosSetitimer(int which, const struct itimerval* new_value, struct itimerval* old_value);
    int DosSelect(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, 
                 struct timeval* timeout);
    int DosPoll(struct pollfd* fds, nfds_t nfds, int timeout);
    int DosEpollCreate(int size);
    int DosEpollCtl(int epfd, int op, int fd, struct epoll_event* event);
    int DosEpollWait(int epfd, struct epoll_event* events, int maxevents, int timeout);
    int DosSocket(int domain, int type, int protocol);
    int DosBind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    int DosConnect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    int DosListen(int sockfd, int backlog);
    int DosAccept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    int DosSend(int sockfd, const void* buf, size_t len, int flags);
    int DosRecv(int sockfd, void* buf, size_t len, int flags);
    int DosSendto(int sockfd, const void* buf, size_t len, int flags,
                 const struct sockaddr* dest_addr, socklen_t addrlen);
    int DosRecvfrom(int sockfd, void* buf, size_t len, int flags,
                   struct sockaddr* src_addr, socklen_t* addrlen);
    int DosSendmsg(int sockfd, const struct msghdr* msg, int flags);
    int DosRecvmsg(int sockfd, struct msghdr* msg, int flags);
    int DosShutdown(int sockfd, int how);
    int DosGetsockopt(int sockfd, int level, int optname, void* optval, socklen_t* optlen);
    int DosSetsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen);
    int DosGetsockname(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    int DosGetpeername(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    int DosSocketpair(int domain, int type, int protocol, int sv[2]);
    int DosIoctl(int fd, unsigned long request, ...);
    int DosFcntl(int fd, int cmd, ...);
    int DosOpenat(int dirfd, const char* pathname, int flags, mode_t mode);
    int DosMkdirat(int dirfd, const char* pathname, mode_t mode);
    int DosMknodat(int dirfd, const char* pathname, mode_t mode, dev_t dev);
    int DosFchownat(int dirfd, const char* pathname, uid_t owner, gid_t group, int flags);
    int DosFutimesat(int dirfd, const char* pathname, const struct timeval times[2]);
    int DosNewfstatat(int dirfd, const char* pathname, struct stat* statbuf, int flags);
    int DosUnlinkat(int dirfd, const char* pathname, int flags);
    int DosRenameat(int olddirfd, const char* oldpath, int newdirfd, const char* newpath);
    int DosLinkat(int olddirfd, const char* oldpath, int newdirfd, const char* newpath, int flags);
    int DosSymlinkat(const char* target, int newdirfd, const char* linkpath);
    int DosReadlinkat(int dirfd, const char* pathname, char* buf, size_t bufsiz);
    int DosFchmodat(int dirfd, const char* pathname, mode_t mode, int flags);
    int DosFaccessat(int dirfd, const char* pathname, int mode, int flags);
    int DosPselect(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds,
                  const struct timespec* timeout, const sigset_t* sigmask);
    int DosPpoll(struct pollfd* fds, nfds_t nfds, const struct timespec* timeout,
                const sigset_t* sigmask);
    int DosUnshare(int flags);
    int DosSetns(int fd, int nstype);
    int DosSplice(int fd_in, loff_t* off_in, int fd_out, loff_t* off_out, size_t len, unsigned int flags);
    int DosVmsplice(int fd, const struct iovec* iov, unsigned long nr_segs, unsigned int flags);
    int DosTee(int fd_in, int fd_out, size_t len, unsigned int flags);
    int DosSyncFileRange(int fd, off64_t offset, off64_t nbytes, unsigned int flags);
    int DosIoSetup(unsigned nr_events, aio_context_t* ctx);
    int DosIoDestroy(aio_context_t ctx);
    int DosIoSubmit(aio_context_t ctx, long nr, struct iocb** iocbpp);
    int DosIoCancel(aio_context_t ctx, struct iocb* iocb, struct io_event* result);
    int DosIoGetEvents(aio_context_t ctx, long min_nr, long nr, struct io_event* events, struct timespec* timeout);
    int DosIoPgetevents(aio_context_t ctx, long min_nr, long nr, struct io_event* events,
                       const struct timespec* timeout, const struct __aio_sigset* sigmask);
    int DosReadahead(int fd, off64_t offset, size_t count);
    int DosKexecLoad(unsigned long entry, unsigned long nr_segments, struct kexec_segment* segments, unsigned long flags);
    int DosKexecFileLoad(int kernel_fd, int initrd_fd, unsigned long cmdline_len, const char* cmdline, unsigned long flags);
    int DosInitModule(void* module_image, unsigned long len, const char* param_values);
    int DosDeleteModule(const char* name, unsigned int flags);
    int DosSyslog(int type, char* bufp, int len);
    int DosAdjtimex(struct timex* buf);
    // int DosSettimeofday(const struct timeval* tv, const struct timezone* tz);  // Duplicate declaration
    int DosClockSettime(clockid_t clk_id, const struct timespec* tp);
    int DosClockGettime(clockid_t clk_id, struct timespec* tp);
    int DosClockGetres(clockid_t clk_id, struct timespec* res);
    int DosClockNanosleep(clockid_t clock_id, int flags, const struct timespec* request, struct timespec* remain);
    // int DosNanosleep(const struct timespec* req, struct timespec* rem);  // Duplicate declaration
    int DosGetrandom(void* buf, size_t buflen, unsigned int flags);
    int DosMemfdCreate(const char* name, unsigned int flags);
    int DosMbind(void* addr, unsigned long len, int mode, const unsigned long* nodemask,
                unsigned long maxnode, unsigned flags);
    int DosSetMempolicy(int mode, const unsigned long* nodemask, unsigned long maxnode);
    int DosGetMempolicy(int* mode, unsigned long* nodemask, unsigned long maxnode, void* addr, unsigned long flags);
    int DosMigratePages(int pid, unsigned long maxnode, const unsigned long* old_nodes, const unsigned long* new_nodes);
    int DosMovePages(int pid, unsigned long count, void** pages, const int* nodes, int* status, int flags);
    int DosAddKey(const char* type, const char* description, const void* payload, size_t plen, int ringid);
    int DosRequestKey(const char* type, const char* description, const char* callout_info, int destringid);
    int DosKeyctl(int cmd, ...);
    int DosSeccomp(unsigned int operation, unsigned int flags, void* args);
    // int DosLandlockCreateRuleset(const struct landlock_ruleset_attr* attr, size_t size, __u32 flags);  // Duplicate declaration
    // int DosLandlockAddRule(int ruleset_fd, enum landlock_rule_type rule_type, const void* rule_attr, __u32 flags);  // Duplicate declaration
    // int DosLandlockRestrictSelf(int ruleset_fd, __u32 flags);  // Duplicate declaration
    int DosPerfEventOpen(struct perf_event_attr* attr, pid_t pid, int cpu, int group_fd, unsigned long flags);
    int DosFanotifyInit(unsigned int flags, unsigned int event_f_flags);
    int DosFanotifyMark(int fanotify_fd, unsigned int flags, uint64_t mask, int dirfd, const char* pathname);
    int DosPrctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);
    int DosArchPrctl(int code, unsigned long addr);
    int DosPersonality(unsigned long persona);
    int DosCapget(cap_user_header_t hdrp, cap_user_data_t datap);
    int DosCapset(cap_user_header_t hdrp, const cap_user_data_t datap);
    int DosIopl(int level);
    int DosIoperm(unsigned long from, unsigned long num, int turn_on);
    int DosCreateModule(const char* name, size_t size);
    int DosQueryModule(const char* name, int which, void* buf, size_t bufsize, size_t* ret);
    int DosGetKernelSyms(struct kernel_sym* table);
    int DosLookupDcookie(uint64_t cookie64, char* buf, size_t len);
    int DosKcmp(pid_t pid1, pid_t pid2, int type, unsigned long idx1, unsigned long idx2);
    int DosProcessVmReadv(pid_t pid, const struct iovec* liov, unsigned long liovcnt,
                         const struct iovec* riov, unsigned long riovcnt, unsigned long flags);
    int DosProcessVmWritev(pid_t pid, const struct iovec* liov, unsigned long liovcnt,
                          const struct iovec* riov, unsigned long riovcnt, unsigned long flags);
    int DosPkeyMprotect(void* addr, size_t len, int prot, int pkey);
    int DosPkeyAlloc(unsigned long flags, unsigned long access_rights);
    int DosPkeyFree(int pkey);
    int DosStatx(int dirfd, const char* pathname, int flags, unsigned int mask, struct statx* statxbuf);
    // int DosIoPgetevents(aio_context_t ctx_id, long min_nr, long nr, struct io_event* events,
//                    const struct timespec* timeout, const struct __aio_sigset* sevp);  // Duplicate declaration
    // int DosRseq(struct rseq* rseq, uint32 rseq_len, int flags, uint32 sig);  // Duplicate declaration
    // int DosPidfdSendSignal(int pidfd, int sig, siginfo_t* info, unsigned int flags);  // Duplicate declaration
    // int DosOpenTree(int dfd, const char* pathname, unsigned int flags);  // Duplicate declaration
    // int DosMoveMount(int from_dfd, const char* from_pathname, int to_dfd, const char* to_pathname, unsigned int flags);  // Duplicate declaration
    // int DosFsopen(const char* fs_name, unsigned int flags);  // Duplicate declaration
    // int DosFsconfig(int fs_fd, unsigned int cmd, const char* key, const void* value, int aux);  // Duplicate declaration
    // int DosFsmount(int fs_fd, unsigned int flags, unsigned int mount_attrs);  // Duplicate declaration
    // int DosFspick(int dfd, const char* path, unsigned int flags);  // Duplicate declaration
    // int DosPidfdOpen(pid_t pid, unsigned int flags);  // Duplicate declaration
    // int DosClone3(struct clone_args* cl_args, size_t size);  // Duplicate declaration
    // int DosCloseRange(unsigned int fd, unsigned int max_fd, unsigned int flags);  // Duplicate declaration
    // int DosOpenat2(int dirfd, const char* pathname, struct open_how* how, size_t size);  // Duplicate declaration
    // int DosPidfdGetfd(int pidfd, int targetfd, unsigned int flags);  // Duplicate declaration
    // int DosFaccessat2(int dirfd, const char* pathname, int mode, int flags);  // Duplicate declaration
    // int DosProcessMadvise(int pidfd, const struct iovec* iov, size_t iovcnt, int advice, unsigned long flags);  // Duplicate declaration
    // int DosEpollPwait2(int epfd, struct epoll_event* events, int maxevents, const struct timespec* timeout,
    //                   const sigset_t* sigmask, size_t sigsetsize);  // Duplicate declaration
    // int DosMountSetattr(int dfd, const char* path, unsigned int flags, struct mount_attr* uattr, size_t usize);  // Duplicate declaration
    // int DosQuotactlFd(unsigned int fd, unsigned int cmd, int id, void* addr);  // Duplicate declaration
    // int DosLandlockCreateRuleset(const struct landlock_ruleset_attr* attr, size_t size, __u32 flags);  // Duplicate declaration
    // int DosLandlockAddRule(int ruleset_fd, enum landlock_rule_type rule_type, const void* rule_attr, __u32 flags);  // Duplicate declaration
    // int DosLandlockRestrictSelf(int ruleset_fd, __u32 flags);  // Duplicate declaration
    // int DosMemfdSecret(unsigned int flags);  // Duplicate declaration
    // int DosProcessMrelease(int pidfd, unsigned int flags);  // Duplicate declaration
    // int DosFutexWaitv(struct futex_waitv* waiters, unsigned int nr_futexes, unsigned int flags,
    //                  struct timespec* timeout, clockid_t clockid);  // Duplicate declaration
    // int DosSetMempolicyHomeNode(unsigned long start, unsigned long len, unsigned long home_node, unsigned long flags);  // Duplicate declaration

private:
    // Internal helper functions
    int DispatchSyscall(DosSyscallContext* context);
    int TranslateLinuxToDosError(int linux_errno);
    int TranslateDosToLinuxError(int dos_error);
    bool IsValidDosPath(const char* path);
    bool ConvertDosPathToUnix(const char* dos_path, char* unix_path, uint32 max_len);
    bool ConvertUnixPathToDos(const char* unix_path, char* dos_path, uint32 max_len);
    uint8 GetDefaultDrive();
    bool SetDefaultDrive(uint8 drive);
    const char* GetDosDrivePath(uint8 drive_letter);
    bool SetDosDrivePath(uint8 drive_letter, const char* path);
    DosPsp* CreatePsp(uint16_t parent_psp_segment, const char* program_name);
    bool DestroyPsp(DosPsp* psp);
    DosDta* CreateDta();
    bool DestroyDta(DosDta* dta);
    DosMcb* CreateMcb(uint8 signature, uint16_t owner_psp, uint16_t size, const char* program_name);
    bool DestroyMcb(DosMcb* mcb);
    uint8* AllocateDosMemory(uint32 paragraphs);
    bool FreeDosMemory(uint8* address);
    bool ResizeDosMemory(uint8* address, uint32 new_paragraphs);
    uint16_t GetDosMemoryBlockOwner(uint8* address);
    bool SetDosMemoryBlockOwner(uint8* address, uint16_t owner_psp);
    uint16_t GetDosMemoryBlockSize(uint8* address);
    bool SetDosMemoryBlockSize(uint8* address, uint16_t size);
    bool ValidateDosMemoryBlock(uint8* address);
    bool SanitizeDosMemoryBlock(uint8* address);
    bool NormalizeDosMemoryBlock(uint8* address);
    int CompareDosMemoryBlocks(uint8* address1, uint8* address2);
    uint8* CloneDosMemoryBlock(uint8* source);
    void FreeDosMemoryBlock(uint8* address);
    uint8* AllocateDosMemoryBlock(uint32 size);
    void DeallocateDosMemoryBlock(uint8* address);
    void PrintDosMemoryBlock(uint8* address);
    void PrintDosMemoryBlocks();
    bool PrintDosMemoryStatistics();
    bool PrintDosMemoryValidation();
    bool PrintDosMemorySanitization();
    bool PrintDosMemoryNormalization();
    bool PrintDosMemoryComparison(uint8* address1, uint8* address2);
    bool PrintDosMemoryCloning(uint8* source);
    bool PrintDosMemoryDeallocation(uint8* address);
    bool PrintDosMemoryAllocation(uint32 size);
};

// Global DOS system call interface instance
extern DosSyscallInterface* g_dos_syscall_interface;

// Initialize the DOS system call interface
bool InitializeDosSyscalls();

// Handle DOS system calls from the kernel
extern "C" int HandleDosSyscall(uint8 interrupt_number, uint8 function_number, 
                              uint32 ax, uint32 bx, uint32 cx, uint32 dx, 
                              uint32 si, uint32 di, uint32 bp, uint32 sp,
                              uint32 ds, uint32 es, uint32 flags,
                              uint32 cs, uint32 ip, uint32 ss);

// Load and run a DOS executable
bool RunDosExecutable(const char* filename, char* const argv[], char* const envp[]);

// Get the current DOS system call interface
DosSyscallInterface* GetDosSyscallInterface();

#endif