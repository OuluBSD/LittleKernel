#ifndef _Kernel_DosKpiV2_h_
#define _Kernel_DosKpiV2_h_

#include "Common.h"
#include "Defs.h"
#include "ProcessControlBlock.h"
#include "AbiMultiplexer.h"

// DOS-KPIv2 syscall numbers (mapped to x86-64 syscall numbers)
// Using DOS-specific syscall numbers to avoid conflicts with native kernel syscalls
#define DOS_KPIV2_BASE_SYSCALL 4000
#define DOS_KPIV2_SYSCALL_EXIT (DOS_KPIV2_BASE_SYSCALL + 0)
#define DOS_KPIV2_SYSCALL_READ (DOS_KPIV2_BASE_SYSCALL + 1)
#define DOS_KPIV2_SYSCALL_WRITE (DOS_KPIV2_BASE_SYSCALL + 2)
#define DOS_KPIV2_SYSCALL_OPEN (DOS_KPIV2_BASE_SYSCALL + 3)
#define DOS_KPIV2_SYSCALL_CLOSE (DOS_KPIV2_BASE_SYSCALL + 4)
#define DOS_KPIV2_SYSCALL_CREAT (DOS_KPIV2_BASE_SYSCALL + 5)
#define DOS_KPIV2_SYSCALL_UNLINK (DOS_KPIV2_BASE_SYSCALL + 6)
#define DOS_KPIV2_SYSCALL_EXEC (DOS_KPIV2_BASE_SYSCALL + 7)
#define DOS_KPIV2_SYSCALL_FORK (DOS_KPIV2_BASE_SYSCALL + 8)
#define DOS_KPIV2_SYSCALL_WAIT (DOS_KPIV2_BASE_SYSCALL + 9)
#define DOS_KPIV2_SYSCALL_GETPID (DOS_KPIV2_BASE_SYSCALL + 10)
#define DOS_KPIV2_SYSCALL_KILL (DOS_KPIV2_BASE_SYSCALL + 11)
#define DOS_KPIV2_SYSCALL_STAT (DOS_KPIV2_BASE_SYSCALL + 12)
#define DOS_KPIV2_SYSCALL_FSTAT (DOS_KPIV2_BASE_SYSCALL + 13)
#define DOS_KPIV2_SYSCALL_LSEEK (DOS_KPIV2_BASE_SYSCALL + 14)
#define DOS_KPIV2_SYSCALL_CHDIR (DOS_KPIV2_BASE_SYSCALL + 15)
#define DOS_KPIV2_SYSCALL_GETCWD (DOS_KPIV2_BASE_SYSCALL + 16)
#define DOS_KPIV2_SYSCALL_MKDIR (DOS_KPIV2_BASE_SYSCALL + 17)
#define DOS_KPIV2_SYSCALL_RMDIR (DOS_KPIV2_BASE_SYSCALL + 18)
#define DOS_KPIV2_SYSCALL_RENAME (DOS_KPIV2_BASE_SYSCALL + 19)
#define DOS_KPIV2_SYSCALL_ACCESS (DOS_KPIV2_BASE_SYSCALL + 20)
#define DOS_KPIV2_SYSCALL_CHMOD (DOS_KPIV2_BASE_SYSCALL + 21)
#define DOS_KPIV2_SYSCALL_CHOWN (DOS_KPIV2_BASE_SYSCALL + 22)
#define DOS_KPIV2_SYSCALL_UTIME (DOS_KPIV2_BASE_SYSCALL + 23)
#define DOS_KPIV2_SYSCALL_PIPE (DOS_KPIV2_BASE_SYSCALL + 24)
#define DOS_KPIV2_SYSCALL_DUP (DOS_KPIV2_BASE_SYSCALL + 25)
#define DOS_KPIV2_SYSCALL_DUP2 (DOS_KPIV2_BASE_SYSCALL + 26)
#define DOS_KPIV2_SYSCALL_SYMLINK (DOS_KPIV2_BASE_SYSCALL + 27)
#define DOS_KPIV2_SYSCALL_READLINK (DOS_KPIV2_BASE_SYSCALL + 28)
#define DOS_KPIV2_SYSCALL_TRUNCATE (DOS_KPIV2_BASE_SYSCALL + 29)
#define DOS_KPIV2_SYSCALL_FTRUNCATE (DOS_KPIV2_BASE_SYSCALL + 30)
#define DOS_KPIV2_SYSCALL_GETDENTS (DOS_KPIV2_BASE_SYSCALL + 31)
#define DOS_KPIV2_SYSCALL_MMAP (DOS_KPIV2_BASE_SYSCALL + 32)
#define DOS_KPIV2_SYSCALL_MUNMAP (DOS_KPIV2_BASE_SYSCALL + 33)
#define DOS_KPIV2_SYSCALL_BRK (DOS_KPIV2_BASE_SYSCALL + 34)
#define DOS_KPIV2_SYSCALL_SBRK (DOS_KPIV2_BASE_SYSCALL + 35)
#define DOS_KPIV2_SYSCALL_MPROTECT (DOS_KPIV2_BASE_SYSCALL + 36)
#define DOS_KPIV2_SYSCALL_MSYNC (DOS_KPIV2_BASE_SYSCALL + 37)
#define DOS_KPIV2_SYSCALL_MINCORE (DOS_KPIV2_BASE_SYSCALL + 38)
#define DOS_KPIV2_SYSCALL_MADVISE (DOS_KPIV2_BASE_SYSCALL + 39)
#define DOS_KPIV2_SYSCALL_MLOCK (DOS_KPIV2_BASE_SYSCALL + 40)
#define DOS_KPIV2_SYSCALL_MUNLOCK (DOS_KPIV2_BASE_SYSCALL + 41)
#define DOS_KPIV2_SYSCALL_MLOCKALL (DOS_KPIV2_BASE_SYSCALL + 42)
#define DOS_KPIV2_SYSCALL_MUNLOCKALL (DOS_KPIV2_BASE_SYSCALL + 43)
#define DOS_KPIV2_SYSCALL_MOUNT (DOS_KPIV2_BASE_SYSCALL + 44)
#define DOS_KPIV2_SYSCALL_UMOUNT (DOS_KPIV2_BASE_SYSCALL + 45)
#define DOS_KPIV2_SYSCALL_UMOUNT2 (DOS_KPIV2_BASE_SYSCALL + 46)
#define DOS_KPIV2_SYSCALL_STATFS (DOS_KPIV2_BASE_SYSCALL + 47)
#define DOS_KPIV2_SYSCALL_FSTATFS (DOS_KPIV2_BASE_SYSCALL + 48)
#define DOS_KPIV2_SYSCALL_USTAT (DOS_KPIV2_BASE_SYSCALL + 49)
#define DOS_KPIV2_SYSCALL_UNAME (DOS_KPIV2_BASE_SYSCALL + 50)
#define DOS_KPIV2_SYSCALL_GETTIMEOFDAY (DOS_KPIV2_BASE_SYSCALL + 51)

// DOS-KPIv2 specific data structures that maintain compatibility with DOS concepts
// but use modern calling conventions

// DOS Program Segment Prefix for DOS-KPIv2
struct DosKpiV2Psp {
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

// DOS-KPIv2 context for a process
struct DosKpiV2Context {
    // Memory management
    DosMcb* memory_blocks;            // Memory control blocks
    uint32 memory_block_count;      // Number of memory blocks
    
    // File system
    char current_directory[DOS_MAX_PATH_LENGTH]; // Current directory for current drive
    uint8 current_drive;            // Current drive letter (0=A:, 1=B:, etc.)
    uint8* file_handles;            // File handle table
    uint32 file_handle_count;       // Number of file handles
    
    // Process state
    uint16_t last_error;              // Last DOS error code
    DosPsp* current_psp;              // Current PSP (Program Segment Prefix)
    DosDta* current_dta;              // Current DTA (Disk Transfer Area)
    uint8* environment_block;       // Environment variables block
    uint32 environment_size;        // Size of environment block
    bool verify_flag;                 // Verify flag for disk operations
};

// DOS-KPIv2 ABI interface class
class DosKpiV2Interface {
private:
    DosKpiV2Context global_context;  // Global context for DOS-KPIv2
    Spinlock dos_kpi_v2_lock;        // Lock for thread safety

public:
    DosKpiV2Interface();
    ~DosKpiV2Interface();
    
    // Initialize the DOS-KPIv2 interface
    bool Initialize();
    
    // Handle DOS-KPIv2 syscalls (using syscall instruction instead of interrupts)
    int HandleSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, 
                     uint32 arg4, uint32 arg5, uint32 arg6);
    
    // DOS-KPIv2 specific implementations (syscall-based equivalents of INT 21h functions)
    int DosKpiV2Read(uint32 fd, void* buffer, uint32 count);
    int DosKpiV2Write(uint32 fd, const void* buffer, uint32 count);
    int DosKpiV2Open(const char* filename, uint32 flags, uint32 mode);
    int DosKpiV2Close(uint32 fd);
    int DosKpiV2Creat(const char* filename, uint32 mode);
    int DosKpiV2Unlink(const char* filename);
    int DosKpiV2Exec(const char* filename, char* const argv[], char* const envp[]);
    int DosKpiV2Fork();
    int DosKpiV2Wait(int* status);
    int DosKpiV2GetPid();
    int DosKpiV2Exit(int status);
    int DosKpiV2Kill(int pid, int signal);
    int DosKpiV2Stat(const char* filename, struct FileStat* statbuf);
    int DosKpiV2Fstat(int fd, struct FileStat* statbuf);
    int DosKpiV2Lseek(int fd, int32_t offset, int origin);
    int DosKpiV2Chdir(const char* path);
    int DosKpiV2Getcwd(char* buf, uint32 size);
    int DosKpiV2Mkdir(const char* path, uint32 mode);
    int DosKpiV2Rmdir(const char* path);
    int DosKpiV2Rename(const char* oldpath, const char* newpath);
    int DosKpiV2Access(const char* path, int mode);
    int DosKpiV2Chmod(const char* path, uint32 mode);
    int DosKpiV2Chown(const char* path, uint32 owner, uint32 group);
    int DosKpiV2Utime(const char* path, struct utimbuf* times);
    int DosKpiV2Dup(int oldfd);
    int DosKpiV2Dup2(int oldfd, int newfd);
    int DosKpiV2Pipe(int pipefd[2]);
    int DosKpiV2Symlink(const char* target, const char* linkpath);
    int DosKpiV2Readlink(const char* path, char* buf, uint32 bufsiz);
    int DosKpiV2Truncate(const char* path, uint32 length);
    int DosKpiV2Ftruncate(int fd, uint32 length);
    int DosKpiV2Getdents(int fd, struct dirent* dirp, uint32 count);
    int DosKpiV2Mmap(void* addr, uint32 length, int prot, int flags, int fd, uint32 offset);
    int DosKpiV2Munmap(void* addr, uint32 length);
    int DosKpiV2Brk(void* addr);
    int DosKpiV2Sbrk(int32 increment);
    int DosKpiV2Mprotect(void* addr, uint32 len, int prot);
    int DosKpiV2Msync(void* addr, uint32 len, int flags);
    int DosKpiV2Mincore(void* addr, uint32 length, unsigned char* vec);
    int DosKpiV2Madvise(void* addr, uint32 length, int advice);
    int DosKpiV2Mlock(const void* addr, uint32 len);
    int DosKpiV2Munlock(const void* addr, uint32 len);
    int DosKpiV2Mlockall(int flags);
    int DosKpiV2Munlockall();
    int DosKpiV2Mount(const char* source, const char* target, const char* filesystemtype, 
                     unsigned long mountflags, const void* data);
    int DosKpiV2Umount(const char* target);
    int DosKpiV2Umount2(const char* target, int flags);
    int DosKpiV2Statfs(const char* path, struct statfs* buf);
    int DosKpiV2Fstatfs(int fd, struct statfs* buf);
    int DosKpiV2Ustat(dev_t dev, struct ustat* ubuf);
    int DosKpiV2Uname(struct utsname* buf);
    int DosKpiV2Gettimeofday(struct timeval* tv, struct timezone* tz);
    
    // DOS-specific functions
    int DosKpiV2SetCurrentDirectory(const char* path);
    int DosKpiV2GetCurrentDirectory(char* buffer, uint32 size);
    int DosKpiV2SetCurrentDrive(uint8 drive);
    uint8 DosKpiV2GetCurrentDrive();
    int DosKpiV2FindFirst(const char* filespec, uint16_t attributes, DosDirEntry* entry);
    int DosKpiV2FindNext(DosDirEntry* entry);
    int DosKpiV2SetFileAttributes(const char* filename, uint16_t attributes);
    int DosKpiV2GetFileAttributes(const char* filename, uint16_t* attributes);
    int DosKpiV2AllocateMemory(uint32 paragraphs, uint16_t* segment);
    int DosKpiV2FreeMemory(uint16_t segment);
    int DosKpiV2ResizeMemory(uint16_t segment, uint32 new_paragraphs);
    
private:
    // Internal helper functions
    int DispatchSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, 
                       uint32 arg4, uint32 arg5, uint32 arg6);
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

// Global DOS-KPIv2 interface instance
extern DosKpiV2Interface* g_dos_kpi_v2_interface;

// Initialize the DOS-KPIv2 interface
bool InitializeDosKpiV2();

// Handle DOS-KPIv2 syscalls (using syscall instruction)
extern "C" int HandleDosKpiV2Syscall(uint32 syscall_num, 
                                    uint32 arg1, uint32 arg2, uint32 arg3, 
                                    uint32 arg4, uint32 arg5, uint32 arg6);

// Setup DOS-KPIv2 syscall table for the ABI multiplexer
bool SetupDosKpiV2SyscallTable();

#endif