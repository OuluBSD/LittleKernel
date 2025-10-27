#ifndef _Kernel_BootDrives_h_
#define _Kernel_BootDrives_h_

#include "Common.h"
#include "Defs.h"
#include "Vfs.h"
#include "Fat32Driver.h"
#include "DriverFramework.h"

// Boot drive implementations - B: and C: drives

// EFI partition structure (for B: drive)
struct EfiPartition {
    uint8_t signature[8];          // "EFI PART" signature
    uint32_t revision;             // Revision of the EFI specification
    uint32_t header_size;          // Size of the header in bytes
    uint32_t header_crc32;         // CRC32 of the header
    uint32_t reserved1;            // Must be 0
    uint64_t header_lba;           // Current LBA of this header
    uint64_t backup_lba;           // Backup header LBA
    uint64_t first_usable_lba;     // First usable LBA for partitions
    uint64_t last_usable_lba;      // Last usable LBA for partitions
    uint8_t disk_guid[16];         // Disk GUID
    uint64_t partition_entry_lba;  // Starting LBA of partition entries
    uint32_t num_partition_entries; // Number of partition entries
    uint32_t sizeof_partition_entry; // Size of a partition entry
    uint32_t partition_entry_crc32; // CRC32 of partition entries
    uint8_t reserved2[420];        // Reserved space
};

// MBR (Master Boot Record) structure for C: drive (traditional BIOS boot)
struct MbrSector {
    uint8_t bootstrap_code[440];   // Bootstrap code area
    uint32_t disk_signature;       // Disk signature
    uint16_t reserved;             // Usually 0x0000
    struct {
        uint8_t status;            // 0x80 = active, 0x00 = inactive
        uint8_t chs_start[3];      // CHS address of first absolute sector
        uint8_t type;              // Partition type
        uint8_t chs_end[3];        // CHS address of last absolute sector
        uint32_t lba_start;        // LBA of first absolute sector
        uint32_t sectors_count;    // Number of sectors in partition
    } partition_table[4];          // 4 partition entries
    uint16_t signature;            // 0xAA55 signature
};

// Pagefile structure for C: drive swap functionality
struct PagefileHeader {
    uint32_t signature;            // Pagefile signature
    uint32_t pagesize;             // Page size in bytes
    uint32_t total_pages;          // Total number of pages
    uint32_t free_pages;           // Number of free pages
    uint32_t reserved;             // Reserved field
    uint64_t used_bitmap_offset;   // Offset to used pages bitmap
    uint64_t data_offset;          // Offset to actual page data
};

// Boot drive manager class
class BootDriveManager {
private:
    Fat32Driver* b_drive_driver;    // EFI partition driver (B: drive)
    Fat32Driver* c_drive_driver;    // Primary storage driver (C: drive)
    Device* efi_device;             // Device for EFI partition
    Device* primary_device;         // Device for primary storage
    PagefileHeader pagefile_info;   // Pagefile information for C: drive
    uint8_t* pagefile_bitmap;       // Bitmap for pagefile allocation
    uint32_t pagefile_bitmap_size;  // Size of the pagefile bitmap
    bool c_drive_swap_enabled;      // Whether swap functionality is enabled on C: drive
    
public:
    BootDriveManager();
    ~BootDriveManager();
    
    // Initialize boot drives
    bool Initialize();
    
    // B: Drive - EFI partition system
    bool InitializeEfiPartition(Device* device);
    bool MountEfiPartition(const char* mount_point = "/B");
    bool WriteEfiBootData(const char* filename, const void* data, uint32_t size);
    bool ReadEfiBootData(const char* filename, void* buffer, uint32_t size);
    bool UpdateEfiBootEntries();
    const EfiPartition* GetEfiPartitionInfo();
    
    // C: Drive - Primary storage with pagefile.sys functionality
    bool InitializePrimaryDrive(Device* device);
    bool MountPrimaryDrive(const char* mount_point = "/C");
    bool CreatePagefile(uint32_t size_mb);  // Create pagefile.sys for swap
    bool EnableSwap();
    bool DisableSwap();
    bool IsSwapEnabled();
    bool AllocatePagefileSpace(uint32_t num_pages, uint32_t* page_indices);
    bool FreePagefilePages(uint32_t* page_indices, uint32_t num_pages);
    bool ReadPagefilePage(uint32_t page_index, void* buffer);
    bool WritePagefilePage(uint32_t page_index, const void* buffer);
    const PagefileHeader* GetPagefileInfo();
    
    // Drive management
    bool IsEfiPartitionReady();
    bool IsPrimaryDriveReady();
    Fat32Driver* GetEfiDriver() { return b_drive_driver; }
    Fat32Driver* GetPrimaryDriver() { return c_drive_driver; }
    
    // System integration
    bool RegisterWithVfs();         // Register drives with VFS
    bool SetupDriveMappings();      // Set up registry mappings for B: and C: drives

private:
    // Internal helper functions
    bool InitializePagefileBitmap(uint32_t total_pages);
    bool FindSwapSpaceOnDevice(Device* device, uint32_t required_size);
    bool UpdateRegistryMappings();
};

// Global boot drive manager instance
extern BootDriveManager* g_boot_drive_manager;

// Initialize boot drives (B: and C: drives)
bool InitializeBootDrives();

// Utility functions for working with pagefile
bool CreateSwapFile(uint32_t size_mb);
bool EnableVirtualMemory();

#endif