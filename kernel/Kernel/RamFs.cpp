#include "Kernel.h"
#include "RamFs.h"
#include "Vfs.h"
#include "Logging.h"

RamFsDriver::RamFsDriver() {
    fs = nullptr;
    vfs_root = nullptr;
}

RamFsDriver::~RamFsDriver() {
    if (fs) {
        // Clean up all nodes in the filesystem
        if (fs->root) {
            DestroyNode(fs->root);
        }
        
        kfree(fs);
        fs = nullptr;
    }
    
    if (vfs_root) {
        // Don't delete the VFS root node as it's managed by VFS
        vfs_root = nullptr;
    }
}

bool RamFsDriver::Initialize(uint32_t size) {
    LOG("Initializing RAM filesystem with size " << size << " bytes");
    
    // Allocate filesystem structure
    fs = (RamFs*)kmalloc(sizeof(RamFs));
    if (!fs) {
        LOG("Failed to allocate RAM filesystem structure");
        return false;
    }
    
    memset(fs, 0, sizeof(RamFs));
    fs->magic = RAMFS_MAGIC;
    fs->total_size = size;
    fs->used_size = 0;
    fs->free_size = size;
    fs->fs_lock.Initialize();
    
    // Create the root directory
    fs->root = CreateNode("/", nullptr, true);
    if (!fs->root) {
        LOG("Failed to create RAM filesystem root");
        kfree(fs);
        fs = nullptr;
        return false;
    }
    
    // Initialize VFS node for the root
    vfs_root = g_vfs->CreateVfsNode("/", nullptr);
    if (!vfs_root) {
        LOG("Failed to create VFS root for RAM filesystem");
        DestroyNode(fs->root);
        kfree(fs);
        fs = nullptr;
        return false;
    }
    
    strcpy_safe(vfs_root->full_path, "/", sizeof(vfs_root->full_path));
    vfs_root->attributes = ATTR_DIRECTORY;
    vfs_root->size = 0;
    vfs_root->fs_specific = this;  // Point to this RAMFS driver instance
    vfs_root->fs_id = RAMFS_MAGIC;
    
    // Set up function pointers for VFS operations
    vfs_root->open = Open;
    vfs_root->close = Close;
    vfs_root->read = Read;
    vfs_root->write = Write;
    vfs_root->seek = Seek;
    vfs_root->stat = Stat;
    vfs_root->readdir = Readdir;
    vfs_root->create = Create;
    vfs_root->delete_fn = Delete;
    
    LOG("RAM filesystem initialized successfully with " << fs->free_size << " bytes free");
    return true;
}

bool RamFsDriver::Mount(const char* mount_point) {
    if (!fs || !mount_point || g_vfs == nullptr) {
        return false;
    }
    
    // Register with the VFS
    if (!g_vfs->Mount(mount_point, nullptr, RAMFS_MAGIC, "RAMFS")) {
        LOG("Failed to mount RAM filesystem at " << mount_point);
        return false;
    }
    
    LOG("RAM filesystem mounted at " << mount_point);
    return true;
}

bool RamFsDriver::Unmount() {
    if (!fs || !vfs_root) {
        return false;
    }
    
    // In a real implementation, we would unmount from VFS
    // For now, we'll just return true
    return true;
}

RamFsNode* RamFsDriver::CreateFile(const char* path, uint8_t attributes) {
    if (!path || !fs || !fs->root) {
        return nullptr;
    }
    
    fs->fs_lock.Acquire();
    
    // Split the path to get the parent directory and filename
    char dir_path[RAMFS_MAX_FILENAME_LENGTH];
    char filename[RAMFS_MAX_FILENAME_LENGTH];
    SplitPath(path, dir_path, filename);
    
    // Find the parent directory
    RamFsNode* parent = FindNode(dir_path);
    if (!parent || !parent->is_directory) {
        fs->fs_lock.Release();
        return nullptr;
    }
    
    // Create the file node
    RamFsNode* file_node = CreateNode(filename, parent, false);
    if (!file_node) {
        fs->fs_lock.Release();
        return nullptr;
    }
    
    file_node->attributes = attributes;
    
    fs->fs_lock.Release();
    return file_node;
}

RamFsNode* RamFsDriver::CreateDirectory(const char* path) {
    if (!path || !fs || !fs->root) {
        return nullptr;
    }
    
    fs->fs_lock.Acquire();
    
    // Split the path to get the parent directory and directory name
    char dir_path[RAMFS_MAX_FILENAME_LENGTH];
    char dirname[RAMFS_MAX_FILENAME_LENGTH];
    SplitPath(path, dir_path, dirname);
    
    // Find the parent directory
    RamFsNode* parent = FindNode(dir_path);
    if (!parent || !parent->is_directory) {
        fs->fs_lock.Release();
        return nullptr;
    }
    
    // Create the directory node
    RamFsNode* dir_node = CreateNode(dirname, parent, true);
    if (!dir_node) {
        fs->fs_lock.Release();
        return nullptr;
    }
    
    dir_node->attributes = ATTR_DIRECTORY;
    
    fs->fs_lock.Release();
    return dir_node;
}

bool RamFsDriver::Delete(const char* path) {
    if (!path || !fs || !fs->root) {
        return false;
    }
    
    fs->fs_lock.Acquire();
    
    RamFsNode* node = FindNode(path);
    if (!node) {
        fs->fs_lock.Release();
        return false;
    }
    
    // Don't delete root
    if (node == fs->root) {
        fs->fs_lock.Release();
        return false;
    }
    
    // For directories, make sure they're empty
    if (node->is_directory && node->children) {
        fs->fs_lock.Release();
        return false;  // Directory not empty
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
    
    // Update filesystem size
    if (node->data) {
        fs->used_size -= node->alloc_size;
        fs->free_size += node->alloc_size;
    }
    
    // Destroy the node
    DestroyNode(node);
    
    fs->fs_lock.Release();
    return true;
}

int RamFsDriver::WriteFile(RamFsNode* node, const void* buffer, uint32_t size, uint32_t offset) {
    if (!node || !buffer || size == 0 || !fs) {
        return VFS_ERROR;
    }
    
    fs->fs_lock.Acquire();
    
    // Check if we need to resize the file
    uint32_t new_size = offset + size;
    if (new_size > node->alloc_size) {
        // We need to allocate more space
        if (!ResizeData(node, new_size)) {
            fs->fs_lock.Release();
            return VFS_ERROR;
        }
    }
    
    // Copy data to the file
    memcpy((uint8_t*)node->data + offset, buffer, size);
    
    // Update file size if necessary
    if (new_size > node->size) {
        node->size = new_size;
    }
    
    // Update modification time
    node->modify_time = global_timer ? global_timer->GetTickCount() : 0;
    
    fs->fs_lock.Release();
    return size;
}

int RamFsDriver::ReadFile(RamFsNode* node, void* buffer, uint32_t size, uint32_t offset) {
    if (!node || !buffer || size == 0 || !fs) {
        return VFS_ERROR;
    }
    
    fs->fs_lock.Acquire();
    
    // Check bounds
    if (offset >= node->size) {
        fs->fs_lock.Release();
        return VFS_EOF;
    }
    
    // Calculate how much we can actually read
    uint32_t remaining = node->size - offset;
    uint32_t bytes_to_read = (size < remaining) ? size : remaining;
    
    // Copy data from the file
    memcpy(buffer, (uint8_t*)node->data + offset, bytes_to_read);
    
    // Update access time
    node->access_time = global_timer ? global_timer->GetTickCount() : 0;
    
    fs->fs_lock.Release();
    return bytes_to_read;
}

int RamFsDriver::GetStat(RamFsNode* node, FileStat* stat) {
    if (!node || !stat) {
        return VFS_ERROR;
    }
    
    memset(stat, 0, sizeof(FileStat));
    
    stat->inode = (uint32_t)node;  // Use pointer as inode (not ideal for persistent FS)
    stat->size = node->size;
    stat->blocks = (node->size + 511) / 512;  // Assuming 512-byte blocks
    stat->block_size = 512;
    stat->access_time = node->access_time;
    stat->modify_time = node->modify_time;
    stat->create_time = node->create_time;
    stat->mode = 0755;  // Default permissions
    stat->attributes = node->attributes;
    stat->owner_uid = 0;
    stat->owner_gid = 0;
    
    return VFS_SUCCESS;
}

RamFsNode* RamFsDriver::FindNode(const char* path) {
    if (!path || !fs || !fs->root) {
        return nullptr;
    }
    
    // Handle root
    if (strcmp(path, "/") == 0 || strcmp(path, "") == 0) {
        return fs->root;
    }
    
    // Skip leading slash if present
    const char* p = path;
    if (*p == '/') p++;
    
    RamFsNode* current = fs->root;
    
    // Tokenize the path and navigate the tree
    char temp_path[RAMFS_MAX_FILENAME_LENGTH];
    strcpy_safe(temp_path, p, sizeof(temp_path));
    
    char* token = strtok(temp_path, "/");
    while (token != nullptr) {
        // Find the child with the matching name
        RamFsNode* child = current->children;
        bool found = false;
        
        while (child != nullptr) {
            if (strcmp(child->name, token) == 0) {
                current = child;
                found = true;
                break;
            }
            child = child->next_sibling;
        }
        
        if (!found) {
            return nullptr;  // Path not found
        }
        
        token = strtok(nullptr, "/");
    }
    
    return current;
}

bool RamFsDriver::GetFsInfo(uint32_t& total_size, uint32_t& used_size, uint32_t& free_size) {
    if (!fs) {
        return false;
    }
    
    fs->fs_lock.Acquire();
    total_size = fs->total_size;
    used_size = fs->used_size;
    free_size = fs->free_size;
    fs->fs_lock.Release();
    
    return true;
}

RamFsNode* RamFsDriver::CreateNode(const char* name, RamFsNode* parent, bool is_directory) {
    RamFsNode* node = (RamFsNode*)kmalloc(sizeof(RamFsNode));
    if (!node) {
        return nullptr;
    }
    
    memset(node, 0, sizeof(RamFsNode));
    
    if (name) {
        strncpy(node->name, name, sizeof(node->name) - 1);
        node->name[sizeof(node->name) - 1] = '\0';
    }
    
    node->parent = parent;
    node->children = nullptr;
    node->next_sibling = nullptr;
    node->prev_sibling = nullptr;
    node->is_directory = is_directory;
    node->attributes = is_directory ? ATTR_DIRECTORY : 0;
    node->ref_count = 0;
    node->access_time = global_timer ? global_timer->GetTickCount() : 0;
    node->modify_time = node->access_time;
    node->create_time = node->access_time;
    
    // Add to parent's children list
    if (parent) {
        if (parent->children == nullptr) {
            parent->children = node;
        } else {
            // Add to the end of the sibling list
            RamFsNode* last = parent->children;
            while (last->next_sibling) {
                last = last->next_sibling;
            }
            last->next_sibling = node;
            node->prev_sibling = last;
        }
    }
    
    return node;
}

void RamFsDriver::DestroyNode(RamFsNode* node) {
    if (!node) {
        return;
    }
    
    // Recursively destroy children
    RamFsNode* child = node->children;
    while (child) {
        RamFsNode* next = child->next_sibling;
        DestroyNode(child);
        child = next;
    }
    
    // Free allocated data
    if (node->data) {
        kfree(node->data);
        node->data = nullptr;
        
        // Update filesystem size tracking
        if (fs) {
            fs->used_size -= node->alloc_size;
            fs->free_size += node->alloc_size;
        }
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

bool RamFsDriver::AllocateData(RamFsNode* node, uint32_t size) {
    if (!node || size == 0) {
        return false;
    }
    
    // Make sure size is within limits
    if (size > RAMFS_MAX_FILE_SIZE) {
        return false;
    }
    
    // First, free existing data if any
    if (node->data) {
        kfree(node->data);
        fs->used_size -= node->alloc_size;
        fs->free_size += node->alloc_size;
    }
    
    // Allocate new data block
    node->data = kmalloc(size);
    if (!node->data) {
        return false;
    }
    
    // Update filesystem size tracking
    uint32_t old_alloc = node->alloc_size;
    node->alloc_size = size;
    fs->used_size += size;
    fs->free_size -= size;
    
    // Check if we have enough space
    if (fs->used_size > fs->total_size) {
        kfree(node->data);
        node->data = nullptr;
        node->alloc_size = old_alloc;
        fs->used_size -= size;
        fs->free_size += size;
        fs->used_size += old_alloc;  // Restore old usage
        fs->free_size -= old_alloc;
        return false;
    }
    
    // Initialize data to zero
    memset(node->data, 0, size);
    node->size = 0;  // File starts empty
    
    return true;
}

bool RamFsDriver::ResizeData(RamFsNode* node, uint32_t new_size) {
    if (!node) {
        return false;
    }
    
    // Make sure size is within limits
    if (new_size > RAMFS_MAX_FILE_SIZE) {
        return false;
    }
    
    // If we already have enough space, just update the size
    if (new_size <= node->alloc_size) {
        node->size = new_size;
        return true;
    }
    
    // Calculate how much additional space we need
    uint32_t needed = new_size - node->alloc_size;
    
    // Check if we have enough free space in the filesystem
    if (needed > fs->free_size) {
        return false;  // Not enough space
    }
    
    // Allocate a new block
    void* new_data = kmalloc(new_size);
    if (!new_data) {
        return false;
    }
    
    // Copy existing data to the new block
    if (node->data) {
        memcpy(new_data, node->data, node->size);
        kfree(node->data);
    }
    
    // Update filesystem size tracking
    fs->used_size += needed;
    fs->free_size -= needed;
    
    // Update node
    node->data = new_data;
    node->alloc_size = new_size;
    if (new_size < node->size) {
        node->size = new_size;  // Truncate if needed
    }
    
    return true;
}

void RamFsDriver::SplitPath(const char* path, char* dir, char* filename) {
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
            strcpy_safe(filename, fname_start, RAMFS_MAX_FILENAME_LENGTH);
        } else {
            filename[0] = '\0';
        }
    } else {
        // No slash in path, entire string is filename
        dir[0] = '.';
        dir[1] = '\0';
        strcpy_safe(filename, path, RAMFS_MAX_FILENAME_LENGTH);
    }
}

// VFS operation implementations

int RamFsDriver::Open(VfsNode* node, uint32_t flags) {
    if (!node || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    RamFsDriver* driver = (RamFsDriver*)node->fs_specific;
    
    // For RAMFS, we don't need special open handling
    return VFS_SUCCESS;
}

int RamFsDriver::Close(VfsNode* node) {
    if (!node || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    // For RAMFS, we don't need special close handling
    return VFS_SUCCESS;
}

int RamFsDriver::Read(VfsNode* node, void* buffer, uint32_t size, uint32_t offset) {
    if (!node || !buffer || size == 0 || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    RamFsDriver* driver = (RamFsDriver*)node->fs_specific;
    
    // Find the corresponding RamFsNode
    // Since we don't have a direct mapping, we'll need to find it by name/path
    // This is a simplified implementation that assumes node name matches
    RamFsNode* ram_node = driver->FindNode(node->full_path);
    if (!ram_node) {
        return VFS_FILE_NOT_FOUND;
    }
    
    return driver->ReadFile(ram_node, buffer, size, offset);
}

int RamFsDriver::Write(VfsNode* node, const void* buffer, uint32_t size, uint32_t offset) {
    if (!node || !buffer || size == 0 || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    RamFsDriver* driver = (RamFsDriver*)node->fs_specific;
    
    // Find the corresponding RamFsNode
    RamFsNode* ram_node = driver->FindNode(node->full_path);
    if (!ram_node) {
        return VFS_FILE_NOT_FOUND;
    }
    
    return driver->WriteFile(ram_node, buffer, size, offset);
}

int RamFsDriver::Seek(VfsNode* node, int32_t offset, int origin) {
    // For now, this is handled by the VFS layer
    return VFS_ERROR;
}

int RamFsDriver::Stat(VfsNode* node, FileStat* stat) {
    if (!node || !stat || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    RamFsDriver* driver = (RamFsDriver*)node->fs_specific;
    
    // Find the corresponding RamFsNode
    RamFsNode* ram_node = driver->FindNode(node->full_path);
    if (!ram_node) {
        return VFS_FILE_NOT_FOUND;
    }
    
    return driver->GetStat(ram_node, stat);
}

int RamFsDriver::Readdir(VfsNode* node, uint32_t index, DirEntry* entry) {
    if (!node || !entry || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    RamFsDriver* driver = (RamFsDriver*)node->fs_specific;
    
    // Find the corresponding RamFsNode
    RamFsNode* ram_node = driver->FindNode(node->full_path);
    if (!ram_node || !ram_node->is_directory) {
        return VFS_ERROR;
    }
    
    // Iterate through children to find the entry at the specified index
    RamFsNode* child = ram_node->children;
    for (uint32_t i = 0; i < index && child; i++) {
        child = child->next_sibling;
    }
    
    if (!child) {
        return VFS_EOF;  // No more entries
    }
    
    // Fill the directory entry
    strcpy_safe(entry->name, child->name, sizeof(entry->name));
    entry->type = child->is_directory ? ATTR_DIRECTORY : 0;
    entry->inode = (uint32_t)child;  // Use pointer as inode
    entry->size = child->size;
    
    return VFS_SUCCESS;
}

int RamFsDriver::Create(VfsNode* node, const char* name, uint8_t attributes) {
    if (!node || !name || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    RamFsDriver* driver = (RamFsDriver*)node->fs_specific;
    
    // We need to create a path for the new file/directory
    // This is a simplified implementation
    char new_path[RAMFS_MAX_FILENAME_LENGTH * 2];
    if (node->full_path[strlen(node->full_path) - 1] == '/') {
        snprintf(new_path, sizeof(new_path), "%s%s", node->full_path, name);
    } else {
        snprintf(new_path, sizeof(new_path), "%s/%s", node->full_path, name);
    }
    
    if (attributes & ATTR_DIRECTORY) {
        RamFsNode* dir_node = driver->CreateDirectory(new_path);
        return dir_node ? VFS_SUCCESS : VFS_ERROR;
    } else {
        RamFsNode* file_node = driver->CreateFile(new_path, attributes);
        return file_node ? VFS_SUCCESS : VFS_ERROR;
    }
}

int RamFsDriver::Delete(VfsNode* node) {
    if (!node || !node->fs_specific) {
        return VFS_ERROR;
    }
    
    RamFsDriver* driver = (RamFsDriver*)node->fs_specific;
    
    return driver->Delete(node->full_path) ? VFS_SUCCESS : VFS_ERROR;
}