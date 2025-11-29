#ifndef _Kernel_FileSystem_h_
#define _Kernel_FileSystem_h_

// Forward declarations for file system types (since actual implementation may not exist yet)
struct File;
struct Directory;
struct FileSystem;
struct MountPoint;

// Basic file system constants
const uint32 MAX_PATH_LENGTH = 260;
const uint32 MAX_FILENAME_LENGTH = 255;

// File access flags
enum FileAccessFlags {
    FILE_READ = 0x1,
    FILE_WRITE = 0x2,
    FILE_EXECUTE = 0x4,
    FILE_APPEND = 0x8
};

// File attributes
enum FileAttributes {
    FILE_ATTR_NORMAL = 0x0,
    FILE_ATTR_READONLY = 0x1,
    FILE_ATTR_HIDDEN = 0x2,
    FILE_ATTR_SYSTEM = 0x4,
    FILE_ATTR_DIRECTORY = 0x10
};

// File system operation results
enum FileOperationResult {
    FILE_SUCCESS = 0,
    FILE_ERROR_ACCESS_DENIED,
    FILE_ERROR_NOT_FOUND,
    FILE_ERROR_ALREADY_EXISTS,
    FILE_ERROR_DISK_FULL,
    FILE_ERROR_INVALID_PATH
};

#endif