#ifndef _Kernel_Vfs_h_
#define _Kernel_Vfs_h_

#include "Common.h"
#include "Defs.h"
#include "DriverFramework.h"
#include "RingBuffer.h"
#include <climits>

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
    uint32_t inode;           // Inode number
    uint32_t size;            // Size in bytes
    uint32_t blocks;          // Number of blocks allocated
    uint32_t block_size;      // Block size
    uint32_t access_time;     // Last access time
    uint32_t modify_time;     // Last modification time
    uint32_t create_time;     // Creation time
    uint32_t mode;            // File mode (permissions and type)
    uint32_t owner_uid;       // Owner user ID
    uint32_t owner_gid;       // Owner group ID
    uint32_t permissions;      // File permissions
    uint8_t  attributes;      // File attributes
};

// Directory entry structure
struct DirEntry {
    char name[MAX_FILENAME_LENGTH];
    uint8_t type;             // File type (directory, regular file, etc.)
    uint32_t inode;           // Inode number
    uint32_t size;            // Size in bytes
};

// File handle structure
struct FileHandle {
    VfsNode* node;            // Pointer to the VFS node
    uint32_t flags;           // Open flags (FILE_READ, FILE_WRITE, etc.)
    uint32_t position;        // Current file position
    uint32_t ref_count;       // Reference count
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
    
    uint32_t inode;           // Inode number
    uint32_t size;            // Size in bytes
    uint8_t attributes;       // File attributes
    uint32_t access_time;     // Last access time
    uint32_t modify_time;     // Last modification time
    uint32_t create_time;     // Creation time
    uint32_t mode;            // File mode
    uint32_t owner_uid;       // Owner user ID
    uint32_t owner_gid;       // Owner group ID
    uint32_t permissions;      // File permissions (like Unix permissions)
    
    // Function pointers for file operations
    int (*open)(VfsNode* node, uint32_t flags);
    int (*close)(VfsNode* node);
    int (*read)(VfsNode* node, void* buffer, uint32_t size, uint32_t offset);
    int (*write)(VfsNode* node, const void* buffer, uint32_t size, uint32_t offset);
    int (*seek)(VfsNode* node, int32_t offset, int origin);
    int (*stat)(VfsNode* node, FileStat* stat);
    int (*readdir)(VfsNode* node, uint32_t index, DirEntry* entry);
    int (*create)(VfsNode* node, const char* name, uint8_t attributes);
    int (*delete)(VfsNode* node);
    
    void* fs_specific;        // Filesystem-specific data
    Device* device;           // Associated device
    uint32_t fs_id;           // Filesystem ID
};

// Mount point structure
struct MountPoint {
    char mount_path[MAX_PATH_LENGTH];  // Path where filesystem is mounted
    VfsNode* root_node;       // Root node of the mounted filesystem
    Device* device;           // Device associated with the filesystem
    uint32_t fs_type;         // Filesystem type (FAT32, etc.)
    bool mounted;             // Whether filesystem is currently mounted
    char fs_name[32];         // Name of the filesystem
};

// Virtual File System class
class Vfs {
private:
    VfsNode* root;                    // Root of the VFS tree
    MountPoint mount_points[MAX_MOUNT_POINTS];  // Array of mount points
    FileHandle open_files[MAX_OPEN_FILES];      // Array of open file handles
    uint32_t mount_count;             // Number of mounted filesystems
    uint32_t open_file_count;         // Number of currently open files
    Spinlock vfs_lock;                // Lock for thread safety
    
    // File system cache
    struct CacheEntry {
        uint32_t block_number;        // Block number in the file system
        void* data;                   // Cached data
        uint32_t size;                // Size of cached data
        bool dirty;                   // Whether the data has been modified
        bool valid;                   // Whether the cache entry is valid
        uint32_t last_access_time;    // Last access time for LRU
        Device* device;               // Device that contains this block
    };
    
    static const uint32_t CACHE_SIZE = 64;  // Number of cache entries
    CacheEntry cache[CACHE_SIZE];           // Cache entries
    uint32_t cache_hits;                    // Number of cache hits
    uint32_t cache_misses;                  // Number of cache misses

public:
    Vfs();
    ~Vfs();
    
    // Initialize the VFS
    bool Initialize();
    
    // Mount a filesystem
    bool Mount(const char* mount_point, Device* device, uint32_t fs_type, const char* fs_name);
    
    // Unmount a filesystem
    bool Unmount(const char* mount_point);
    
    // Open a file
    int Open(const char* path, uint32_t flags);
    
    // Close a file
    int Close(int fd);
    
    // Read from a file
    int Read(int fd, void* buffer, uint32_t size);
    
    // Write to a file
    int Write(int fd, const void* buffer, uint32_t size);
    
    // Seek in a file
    int Seek(int fd, int32_t offset, int origin);
    
    // Get file statistics
    int Stat(const char* path, FileStat* stat);
    
    // Create a directory
    int Mkdir(const char* path, uint32_t mode);
    
    // Remove a directory
    int Rmdir(const char* path);
    
    // Create a file
    int Create(const char* path, uint32_t mode);
    
    // Delete a file
    int Unlink(const char* path);
    
    // Change current directory
    int Chdir(const char* path);
    
    // List directory contents
    int Readdir(const char* path, DirEntry* entries, uint32_t max_entries);
    
    // Get current working directory
    const char* GetCwd();
    
    // Resolve a path to a VFS node
    VfsNode* ResolvePath(const char* path);
    
    // Find a mount point for a given path
    MountPoint* FindMountPoint(const char* path);
    
    // Convert relative path to absolute path
    void GetAbsolutePath(const char* relative_path, char* absolute_path, uint32_t max_len);
    
    // Get the root node
    VfsNode* GetRoot() { return root; }
    
    // Access control functions
    bool CheckPermissions(VfsNode* node, uint32_t uid, uint32_t gid, uint32_t required_permissions);
    
    // Cache management functions
    bool ReadFromCache(Device* device, uint32_t block_number, void* buffer, uint32_t size);
    bool WriteToCache(Device* device, uint32_t block_number, const void* buffer, uint32_t size);
    void InvalidateCache(Device* device, uint32_t block_number = UINT32_MAX);  // Use UINT32_MAX to invalidate all blocks for device
    void FlushCache(Device* device = nullptr);  // Use nullptr to flush all devices

private:
    // Internal helper functions
    VfsNode* CreateVfsNode(const char* name, VfsNode* parent);
    void DestroyVfsNode(VfsNode* node);
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