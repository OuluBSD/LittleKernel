#include "Kernel.h"
#include "Vfs.h"
#include "Logging.h"
#include "DriverFramework.h"

// Global VFS instance
Vfs* g_vfs = nullptr;

Vfs::Vfs() {
    root = nullptr;
    mount_count = 0;
    open_file_count = 0;
    vfs_lock.Initialize();
    cache_hits = 0;
    cache_misses = 0;
    
    // Initialize mount points
    for (int i = 0; i < MAX_MOUNT_POINTS; i++) {
        mount_points[i].mounted = false;
        mount_points[i].mount_path[0] = '\0';
    }
    
    // Initialize open files
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        open_files[i].is_open = false;
        open_files[i].node = nullptr;
    }
    
    // Initialize cache
    for (uint32_t i = 0; i < CACHE_SIZE; i++) {
        cache[i].valid = false;
        cache[i].data = nullptr;
        cache[i].size = 0;
        cache[i].dirty = false;
        cache[i].block_number = 0;
        cache[i].device = nullptr;
    }
}

Vfs::~Vfs() {
    // Clean up all resources
    // Note: In a real implementation, we would need to ensure no files are open
    // and properly unmount all filesystems before destroying the VFS
    
    // Unmount all mounted filesystems
    for (uint32_t i = 0; i < mount_count; i++) {
        if (mount_points[i].mounted) {
            Unmount(mount_points[i].mount_path);
        }
    }
    
    // Clean up root node
    if (root) {
        DestroyVfsNode(root);
        root = nullptr;
    }
}

bool Vfs::Initialize() {
    LOG("Initializing Virtual File System");
    
    // Create the root node
    root = CreateVfsNode("/", nullptr);
    if (!root) {
        LOG("Failed to create VFS root node");
        return false;
    }
    
    root->attributes = ATTR_DIRECTORY;
    strcpy_safe(root->full_path, "/", sizeof(root->full_path));
    
    // Set up initial access control - root owned by user ID 0 (system)
    root->owner_uid = 0;
    root->owner_gid = 0;
    root->permissions = 0755;  // rwxr-xr-x for root directory
    
    LOG("Virtual File System initialized successfully");
    return true;
}

bool Vfs::Mount(const char* mount_point, Device* device, uint32_t fs_type, const char* fs_name) {
    if (!mount_point || !device) {
        return false;
    }
    
    vfs_lock.Acquire();
    
    // Check if mount point already exists
    for (uint32_t i = 0; i < mount_count; i++) {
        if (mount_points[i].mounted && 
            strcmp(mount_points[i].mount_path, mount_point) == 0) {
            LOG("Mount point " << mount_point << " already exists");
            vfs_lock.Release();
            return false;
        }
    }
    
    // Find a free mount point slot
    uint32_t slot = MAX_MOUNT_POINTS;
    for (uint32_t i = 0; i < MAX_MOUNT_POINTS; i++) {
        if (!mount_points[i].mounted) {
            slot = i;
            break;
        }
    }
    
    if (slot == MAX_MOUNT_POINTS) {
        LOG("No free mount point slots");
        vfs_lock.Release();
        return false;
    }
    
    // Create the mount point
    strcpy_safe(mount_points[slot].mount_path, mount_point, sizeof(mount_points[slot].mount_path));
    
    // Create a root node for this filesystem
    VfsNode* fs_root = CreateVfsNode(mount_point, root);
    if (!fs_root) {
        LOG("Failed to create root node for " << fs_name << " filesystem");
        vfs_lock.Release();
        return false;
    }
    
    fs_root->attributes = ATTR_DIRECTORY;
    strcpy_safe(fs_root->full_path, mount_point, sizeof(fs_root->full_path));
    fs_root->device = device;
    fs_root->fs_specific = nullptr;  // Will be set by the specific filesystem
    fs_root->fs_id = fs_type;
    
    mount_points[slot].root_node = fs_root;
    mount_points[slot].device = device;
    mount_points[slot].fs_type = fs_type;
    mount_points[slot].mounted = true;
    strcpy_safe(mount_points[slot].fs_name, fs_name, sizeof(mount_points[slot].fs_name));
    
    mount_count++;
    
    LOG("Mounted " << fs_name << " filesystem at " << mount_point);
    vfs_lock.Release();
    return true;
}

bool Vfs::Unmount(const char* mount_point) {
    if (!mount_point) {
        return false;
    }
    
    vfs_lock.Acquire();
    
    for (uint32_t i = 0; i < mount_count; i++) {
        if (mount_points[i].mounted && 
            strcmp(mount_points[i].mount_path, mount_point) == 0) {
            
            // Check if any files from this filesystem are still open
            for (int j = 0; j < MAX_OPEN_FILES; j++) {
                if (open_files[j].is_open && 
                    open_files[j].node && 
                    open_files[j].node->fs_id == mount_points[i].fs_type) {
                    LOG("Cannot unmount " << mount_point << ", files still open");
                    vfs_lock.Release();
                    return false;
                }
            }
            
            // Clean up the filesystem's nodes
            if (mount_points[i].root_node) {
                DestroyVfsNode(mount_points[i].root_node);
            }
            
            // Clear the mount point
            mount_points[i].mounted = false;
            mount_points[i].mount_path[0] = '\0';
            mount_points[i].root_node = nullptr;
            mount_points[i].device = nullptr;
            mount_points[i].fs_name[0] = '\0';
            
            mount_count--;
            
            LOG("Unmounted filesystem from " << mount_point);
            vfs_lock.Release();
            return true;
        }
    }
    
    vfs_lock.Release();
    return false;  // Mount point not found
}

int Vfs::Open(const char* path, uint32_t flags) {
    if (!path) {
        return VFS_ERROR;
    }
    
    vfs_lock.Acquire();
    
    VfsNode* node = ResolvePath(path);
    if (!node) {
        if (flags & FILE_CREATE) {
            // Try to create the file
            char dir_path[MAX_PATH_LENGTH];
            char filename[MAX_FILENAME_LENGTH];
            SplitPath(path, dir_path, filename);
            
            VfsNode* parent = ResolvePath(dir_path);
            if (parent && parent->create) {
                if (parent->create(parent, filename, 0) == VFS_SUCCESS) {
                    // Try to resolve the newly created file
                    node = ResolvePath(path);
                }
            }
        }
        
        if (!node) {
            vfs_lock.Release();
            return VFS_FILE_NOT_FOUND;
        }
    }
    
    // Check permissions
    if ((flags & FILE_WRITE) && (node->attributes & ATTR_READONLY)) {
        vfs_lock.Release();
        return VFS_ACCESS_DENIED;
    }
    
    // Get a free file handle
    FileHandle* handle = GetFreeFileHandle();
    if (!handle) {
        vfs_lock.Release();
        return VFS_TOO_MANY_OPEN_FILES;
    }
    
    // Call the filesystem-specific open function
    if (node->open) {
        int result = node->open(node, flags);
        if (result != VFS_SUCCESS) {
            ReleaseFd(handle - open_files);
            vfs_lock.Release();
            return result;
        }
    }
    
    // Initialize the file handle
    handle->node = node;
    handle->flags = flags;
    handle->position = (flags & FILE_APPEND) ? node->size : 0;
    handle->ref_count = 1;
    handle->is_open = true;
    
    int fd = handle - open_files;  // Calculate the file descriptor
    
    vfs_lock.Release();
    return fd;
}

int Vfs::Close(int fd) {
    if (!IsValidFileHandle(fd)) {
        return VFS_ERROR;
    }
    
    vfs_lock.Acquire();
    
    FileHandle* handle = &open_files[fd];
    if (!handle->is_open) {
        vfs_lock.Release();
        return VFS_ERROR;
    }
    
    // Call the filesystem-specific close function
    if (handle->node && handle->node->close) {
        handle->node->close(handle->node);
    }
    
    // Mark the handle as free
    handle->is_open = false;
    handle->node = nullptr;
    
    vfs_lock.Release();
    return VFS_SUCCESS;
}

int Vfs::Read(int fd, void* buffer, uint32_t size) {
    if (!IsValidFileHandle(fd) || !buffer || size == 0) {
        return VFS_ERROR;
    }
    
    vfs_lock.Acquire();
    
    FileHandle* handle = &open_files[fd];
    if (!handle->is_open || !(handle->flags & FILE_READ)) {
        vfs_lock.Release();
        return VFS_ERROR;
    }
    
    if (!handle->node || !handle->node->read) {
        vfs_lock.Release();
        return VFS_ERROR;
    }
    
    // Perform the read operation
    int bytes_read = handle->node->read(handle->node, buffer, size, handle->position);
    
    if (bytes_read > 0) {
        handle->position += bytes_read;
    }
    
    vfs_lock.Release();
    return bytes_read;
}

int Vfs::Write(int fd, const void* buffer, uint32_t size) {
    if (!IsValidFileHandle(fd) || !buffer || size == 0) {
        return VFS_ERROR;
    }
    
    vfs_lock.Acquire();
    
    FileHandle* handle = &open_files[fd];
    if (!handle->is_open || !(handle->flags & FILE_WRITE)) {
        vfs_lock.Release();
        return VFS_ERROR;
    }
    
    if (!handle->node || !handle->node->write) {
        vfs_lock.Release();
        return VFS_ERROR;
    }
    
    // Perform the write operation
    int bytes_written = handle->node->write(handle->node, buffer, size, handle->position);
    
    if (bytes_written > 0) {
        handle->position += bytes_written;
    }
    
    vfs_lock.Release();
    return bytes_written;
}

int Vfs::Seek(int fd, int32_t offset, int origin) {
    if (!IsValidFileHandle(fd)) {
        return VFS_ERROR;
    }
    
    vfs_lock.Acquire();
    
    FileHandle* handle = &open_files[fd];
    if (!handle->is_open) {
        vfs_lock.Release();
        return VFS_ERROR;
    }
    
    // Calculate new position based on origin
    uint32_t new_pos = 0;
    switch (origin) {
        case SEEK_SET:
            new_pos = offset;
            break;
        case SEEK_CUR:
            new_pos = handle->position + offset;
            break;
        case SEEK_END:
            new_pos = handle->node ? handle->node->size + offset : offset;
            break;
        default:
            vfs_lock.Release();
            return VFS_ERROR;
    }
    
    // Validate the new position
    if (handle->node && new_pos > handle->node->size) {
        vfs_lock.Release();
        return VFS_ERROR;
    }
    
    if (new_pos < 0) {
        vfs_lock.Release();
        return VFS_ERROR;
    }
    
    handle->position = new_pos;
    
    vfs_lock.Release();
    return VFS_SUCCESS;
}

int Vfs::Stat(const char* path, FileStat* stat) {
    if (!path || !stat) {
        return VFS_ERROR;
    }
    
    vfs_lock.Acquire();
    
    VfsNode* node = ResolvePath(path);
    if (!node) {
        vfs_lock.Release();
        return VFS_FILE_NOT_FOUND;
    }
    
    if (node->stat) {
        int result = node->stat(node, stat);
        vfs_lock.Release();
        return result;
    } else {
        // Fill in basic stats from the VFS node
        stat->inode = node->inode;
        stat->size = node->size;
        stat->access_time = node->access_time;
        stat->modify_time = node->modify_time;
        stat->create_time = node->create_time;
        stat->mode = node->mode;
        stat->attributes = node->attributes;
        stat->permissions = node->permissions;
        
        // Calculate blocks (assuming 512-byte blocks)
        stat->blocks = (node->size + 511) / 512;
        stat->block_size = 512;
        stat->owner_uid = node->owner_uid;
        stat->owner_gid = node->owner_gid;
        
        vfs_lock.Release();
        return VFS_SUCCESS;
    }
}

int Vfs::Mkdir(const char* path, uint32_t mode) {
    if (!path) {
        return VFS_ERROR;
    }
    
    vfs_lock.Acquire();
    
    char dir_path[MAX_PATH_LENGTH];
    char dirname[MAX_FILENAME_LENGTH];
    SplitPath(path, dir_path, dirname);
    
    VfsNode* parent = ResolvePath(dir_path);
    if (!parent) {
        vfs_lock.Release();
        return VFS_FILE_NOT_FOUND;
    }
    
    // Try to create the directory
    if (parent->create) {
        int result = parent->create(parent, dirname, ATTR_DIRECTORY);
        vfs_lock.Release();
        return result;
    } else {
        vfs_lock.Release();
        return VFS_ERROR;
    }
}

int Vfs::Rmdir(const char* path) {
    if (!path) {
        return VFS_ERROR;
    }
    
    vfs_lock.Acquire();
    
    VfsNode* node = ResolvePath(path);
    if (!node) {
        vfs_lock.Release();
        return VFS_FILE_NOT_FOUND;
    }
    
    // Check if it's a directory
    if (!(node->attributes & ATTR_DIRECTORY)) {
        vfs_lock.Release();
        return VFS_ERROR;  // Not a directory
    }
    
    // Check if directory is empty
    // In a real implementation, we would check if the directory has children
    // For now, we'll assume it's safe to remove
    
    if (node->delete_fn) {
        int result = node->delete_fn(node);
        vfs_lock.Release();
        return result;
    } else {
        vfs_lock.Release();
        return VFS_ERROR;
    }
}

int Vfs::Create(const char* path, uint32_t mode) {
    if (!path) {
        return VFS_ERROR;
    }
    
    vfs_lock.Acquire();
    
    char dir_path[MAX_PATH_LENGTH];
    char filename[MAX_FILENAME_LENGTH];
    SplitPath(path, dir_path, filename);
    
    VfsNode* parent = ResolvePath(dir_path);
    if (!parent) {
        vfs_lock.Release();
        return VFS_FILE_NOT_FOUND;
    }
    
    // Try to create the file
    if (parent->create) {
        int result = parent->create(parent, filename, 0);
        vfs_lock.Release();
        return result;
    } else {
        vfs_lock.Release();
        return VFS_ERROR;
    }
}

int Vfs::Unlink(const char* path) {
    if (!path) {
        return VFS_ERROR;
    }
    
    vfs_lock.Acquire();
    
    VfsNode* node = ResolvePath(path);
    if (!node) {
        vfs_lock.Release();
        return VFS_FILE_NOT_FOUND;
    }
    
    // Check if it's a directory
    if (node->attributes & ATTR_DIRECTORY) {
        vfs_lock.Release();
        return VFS_ERROR;  // Use Rmdir for directories
    }
    
    if (node->delete_fn) {
        int result = node->delete_fn(node);
        vfs_lock.Release();
        return result;
    } else {
        vfs_lock.Release();
        return VFS_ERROR;
    }
}

int Vfs::Chdir(const char* path) {
    // This would change the current working directory
    // For now, we'll just return success
    return VFS_SUCCESS;
}

int Vfs::Readdir(const char* path, DirEntry* entries, uint32_t max_entries) {
    if (!path || !entries || max_entries == 0) {
        return VFS_ERROR;
    }
    
    vfs_lock.Acquire();
    
    VfsNode* node = ResolvePath(path);
    if (!node) {
        vfs_lock.Release();
        return VFS_FILE_NOT_FOUND;
    }
    
    // Check if it's a directory
    if (!(node->attributes & ATTR_DIRECTORY)) {
        vfs_lock.Release();
        return VFS_ERROR;
    }
    
    if (node->readdir) {
        // Read directory entries using the filesystem-specific function
        for (uint32_t i = 0; i < max_entries; i++) {
            int result = node->readdir(node, i, &entries[i]);
            if (result != VFS_SUCCESS) {
                vfs_lock.Release();
                return i;  // Return number of entries read
            }
        }
        
        vfs_lock.Release();
        return max_entries;
    } else {
        vfs_lock.Release();
        return VFS_ERROR;
    }
}

const char* Vfs::GetCwd() {
    // Return the current working directory
    // For now, return root
    return "/";
}

VfsNode* Vfs::ResolvePath(const char* path) {
    if (!path) {
        return nullptr;
    }
    
    if (!root) {
        return nullptr;
    }
    
    // Handle absolute vs relative paths
    if (path[0] == '/') {
        // Absolute path - start from root
        VfsNode* current = root;
        
        // Skip leading slash and tokenize the path
        char temp_path[MAX_PATH_LENGTH];
        strcpy_safe(temp_path, path, sizeof(temp_path));
        
        char* token = strtok(temp_path, "/");
        VfsNode* child = current->children;
        
        while (token != nullptr) {
            // Find the child with the matching name
            bool found = false;
            while (child != nullptr) {
                if (strcmp(child->name, token) == 0) {
                    current = child;
                    child = current->children;
                    found = true;
                    break;
                }
                child = child->next_sibling;
            }
            
            if (!found) {
                // Check if this is a mount point
                // For now, return null - in a real system we'd check mount points
                return nullptr;
            }
            
            token = strtok(nullptr, "/");
        }
        
        return current;
    } else {
        // Relative path - would need to resolve relative to current directory
        // For now, we'll just return null
        return nullptr;
    }
}

MountPoint* Vfs::FindMountPoint(const char* path) {
    if (!path) {
        return nullptr;
    }
    
    // Find the longest matching mount point prefix
    int best_match_len = 0;
    MountPoint* best_match = nullptr;
    
    for (uint32_t i = 0; i < mount_count; i++) {
        if (mount_points[i].mounted) {
            int len = strlen(mount_points[i].mount_path);
            
            // Check if the path starts with this mount point
            if (strncmp(path, mount_points[i].mount_path, len) == 0) {
                // Exact match or path continues after mount point
                if (path[len] == '/' || path[len] == '\0') {
                    if (len > best_match_len) {
                        best_match_len = len;
                        best_match = &mount_points[i];
                    }
                }
            }
        }
    }
    
    return best_match;
}

void Vfs::GetAbsolutePath(const char* relative_path, char* absolute_path, uint32_t max_len) {
    if (!relative_path || !absolute_path) {
        return;
    }
    
    if (IsAbsolutePath(relative_path)) {
        // Already absolute
        strncpy(absolute_path, relative_path, max_len - 1);
        absolute_path[max_len - 1] = '\0';
    } else {
        // Relative path - prepend current directory
        const char* cwd = GetCwd();
        snprintf(absolute_path, max_len, "%s/%s", cwd, relative_path);
    }
}

VfsNode* Vfs::CreateVfsNode(const char* name, VfsNode* parent) {
    VfsNode* node = (VfsNode*)kmalloc(sizeof(VfsNode));
    if (!node) {
        return nullptr;
    }
    
    memset(node, 0, sizeof(VfsNode));
    
    if (name) {
        strncpy(node->name, name, sizeof(node->name) - 1);
        node->name[sizeof(node->name) - 1] = '\0';
    }
    
    node->parent = parent;
    node->children = nullptr;
    node->next_sibling = nullptr;
    node->prev_sibling = nullptr;
    
    // Add to parent's children list
    if (parent && parent->children == nullptr) {
        parent->children = node;
    } else if (parent) {
        // Add to the end of the sibling list
        VfsNode* last = parent->children;
        while (last->next_sibling) {
            last = last->next_sibling;
        }
        last->next_sibling = node;
        node->prev_sibling = last;
    }
    
    return node;
}

void Vfs::DestroyVfsNode(VfsNode* node) {
    if (!node) {
        return;
    }
    
    // Recursively destroy children
    VfsNode* child = node->children;
    while (child) {
        VfsNode* next = child->next_sibling;
        DestroyVfsNode(child);
        child = next;
    }
    
    // Remove from parent's children list
    if (node->parent) {
        if (node->parent->children == node) {
            node->parent->children = node->next_sibling;
        } else {
            if (node->prev_sibling) {
                node->prev_sibling->next_sibling = node->next_sibling;
            }
        }
        
        if (node->next_sibling) {
            node->next_sibling->prev_sibling = node->prev_sibling;
        }
    }
    
    // Free the node
    kfree(node);
}

FileHandle* Vfs::GetFreeFileHandle() {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (!open_files[i].is_open) {
            return &open_files[i];
        }
    }
    return nullptr;  // No free handles
}

FileHandle* Vfs::GetFileHandle(int fd) {
    if (fd < 0 || fd >= MAX_OPEN_FILES) {
        return nullptr;
    }
    
    if (open_files[fd].is_open) {
        return &open_files[fd];
    }
    
    return nullptr;
}

bool Vfs::IsValidFileHandle(int fd) {
    return (fd >= 0 && fd < MAX_OPEN_FILES && open_files[fd].is_open);
}

int Vfs::AllocateFd() {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (!open_files[i].is_open) {
            open_files[i].is_open = true;
            open_file_count++;
            return i;
        }
    }
    return -1;  // No free file descriptors
}

void Vfs::ReleaseFd(int fd) {
    if (fd >= 0 && fd < MAX_OPEN_FILES && open_files[fd].is_open) {
        open_files[fd].is_open = false;
        open_files[fd].node = nullptr;
        open_file_count--;
    }
}

void Vfs::SplitPath(const char* path, char* dir, char* filename) {
    if (!path || !dir || !filename) {
        return;
    }
    
    const char* last_slash = strrchr(path, '/');
    if (last_slash) {
        // Copy directory part
        int dir_len = last_slash - path;
        if (dir_len > 0) {
            strncpy(dir, path, dir_len);
            dir[dir_len] = '\0';
        } else {
            dir[0] = '/';  // Root directory
            dir[1] = '\0';
        }
        
        // Copy filename part
        const char* fname_start = last_slash + 1;
        if (strlen(fname_start) > 0) {
            strcpy_safe(filename, fname_start, MAX_FILENAME_LENGTH);
        } else {
            filename[0] = '\0';
        }
    } else {
        // No slash in path, entire string is filename
        dir[0] = '.';
        dir[1] = '\0';
        strcpy_safe(filename, path, MAX_FILENAME_LENGTH);
    }
}

bool Vfs::IsAbsolutePath(const char* path) {
    return path && path[0] == '/';
}

bool Vfs::CheckPermissions(VfsNode* node, uint32_t uid, uint32_t gid, uint32_t required_permissions) {
    if (!node) {
        return false;
    }
    
    // If uid is 0 (superuser), grant access
    if (uid == 0) {
        return true;
    }
    
    // Check owner permissions
    if (node->owner_uid == uid) {
        if ((node->permissions & (required_permissions << 6)) == (required_permissions << 6)) {
            return true;
        }
    }
    // Check group permissions
    else if (node->owner_gid == gid) {
        if ((node->permissions & (required_permissions << 3)) == (required_permissions << 3)) {
            return true;
        }
    }
    // Check other permissions
    else {
        if ((node->permissions & required_permissions) == required_permissions) {
            return true;
        }
    }
    
    return false;
}

// Cache management functions

bool Vfs::ReadFromCache(Device* device, uint32_t block_number, void* buffer, uint32_t size) {
    if (!device || !buffer || size == 0) {
        return false;
    }
    
    vfs_lock.Acquire();
    
    // Look for the block in cache
    for (uint32_t i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].valid && 
            cache[i].device == device && 
            cache[i].block_number == block_number && 
            cache[i].size >= size) {
            
            // Found in cache
            memcpy(buffer, cache[i].data, size);
            cache[i].last_access_time = global_timer ? global_timer->GetTickCount() : 0;
            cache_hits++;
            
            vfs_lock.Release();
            return true;
        }
    }
    
    // Not in cache
    cache_misses++;
    vfs_lock.Release();
    return false;
}

bool Vfs::WriteToCache(Device* device, uint32_t block_number, const void* buffer, uint32_t size) {
    if (!device || !buffer || size == 0 || size > 4096) {  // Use a reasonable max block size
        return false;
    }
    
    vfs_lock.Acquire();
    
    // Find a free cache slot or replace the least recently used
    uint32_t slot = CACHE_SIZE;
    
    // First, try to find an invalid slot
    for (uint32_t i = 0; i < CACHE_SIZE; i++) {
        if (!cache[i].valid) {
            slot = i;
            break;
        }
    }
    
    // If no invalid slot found, find the least recently used
    if (slot == CACHE_SIZE) {
        uint32_t oldest_time = UINT32_MAX;
        for (uint32_t i = 0; i < CACHE_SIZE; i++) {
            if (cache[i].last_access_time < oldest_time) {
                oldest_time = cache[i].last_access_time;
                slot = i;
            }
        }
    }
    
    // If we found a slot
    if (slot < CACHE_SIZE) {
        // Free existing data if any
        if (cache[slot].data) {
            kfree(cache[slot].data);
        }
        
        // Allocate new data
        cache[slot].data = kmalloc(size);
        if (!cache[slot].data) {
            vfs_lock.Release();
            return false;
        }
        
        // Copy the data
        memcpy(cache[slot].data, buffer, size);
        cache[slot].block_number = block_number;
        cache[slot].size = size;
        cache[slot].valid = true;
        cache[slot].dirty = true;
        cache[slot].device = device;
        cache[slot].last_access_time = global_timer ? global_timer->GetTickCount() : 0;
        
        vfs_lock.Release();
        return true;
    }
    
    vfs_lock.Release();
    return false;
}

void Vfs::InvalidateCache(Device* device, uint32_t block_number) {
    vfs_lock.Acquire();
    
    for (uint32_t i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].valid && 
            cache[i].device == device && 
            (block_number == UINT32_MAX || cache[i].block_number == block_number)) {
            
            cache[i].valid = false;
            
            if (cache[i].data) {
                kfree(cache[i].data);
                cache[i].data = nullptr;
            }
        }
    }
    
    vfs_lock.Release();
}

void Vfs::FlushCache(Device* device) {
    vfs_lock.Acquire();
    
    for (uint32_t i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].valid && cache[i].dirty && 
            (device == nullptr || cache[i].device == device)) {
            
            // Write dirty block back to device
            if (driver_framework && cache[i].device) {
                // Calculate the byte offset for this block (assuming 512-byte sectors)
                uint32_t byte_offset = cache[i].block_number * 512;
                driver_framework->Write(cache[i].device->id, cache[i].data, cache[i].size, byte_offset);
            }
            
            cache[i].dirty = false;
        }
    }
    
    vfs_lock.Release();
}

bool InitializeVfs() {
    if (!g_vfs) {
        g_vfs = new Vfs();
        if (!g_vfs) {
            LOG("Failed to create VFS instance");
            return false;
        }
        
        if (!g_vfs->Initialize()) {
            LOG("Failed to initialize VFS");
            delete g_vfs;
            g_vfs = nullptr;
            return false;
        }
        
        LOG("VFS initialized successfully");
    }
    
    return true;
}