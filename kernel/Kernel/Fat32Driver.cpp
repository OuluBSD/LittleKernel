#include "Kernel.h"
#include "Fat32Driver.h"
#include "Vfs.h"
#include "Logging.h"
#include "DriverFramework.h"

Fat32Driver::Fat32Driver() {
    memset(&fs_info, 0, sizeof(fs_info));
    root_node = nullptr;
    driver_lock.Initialize();
}

Fat32Driver::~Fat32Driver() {
    // Clean up resources
    if (root_node) {
        // Note: We don't delete the root node here as it's managed by VFS
        root_node = nullptr;
    }
}

bool Fat32Driver::Initialize(Device* device) {
    if (!device) {
        LOG("Invalid device for FAT32 initialization");
        return false;
    }
    
    LOG("Initializing FAT32 filesystem on device");
    
    // Read the boot sector
    uint8 boot_sector[FAT32_SECTOR_SIZE];
    if (!ReadSector(0, boot_sector)) {
        LOG("Failed to read boot sector");
        return false;
    }
    
    // Copy BPB information
    memcpy(&fs_info.bpb, boot_sector, sizeof(Fat32Bpb));
    
    // Validate FAT32 signature
    if (fs_info.bpb.boot_signature != 0x29 || 
        strncmp(fs_info.bpb.fs_type, "FAT32", 5) != 0) {
        LOG("Invalid FAT32 filesystem signature");
        return false;
    }
    
    // Calculate filesystem layout
    fs_info.fat_start_sector = fs_info.bpb.reserved_sectors;
    fs_info.root_dir_start = fs_info.bpb.root_cluster;
    fs_info.sectors_per_cluster = fs_info.bpb.sectors_per_cluster;
    fs_info.bytes_per_cluster = fs_info.bpb.bytes_per_sector * fs_info.bpb.sectors_per_cluster;
    fs_info.data_start_sector = fs_info.fat_start_sector + 
                               (fs_info.bpb.num_fats * fs_info.bpb.sectors_per_fat);
    
    // Calculate total clusters
    uint32 total_sectors = fs_info.bpb.total_sectors_long ? 
                            fs_info.bpb.total_sectors_long : fs_info.bpb.total_sectors_short;
    fs_info.total_clusters = (total_sectors - fs_info.data_start_sector) / fs_info.bpb.sectors_per_cluster;
    fs_info.first_data_cluster = 2; // Clusters 0 and 1 are reserved
    
    fs_info.device = device;
    
    LOG("FAT32 filesystem initialized:");
    LOG("  Bytes per sector: " << fs_info.bpb.bytes_per_sector);
    LOG("  Sectors per cluster: " << (uint32)fs_info.bpb.sectors_per_cluster);
    LOG("  Number of FATs: " << (uint32)fs_info.bpb.num_fats);
    LOG("  Sectors per FAT: " << fs_info.bpb.sectors_per_fat);
    LOG("  Root cluster: " << fs_info.bpb.root_cluster);
    LOG("  Total sectors: " << total_sectors);
    LOG("  Total clusters: " << fs_info.total_clusters);
    
    // Create the root VFS node
    root_node = g_vfs->CreateVfsNode("/", nullptr);
    if (!root_node) {
        LOG("Failed to create FAT32 root VFS node");
        return false;
    }
    
    strcpy_safe(root_node->full_path, "/", sizeof(root_node->full_path));
    root_node->attributes = ATTR_DIRECTORY;
    root_node->size = 0;
    root_node->fs_specific = this;    // Point to this FAT32 driver instance
    root_node->device = device;
    root_node->fs_id = 0x54414633;    // "FAT3" as uint32
    
    // Set up function pointers for VFS operations
    root_node->open = Open;
    root_node->close = Close;
    root_node->read = Read;
    root_node->write = Write;
    root_node->seek = Seek;
    root_node->stat = Stat;
    root_node->readdir = Readdir;
    root_node->create = Create;
    root_node->delete_fn = Delete;
    
    LOG("FAT32 driver initialized successfully");
    return true;
}

bool Fat32Driver::ReadSector(uint32 sector, void* buffer) {
    if (!fs_info.device || !buffer) {
        return false;
    }
    
    // Use the device's read function
    if (!driver_framework) {
        return false;
    }
    
    return driver_framework->Read(fs_info.device->id, buffer, FAT32_SECTOR_SIZE, 
                                  sector * FAT32_SECTOR_SIZE);
}

bool Fat32Driver::WriteSector(uint32 sector, const void* buffer) {
    if (!fs_info.device || !buffer) {
        return false;
    }
    
    // Use the device's write function
    if (!driver_framework) {
        return false;
    }
    
    return driver_framework->Write(fs_info.device->id, buffer, FAT32_SECTOR_SIZE, 
                                   sector * FAT32_SECTOR_SIZE);
}

bool Fat32Driver::ReadCluster(uint32 cluster, void* buffer) {
    if (cluster < fs_info.first_data_cluster || cluster >= (fs_info.first_data_cluster + fs_info.total_clusters)) {
        return false;
    }
    
    uint32 sector = ClusterToSector(cluster);
    uint8* buf = (uint8*)buffer;
    
    for (uint32 i = 0; i < fs_info.bpb.sectors_per_cluster; i++) {
        if (!ReadSector(sector + i, buf + (i * FAT32_SECTOR_SIZE))) {
            return false;
        }
    }
    
    return true;
}

bool Fat32Driver::WriteCluster(uint32 cluster, const void* buffer) {
    if (cluster < fs_info.first_data_cluster || cluster >= (fs_info.first_data_cluster + fs_info.total_clusters)) {
        return false;
    }
    
    uint32 sector = ClusterToSector(cluster);
    const uint8* buf = (const uint8*)buffer;
    
    for (uint32 i = 0; i < fs_info.bpb.sectors_per_cluster; i++) {
        if (!WriteSector(sector + i, buf + (i * FAT32_SECTOR_SIZE))) {
            return false;
        }
    }
    
    return true;
}

uint32 Fat32Driver::GetNextCluster(uint32 cluster) {
    if (!IsValidCluster(cluster)) {
        return 0;
    }
    
    return GetFatEntry(cluster);
}

uint32 Fat32Driver::AllocateCluster() {
    // Find a free cluster in the FAT
    for (uint32 i = fs_info.first_data_cluster; i < fs_info.first_data_cluster + fs_info.total_clusters; i++) {
        if (GetFatEntry(i) == 0) {
            // Found a free cluster
            SetFatEntry(i, FAT32_EOF_CLUSTER);  // Mark as end of chain for now
            return i;
        }
    }
    
    return 0;  // No free clusters
}

void Fat32Driver::FreeCluster(uint32 cluster) {
    if (IsValidCluster(cluster)) {
        SetFatEntry(cluster, 0);  // Mark as free
    }
}

bool Fat32Driver::ReadDirEntry(uint32 cluster, uint32 index, Fat32DirEntry* entry) {
    if (!entry) {
        return false;
    }
    
    // Calculate which sector and offset the entry is in
    uint32 entries_per_cluster = (fs_info.bytes_per_cluster) / sizeof(Fat32DirEntry);
    
    if (index >= entries_per_cluster) {
        // Need to follow cluster chain
        uint32 chain_index = index / entries_per_cluster;
        uint32 local_index = index % entries_per_cluster;
        
        // Follow the cluster chain
        uint32 current_cluster = cluster;
        for (uint32 i = 0; i < chain_index; i++) {
            uint32 next_cluster = GetNextCluster(current_cluster);
            if (IsEndOfChain(next_cluster)) {
                return false;  // No more clusters in chain
            }
            current_cluster = next_cluster;
        }
        
        cluster = current_cluster;
        index = local_index;
    }
    
    // Read the cluster
    uint8 cluster_data[FAT32_MAX_CLUSTER_SIZE];
    if (!ReadCluster(cluster, cluster_data)) {
        return false;
    }
    
    // Copy the directory entry
    const Fat32DirEntry* src_entry = (const Fat32DirEntry*)(cluster_data + index * sizeof(Fat32DirEntry));
    memcpy(entry, src_entry, sizeof(Fat32DirEntry));
    
    return true;
}

bool Fat32Driver::FindFile(uint32 dir_cluster, const char* name, Fat32DirEntry* entry) {
    uint32 cluster = dir_cluster;
    uint32 index = 0;
    
    while (cluster > 0 && !IsEndOfChain(cluster)) {
        Fat32DirEntry dir_entry;
        
        // Try to read the directory entry
        if (!ReadDirEntry(cluster, index, &dir_entry)) {
            // Move to next cluster in chain
            cluster = GetNextCluster(cluster);
            index = 0;
            continue;
        }
        
        // Check if this is the entry we're looking for
        // Skip deleted entries (first byte = 0xE5) and empty entries (first byte = 0x00)
        if (dir_entry.name[0] == 0x00) {
            return false;  // End of directory
        }
        
        if (dir_entry.name[0] != 0xE5 && !(dir_entry.attr & 0x0F)) {  // Skip long filename entries
            // Convert 8.3 name to compare
            char short_name[13] = {0};
            int j = 0;
            
            // Copy filename (first 8 chars)
            for (int i = 0; i < 8; i++) {
                if (dir_entry.name[i] != ' ') {
                    short_name[j++] = (dir_entry.name[i] >= 'A' && dir_entry.name[i] <= 'Z') ? 
                                      dir_entry.name[i] + 32 : dir_entry.name[i];
                }
            }
            
            // Add extension if present (after the dot)
            if (dir_entry.name[8] != ' ') {
                short_name[j++] = '.';
                for (int i = 8; i < 11; i++) {
                    if (dir_entry.name[i] != ' ') {
                        short_name[j++] = (dir_entry.name[i] >= 'A' && dir_entry.name[i] <= 'Z') ? 
                                          dir_entry.name[i] + 32 : dir_entry.name[i];
                    }
                }
            }
            
            // Convert search name to lowercase for comparison
            char search_name[13] = {0};
            strncpy(search_name, name, sizeof(search_name) - 1);
            for (j = 0; search_name[j]; j++) {
                if (search_name[j] >= 'A' && search_name[j] <= 'Z') {
                    search_name[j] += 32;
                }
            }
            
            if (strcmp(short_name, search_name) == 0) {
                // Found the file
                *entry = dir_entry;
                return true;
            }
        }
        
        index++;
        
        // If we've reached the end of the current cluster, move to the next one
        uint32 entries_per_cluster = fs_info.bytes_per_cluster / sizeof(Fat32DirEntry);
        if (index >= entries_per_cluster) {
            cluster = GetNextCluster(cluster);
            index = 0;
        }
    }
    
    return false;  // File not found
}

uint8 Fat32Driver::ConvertFat32ToVfsAttr(uint8 fat_attr) {
    uint8 vfs_attr = 0;
    
    if (fat_attr & FAT32_ATTR_READ_ONLY) vfs_attr |= ATTR_READONLY;
    if (fat_attr & FAT32_ATTR_HIDDEN) vfs_attr |= ATTR_HIDDEN;
    if (fat_attr & FAT32_ATTR_SYSTEM) vfs_attr |= ATTR_SYSTEM;
    if (fat_attr & FAT32_ATTR_DIRECTORY) vfs_attr |= ATTR_DIRECTORY;
    if (fat_attr & FAT32_ATTR_ARCHIVE) vfs_attr |= ATTR_ARCHIVE;
    
    return vfs_attr;
}

uint8 Fat32Driver::ConvertVfsToFat32Attr(uint8 vfs_attr) {
    uint8 fat_attr = 0;
    
    if (vfs_attr & ATTR_READONLY) fat_attr |= FAT32_ATTR_READ_ONLY;
    if (vfs_attr & ATTR_HIDDEN) fat_attr |= FAT32_ATTR_HIDDEN;
    if (vfs_attr & ATTR_SYSTEM) fat_attr |= FAT32_ATTR_SYSTEM;
    if (vfs_attr & ATTR_DIRECTORY) fat_attr |= FAT32_ATTR_DIRECTORY;
    if (vfs_attr & ATTR_ARCHIVE) fat_attr |= FAT32_ATTR_ARCHIVE;
    
    return fat_attr;
}

// VFS operation implementations

int Fat32Driver::Open(VfsNode* node, uint32 flags) {
    if (!node || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    Fat32Driver* driver = (Fat32Driver*)node->fs_specific;
    
    // For FAT32, we don't need to do much for open, as FAT32 is a simpler filesystem
    // The actual file operations will be handled by the VFS layer
    return VFS_SUCCESS;
}

int Fat32Driver::Close(VfsNode* node) {
    if (!node || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    // Similarly, close doesn't require special handling for FAT32
    return VFS_SUCCESS;
}

int Fat32Driver::Read(VfsNode* node, void* buffer, uint32 size, uint32 offset) {
    if (!node || !buffer || size == 0 || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    Fat32Driver* driver = (Fat32Driver*)node->fs_specific;
    
    // For now, return an error as FAT32 reading is complex and requires proper implementation
    // This is a placeholder for the actual implementation
    return VFS_ERROR;
}

int Fat32Driver::Write(VfsNode* node, const void* buffer, uint32 size, uint32 offset) {
    if (!node || !buffer || size == 0 || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    Fat32Driver* driver = (Fat32Driver*)node->fs_specific;
    
    // For now, return an error as FAT32 writing is complex and requires proper implementation
    // This is a placeholder for the actual implementation
    return VFS_ERROR;
}

int Fat32Driver::Seek(VfsNode* node, int32_t offset, int origin) {
    if (!node) {
        return VFS_ERROR;
    }
    
    // For now, return an error as seeking is handled by VFS layer
    return VFS_ERROR;
}

int Fat32Driver::Stat(VfsNode* node, FileStat* stat) {
    if (!node || !stat || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    Fat32Driver* driver = (Fat32Driver*)node->fs_specific;
    
    // Initialize stat structure
    memset(stat, 0, sizeof(FileStat));
    
    // Fill in basic information
    // In a real implementation, we would get this from the actual FAT32 directory entry
    stat->inode = node->inode;
    stat->size = node->size;
    stat->attributes = node->attributes;
    stat->access_time = 0;  // TODO: Implement time tracking
    stat->modify_time = 0;  // TODO: Implement time tracking
    stat->create_time = 0;  // TODO: Implement time tracking
    stat->mode = 0755;      // Default permissions
    stat->block_size = driver->fs_info.bytes_per_cluster;
    stat->blocks = (node->size + driver->fs_info.bytes_per_cluster - 1) / driver->fs_info.bytes_per_cluster;
    stat->owner_uid = 0;
    stat->owner_gid = 0;
    
    return VFS_SUCCESS;
}

int Fat32Driver::Readdir(VfsNode* node, uint32 index, DirEntry* entry) {
    if (!node || !entry || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    Fat32Driver* driver = (Fat32Driver*)node->fs_specific;
    
    // For a directory node, read the directory entry at the specified index
    // This is a simplified implementation - a full implementation would need to
    // properly traverse the directory's cluster chain
    
    // For now, return an error as this requires a full implementation
    return VFS_ERROR;
}

int Fat32Driver::Create(VfsNode* node, const char* name, uint8 attributes) {
    if (!node || !name || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    Fat32Driver* driver = (Fat32Driver*)node->fs_specific;
    
    // For now, return an error as creating files is complex in FAT32
    return VFS_ERROR;
}

int Fat32Driver::Delete(VfsNode* node) {
    if (!node || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    Fat32Driver* driver = (Fat32Driver*)node->fs_specific;
    
    // For now, return an error as deleting files is complex in FAT32
    return VFS_ERROR;
}

// Helper functions

uint32 Fat32Driver::SectorToByte(uint32 sector) {
    return sector * fs_info.bpb.bytes_per_sector;
}

uint32 Fat32Driver::ClusterToSector(uint32 cluster) {
    if (cluster < 2) {
        return 0;  // Reserved clusters
    }
    return fs_info.data_start_sector + ((cluster - 2) * fs_info.bpb.sectors_per_cluster);
}

uint32 Fat32Driver::GetFatEntry(uint32 cluster) {
    // Calculate which sector of the FAT contains this cluster entry
    uint32 fat_sector = fs_info.fat_start_sector + (cluster * 4) / FAT32_SECTOR_SIZE;
    uint32 entry_offset = (cluster * 4) % FAT32_SECTOR_SIZE;
    
    uint8 fat_sector_data[FAT32_SECTOR_SIZE];
    if (!ReadSector(fat_sector, fat_sector_data)) {
        return 0;
    }
    
    // FAT32 entries are 32-bit values
    return *((uint32*)(fat_sector_data + entry_offset)) & 0x0FFFFFFF;
}

void Fat32Driver::SetFatEntry(uint32 cluster, uint32 value) {
    // Calculate which sector of the FAT contains this cluster entry
    uint32 fat_sector = fs_info.fat_start_sector + (cluster * 4) / FAT32_SECTOR_SIZE;
    uint32 entry_offset = (cluster * 4) % FAT32_SECTOR_SIZE;
    
    uint8 fat_sector_data[FAT32_SECTOR_SIZE];
    if (!ReadSector(fat_sector, fat_sector_data)) {
        return;
    }
    
    // Update the entry in the in-memory copy
    uint32* entries = (uint32*)fat_sector_data;
    uint32 entry_idx = entry_offset / 4;
    entries[entry_idx] = (entries[entry_idx] & 0xF0000000) | (value & 0x0FFFFFFF);
    
    // Write the sector back to the device
    WriteSector(fat_sector, fat_sector_data);
}

uint32 Fat32Driver::GetFreeClusterCount() {
    uint32 free_count = 0;
    
    // Count free clusters (those with value 0 in the FAT)
    for (uint32 i = fs_info.first_data_cluster; i < fs_info.first_data_cluster + fs_info.total_clusters; i++) {
        if (GetFatEntry(i) == 0) {
            free_count++;
        }
    }
    
    return free_count;
}

bool Fat32Driver::IsEndOfChain(uint32 cluster) {
    return cluster >= FAT32_EOF_CLUSTER;
}

bool Fat32Driver::IsValidCluster(uint32 cluster) {
    return cluster >= fs_info.first_data_cluster && 
           cluster < (fs_info.first_data_cluster + fs_info.total_clusters);
}

bool Fat32Driver::ValidateChecksum(const char* short_name, const uint8* long_name, uint32 name_len) {
    // Calculate the checksum for the 8.3 short name
    uint8 checksum = 0;
    for (int i = 0; i < 11; i++) {
        checksum = ((checksum & 1) ? 0x80 : 0) + (checksum >> 1) + short_name[i];
    }
    
    // For a complete implementation, we'd validate against the long filename entries
    return true;  // Placeholder
}