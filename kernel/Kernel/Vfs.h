#ifndef _Kernel_Vfs_h_
#define _Kernel_Vfs_h_

#include "Common.h"
#include "Defs.h"
#include "DriverFramework.h"
// #include "RingBuffer.h"  // Not yet implemented
// #include <climits>  // Not available in kernel space, defining constants directly

// Standard constants that would normally come from climits
#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

#ifndef SCHAR_MIN
#define SCHAR_MIN (-128)
#endif

#ifndef SCHAR_MAX
#define SCHAR_MAX 127
#endif

#ifndef UCHAR_MAX
#define UCHAR_MAX 255
#endif

#ifndef CHAR_MIN
#define CHAR_MIN SCHAR_MIN
#endif

#ifndef CHAR_MAX
#define CHAR_MAX SCHAR_MAX
#endif

#ifndef MB_LEN_MAX
#define MB_LEN_MAX 16
#endif

#ifndef SHRT_MIN
#define SHRT_MIN (-32768)
#endif

#ifndef SHRT_MAX
#define SHRT_MAX 32767
#endif

#ifndef USHRT_MAX
#define USHRT_MAX 65535
#endif

#ifndef INT_MIN
#define INT_MIN (-2147483648)
#endif

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

#ifndef UINT_MAX
#define UINT_MAX 4294967295U
#endif

#ifndef LONG_MIN
#define LONG_MIN INT_MIN
#endif

#ifndef LONG_MAX
#define LONG_MAX INT_MAX
#endif

#ifndef ULONG_MAX
#define ULONG_MAX UINT_MAX
#endif

#ifndef LLONG_MIN
#define LLONG_MIN (-9223372036854775808LL)
#endif

#ifndef LLONG_MAX
#define LLONG_MAX 9223372036854775807LL
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX 18446744073709551615ULL
#endif

// File system constants
#define MAX_PATH_LENGTH 260
#define MAX_FILENAME_LENGTH 256
#define MAX_MOUNT_POINTS 32
#define MAX_OPEN_FILES 256

// File access flags
#define FILE_READ 0x01
#define FILE_WRITE 0x02
#define FILE_EXECUTE 0x04
#define FILE_CREATE 0x08
#define FILE_TRUNCATE 0x10
#define FILE_APPEND 0x20

// File attributes
#define ATTR_READONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20

// File seek origins
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// Common return values
#define VFS_SUCCESS 0
#define VFS_ERROR -1
#define VFS_EOF -2
#define VFS_FILE_NOT_FOUND -3
#define VFS_ACCESS_DENIED -4
#define VFS_TOO_MANY_OPEN_FILES -5

// Forward declarations
struct VfsNode;
struct MountPoint;

// File statistics structure
struct FileStat {
    uint32 inode;           // Inode number
    uint32 size;            // Size in bytes
    uint32 blocks;          // Number of blocks allocated
    uint32 block_size;      // Block size
    uint32 access_time;     // Last access time
    uint32 modify_time;     // Last modification time
    uint32 create_time;     // Creation time
    uint32 mode;            // File mode (permissions and type)
    uint32 owner_uid;       // Owner user ID
    uint32 owner_gid;       // Owner group ID
    uint32 permissions;      // File permissions
    uint8  attributes;      // File attributes
    uint32 st_size;         // For compatibility with POSIX stat structure
};

// Directory entry structure
struct DirEntry {
    char name[MAX_FILENAME_LENGTH];
    uint8 type;             // File type (directory, regular file, etc.)
    uint32 inode;           // Inode number
    uint32 size;            // Size in bytes
};

// File handle structure
struct FileHandle {
    VfsNode* node;            // Pointer to the VFS node
    uint32 flags;           // Open flags (FILE_READ, FILE_WRITE, etc.)
    uint32 position;        // Current file position
    uint32 ref_count;       // Reference count
    bool is_open;             // Whether the file is currently open
};

// VFS node structure representing a file or directory
struct VfsNode {
    char name[MAX_FILENAME_LENGTH];  // File/directory name
    char full_path[MAX_PATH_LENGTH]; // Full path
    VfsNode* parent;          // Parent directory
    VfsNode* children;        // First child in linked list
    VfsNode* next_sibling;    // Next sibling in linked list
    VfsNode* prev_sibling;    // Previous sibling in linked list
    
    uint32 inode;           // Inode number
    uint32 size;            // Size in bytes
    uint8 attributes;       // File attributes
    uint32 access_time;     // Last access time
    uint32 modify_time;     // Last modification time
    uint32 create_time;     // Creation time
    uint32 mode;            // File mode
    uint32 owner_uid;       // Owner user ID
    uint32 owner_gid;       // Owner group ID
    uint32 permissions;      // File permissions (like Unix permissions)
    
    // Function pointers for file operations
    int (*open)(VfsNode* node, uint32 flags);
    int (*close)(VfsNode* node);
    int (*read)(VfsNode* node, void* buffer, uint32 size, uint32 offset);
    int (*write)(VfsNode* node, const void* buffer, uint32 size, uint32 offset);
    int (*seek)(VfsNode* node, int32 offset, int origin);
    int (*stat)(VfsNode* node, FileStat* stat);
    int (*readdir)(VfsNode* node, uint32 index, DirEntry* entry);
    int (*create)(VfsNode* node, const char* name, uint8 attributes);
    int (*Delete)(VfsNode* node);
    int (*delete_fn)(VfsNode* node);  // For backward compatibility with existing code
    
    void* fs_specific;        // Filesystem-specific data
    Device* device;           // Associated device
    uint32 fs_id;           // Filesystem ID
};

// Mount point structure
struct MountPoint {
    char mount_path[MAX_PATH_LENGTH];  // Path where filesystem is mounted
    VfsNode* root_node;       // Root node of the mounted filesystem
    Device* device;           // Device associated with the filesystem
    uint32 fs_type;         // Filesystem type (FAT32, etc.)
    bool mounted;             // Whether filesystem is currently mounted
    char fs_name[32];         // Name of the filesystem
};

// Virtual File System class
class Vfs {
private:
    VfsNode* root;                    // Root of the VFS tree
    MountPoint mount_points[MAX_MOUNT_POINTS];  // Array of mount points
    FileHandle open_files[MAX_OPEN_FILES];      // Array of open file handles
    uint32 mount_count;             // Number of mounted filesystems
    uint32 open_file_count;         // Number of currently open files
    Spinlock vfs_lock;                // Lock for thread safety
    
    // File system cache
    struct CacheEntry {
        uint32 block_number;        // Block number in the file system
        void* data;                   // Cached data
        uint32 size;                // Size of cached data
        bool dirty;                   // Whether the data has been modified
        bool valid;                   // Whether the cache entry is valid
        uint32 last_access_time;    // Last access time for LRU
        Device* device;               // Device that contains this block
    };
    
    static const uint32 CACHE_SIZE = 64;  // Number of cache entries
    CacheEntry cache[CACHE_SIZE];           // Cache entries
    uint32 cache_hits;                    // Number of cache hits
    uint32 cache_misses;                  // Number of cache misses

public:
    Vfs();
    ~Vfs();
    
    // Initialize the VFS
    bool Initialize();
    
    // Mount a filesystem
    bool Mount(const char* mount_point, Device* device, uint32 fs_type, const char* fs_name);
    
    // Unmount a filesystem
    bool Unmount(const char* mount_point);
    
    // Open a file
    int Open(const char* path, uint32 flags);
    
    // Close a file
    int Close(int fd);
    
    // Read from a file
    int Read(int fd, void* buffer, uint32 size);
    
    // Write to a file
    int Write(int fd, const void* buffer, uint32 size);
    
    // Seek in a file
    int Seek(int fd, int32 offset, int origin);
    
    // Get file statistics
    int Stat(const char* path, FileStat* stat);
    
    // Create a directory
    int Mkdir(const char* path, uint32 mode);
    
    // Remove a directory
    int Rmdir(const char* path);
    
    // Create a file
    int Create(const char* path, uint32 mode);
    
    // Delete a file
    int Unlink(const char* path);
    
    // Change current directory
    int Chdir(const char* path);
    
    // List directory contents
    int Readdir(const char* path, DirEntry* entries, uint32 max_entries);
    
    // Get current working directory
    const char* GetCwd();
    
    // Resolve a path to a VFS node
    VfsNode* ResolvePath(const char* path);
    
    // Find a mount point for a given path
    MountPoint* FindMountPoint(const char* path);
    
    // Convert relative path to absolute path
    void GetAbsolutePath(const char* relative_path, char* absolute_path, uint32 max_len);
    
    // Get the root node
    VfsNode* GetRoot() { return root; }
    
    // Access control functions
    bool CheckPermissions(VfsNode* node, uint32 uid, uint32 gid, uint32 required_permissions);
    
    // Cache management functions
    bool ReadFromCache(Device* device, uint32 block_number, void* buffer, uint32 size);
    bool WriteToCache(Device* device, uint32 block_number, const void* buffer, uint32 size);
    void InvalidateCache(Device* device, uint32 block_number = UINT32_MAX);  // Use UINT32_MAX to invalidate all blocks for device
    void FlushCache(Device* device = nullptr);  // Use nullptr to flush all devices

private:
    // Internal helper functions
    void DestroyVfsNode(VfsNode* node);

public:
    // Public helper functions
    VfsNode* CreateVfsNode(const char* name, VfsNode* parent);
    FileHandle* GetFreeFileHandle();
    FileHandle* GetFileHandle(int fd);
    bool IsValidFileHandle(int fd);
    int AllocateFd();
    void ReleaseFd(int fd);
    void SplitPath(const char* path, char* dir, char* filename);
    bool IsAbsolutePath(const char* path);
};

// Global VFS instance
extern Vfs* g_vfs;

// Initialize the global VFS
bool InitializeVfs();

#endif