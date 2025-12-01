#ifndef _Kernel_Defs_h_
#define _Kernel_Defs_h_

// Type definitions following Ultimate++ conventions

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long long uint64;

typedef signed char  int8;
typedef signed short int16;
typedef signed int   int32;
typedef signed long long int64;

// Additional type definitions for system calls compatibility
typedef unsigned int size_t;
typedef int32 pid_t;
typedef uint32 mode_t;
typedef int32 off_t;
typedef int32 ssize_t;  // Add ssize_t definition
typedef uint32 socklen_t;
typedef int32 uid_t;
typedef int32 gid_t;
typedef uint32 uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned short uint16_t;
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef int32 int32_t;
typedef signed short int16_t;
typedef signed long long int64_t;

// Common constants
#define UINT32_MAX 0xFFFFFFFF
#define UINT64_MAX 0xFFFFFFFFFFFFFFFFULL
#define INT32_MAX 0x7FFFFFFF
#define INT32_MIN (-0x7FFFFFFF - 1)
#define UINT16_MAX 0xFFFF
#define INT16_MAX 0x7FFF
#define INT16_MIN (-0x7FFF - 1)
#define UINT8_MAX 0xFF
#define INT8_MAX 0x7F
#define INT8_MIN (-0x7F - 1)

// Additional constants for compatibility
#define UINT64_MAX 0xFFFFFFFFFFFFFFFFULL  // Already defined above but ensuring it's available
#define ULONG_MAX UINT32_MAX
#define LONG_MAX INT32_MAX
#define LONG_MIN INT32_MIN
#define USHRT_MAX UINT16_MAX
#define SHRT_MAX INT16_MAX
#define SHRT_MIN INT16_MIN
#define UCHAR_MAX UINT8_MAX
#define SCHAR_MAX INT8_MAX
#define SCHAR_MIN INT8_MIN

// Page size constants
#define KERNEL_PAGE_SIZE 4096
#define PAGE_SIZE KERNEL_PAGE_SIZE

// Types for Linux compatibility
struct fd_set {
    uint32_t fds_bits[32];  // 1024 bits (enough for 1024 file descriptors)
};

struct sigset_t {
    uint32_t sig[4];  // 128 signal bits
};

struct timeval {
    int32_t tv_sec;   // seconds
    int32_t tv_usec;  // microseconds
};

// Additional Linux compatibility types
typedef int32_t off64_t;
typedef int32_t loff_t;
typedef int32_t dev_t;
typedef int32_t key_t;
typedef int32_t cpu_set_t;
typedef int32_t idtype_t;
typedef int32_t id_t;
typedef int32_t clockid_t;
typedef int32_t aio_context_t;
typedef uint32_t __u32;
typedef int32_t nfds_t;
typedef char* caddr_t;

struct siginfo_t {
    int si_signo;
    int si_errno;
    int si_code;
    // Additional fields could be added as needed
};

struct stack_t {
    void* ss_sp;
    int ss_flags;
    size_t ss_size;
};

struct timespec {
    int32_t tv_sec;
    int32_t tv_nsec;
};

struct __aio_sigset {
    const sigset_t* sigmask;
    size_t sigsetsize;
};

// String functions needed for compatibility
#define strncpy(dest, src, n) __builtin_strncpy(dest, src, n)

// Capabilities types
typedef struct cap_user_header {
    uint32_t version;
    int pid;
} *cap_user_header_t;

typedef struct cap_user_data {
    uint32_t effective;
    uint32_t permitted;
    uint32_t inheritable;
} *cap_user_data_t;

// Landlock types
struct landlock_ruleset_attr {
    uint64_t handled_access_fs;
};

// IO event structures
struct iocb {
    uint64_t aio_data;
    uint32_t aio_key;
    uint32_t aio_reserved1;
    uint16_t aio_lio_opcode;
    int16_t aio_reqprio;
    uint32_t aio_fildes;
    uint64_t aio_buf;
    uint64_t aio_nbytes;
    int64_t aio_offset;
    uint64_t aio_reserved2;
    uint32_t aio_flags;
    uint32_t aio_resfd;
};

struct io_event {
    uint64_t data;
    uint64_t obj;
    int64_t res;
    int64_t res2;
};

// System information structure (utsname)
struct utsname {
    char sysname[65];    // Operating system name
    char nodename[65];   // Network node name
    char release[65];    // Operating system release
    char version[65];    // Operating system version
    char machine[65];    // Hardware identifier
    char domainname[65]; // NIS or YP domain name
};

// Timezone structure
struct timezone {
    int tz_minuteswest;  // Minutes west of GMT
    int tz_dsttime;      // Daylight saving time correction
};

// Additional system call error codes (Linux-compatible)
#define ENOENT      2    // No such file or directory
#define EACCES      13   // Permission denied
#define ENOMEM      12   // Cannot allocate memory
#define EEXIST      17   // File exists
#define EINVAL      22   // Invalid argument
#define EISDIR      21   // Is a directory
#define ENOTDIR     20   // Not a directory
#define ENFILE      23   // Too many open files in system
#define EMFILE      24   // Too many open files
#define ENOTTY      25   // Inappropriate I/O control operation
#define EFBIG       27   // File too large
#define ENOSPC      28   // No space left on device
#define ESPIPE      29   // Invalid seek
#define EROFS       30   // Read-only file system
#define EMLINK      31   // Too many links
#define EPIPE       32   // Broken pipe
#define EAGAIN      11   // Resource temporarily unavailable
#define EWOULDBLOCK 11   // Operation would block
#define EINPROGRESS 115  // Operation now in progress
#define EALREADY    114  // Operation already in progress
#define ENOTSOCK    88   // Socket operation on non-socket
#define EDESTADDRREQ 89   // Destination address required
#define EMSGSIZE    90   // Message too long
#define EPROTOTYPE  91   // Protocol wrong type for socket
#define ENOPROTOOPT 92   // Protocol not available
#define EPROTONOSUPPORT 93 // Protocol not supported
#define ESOCKTNOSUPPORT 94 // Socket type not supported
#define EOPNOTSUPP  95   // Operation not supported
#define EPFNOSUPPORT 96  // Protocol family not supported
#define EAFNOSUPPORT 97   // Address family not supported
#define EADDRINUSE  98   // Address already in use
#define EADDRNOTAVAIL 99  // Cannot assign requested address
#define ENETDOWN    100  // Network is down
#define ENETUNREACH 101  // Network is unreachable
#define ENETRESET   102  // Network dropped connection on reset
#define ECONNABORTED 103 // Software caused connection abort
#define ECONNRESET  104  // Connection reset by peer
#define ENOBUFS     105  // No buffer space available
#define EISCONN     106  // Transport endpoint is already connected
#define ENOTCONN    107  // Transport endpoint is not connected
#define ESHUTDOWN   108  // Cannot send after transport endpoint shutdown
#define ETOOMANYREFS 109 // Too many references: cannot splice
#define ETIMEDOUT   110  // Connection timed out
#define ECONNREFUSED 111 // Connection refused
#define EHOSTDOWN   112  // Host is down
#define EHOSTUNREACH 113 // No route to host

// Define Boolean values as macros for compatibility
#define TRUE 1
#define FALSE 0
#define True 1
#define False 0
#define true 1
#define false 0

// Hex manipulator for logging
#define hex 

// Null pointer
#define NULL 0

// Common constants
#define SECTOR_SIZE 512
#define DOS_MAX_PATH_LENGTH 260

// Memory layout
#define KERNEL_VIRTUAL_BASE 0xC0000000
#define DEFAULT_KERNEL_HEAP_START 0xD0000000
#define DEFAULT_KERNEL_HEAP_SIZE 0x1000000  // 16MB

// DOS compatibility constants
#define DOS_MAX_DRIVE_LETTERS 26  // A-Z drives

// Useful macros
#define HIGH_BYTE_OF_WORD(x) ((x & 0xFF00) >> 8)
#define LOW_BYTE_OF_WORD(x)  (x & 0x00FF)
#define CONCAT_BYTES(h, l)   ((h << 8) | l)

// Alignment macros
#define ALIGN_UP(addr, size)   (((addr) + (size) - 1) & ~((size) - 1))
#define ALIGN_DOWN(addr, size) ((addr) & ~((size) - 1))

// Bit manipulation macros
#define SET_BIT(value, bit)    ((value) |= (1 << (bit)))
#define CLEAR_BIT(value, bit)  ((value) &= ~(1 << (bit)))
#define TEST_BIT(value, bit)   (((value) >> (bit)) & 1)

// Define landlock_rule_type enum for Linuxulator compatibility
enum landlock_rule_type {
    LANDLOCK_RULE_PATH_BENEATH = 1,
    LANDLOCK_RULE_NET_SERVICE = 2
};

// Landlock ruleset attributes structure
// struct landlock_ruleset_attr {
//     uint64_t handled_access_fs;
// };

#endif