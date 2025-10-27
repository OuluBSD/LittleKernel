#ifndef _Kernel_RamFs_h_
#define _Kernel_RamFs_h_

#include "Common.h"
#include "Defs.h"
#include "Vfs.h"

// RAM filesystem constants
#define RAMFS_MAX_FILES 128
#define RAMFS_MAX_FILE_SIZE (2 * 1024 * 1024)  // 2MB max file size
#define RAMFS_MAX_FILENAME_LENGTH 256
#define RAMFS_MAGIC 0x52414D46  // 'RAMF'

// RAM file node structure
struct RamFsNode {
    char name[RAMFS_MAX_FILENAME_LENGTH];
    uint8 attributes;
    uint32 size;
    uint32 alloc_size;  // Allocated size (may be larger than actual size)
    uint32 access_time;
    uint32 modify_time;
    uint32 create_time;
    void* data;           // Pointer to file data
    RamFsNode* parent;
    RamFsNode* children;
    RamFsNode* next_sibling;
    RamFsNode* prev_sibling;
    bool is_directory;
    uint32 ref_count;
};

// RAM filesystem structure
struct RamFs {
    uint32 magic;
    RamFsNode* root;
    uint32 total_size;
    uint32 used_size;
    uint32 free_size;
    Spinlock fs_lock;
};

// RAM filesystem driver class
class RamFsDriver {
private:
    RamFs* fs;
    VfsNode* vfs_root;

public:
    RamFsDriver();
    ~RamFsDriver();
    
    // Initialize the RAM filesystem
    bool Initialize(uint32 size = 4 * 1024 * 1024);  // Default 4MB
    
    // Mount the RAM filesystem to VFS
    bool Mount(const char* mount_point);
    
    // Unmount the RAM filesystem
    bool Unmount();
    
    // Create a file
    RamFsNode* CreateFile(const char* path, uint8 attributes = 0);
    
    // Create a directory
    RamFsNode* CreateDirectory(const char* path);
    
    // Delete a file or directory
    bool Delete(const char* path);
    
    // Write to a file
    int WriteFile(RamFsNode* node, const void* buffer, uint32 size, uint32 offset);
    
    // Read from a file
    int ReadFile(RamFsNode* node, void* buffer, uint32 size, uint32 offset);
    
    // Get file statistics
    int GetStat(RamFsNode* node, FileStat* stat);
    
    // Find a file or directory
    RamFsNode* FindNode(const char* path);
    
    // Get file system information
    bool GetFsInfo(uint32& total_size, uint32& used_size, uint32& free_size);
    
    // Get the VFS root node
    VfsNode* GetVfsRoot() { return vfs_root; }

private:
    // Internal helper functions
    RamFsNode* CreateNode(const char* name, RamFsNode* parent, bool is_directory);
    void DestroyNode(RamFsNode* node);
    bool AllocateData(RamFsNode* node, uint32 size);
    bool ResizeData(RamFsNode* node, uint32 new_size);
    void SplitPath(const char* path, char* dir, char* filename);
    
    // VFS operation implementations
    static int Open(VfsNode* node, uint32 flags);
    static int Close(VfsNode* node);
    static int Read(VfsNode* node, void* buffer, uint32 size, uint32 offset);
    static int Write(VfsNode* node, const void* buffer, uint32 size, uint32 offset);
    static int Seek(VfsNode* node, int32 offset, int origin);
    static int Stat(VfsNode* node, FileStat* stat);
    static int Readdir(VfsNode* node, uint32 index, DirEntry* entry);
    static int Create(VfsNode* node, const char* name, uint8 attributes);
    static int Delete(VfsNode* node);
};

#endif