#include "Kernel.h"
#include "BootDrives.h"
#include "Logging.h"
#include "Vfs.h"
#include "Registry.h"

// Global boot drive manager instance
BootDriveManager* g_boot_drive_manager = nullptr;

BootDriveManager::BootDriveManager() {
    b_drive_driver = nullptr;
    c_drive_driver = nullptr;
    efi_device = nullptr;
    primary_device = nullptr;
    memset(&pagefile_info, 0, sizeof(PagefileHeader));
    pagefile_bitmap = nullptr;
    pagefile_bitmap_size = 0;
    c_drive_swap_enabled = false;
}

BootDriveManager::~BootDriveManager() {
    // Clean up resources
    if (pagefile_bitmap) {
        kfree(pagefile_bitmap);
        pagefile_bitmap = nullptr;
    }
    
    if (b_drive_driver) {
        delete b_drive_driver;
        b_drive_driver = nullptr;
    }
    
    if (c_drive_driver) {
        delete c_drive_driver;
        c_drive_driver = nullptr;
    }
}

bool BootDriveManager::Initialize() {
    LOG("Initializing boot drives (B: and C: drives)");
    
    // For now, we'll simulate initialization
    // In a real system, we would detect appropriate devices
    
    // Initialize C: drive as primary storage
    // This would typically be the main storage device
    LOG("Boot drives initialized successfully");
    return true;
}

bool BootDriveManager::InitializeEfiPartition(Device* device) {
    if (!device) {
        return false;
    }
    
    efi_device = device;
    
    // Create FAT32 driver for EFI partition (FAT32 is typically used for EFI)
    b_drive_driver = new Fat32Driver();
    if (!b_drive_driver) {
        LOG("Failed to create EFI partition driver");
        return false;
    }
    
    if (!b_drive_driver->Initialize(device)) {
        LOG("Failed to initialize EFI partition driver");
        delete b_drive_driver;
        b_drive_driver = nullptr;
        return false;
    }
    
    LOG("EFI partition (B: drive) initialized successfully");
    return true;
}

bool BootDriveManager::MountEfiPartition(const char* mount_point) {
    if (!b_drive_driver || !g_vfs) {
        return false;
    }
    
    // Mount the EFI partition using the VFS
    bool result = g_vfs->Mount(mount_point, efi_device, 0x54414633, "FAT32"); // "FAT3"
    if (result) {
        LOG("EFI partition (B: drive) mounted at " << mount_point);
    } else {
        LOG("Failed to mount EFI partition (B: drive) at " << mount_point);
    }
    
    return result;
}

bool BootDriveManager::WriteEfiBootData(const char* filename, const void* data, uint32_t size) {
    if (!b_drive_driver || !filename || !data || size == 0) {
        return false;
    }
    
    // In a real implementation, this would write to the EFI partition
    // For now, we'll log the operation
    LOG("Writing " << size << " bytes to EFI boot file: " << filename);
    return true;
}

bool BootDriveManager::ReadEfiBootData(const char* filename, void* buffer, uint32_t size) {
    if (!b_drive_driver || !filename || !buffer || size == 0) {
        return false;
    }
    
    // In a real implementation, this would read from the EFI partition
    // For now, we'll log the operation
    LOG("Reading " << size << " bytes from EFI boot file: " << filename);
    return true;
}

bool BootDriveManager::UpdateEfiBootEntries() {
    if (!b_drive_driver) {
        return false;
    }
    
    // Update EFI boot entries
    // This would involve writing to the EFI system partition
    LOG("Updating EFI boot entries");
    return true;
}

const EfiPartition* BootDriveManager::GetEfiPartitionInfo() {
    // This would return actual partition information
    // For now, returning nullptr as a placeholder
    static EfiPartition dummy_partition;
    return &dummy_partition;
}

bool BootDriveManager::InitializePrimaryDrive(Device* device) {
    if (!device) {
        return false;
    }
    
    primary_device = device;
    
    // Create FAT32 driver for primary partition (C: drive)
    c_drive_driver = new Fat32Driver();
    if (!c_drive_driver) {
        LOG("Failed to create C: drive driver");
        return false;
    }
    
    if (!c_drive_driver->Initialize(device)) {
        LOG("Failed to initialize C: drive driver");
        delete c_drive_driver;
        c_drive_driver = nullptr;
        return false;
    }
    
    LOG("C: drive (primary storage) initialized successfully");
    return true;
}

bool BootDriveManager::MountPrimaryDrive(const char* mount_point) {
    if (!c_drive_driver || !g_vfs) {
        return false;
    }
    
    // Mount the primary drive using the VFS
    bool result = g_vfs->Mount(mount_point, primary_device, 0x54414633, "FAT32"); // "FAT3"
    if (result) {
        LOG("C: drive (primary storage) mounted at " << mount_point);
    } else {
        LOG("Failed to mount C: drive (primary storage) at " << mount_point);
    }
    
    return result;
}

bool BootDriveManager::CreatePagefile(uint32_t size_mb) {
    if (size_mb == 0) {
        return false;
    }
    
    uint32_t size_bytes = size_mb * 1024 * 1024;
    uint32_t page_size = 4096; // Standard 4KB page size
    uint32_t total_pages = size_bytes / page_size;
    
    // Initialize pagefile header
    pagefile_info.signature = 0x454C4946; // "FILE" in hex
    pagefile_info.pagesize = page_size;
    pagefile_info.total_pages = total_pages;
    pagefile_info.free_pages = total_pages;
    
    // Allocate bitmap for page tracking
    uint32_t bitmap_size = (total_pages + 31) / 32; // Number of 32-bit integers needed
    pagefile_bitmap = (uint8_t*)kmalloc(bitmap_size * 4);
    if (!pagefile_bitmap) {
        LOG("Failed to allocate pagefile bitmap");
        return false;
    }
    
    // Initialize bitmap (all pages free initially)
    memset(pagefile_bitmap, 0, bitmap_size * 4);
    pagefile_bitmap_size = bitmap_size;
    
    // In a real implementation, we would create the actual pagefile on disk
    LOG("Created pagefile of size " << size_mb << " MB (" << total_pages << " pages)");
    return true;
}

bool BootDriveManager::EnableSwap() {
    if (!c_drive_swap_enabled && pagefile_bitmap) {
        c_drive_swap_enabled = true;
        LOG("Virtual memory (swap) enabled");
        return true;
    }
    return false;
}

bool BootDriveManager::DisableSwap() {
    if (c_drive_swap_enabled) {
        c_drive_swap_enabled = false;
        LOG("Virtual memory (swap) disabled");
        return true;
    }
    return false;
}

bool BootDriveManager::IsSwapEnabled() {
    return c_drive_swap_enabled;
}

bool BootDriveManager::AllocatePagefileSpace(uint32_t num_pages, uint32_t* page_indices) {
    if (!pagefile_bitmap || !page_indices || num_pages == 0) {
        return false;
    }
    
    uint32_t allocated_count = 0;
    uint32_t total_pages = pagefile_info.total_pages;
    
    // Find free pages in the bitmap
    for (uint32_t bit_idx = 0; bit_idx < total_pages && allocated_count < num_pages; bit_idx++) {
        uint32_t word_idx = bit_idx / 32;
        uint32_t bit_pos = bit_idx % 32;
        
        if (!(pagefile_bitmap[word_idx * 4 + bit_pos / 8] & (1 << (bit_pos % 8)))) {
            // Page is free, allocate it
            pagefile_bitmap[word_idx * 4 + bit_pos / 8] |= (1 << (bit_pos % 8));
            page_indices[allocated_count] = bit_idx;
            allocated_count++;
        }
    }
    
    if (allocated_count == num_pages) {
        pagefile_info.free_pages -= num_pages;
        return true;
    } else {
        // Rollback allocated pages
        for (uint32_t i = 0; i < allocated_count; i++) {
            uint32_t word_idx = page_indices[i] / 32;
            uint32_t bit_pos = page_indices[i] % 32;
            pagefile_bitmap[word_idx * 4 + bit_pos / 8] &= ~(1 << (bit_pos % 8));
        }
        return false;
    }
}

bool BootDriveManager::FreePagefilePages(uint32_t* page_indices, uint32_t num_pages) {
    if (!pagefile_bitmap || !page_indices || num_pages == 0) {
        return false;
    }
    
    for (uint32_t i = 0; i < num_pages; i++) {
        uint32_t page_idx = page_indices[i];
        if (page_idx >= pagefile_info.total_pages) {
            continue; // Skip invalid page indices
        }
        
        uint32_t word_idx = page_idx / 32;
        uint32_t bit_pos = page_idx % 32;
        
        // Mark page as free
        pagefile_bitmap[word_idx * 4 + bit_pos / 8] &= ~(1 << (bit_pos % 8));
    }
    
    pagefile_info.free_pages += num_pages;
    return true;
}

bool BootDriveManager::ReadPagefilePage(uint32_t page_index, void* buffer) {
    if (!buffer || page_index >= pagefile_info.total_pages) {
        return false;
    }
    
    // In a real implementation, this would read from the actual pagefile on disk
    // For now, we'll just return false (not implemented)
    LOG("Reading page " << page_index << " from pagefile");
    return false;
}

bool BootDriveManager::WritePagefilePage(uint32_t page_index, const void* buffer) {
    if (!buffer || page_index >= pagefile_info.total_pages) {
        return false;
    }
    
    // In a real implementation, this would write to the actual pagefile on disk
    // For now, we'll just return false (not implemented)
    LOG("Writing page " << page_index << " to pagefile");
    return false;
}

const PagefileHeader* BootDriveManager::GetPagefileInfo() {
    return &pagefile_info;
}

bool BootDriveManager::IsEfiPartitionReady() {
    return b_drive_driver != nullptr;
}

bool BootDriveManager::IsPrimaryDriveReady() {
    return c_drive_driver != nullptr;
}

bool BootDriveManager::RegisterWithVfs() {
    // Register both drives with the VFS
    bool b_success = true; // B: drive already registered if it exists
    bool c_success = true; // C: drive already registered if it exists
    
    return b_success && c_success;
}

bool BootDriveManager::SetupDriveMappings() {
    // Set up registry mappings for B: and C: drives
    if (g_registry) {
        // Register EFI partition (B: drive) mapping
        if (IsEfiPartitionReady()) {
            RegistryWriteString("HKEY_LOCAL_MACHINE\\SYSTEM\\MountPoints", "B:", "/B", KEY_WRITE);
        }
        
        // Register primary drive (C: drive) mapping
        if (IsPrimaryDriveReady()) {
            RegistryWriteString("HKEY_LOCAL_MACHINE\\SYSTEM\\MountPoints", "C:", "/C", KEY_WRITE);
        }
        
        LOG("Drive letter mappings registered in registry");
        return true;
    }
    
    return false;
}

bool BootDriveManager::InitializePagefileBitmap(uint32_t total_pages) {
    // Calculate required bitmap size
    uint32_t bitmap_size = (total_pages + 31) / 32; // Number of 32-bit integers needed
    bitmap_size *= 4; // Convert to bytes
    
    pagefile_bitmap = (uint8_t*)kmalloc(bitmap_size);
    if (!pagefile_bitmap) {
        return false;
    }
    
    // Initialize bitmap (all pages free initially)
    memset(pagefile_bitmap, 0, bitmap_size);
    pagefile_bitmap_size = bitmap_size / 4; // Store as number of 32-bit words
    return true;
}

bool BootDriveManager::FindSwapSpaceOnDevice(Device* device, uint32_t required_size) {
    // In a real implementation, this would query the device for available space
    // For now, we'll just return true as a placeholder
    LOG("Looking for " << required_size << " bytes of swap space on device");
    return true;
}

bool BootDriveManager::UpdateRegistryMappings() {
    // Update registry with drive mappings
    return SetupDriveMappings();
}

bool InitializeBootDrives() {
    if (!g_boot_drive_manager) {
        g_boot_drive_manager = new BootDriveManager();
        if (!g_boot_drive_manager) {
            LOG("Failed to create boot drive manager instance");
            return false;
        }
        
        if (!g_boot_drive_manager->Initialize()) {
            LOG("Failed to initialize boot drive manager");
            delete g_boot_drive_manager;
            g_boot_drive_manager = nullptr;
            return false;
        }
        
        LOG("Boot drive manager initialized successfully");
    }
    
    return true;
}

bool CreateSwapFile(uint32_t size_mb) {
    if (!g_boot_drive_manager) {
        return false;
    }
    
    return g_boot_drive_manager->CreatePagefile(size_mb);
}

bool EnableVirtualMemory() {
    if (!g_boot_drive_manager) {
        return false;
    }
    
    return g_boot_drive_manager->EnableSwap();
}