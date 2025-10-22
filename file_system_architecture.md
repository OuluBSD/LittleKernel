# File System Architecture with FAT32 and DOS Compatibility

## Overview
This document outlines the design of the file system architecture that provides FAT32 support with full DOS compatibility. The system will follow Ultimate++ and Windows naming conventions while maintaining compatibility with both legacy DOS applications and modern OS concepts.

## Naming Conventions
- Class names use PascalCase (e.g., FileSystemManager, Fat32Driver)
- Function names follow Windows/Ultimate++ style (e.g., CreateFile, ReadFile)
- Variable names use lowercase with underscores (e.g., file_name, file_size)
- Base classes use Base suffix (e.g., FileSystemBase, VolumeBase)
- Macros use UPPER_CASE (e.g., MAX_PATH, SECTOR_SIZE)

## Architecture Overview

### Core Components
```
+----------------------+
|   Virtual File System|
|   (VFS Layer)        |
+----------------------+
|   File System Drivers| 
|   (FAT32, etc.)      |
+----------------------+
|   Volume Manager     |
|   (Mount Points)     |
+----------------------+
|   Cache Manager      |
|   (Buffer Caching)   |
+----------------------+
|   Hardware Layer     |
|   (Block Devices)    |
+----------------------+
```

## 1. Virtual File System (VFS) Layer

### Base File System Classes
```cpp
class FileSystemBase {
protected:
    char fs_name[32];                  // Name of file system (e.g., "FAT32")
    uint32 fs_version;                 // Version of file system
    FileSystemFlags flags;             // File system flags
    
public:
    virtual bool InitializeVolume(void* device_handle) = 0;
    virtual FileNode* OpenFile(const char* path, FileAccess access) = 0;
    virtual DirectoryNode* OpenDirectory(const char* path) = 0;
    virtual bool CreateFile(const char* path, FileAttributes attributes) = 0;
    virtual bool CreateDirectory(const char* path, FileAttributes attributes) = 0;
    virtual bool DeleteFile(const char* path) = 0;
    virtual bool DeleteDirectory(const char* path, bool recursive) = 0;
    virtual bool Rename(const char* old_path, const char* new_path) = 0;
    virtual uint32 GetFileSize(const char* path) = 0;
    virtual bool GetFileInfo(const char* path, FileInfo* info) = 0;
    virtual ~FileSystemBase();
};

class FileNode {
public:
    char name[260];                    // File name (with DOS compatibility)
    char full_path[1024];              // Full path to file
    uint32 size;                       // File size in bytes
    FileAttributes attributes;         // File attributes
    uint32 creation_time;              // Creation timestamp
    uint32 last_access_time;          // Last access timestamp
    uint32 last_write_time;           // Last write timestamp
    FileSystemBase* fs;               // Pointer to parent file system
    void* fs_specific_data;           // File system specific data
    uint32 ref_count;                 // Reference count
    FileNode* next, *prev;            // Linked list pointers
    
    virtual uint32 Read(void* buffer, uint32 bytes_to_read, uint32 offset = 0) = 0;
    virtual uint32 Write(const void* buffer, uint32 bytes_to_write, uint32 offset = 0) = 0;
    virtual bool SetSize(uint32 new_size) = 0;
    virtual bool Flush() = 0;
    virtual bool Close() = 0;
    virtual ~FileNode();
};

class DirectoryNode {
public:
    char name[260];                    // Directory name
    char full_path[1024];              // Full path to directory
    FileAttributes attributes;         // Directory attributes
    uint32 creation_time;              // Creation timestamp
    uint32 last_access_time;          // Last access timestamp
    uint32 last_write_time;           // Last write timestamp
    FileSystemBase* fs;               // Pointer to parent file system
    void* fs_specific_data;           // File system specific data
    uint32 entry_count;               // Number of entries in directory
    uint32 ref_count;                 // Reference count
    
    virtual DirectoryEntry* ReadEntry(uint32 index) = 0;
    virtual DirectoryEntry* FindEntry(const char* name) = 0;
    virtual Vector<DirectoryEntry*> GetAllEntries() = 0;
    virtual bool Close() = 0;
    virtual ~DirectoryNode();
};

struct DirectoryEntry {
    char name[260];                    // Entry name
    uint32 size;                       // Size (0 for directories)
    FileAttributes attributes;         // Entry attributes
    uint32 creation_time;              // Creation timestamp
    uint32 last_access_time;          // Last access timestamp
    uint32 last_write_time;           // Last write timestamp
    bool is_directory;                // True if directory, false if file
};
```

### File System Types
```cpp
enum FileSystemType {
    FS_FAT12 = 0,                     // FAT12 file system
    FS_FAT16 = 1,                     // FAT16 file system
    FS_FAT32 = 2,                     // FAT32 file system
    FS_NTFS_READONLY = 3,             // NTFS read-only
    FS_ISO9660 = 4,                   // ISO 9660 (CD-ROM)
    FS_EXT2 = 5,                      // EXT2 (read-write)
    FS_RAMFS = 6                      // RAM-based file system
};

enum FileAccess {
    FILE_READ = 0x01,
    FILE_WRITE = 0x02,
    FILE_EXECUTE = 0x04,
    FILE_APPEND = 0x08,
    FILE_DELETE = 0x10
};

enum FileAttributes {
    ATTR_READ_ONLY = 0x01,
    ATTR_HIDDEN = 0x02,
    ATTR_SYSTEM = 0x04,
    ATTR_VOLUME_ID = 0x08,
    ATTR_DIRECTORY = 0x10,
    ATTR_ARCHIVE = 0x20,
    ATTR_DEVICE = 0x40
};

enum FileSystemFlags {
    FS_CASE_SENSITIVE = 0x01,
    FS_CASE_PRESERVED = 0x02,
    FS_UNICODE_SUPPORT = 0x04,
    FS_JOURNALED = 0x08,
    FS_COMPRESSION = 0x10,
    FS_ENCRYPTION = 0x20
};
```

## 2. FAT32 Implementation

### FAT32 Driver Core
```cpp
class Fat32Driver : public FileSystemBase {
private:
    Fat32BootSector boot_sector;       // Boot sector information
    void* device_handle;               // Handle to block device
    uint32 sectors_per_cluster;        // Number of sectors per cluster
    uint32 bytes_per_sector;           // Number of bytes per sector
    uint32 first_data_sector;          // First sector of data area
    uint32 root_cluster;               // Root directory cluster
    uint32 total_clusters;             // Total number of clusters
    uint32 fat_start_sector;           // Start sector of FAT
    uint32 fat_size_sectors;           // Size of FAT in sectors
    uint32 number_of_fats;             // Number of FAT copies
    uint8* fat_cache;                 // Cached FAT entries
    uint32 cache_dirty;               // Dirty flag for FAT cache
    
public:
    Fat32Driver();
    bool InitializeVolume(void* device_handle) override;
    FileNode* OpenFile(const char* path, FileAccess access) override;
    DirectoryNode* OpenDirectory(const char* path) override;
    bool CreateFile(const char* path, FileAttributes attributes) override;
    bool CreateDirectory(const char* path, FileAttributes attributes) override;
    bool DeleteFile(const char* path) override;
    bool DeleteDirectory(const char* path, bool recursive) override;
    bool Rename(const char* old_path, const char* new_path) override;
    uint32 GetFileSize(const char* path) override;
    bool GetFileInfo(const char* path, FileInfo* info) override;
    
private:
    uint32 ClusterToSector(uint32 cluster);
    uint32 GetNextCluster(uint32 current_cluster);
    uint32 SetNextCluster(uint32 current_cluster, uint32 next_cluster);
    uint32 AllocateCluster();
    bool FreeClusterChain(uint32 start_cluster);
    bool FlushFatCache();
    uint32 GetFileStartCluster(const char* path);
    Fat32DirectoryEntry* FindDirectoryEntry(const char* path, char* filename);
    bool WriteBootSector();
    bool ReadBootSector();
};

struct Fat32BootSector {
    uint8 jump_instruction[3];         // Jump instruction
    char oem_name[8];                  // OEM name
    uint16 bytes_per_sector;           // Bytes per sector
    uint8 sectors_per_cluster;         // Sectors per cluster
    uint16 reserved_sectors;           // Reserved sectors count
    uint8 number_of_fats;              // Number of FATs
    uint16 root_entries_count;         // Root directory entry count
    uint16 total_sectors_16;           // Total sectors (16-bit)
    uint8 media_descriptor;            // Media descriptor
    uint16 fat_size_16;                // FAT size (16-bit)
    uint16 sectors_per_track;          // Sectors per track
    uint16 heads;                      // Number of heads
    uint32 hidden_sectors;             // Hidden sectors
    uint32 total_sectors_32;           // Total sectors (32-bit)
    
    // FAT32 Extended
    uint32 fat_size_32;                // FAT size (32-bit)
    uint16 extended_flags;             // Extended flags
    uint16 fs_version;                 // File system version
    uint32 root_cluster;               // Root directory start cluster
    uint16 fs_info_sector;             // File system info sector
    uint16 backup_boot_sector;         // Backup boot sector
    uint8 reserved[12];                // Reserved
    uint8 drive_number;                // Drive number
    uint8 reserved1;                   // Reserved
    uint8 boot_signature;              // Boot signature
    uint32 volume_serial;              // Volume serial number
    char volume_label[11];             // Volume label
    char fs_type[8];                   // File system type
} __attribute__((packed));

struct Fat32DirectoryEntry {
    char name[11];                     // File name (8.3 format)
    uint8 attributes;                  // File attributes
    uint8 nt_reserved;                 // NT reserved
    uint8 creation_time_tenth;         // Creation time (tenth of second)
    uint16 creation_time;              // Creation time
    uint16 creation_date;              // Creation date
    uint16 last_access_date;           // Last access date
    uint16 first_cluster_high;         // First cluster (high 16 bits)
    uint16 last_write_time;            // Last write time
    uint16 last_write_date;            // Last write date
    uint16 first_cluster_low;          // First cluster (low 16 bits)
    uint32 file_size;                  // File size
} __attribute__((packed));

struct Fat32LongNameEntry {
    uint8 order;                       // Order of entry
    uint16 name1[5];                   // First 5 characters
    uint8 attributes;                  // Always 0x0F
    uint8 type;                        // Always 0x00
    uint8 checksum;                    // Checksum of 8.3 name
    uint16 name2[6];                   // Next 6 characters
    uint16 first_cluster;              // Always 0x0000
    uint16 name3[2];                   // Last 2 characters
} __attribute__((packed));
```

### FAT32 File and Directory Implementations
```cpp
class Fat32FileNode : public FileNode {
private:
    uint32 start_cluster;              // First cluster of file
    uint32 current_cluster;            // Current cluster during access
    uint32 cluster_offset;             // Offset within current cluster
    Fat32Driver* fat_driver;           // Pointer to parent FAT driver
    
public:
    Fat32FileNode(Fat32Driver* driver, const char* path);
    uint32 Read(void* buffer, uint32 bytes_to_read, uint32 offset = 0) override;
    uint32 Write(const void* buffer, uint32 bytes_to_write, uint32 offset = 0) override;
    bool SetSize(uint32 new_size) override;
    bool Flush() override;
    bool Close() override;
    
private:
    uint32 ReadCluster(uint32 cluster, void* buffer);
    uint32 WriteCluster(uint32 cluster, const void* buffer);
    uint32 GetClusterAtOffset(uint32 offset);
    bool ExtendFileSize(uint32 new_size);
};

class Fat32DirectoryNode : public DirectoryNode {
private:
    uint32 start_cluster;              // First cluster of directory
    Fat32Driver* fat_driver;           // Pointer to parent FAT driver
    
public:
    Fat32DirectoryNode(Fat32Driver* driver, const char* path);
    DirectoryEntry* ReadEntry(uint32 index) override;
    DirectoryEntry* FindEntry(const char* name) override;
    Vector<DirectoryEntry*> GetAllEntries() override;
    bool Close() override;
    
private:
    bool ReadDirectoryCluster(uint32 cluster, Fat32DirectoryEntry* entries, 
                            uint32 max_entries);
    DirectoryEntry* ConvertFatEntry(const Fat32DirectoryEntry* fat_entry);
};
```

## 3. Volume and Mount Management

### Volume Manager
```cpp
class VolumeManager {
private:
    Vector<MountedVolume> mounted_volumes; // List of mounted volumes
    Vector<MountPoint> mount_points;    // Mount point mappings
    spinlock lock;                      // For thread-safe access
    
public:
    bool MountVolume(const char* device_path, const char* mount_point, 
                    FileSystemType fs_type);
    bool UnmountVolume(const char* mount_point);
    bool RegisterFileSystem(FileSystemType type, FileSystemBase* fs_driver);
    MountedVolume* FindVolume(const char* path);
    bool EnumerateVolumes();
    FileSystemBase* GetFileSystemForPath(const char* path);
    const char* GetMountPointForDevice(const char* device_path);
};

struct MountedVolume {
    char device_path[260];              // Path to block device
    char mount_point[260];              // Mount point (e.g., "C:")
    FileSystemType fs_type;            // Type of file system
    FileSystemBase* fs_driver;         // File system driver instance
    uint32 total_size;                 // Total size in bytes
    uint32 free_size;                  // Free space in bytes
    uint32 block_size;                 // Block size
    bool read_only;                    // Read-only flag
};

struct MountPoint {
    char mount_point[260];              // Mount point path
    char target_device[260];            // Target device path
    MountFlags flags;                  // Mount flags
};
```

## 4. DOS Compatibility Layer

### DOS File System Compatibility
```cpp
class DosCompatibilityLayer {
private:
    bool is_dos_mode;                   // Running in DOS compatibility mode
    uint8* dos_memory;                 // DOS memory space (first 1MB)
    uint32 dos_memory_size;            // Size of DOS memory
    Vector<DosFileHandle> file_handles; // DOS file handle table
    uint16 current_psp;                // Current Program Segment Prefix
    char current_directory[260];       // Current directory
    
public:
    bool InitializeDosLayer();
    uint16 DosOpenFile(const char* dos_path, uint8 access_mode);
    bool DosCloseFile(uint16 handle);
    uint32 DosReadFile(uint16 handle, void* buffer, uint32 bytes_to_read);
    uint32 DosWriteFile(uint16 handle, const void* buffer, uint32 bytes_to_write);
    uint16 DosCreateFile(const char* dos_path, uint8 attributes);
    bool DosDeleteFile(const char* dos_path);
    bool DosRenameFile(const char* old_path, const char* new_path);
    uint16 DosFindFirst(const char* path, uint8 attribute_mask, DosFindData* find_data);
    bool DosFindNext(uint16 handle, DosFindData* find_data);
    bool DosFindClose(uint16 handle);
    bool DosGetFileTime(uint16 handle, uint16* date, uint16* time);
    bool DosSetFileTime(uint16 handle, uint16 date, uint16 time);
    uint32 DosGetFileSize(uint16 handle);
    bool DosSetFileSize(uint16 handle, uint32 new_size);
    
    // DOS-style path manipulation
    bool IsValidDosPath(const char* path);
    bool ConvertDosToLongPath(const char* dos_path, char* long_path);
    bool ConvertLongToDosPath(const char* long_path, char* dos_path);
    bool GetDosShortName(const char* long_name, char* short_name);
};

struct DosFileHandle {
    uint16 handle;                     // DOS handle number
    uint32 kernel_handle;              // Kernel file handle
    uint32 file_position;              // Current file position
    uint8 access_mode;                 // Access mode (read/write)
    char original_path[260];           // Original file path
    bool is_open;                      // Is file open
};

struct DosFindData {
    uint32 size;                       // File size
    uint16 date;                       // Last write date
    uint16 time;                       // Last write time
    uint8 attributes;                  // File attributes
    char name[13];                     // File name (8.3 format)
};

// DOS attributes mapping
#define DOS_ATTR_READ_ONLY  0x01
#define DOS_ATTR_HIDDEN     0x02
#define DOS_ATTR_SYSTEM     0x04
#define DOS_ATTR_VOLUME_ID  0x08
#define DOS_ATTR_DIRECTORY  0x10
#define DOS_ATTR_ARCHIVE    0x20
```

### Windows 98 File System Compatibility
```cpp
class Win98CompatibilityLayer {
private:
    bool is_win98_compat;              // Windows 98 compatibility mode
    Vector<Win98VfsNode> vfs_nodes;    // Virtual File System nodes
    char current_directory[260];       // Current directory
    char current_drive;                // Current drive
    uint32 extended_attributes;        // Extended attributes support
    
public:
    bool InitializeWin98Layer();
    uint32 Win98CreateFile(const char* filename, uint32 access, 
                          uint32 share_mode, uint32 creation_disposition,
                          uint32 flags_and_attributes);
    uint32 Win98ReadFile(uint32 handle, void* buffer, uint32 bytes_to_read,
                        uint32* bytes_read, void* overlapped);
    uint32 Win98WriteFile(uint32 handle, const void* buffer, 
                         uint32 bytes_to_write, uint32* bytes_written,
                         void* overlapped);
    bool Win98CloseHandle(uint32 handle);
    uint32 Win98SetFilePointer(uint32 handle, int32 distance_to_move,
                              int32* distance_to_move_high, uint32 move_method);
    bool Win98CreateDirectory(const char* path, void* security_attributes);
    bool Win98RemoveDirectory(const char* path);
    uint32 Win98GetFileAttributes(const char* path);
    bool Win98SetFileAttributes(const char* path, uint32 attributes);
    bool Win98GetCurrentDirectory(uint32 buffer_length, char* buffer);
    bool Win98SetCurrentDirectory(const char* path);
    
    // Long file name support
    bool IsLfnSupported();
    bool EnableLfnSupport();
    bool DisableLfnSupport();
    
    // Compatibility APIs
    bool Win98Int21Handler(uint8 ah, uint8 al, uint16* registers);
};
```

## 5. Cache Management

### Block and Directory Caching
```cpp
class CacheManager {
private:
    CacheBlock* block_cache;           // Cached data blocks
    DirectoryCacheEntry* dir_cache;    // Cached directory entries
    uint32 block_cache_size;           // Number of cached blocks
    uint32 dir_cache_size;             // Number of cached directory entries
    spinlock lock;                     // For thread-safe access
    
public:
    CacheManager(uint32 block_cache_entries, uint32 dir_cache_entries);
    void* ReadBlock(void* device, uint32 sector, uint32 sector_count);
    bool WriteBlock(void* device, uint32 sector, const void* data, uint32 sector_count);
    bool InvalidateBlock(void* device, uint32 sector);
    bool InvalidateAllBlocks(void* device);
    
    DirectoryEntry* ReadDirectory(const char* path);
    bool WriteDirectory(const char* path, DirectoryEntry* entries, uint32 count);
    bool InvalidateDirectory(const char* path);
    
private:
    CacheBlock* FindBlockInCache(void* device, uint32 sector);
    CacheBlock* AllocateCacheBlock(void* device, uint32 sector);
    DirectoryCacheEntry* FindDirInCache(const char* path);
    DirectoryCacheEntry* AllocateDirCacheEntry(const char* path);
};

struct CacheBlock {
    void* device;                      // Device this block belongs to
    uint32 sector;                     // Sector number
    void* data;                        // Cached data
    bool dirty;                        // True if data has been modified
    bool valid;                        // True if data is valid
    uint32 access_count;               // Number of accesses
    uint32 timestamp;                  // Last access time
};

struct DirectoryCacheEntry {
    char path[260];                    // Directory path
    DirectoryEntry* entries;           // Cached directory entries
    uint32 entry_count;                // Number of cached entries
    bool dirty;                        // True if entries have been modified
    bool valid;                        // True if entries are valid
    uint32 access_count;               // Number of accesses
    uint32 timestamp;                  // Last access time
};
```

## 6. System Calls Interface

### File System System Calls
```cpp
// Windows-style function names for system calls
uint32 SyscallCreateFile(const char* filename, uint32 access, 
                        uint32 share_mode, uint32 creation_disposition,
                        uint32 flags_and_attributes, uint32 template_file);
uint32 SyscallReadFile(uint32 handle, void* buffer, uint32 bytes_to_read,
                      uint32* bytes_read, void* overlapped);
uint32 SyscallWriteFile(uint32 handle, const void* buffer, 
                       uint32 bytes_to_write, uint32* bytes_written,
                       void* overlapped);
uint32 SyscallCloseHandle(uint32 handle);
uint32 SyscallSetFilePointer(uint32 handle, int32 distance_to_move,
                            int32* distance_to_move_high, uint32 move_method);
bool SyscallCreateDirectory(const char* path, void* security_attributes);
bool SyscallRemoveDirectory(const char* path);
uint32 SyscallGetFileAttributes(const char* path);
bool SyscallSetFileAttributes(const char* path, uint32 attributes);
bool SyscallGetCurrentDirectory(uint32 buffer_length, char* buffer);
bool SyscallSetCurrentDirectory(const char* path);

// For Linux compatibility layer (internal naming)
int32 SyscallOpen(const char* pathname, int32 flags, uint32 mode);
int32 SyscallClose(uint32 fd);
int32 SyscallRead(uint32 fd, void* buf, uint32 count);
int32 SyscallWrite(uint32 fd, const void* buf, uint32 count);
int32 SyscallUnlink(const char* pathname);
int32 SyscallMkdir(const char* pathname, uint32 mode);
int32 SyscallRmdir(const char* pathname);
int32 SyscallChdir(const char* path);
int32 SyscallGetcwd(char* buf, uint32 size);
int32 SyscallStat(const char* pathname, struct stat* statbuf);
int32 SyscallFstat(uint32 fd, struct stat* statbuf);
int32 SyscallLseek(uint32 fd, int32 offset, int32 whence);