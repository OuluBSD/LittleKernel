#ifndef _Kernel_Fat32Driver_h_
#define _Kernel_Fat32Driver_h_

#include "Common.h"
#include "Defs.h"
#include "Vfs.h"
#include "DriverFramework.h"

// FAT32 constants
#define FAT32_SIGNATURE 0x41615252
#define FAT32_SECTOR_SIZE 512
#define FAT32_MAX_CLUSTER_SIZE 4096  // 4KB
#define FAT32_RESERVED_CLUSTERS 2    // Cluster 0 and 1 are reserved
#define FAT32_EOF_CLUSTER 0x0FFFFFF8 // End of file cluster marker
#define FAT32_BAD_CLUSTER 0x0FFFFFF7 // Bad cluster marker

// FAT32 attribute flags
#define FAT32_ATTR_READ_ONLY 0x01
#define FAT32_ATTR_HIDDEN 0x02
#define FAT32_ATTR_SYSTEM 0x04
#define FAT32_ATTR_VOLUME_ID 0x08
#define FAT32_ATTR_DIRECTORY 0x10
#define FAT32_ATTR_ARCHIVE 0x20
#define FAT32_ATTR_LONG_NAME (FAT32_ATTR_READ_ONLY | FAT32_ATTR_HIDDEN | FAT32_ATTR_SYSTEM | FAT32_ATTR_VOLUME_ID)

// FAT32 BIOS Parameter Block (BPB)
struct Fat32Bpb {
    uint8_t jump[3];           // Jump instruction
    char oem_name[8];          // OEM name
    uint16_t bytes_per_sector; // Bytes per sector
    uint8_t sectors_per_cluster; // Sectors per cluster
    uint16_t reserved_sectors; // Reserved sectors count
    uint8_t num_fats;          // Number of FATs
    uint16_t root_entries;     // Number of root directory entries (0 for FAT32)
    uint16_t total_sectors_short; // Total sectors (short) - 0 for FAT32
    uint8_t media_type;        // Media type
    uint16_t sectors_per_fat_short; // Sectors per FAT (short) - 0 for FAT32
    uint16_t sectors_per_track; // Sectors per track
    uint16_t heads;            // Number of heads
    uint32_t hidden_sectors;   // Hidden sectors
    uint32_t total_sectors_long; // Total sectors (long)
    
    // FAT32 Extended BPB
    uint32_t sectors_per_fat;  // Sectors per FAT
    uint16_t extended_flags;   // Extended flags
    uint16_t fs_version;       // Filesystem version
    uint32_t root_cluster;     // Root directory cluster
    uint16_t fs_info_sector;   // Filesystem info sector
    uint16_t backup_boot_sector; // Backup boot sector
    uint8_t reserved[12];      // Reserved
    uint8_t drive_number;      // Drive number
    uint8_t reserved1;         // Reserved
    uint8_t boot_signature;    // Extended boot signature
    uint32_t volume_id;        // Volume serial number
    char volume_label[11];     // Volume label
    char fs_type[8];           // Filesystem type
} __attribute__((packed));

// FAT32 directory entry
struct Fat32DirEntry {
    char name[11];             // 8.3 filename
    uint8_t attr;              // Attributes
    uint8_t nt_res;            // Reserved for NT
    uint8_t creation_time_tenth; // Creation time (tenth of second)
    uint16_t creation_time;    // Creation time
    uint16_t creation_date;    // Creation date
    uint16_t last_access_date; // Last access date
    uint16_t first_cluster_high; // High word of first cluster
    uint16_t write_time;       // Last write time
    uint16_t write_date;       // Last write date
    uint16_t first_cluster_low; // Low word of first cluster
    uint32_t file_size;        // File size in bytes
} __attribute__((packed));

// Long filename directory entry
struct Fat32LongDirEntry {
    uint8_t order;             // Order of this entry in sequence
    uint16_t name1[5];         // First 5 characters of name
    uint8_t attr;              // Always 0x0F
    uint8_t type;              // Always 0x00
    uint8_t checksum;          // Checksum of 8.3 name
    uint16_t name2[6];         // Next 6 characters of name
    uint16_t first_cluster;    // Always 0x0000
    uint16_t name3[2];         // Last 2 characters of name
} __attribute__((packed));

// FAT32 File Handle structure
struct Fat32FileHandle {
    VfsNode* node;                // Associated VFS node
    uint32_t current_cluster;     // Current data cluster
    uint32_t cluster_offset;      // Offset within current cluster
    uint32_t logical_position;    // Logical position in file
    uint32_t flags;               // Open flags
    bool is_directory;            // Whether this is a directory
    uint32_t dir_cluster;         // For directories: current cluster being read
    uint32_t dir_offset;          // For directories: current offset in cluster
};

// FAT32 filesystem information structure
struct Fat32Info {
    Fat32Bpb bpb;                 // Boot sector
    uint32_t fat_start_sector;    // Starting sector of FAT
    uint32_t root_dir_start;      // Root directory start cluster
    uint32_t data_start_sector;   // First data sector
    uint32_t sectors_per_cluster; // Sectors per cluster
    uint32_t bytes_per_cluster;   // Bytes per cluster
    uint32_t total_clusters;      // Total number of clusters
    uint32_t first_data_cluster;  // First available data cluster
    uint32_t last_allocated_cluster; // Last allocated cluster
    Device* device;               // Associated device
};

// FAT32 driver class
class Fat32Driver {
private:
    Fat32Info fs_info;
    VfsNode* root_node;
    Spinlock driver_lock;        // Lock for thread safety

public:
    Fat32Driver();
    ~Fat32Driver();
    
    // Initialize the FAT32 filesystem on a device
    bool Initialize(Device* device);
    
    // Read a sector from the device
    bool ReadSector(uint32_t sector, void* buffer);
    
    // Write a sector to the device
    bool WriteSector(uint32_t sector, const void* buffer);
    
    // Read a cluster from the device
    bool ReadCluster(uint32_t cluster, void* buffer);
    
    // Write a cluster to the device
    bool WriteCluster(uint32_t cluster, const void* buffer);
    
    // Get the next cluster in the chain
    uint32_t GetNextCluster(uint32_t cluster);
    
    // Allocate a new cluster
    uint32_t AllocateCluster();
    
    // Free a cluster back to the filesystem
    void FreeCluster(uint32_t cluster);
    
    // Read a directory entry
    bool ReadDirEntry(uint32_t cluster, uint32_t index, Fat32DirEntry* entry);
    
    // Find a file in a directory
    bool FindFile(uint32_t dir_cluster, const char* name, Fat32DirEntry* entry);
    
    // Convert FAT32 attributes to VFS attributes
    uint8_t ConvertFat32ToVfsAttr(uint8_t fat_attr);
    
    // Convert VFS attributes to FAT32 attributes
    uint8_t ConvertVfsToFat32Attr(uint8_t vfs_attr);
    
    // Get the VFS root node for this filesystem
    VfsNode* GetRootNode() { return root_node; }

private:
    // VFS operation implementations
    static int Open(VfsNode* node, uint32_t flags);
    static int Close(VfsNode* node);
    static int Read(VfsNode* node, void* buffer, uint32_t size, uint32_t offset);
    static int Write(VfsNode* node, const void* buffer, uint32_t size, uint32_t offset);
    static int Seek(VfsNode* node, int32_t offset, int origin);
    static int Stat(VfsNode* node, FileStat* stat);
    static int Readdir(VfsNode* node, uint32_t index, DirEntry* entry);
    static int Create(VfsNode* node, const char* name, uint8_t attributes);
    static int Delete(VfsNode* node);
    
    // Helper functions
    uint32_t SectorToByte(uint32_t sector);
    uint32_t ClusterToSector(uint32_t cluster);
    uint32_t GetFatEntry(uint32_t cluster);
    void SetFatEntry(uint32_t cluster, uint32_t value);
    uint32_t GetFreeClusterCount();
    bool IsEndOfChain(uint32_t cluster);
    bool IsValidCluster(uint32_t cluster);
    bool ValidateChecksum(const char* short_name, const uint8_t* long_name, uint32_t name_len);
};

#endif