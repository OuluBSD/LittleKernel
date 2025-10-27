#include "Kernel.h"
#include "DosSyscalls.h"
#include "Logging.h"
#include "Vfs.h"
#include "ProcessControlBlock.h"
#include "Interrupts.h"
#include "Linuxulator.h"  // For O_* constants

// Global instance of the DOS system call interface
DosSyscallInterface* g_dos_syscall_interface = nullptr;

DosSyscallInterface::DosSyscallInterface() {
    current_drive = 0;  // A: drive by default
    memset(current_directory, 0, sizeof(current_directory));
    strcpy_safe(current_directory, "C:\\", sizeof(current_directory));
    last_error = DOS_ERROR_NONE;
    verify_flag = false;
    current_dta = nullptr;
    environment_block = nullptr;
    environment_size = 0;
    file_handles = nullptr;
    file_handle_count = 0;
    extended_file_handles = nullptr;
    extended_file_handle_count = 0;
    interrupt_vectors = nullptr;
    interrupt_vector_count = 0;
    memory_blocks = nullptr;
    memory_block_count = 0;
    file_control_blocks = nullptr;
    file_control_block_count = 0;
    disk_transfer_areas = nullptr;
    disk_transfer_area_count = 0;
    program_segment_prefixes = nullptr;
    program_segment_prefix_count = 0;
    search_paths = nullptr;
    search_path_count = 0;
    dos_syscall_lock.Initialize();
}

DosSyscallInterface::~DosSyscallInterface() {
    // Cleanup handled by kernel shutdown
}

bool DosSyscallInterface::Initialize() {
    LOG("Initializing DOS system call interface");
    
    // Allocate and initialize core DOS structures
    current_dta = CreateDta();
    if (!current_dta) {
        LOG("Failed to create initial DTA for DOS system calls");
        return false;
    }
    
    // Set up default interrupt vectors
    interrupt_vectors = (uint8*)malloc(256 * sizeof(uint32) * 2);  // Segment:Offset pairs
    if (interrupt_vectors) {
        memset(interrupt_vectors, 0, 256 * sizeof(uint32) * 2);
        interrupt_vector_count = 256;
    }
    
    LOG("DOS system call interface initialized successfully");
    return true;
}

int DosSyscallInterface::HandleSyscall(DosSyscallContext* context) {
    if (!context) {
        return -1;
    }
    
    return DispatchSyscall(context);
}

int DosSyscallInterface::DispatchSyscall(DosSyscallContext* context) {
    if (!context) {
        return -1;
    }
    
    // Log the DOS system call (for debugging)
    DLOG("DOS interrupt: 0x" << (int)context->interrupt_number 
         << ", function: 0x" << (int)context->function_number);
    
    // Handle different DOS interrupt numbers
    switch (context->interrupt_number) {
        case DOS_SYSCALL_INT21:  // INT 21h - DOS system calls
            return HandleDosInt21(context);
            
        case DOS_SYSCALL_INT20:  // INT 20h - Program termination
            return DosExit(0);
            
        default:
            LOG("Unsupported DOS interrupt: 0x" << hex << (int)context->interrupt_number);
            return -1;
    }
}

int DosSyscallInterface::HandleDosInt21(DosSyscallContext* context) {
    if (!context) {
        return -1;
    }
    
    // Handle different function numbers in INT 21h
    switch (context->function_number) {
        case DOS_INT21_TERMINATE_PROGRAM:  // Function 00h - Program termination
            return DosExit(0);
            
        case DOS_INT21_CHARACTER_INPUT:  // Function 01h - Character input
            return HandleDosCharacterInput(context);
            
        case DOS_INT21_CHARACTER_OUTPUT:  // Function 02h - Character output
            return HandleDosCharacterOutput(context);
            
        case DOS_INT21_WRITE_STRING:  // Function 09h - Write string to stdout
            return HandleDosWriteString(context);
            
        case DOS_INT21_BUFFERED_INPUT:  // Function 0Ah - Buffered input
            return HandleDosBufferedInput(context);
            
        case DOS_INT21_GET_DEFAULT_DRIVE:  // Function 19h - Get current drive
            return HandleDosGetDefaultDrive(context);
            
        case DOS_INT21_SET_DEFAULT_DRIVE:  // Function 0Eh - Set default drive
            return HandleDosSetDefaultDrive(context);
            
        case DOS_INT21_GET_CURRENT_DIRECTORY:  // Function 47h - Get current directory
            return HandleDosGetCurrentDirectory(context);
            
        case DOS_INT21_SET_CURRENT_DIRECTORY:  // Function 3Bh - Set current directory
            return HandleDosSetCurrentDirectory(context);
            
        case DOS_INT21_OPEN_FILE:  // Function 3Dh - Open file
            return HandleDosOpenFile(context);
            
        case DOS_INT21_CLOSE_FILE:  // Function 3Eh - Close file
            return HandleDosCloseFile(context);
            
        case DOS_INT21_READ_FILE:  // Function 3Fh - Read from file
            return HandleDosReadFile(context);
            
        case DOS_INT21_WRITE_FILE:  // Function 40h - Write to file
            return HandleDosWriteFile(context);
            
        case DOS_INT21_CREATE_FILE:  // Function 3Ch - Create file
            return HandleDosCreateFile(context);
            
        case DOS_INT21_DELETE_FILE:  // Function 41h - Delete file
            return HandleDosDeleteFile(context);
            
        case DOS_INT21_GET_FILE_ATTRIBUTES:  // Function 43h - Get/set file attributes
            return HandleDosGetFileAttributes(context);
            
        case DOS_INT21_SET_FILE_ATTRIBUTES:  // Function 43h - Get/set file attributes
            return HandleDosSetFileAttributes(context);
            
        case DOS_INT21_SET_FILE_POINTER:  // Function 42h - Set file pointer
            return HandleDosSetFilePointer(context);
            
        case DOS_INT21_GET_FILE_SIZE:  // Function 42h with AL=02h - Get file size
            return HandleDosGetFileSize(context);
            
        case DOS_INT21_CREATE_DIRECTORY:  // Function 39h - Create directory
            return HandleDosCreateDirectory(context);
            
        case DOS_INT21_REMOVE_DIRECTORY:  // Function 3Ah - Remove directory
            return HandleDosRemoveDirectory(context);
            
        case DOS_INT21_RENAME_FILE:  // Function 56h - Rename file
            return HandleDosRenameFile(context);
            
        case DOS_INT21_GET_VERSION:  // Function 30h - Get DOS version
            return HandleDosGetVersion(context);
            
        case DOS_INT21_ALLOCATE_MEMORY:  // Function 48h - Allocate memory
            return HandleDosAllocateMemory(context);
            
        case DOS_INT21_RELEASE_MEMORY:  // Function 49h - Release memory
            return HandleDosReleaseMemory(context);
            
        case DOS_INT21_RESIZE_MEMORY:  // Function 4Ah - Resize memory block
            return HandleDosResizeMemory(context);
            
        case DOS_INT21_EXEC:  // Function 4Bh - Exec program
            return HandleDosExec(context);
            
        case DOS_INT21_EXIT:  // Function 4Ch - Exit with return code
            return DosExit(context->al);
            
        case DOS_INT21_FIND_FIRST:  // Function 4Eh - Find first file
            return HandleDosFindFirst(context);
            
        case DOS_INT21_FIND_NEXT:  // Function 4Fh - Find next file
            return HandleDosFindNext(context);
            
        default:
            LOG("Unsupported DOS INT 21h function: 0x" << hex << (int)context->function_number);
            last_error = DOS_ERROR_FUNCTION_NUMBER_INVALID;
            return -1;
    }
}

// Helper functions for specific INT 21h functions
int DosSyscallInterface::HandleDosCharacterInput(DosSyscallContext* context) {
    // Function 01h - Read character from stdin, with echo
    // This is a simplified implementation
    
    // For now, return a dummy value
    // In a real implementation, we'd read from the keyboard buffer
    return 0;  // Return character read
}

int DosSyscallInterface::HandleDosCharacterOutput(DosSyscallContext* context) {
    // Function 02h - Write character to stdout
    // DL register contains the character to output
    
    char ch = (char)(context->dx & 0xFF);
    LOG("DOS Character Output: " << ch);
    
    // Output the character to console
    if (g_vga_text_buffer) {
        g_vga_text_buffer->PutChar(ch);
    }
    
    return 0;
}

int DosSyscallInterface::HandleDosWriteString(DosSyscallContext* context) {
    // Function 09h - Write string to stdout
    // DS:DX points to string ending in '$'
    
    uint32 ds_base = context->ds << 4;  // Convert segment to linear address
    char* str_ptr = (char*)(ds_base + (context->dx & 0xFFFF));
    
    // Output the string until we hit '$'
    int i = 0;
    while (str_ptr[i] != '$' && i < 256) {  // Limit to prevent infinite loops
        if (g_vga_text_buffer) {
            g_vga_text_buffer->PutChar(str_ptr[i]);
        }
        i++;
    }
    
    return 0;
}

int DosSyscallInterface::HandleDosBufferedInput(DosSyscallContext* context) {
    // Function 0Ah - Buffered input
    // DS:DX points to input buffer
    
    uint32 ds_base = context->ds << 4;  // Convert segment to linear address
    uint8* buffer_ptr = (uint8*)(ds_base + (context->dx & 0xFFFF));
    
    // First byte is max characters, second byte is actual characters read
    uint8 max_chars = buffer_ptr[0];
    uint8* actual_chars = &buffer_ptr[1];
    
    // For now, we'll just set the actual characters to 0 as input isn't implemented
    *actual_chars = 0;
    
    return 0;
}

int DosSyscallInterface::HandleDosGetDefaultDrive(DosSyscallContext* context) {
    // Function 19h - Get current drive (0=A:, 1=B:, etc.)
    return current_drive;
}

int DosSyscallInterface::HandleDosSetDefaultDrive(DosSyscallContext* context) {
    // Function 0Eh - Set current drive
    uint8 drive = context->dx & 0xFF;  // DL contains drive number (0=A:, 1=B:, etc.)
    
    if (drive < DOS_MAX_DRIVE_LETTERS) {
        current_drive = drive;
        return 0;
    }
    
    last_error = DOS_ERROR_INVALID_DRIVE;
    return -1;
}

int DosSyscallInterface::HandleDosGetCurrentDirectory(DosSyscallContext* context) {
    // Function 47h - Get current directory
    // DL = drive number (0=default, 1=A:, etc.), DS:SI = buffer address
    
    uint8 drive = (context->dx & 0xFF);
    if (drive == 0) {
        drive = current_drive;  // Use current drive if 0 is specified
    } else {
        drive--;  // DOS uses 1-based indexing (1=A:, 2=B:, etc.), convert to 0-based
    }
    
    uint32 ds_base = context->ds << 4;
    char* buffer = (char*)(ds_base + (context->si & 0xFFFF));
    
    // Copy current directory to buffer (without drive letter)
    // Format should be something like "DIR1\\DIR2" without leading backslash
    strcpy_safe(buffer, current_directory + 3, 64);  // Skip "C:\\" part for example
    
    return 0;
}

int DosSyscallInterface::HandleDosSetCurrentDirectory(DosSyscallContext* context) {
    // Function 3Bh - Change current directory
    // DS:DX points to directory path
    
    uint32 ds_base = context->ds << 4;
    char* dir_path = (char*)(ds_base + (context->dx & 0xFFFF));
    
    // Convert DOS path to Unix path (simplified)
    char unix_path[DOS_MAX_PATH_LENGTH];
    if (ConvertDosPathToUnix(dir_path, unix_path, sizeof(unix_path))) {
        int result = DosChdir(unix_path);
        if (result == 0) {
            // Update our internal current directory
            strcpy_safe(current_directory, dir_path, sizeof(current_directory));
        }
        return result;
    }
    
    last_error = DOS_ERROR_PATH_NOT_FOUND;
    return -1;
}

int DosSyscallInterface::HandleDosOpenFile(DosSyscallContext* context) {
    // Function 3Dh - Open file
    // DS:DX = filename, AL = access mode
    
    uint32 ds_base = context->ds << 4;
    char* filename = (char*)(ds_base + (context->dx & 0xFFFF));
    uint8 access_mode = context->al;
    
    // Convert DOS path to Unix path
    char unix_path[DOS_MAX_PATH_LENGTH];
    if (!ConvertDosPathToUnix(filename, unix_path, sizeof(unix_path))) {
        last_error = DOS_ERROR_PATH_NOT_FOUND;
        return -1;
    }
    
    // Map DOS access mode to Unix flags
    int flags = 0;
    switch (access_mode & 0x03) {  // Lower 2 bits contain access mode
        case DOS_FILE_ACCESS_READ:
            flags = O_RDONLY;
            break;
        case DOS_FILE_ACCESS_WRITE:
            flags = O_WRONLY;
            break;
        case DOS_FILE_ACCESS_READ_WRITE:
            flags = O_RDWR;
            break;
        default:
            flags = O_RDONLY;
            break;
    }
    
    // Additional flags based on sharing mode
    uint8 sharing_mode = access_mode & 0xF0;  // Upper 4 bits contain sharing mode
    // For simplicity, we'll ignore sharing modes in this implementation
    
    int fd = DosOpen(unix_path, flags, 0644);
    if (fd < 0) {
        last_error = DOS_ERROR_FILE_NOT_FOUND;
        return -1;
    }
    
    return fd;
}

int DosSyscallInterface::HandleDosCloseFile(DosSyscallContext* context) {
    // Function 3Eh - Close file
    // BX = file handle
    
    int fd = context->bx;
    return DosClose(fd);
}

int DosSyscallInterface::HandleDosReadFile(DosSyscallContext* context) {
    // Function 3Fh - Read from file
    // BX = file handle, CX = number of bytes to read, DS:DX = buffer
    
    int fd = context->bx;
    uint16_t count = (uint16_t)context->cx;
    
    uint32 ds_base = context->ds << 4;
    void* buffer = (void*)(ds_base + (context->dx & 0xFFFF));
    
    return DosRead(fd, buffer, count);
}

int DosSyscallInterface::HandleDosWriteFile(DosSyscallContext* context) {
    // Function 40h - Write to file
    // BX = file handle, CX = number of bytes to write, DS:DX = buffer
    
    int fd = context->bx;
    uint16_t count = (uint16_t)context->cx;
    
    uint32 ds_base = context->ds << 4;
    const void* buffer = (const void*)(ds_base + (context->dx & 0xFFFF));
    
    return DosWrite(fd, buffer, count);
}

int DosSyscallInterface::HandleDosCreateFile(DosSyscallContext* context) {
    // Function 3Ch - Create file
    // DS:DX = filename, CX = file attributes, AL = reserved (usually 0)
    
    uint32 ds_base = context->ds << 4;
    char* filename = (char*)(ds_base + (context->dx & 0xFFFF));
    
    // Convert DOS path to Unix path
    char unix_path[DOS_MAX_PATH_LENGTH];
    if (!ConvertDosPathToUnix(filename, unix_path, sizeof(unix_path))) {
        last_error = DOS_ERROR_PATH_NOT_FOUND;
        return -1;
    }
    
    return DosCreat(unix_path, 0644);  // Default permissions
}

int DosSyscallInterface::HandleDosDeleteFile(DosSyscallContext* context) {
    // Function 41h - Delete file
    // DS:DX = filename
    
    uint32 ds_base = context->ds << 4;
    char* filename = (char*)(ds_base + (context->dx & 0xFFFF));
    
    // Convert DOS path to Unix path
    char unix_path[DOS_MAX_PATH_LENGTH];
    if (!ConvertDosPathToUnix(filename, unix_path, sizeof(unix_path))) {
        last_error = DOS_ERROR_PATH_NOT_FOUND;
        return -1;
    }
    
    return DosUnlink(unix_path);
}

int DosSyscallInterface::HandleDosGetFileAttributes(DosSyscallContext* context) {
    // Function 43h - Get file attributes (AL=00h)
    // DS:DX = filename
    
    if (context->al != 0x00) {
        // This is the set attributes function, which we handle elsewhere
        last_error = DOS_ERROR_FUNCTION_NUMBER_INVALID;
        return -1;
    }
    
    uint32 ds_base = context->ds << 4;
    char* filename = (char*)(ds_base + (context->dx & 0xFFFF));
    
    // Convert DOS path to Unix path
    char unix_path[DOS_MAX_PATH_LENGTH];
    if (!ConvertDosPathToUnix(filename, unix_path, sizeof(unix_path))) {
        last_error = DOS_ERROR_PATH_NOT_FOUND;
        return -1;
    }
    
    FileStat statbuf;
    int result = DosStat(unix_path, &statbuf);
    if (result < 0) {
        last_error = DOS_ERROR_FILE_NOT_FOUND;
        return -1;
    }
    
    // Convert Unix stat to DOS attributes
    uint16_t dos_attrs = 0;
    if (S_ISDIR(statbuf.st_mode)) {
        dos_attrs |= DOS_ATTR_DIRECTORY;
    } else {
        // Regular file - check other attributes
        if (!(statbuf.st_mode & S_IWUSR)) {
            dos_attrs |= DOS_ATTR_READ_ONLY;
        }
    }
    
    // In a real implementation, we'd set this in the return registers
    // For now, return the attributes directly
    return dos_attrs;
}

int DosSyscallInterface::HandleDosSetFileAttributes(DosSyscallContext* context) {
    // Function 43h - Set file attributes (AL=01h)
    // DS:DX = filename, CX = attributes
    
    if (context->al != 0x01) {
        // This should be getting attributes, handled elsewhere
        last_error = DOS_ERROR_FUNCTION_NUMBER_INVALID;
        return -1;
    }
    
    uint32 ds_base = context->ds << 4;
    char* filename = (char*)(ds_base + (context->dx & 0xFFFF));
    uint16_t dos_attrs = (uint16_t)context->cx;
    
    // Convert DOS path to Unix path
    char unix_path[DOS_MAX_PATH_LENGTH];
    if (!ConvertDosPathToUnix(filename, unix_path, sizeof(unix_path))) {
        last_error = DOS_ERROR_PATH_NOT_FOUND;
        return -1;
    }
    
    // Map DOS attributes to Unix permissions
    // This is a simplified approach
    mode_t new_mode = 0644;  // Default permissions
    
    if (dos_attrs & DOS_ATTR_READ_ONLY) {
        // Make file read-only by removing write permissions
        new_mode = 0444;  // Read-only for owner, group, and others
    } else {
        // Allow read/write
        new_mode = 0644;  // Read/write for owner, read for group and others
    }
    
    return DosChmod(unix_path, new_mode);
}

int DosSyscallInterface::HandleDosSetFilePointer(DosSyscallContext* context) {
    // Function 42h - Set file pointer
    // BX = file handle, CX:DX = offset, AL = method (0=beginning, 1=current, 2=end)
    
    int fd = context->bx;
    
    // Combine CX:DX to get 32-bit offset
    int32_t offset = (context->cx << 16) | (context->dx & 0xFFFF);
    
    int origin;
    switch (context->al) {
        case 0: origin = SEEK_SET; break;  // From beginning
        case 1: origin = SEEK_CUR; break;  // From current position
        case 2: origin = SEEK_END; break;  // From end
        default: 
            last_error = DOS_ERROR_INVALID_FUNCTION;
            return -1;
    }
    
    return DosLseek(fd, offset, origin);
}

int DosSyscallInterface::HandleDosGetFileSize(DosSyscallContext* context) {
    // Getting file size is done via SetFilePointer with AL=2 and offset 0
    // This is essentially lseek(fd, 0, SEEK_END)
    
    int fd = context->bx;
    return DosLseek(fd, 0, SEEK_END);
}

int DosSyscallInterface::HandleDosCreateDirectory(DosSyscallContext* context) {
    // Function 39h - Create directory
    // DS:DX = directory path
    
    uint32 ds_base = context->ds << 4;
    char* pathname = (char*)(ds_base + (context->dx & 0xFFFF));
    
    // Convert DOS path to Unix path
    char unix_path[DOS_MAX_PATH_LENGTH];
    if (!ConvertDosPathToUnix(pathname, unix_path, sizeof(unix_path))) {
        last_error = DOS_ERROR_PATH_NOT_FOUND;
        return -1;
    }
    
    return DosMkdir(unix_path, 0755);  // Default directory permissions
}

int DosSyscallInterface::HandleDosRemoveDirectory(DosSyscallContext* context) {
    // Function 3Ah - Remove directory
    // DS:DX = directory path
    
    uint32 ds_base = context->ds << 4;
    char* pathname = (char*)(ds_base + (context->dx & 0xFFFF));
    
    // Convert DOS path to Unix path
    char unix_path[DOS_MAX_PATH_LENGTH];
    if (!ConvertDosPathToUnix(pathname, unix_path, sizeof(unix_path))) {
        last_error = DOS_ERROR_PATH_NOT_FOUND;
        return -1;
    }
    
    return DosRmdir(unix_path);
}

int DosSyscallInterface::HandleDosRenameFile(DosSyscallContext* context) {
    // Function 56h - Rename file
    // DS:DX = old filename, ES:DI = new filename
    
    uint32 ds_base = context->ds << 4;
    char* oldpath = (char*)(ds_base + (context->dx & 0xFFFF));
    
    uint32 es_base = context->es << 4;
    char* newpath = (char*)(es_base + (context->di & 0xFFFF));
    
    // Convert DOS paths to Unix paths
    char unix_oldpath[DOS_MAX_PATH_LENGTH];
    char unix_newpath[DOS_MAX_PATH_LENGTH];
    
    if (!ConvertDosPathToUnix(oldpath, unix_oldpath, sizeof(unix_oldpath)) ||
        !ConvertDosPathToUnix(newpath, unix_newpath, sizeof(unix_newpath))) {
        last_error = DOS_ERROR_PATH_NOT_FOUND;
        return -1;
    }
    
    return DosRename(unix_oldpath, unix_newpath);
}

int DosSyscallInterface::HandleDosGetVersion(DosSyscallContext* context) {
    // Function 30h - Get DOS version
    // Return version in AX (AL=minor, AH=major)
    
    // Return version 5.0 (for MS-DOS 5.0 compatibility)
    // This would be set in the AX register in a real implementation
    return (5 << 8) | 0;  // AH=5 (major), AL=0 (minor)
}

int DosSyscallInterface::HandleDosAllocateMemory(DosSyscallContext* context) {
    // Function 48h - Allocate memory
    // BX = number of paragraphs (16-byte blocks) to allocate
    
    uint16_t paragraphs = (uint16_t)context->bx;
    uint32 bytes = paragraphs * 16;
    
    // Allocate memory block and create MCB
    DosMcb* mcb = CreateMcb('M', g_current_process ? g_current_process->pid : 0, paragraphs, "DOSMEM");
    if (!mcb) {
        last_error = DOS_ERROR_INSUFFICIENT_MEMORY;
        return 0xFFFF;  // Indicate error by returning invalid segment
    }
    
    // In a real implementation, we'd return the segment address
    // For now, return a dummy value
    return 0x1000;  // Dummy segment address
}

int DosSyscallInterface::HandleDosReleaseMemory(DosSyscallContext* context) {
    // Function 49h - Release memory
    // ES = segment of memory block to release
    
    uint16_t segment = (uint16_t)context->es;
    
    // In a real implementation, find and free the memory block
    // For now, just return success
    return 0;
}

int DosSyscallInterface::HandleDosResizeMemory(DosSyscallContext* context) {
    // Function 4Ah - Resize memory block
    // BX = new size in paragraphs, ES = segment of memory block
    
    uint16_t new_paragraphs = (uint16_t)context->bx;
    uint16_t segment = (uint16_t)context->es;
    
    // In a real implementation, resize the memory block
    // For now, just return success
    return 0;
}

int DosSyscallInterface::HandleDosExec(DosSyscallContext* context) {
    // Function 4Bh - Exec program
    // DS:DX = program filename, AL = mode (00h = exec, terminate current process)
    
    uint32 ds_base = context->ds << 4;
    char* filename = (char*)(ds_base + (context->dx & 0xFFFF));
    
    // Convert DOS path to Unix path
    char unix_path[DOS_MAX_PATH_LENGTH];
    if (!ConvertDosPathToUnix(filename, unix_path, sizeof(unix_path))) {
        last_error = DOS_ERROR_PATH_NOT_FOUND;
        return -1;
    }
    
    // Create a dummy PSP for the new process
    DosPsp* psp = CreatePsp(g_current_process ? g_current_process->pid : 0, filename);
    if (!psp) {
        last_error = DOS_ERROR_INSUFFICIENT_MEMORY;
        return -1;
    }
    
    // In a real implementation, we would load and execute the program
    // For now, just return an error as this is complex to implement
    return -1;
}

int DosSyscallInterface::HandleDosFindFirst(DosSyscallContext* context) {
    // Function 4Eh - Find first matching file
    // DS:DX = filespec, CX = attributes to match, ES:DI = DTA address
    
    uint32 ds_base = context->ds << 4;
    char* filespec = (char*)(ds_base + (context->dx & 0xFFFF));
    uint16_t attributes = (uint16_t)context->cx;
    
    // Set current DTA if specified
    uint32 es_base = context->es << 4;
    DosDta* dta = (DosDta*)(es_base + (context->di & 0xFFFF));
    if (dta) {
        current_dta = dta;
    }
    
    // Copy search pattern to DTA
    if (dta) {
        // Extract just the filename part from the full path
        const char* filename_part = strrchr(filespec, '\\');
        if (!filename_part) {
            filename_part = strrchr(filespec, '/');
        }
        if (!filename_part) {
            filename_part = filespec;  // Use whole string if no path separator
        } else {
            filename_part++;  // Skip the separator
        }
        
        strncpy(dta->pattern, filename_part, 11);
        dta->pattern[11] = '\0';  // Ensure null termination
        dta->attributes = (uint8)attributes;
    }
    
    // For now, return an error as this requires directory iteration
    return -1;
}

int DosSyscallInterface::HandleDosFindNext(DosSyscallContext* context) {
    // Function 4Fh - Find next matching file
    // ES:DI = DTA address (use current DTA if not specified)
    
    uint32 es_base = context->es << 4;
    DosDta* dta = (DosDta*)(es_base + (context->di & 0xFFFF));
    if (!dta) {
        dta = current_dta;  // Use current DTA if not specified
    }
    
    if (!dta) {
        last_error = DOS_ERROR_NO_MORE_FILES;
        return -1;
    }
    
    // For now, return an error as this requires directory iteration
    return -1;
}

// Core DOS system call implementations
int DosSyscallInterface::DosRead(uint32 fd, void* buffer, uint32 count) {
    if (!g_vfs) {
        return -1;
    }
    
    return g_vfs->Read(fd, buffer, count);
}

int DosSyscallInterface::DosWrite(uint32 fd, const void* buffer, uint32 count) {
    if (!g_vfs) {
        return -1;
    }
    
    return g_vfs->Write(fd, (void*)buffer, count);
}

int DosSyscallInterface::DosOpen(const char* filename, uint32 flags, uint32 mode) {
    if (!filename || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Open(filename, flags);
}

int DosSyscallInterface::DosClose(uint32 fd) {
    if (!g_vfs) {
        return -1;
    }
    
    return g_vfs->Close(fd);
}

int DosSyscallInterface::DosCreat(const char* filename, uint32 mode) {
    if (!filename || !g_vfs) {
        return -1;
    }
    
    // Create file with write only access
    return g_vfs->Open(filename, O_CREAT | O_WRONLY | O_TRUNC);
}

int DosSyscallInterface::DosUnlink(const char* filename) {
    if (!filename || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Unlink(filename);
}

int DosSyscallInterface::DosExec(const char* filename, char* const argv[], char* const envp[]) {
    if (!filename || !process_manager) {
        LOG("Invalid parameters for exec");
        return -1;
    }
    
    LOG("DOS Exec system call not implemented yet (filename: " << filename << ")");
    
    // For now, return -1 to indicate unimplemented
    return -1;
}

int DosSyscallInterface::DosFork() {
    if (!process_manager) {
        LOG("Process manager not available for fork");
        return -1;
    }
    
    LOG("DOS Fork system call not implemented yet");
    
    // For now, return -1 to indicate unimplemented
    return -1;
}

int DosSyscallInterface::DosWait(int* status) {
    if (!process_manager) {
        LOG("Process manager not available for wait");
        return -1;
    }
    
    LOG("DOS Wait system call not implemented yet");
    
    // For now, return -1 to indicate unimplemented
    return -1;
}

int DosSyscallInterface::DosGetPid() {
    // Get the current process ID
    if (g_current_process) {
        return g_current_process->pid;
    }
    return 1; // Default to PID 1 if no current process
}

int DosSyscallInterface::DosExit(int status) {
    // Exit the current DOS process
    LOG("DOS Process exiting with status: " << status);
    
    // In a real implementation, this would terminate the current process
    // For now, we'll just return
    return 0;
}

int DosSyscallInterface::DosKill(int pid, int signal) {
    LOG("DOS Kill system call not implemented yet (pid: " << pid << ", sig: " << signal << ")");
    return -1;
}

int DosSyscallInterface::DosStat(const char* filename, struct FileStat* statbuf) {
    if (!filename || !statbuf || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Stat(filename, statbuf);
}

int DosSyscallInterface::DosFstat(int fd, struct FileStat* statbuf) {
    LOG("DOS Fstat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosLseek(int fd, int32_t offset, int origin) {
    if (!g_vfs) {
        return -1;
    }
    
    return g_vfs->Seek(fd, offset, origin);
}

int DosSyscallInterface::DosChdir(const char* path) {
    if (!path || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Chdir(path);
}

int DosSyscallInterface::DosGetcwd(char* buf, uint32 size) {
    if (!buf || size == 0 || !g_vfs) {
        return -1;
    }
    
    const char* cwd = g_vfs->GetCwd();
    if (strlen(cwd) >= size) {
        return -1;  // Buffer too small
    }
    
    strcpy_safe(buf, cwd, size);
    return 0;
}

int DosSyscallInterface::DosMkdir(const char* path, uint32 mode) {
    if (!path || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Mkdir(path, mode);
}

int DosSyscallInterface::DosRmdir(const char* path) {
    if (!path || !g_vfs) {
        return -1;
    }
    
    return g_vfs->Unlink(path);  // VFS Unlink should handle directories
}

int DosSyscallInterface::DosRename(const char* oldpath, const char* newpath) {
    LOG("DOS Rename system call not implemented yet (old: " << oldpath << ", new: " << newpath << ")");
    return -1;
}

int DosSyscallInterface::DosAccess(const char* path, int mode) {
    LOG("DOS Access system call not implemented yet (path: " << path << ", mode: " << mode << ")");
    return -1;
}

int DosSyscallInterface::DosChmod(const char* path, uint32 mode) {
    LOG("DOS Chmod system call not implemented yet (path: " << path << ", mode: " << mode << ")");
    return -1;
}

int DosSyscallInterface::DosChown(const char* path, uint32 owner, uint32 group) {
    LOG("DOS Chown system call not implemented yet (path: " << path << ", owner: " << owner << ", group: " << group << ")");
    return -1;
}

int DosSyscallInterface::DosUtime(const char* path, struct utimbuf* times) {
    LOG("DOS Utime system call not implemented yet (path: " << path << ")");
    return -1;
}

int DosSyscallInterface::DosDup(int oldfd) {
    LOG("DOS Dup system call not implemented yet (oldfd: " << oldfd << ")");
    return -1;
}

int DosSyscallInterface::DosDup2(int oldfd, int newfd) {
    LOG("DOS Dup2 system call not implemented yet (oldfd: " << oldfd << ", newfd: " << newfd << ")");
    return -1;
}

int DosSyscallInterface::DosPipe(int pipefd[2]) {
    if (!pipefd) {
        return -1;
    }
    
    if (!ipc_manager) {
        return -1;
    }
    
    LOG("DOS Pipe system call not fully implemented yet");
    return -1;
}

int DosSyscallInterface::DosLink(const char* oldpath, const char* newpath) {
    LOG("DOS Link system call not implemented yet (old: " << oldpath << ", new: " << newpath << ")");
    return -1;
}

int DosSyscallInterface::DosSymlink(const char* target, const char* linkpath) {
    LOG("DOS Symlink system call not implemented yet (target: " << target << ", link: " << linkpath << ")");
    return -1;
}

int DosSyscallInterface::DosReadlink(const char* path, char* buf, uint32 bufsiz) {
    LOG("DOS Readlink system call not implemented yet (path: " << path << ")");
    return -1;
}

int DosSyscallInterface::DosTruncate(const char* path, uint32 length) {
    LOG("DOS Truncate system call not implemented yet (path: " << path << ", length: " << length << ")");
    return -1;
}

int DosSyscallInterface::DosFtruncate(int fd, uint32 length) {
    LOG("DOS Ftruncate system call not implemented yet (fd: " << fd << ", length: " << length << ")");
    return -1;
}

int DosSyscallInterface::DosGetdents(int fd, struct dirent* dirp, uint32 count) {
    LOG("DOS Getdents system call not implemented yet (fd: " << fd << ")");
    return -1;
}

int DosSyscallInterface::DosMmap(void* addr, uint32 length, int prot, int flags, int fd, uint32 offset) {
    LOG("DOS Mmap system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMunmap(void* addr, uint32 length) {
    LOG("DOS Munmap system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosBrk(void* addr) {
    LOG("DOS Brk system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSbrk(int32_t increment) {
    LOG("DOS Sbrk system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMprotect(void* addr, uint32 len, int prot) {
    LOG("DOS Mprotect system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMsync(void* addr, uint32 len, int flags) {
    LOG("DOS Msync system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMincore(void* addr, uint32 length, unsigned char* vec) {
    LOG("DOS Mincore system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMadvise(void* addr, uint32 length, int advice) {
    LOG("DOS Madvise system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMlock(const void* addr, uint32 len) {
    LOG("DOS Mlock system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMunlock(const void* addr, uint32 len) {
    LOG("DOS Munlock system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMlockall(int flags) {
    LOG("DOS Mlockall system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMunlockall() {
    LOG("DOS Munlockall system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMount(const char* source, const char* target, const char* filesystemtype, 
                                  unsigned long mountflags, const void* data) {
    LOG("DOS Mount system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosUmount(const char* target) {
    LOG("DOS Umount system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosUmount2(const char* target, int flags) {
    LOG("DOS Umount2 system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosStatfs(const char* path, struct statfs* buf) {
    LOG("DOS Statfs system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFstatfs(int fd, struct statfs* buf) {
    LOG("DOS Fstatfs system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosUstat(dev_t dev, struct ustat* ubuf) {
    LOG("DOS Ustat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosUname(struct utsname* buf) {
    if (!buf) {
        return -1;
    }
    
    // Fill in system information (DOS-compatible)
    strcpy_safe(buf->sysname, "LittleKernel", sizeof(buf->sysname));
    strcpy_safe(buf->nodename, "localhost", sizeof(buf->nodename));
    strcpy_safe(buf->release, "1.0.0", sizeof(buf->release));
    strcpy_safe(buf->version, "LittleKernel DOS-like 1.0", sizeof(buf->version));
    strcpy_safe(buf->machine, "i86", sizeof(buf->machine));
    
    return 0;
}

int DosSyscallInterface::DosGettimeofday(struct timeval* tv, struct timezone* tz) {
    if (!tv) {
        return -1;
    }
    
    // Get current time - in a real implementation, this would use the system timer
    if (global_timer) {
        uint64_t ticks = global_timer->GetTickCount();
        tv->tv_sec = ticks / global_timer->GetFrequency();  // Convert ticks to seconds
        // Calculate microseconds from remaining ticks
        tv->tv_usec = ((ticks % global_timer->GetFrequency()) * 1000000) / global_timer->GetFrequency();
    } else {
        tv->tv_sec = 0;
        tv->tv_usec = 0;
    }
    
    // Timezone not implemented
    if (tz) {
        tz->tz_minuteswest = 0;
        tz->tz_dsttime = 0;
    }
    
    return 0;
}

int DosSyscallInterface::DosSettimeofday(const struct timeval* tv, const struct timezone* tz) {
    LOG("DOS Settimeofday system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetrlimit(int resource, struct rlimit* rlim) {
    LOG("DOS Getrlimit system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSetrlimit(int resource, const struct rlimit* rlim) {
    LOG("DOS Setrlimit system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetrusage(int who, struct rusage* usage) {
    LOG("DOS Getrusage system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSysinfo(struct sysinfo* info) {
    LOG("DOS Sysinfo system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosTimes(struct tms* buf) {
    LOG("DOS Times system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPtrace(long request, pid_t pid, void* addr, void* data) {
    LOG("DOS Ptrace system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetuid() {
    // For DOS compatibility, return a default user ID
    return 0;  // Root user
}

int DosSyscallInterface::DosGeteuid() {
    // For DOS compatibility, return a default user ID
    return 0;  // Root user
}

int DosSyscallInterface::DosGetgid() {
    // For DOS compatibility, return a default group ID
    return 0;  // Root group
}

int DosSyscallInterface::DosGetegid() {
    // For DOS compatibility, return a default group ID
    return 0;  // Root group
}

int DosSyscallInterface::DosSetuid(uid_t uid) {
    LOG("DOS Setuid system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSetgid(gid_t gid) {
    LOG("DOS Setgid system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetgroups(int size, gid_t list[]) {
    LOG("DOS Getgroups system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSetgroups(size_t size, const gid_t* list) {
    LOG("DOS Setgroups system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetpgrp() {
    LOG("DOS Getpgrp system call not implemented yet");
    return 1;  // Default process group
}

int DosSyscallInterface::DosSetpgrp(pid_t pid, pid_t pgrp) {
    LOG("DOS Setpgrp system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSetsid() {
    LOG("DOS Setsid system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetsid(pid_t pid) {
    LOG("DOS Getsid system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetpgid(pid_t pid) {
    LOG("DOS Getpgid system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSetpgid(pid_t pid, pid_t pgid) {
    LOG("DOS Setpgid system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetppid() {
    // Return parent process ID
    if (g_current_process && g_current_process->parent_pcb) {
        return g_current_process->parent_pcb->pid;
    }
    return 1;  // Default to 1 if no parent
}

int DosSyscallInterface::DosSignal(int signum, void (*handler)(int)) {
    LOG("DOS Signal system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSigaction(int signum, const struct sigaction* act, struct sigaction* oldact) {
    LOG("DOS Sigaction system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSigprocmask(int how, const sigset_t* set, sigset_t* oldset) {
    LOG("DOS Sigprocmask system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSigpending(sigset_t* set) {
    LOG("DOS Sigpending system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSigsuspend(const sigset_t* mask) {
    LOG("DOS Sigsuspend system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSigaltstack(const stack_t* ss, stack_t* oss) {
    LOG("DOS Sigaltstack system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosKillpg(int pgrp, int sig) {
    LOG("DOS Killpg system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosAlarm(unsigned int seconds) {
    LOG("DOS Alarm system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPause() {
    LOG("DOS Pause system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSleep(unsigned int seconds) {
    LOG("DOS Sleep system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosUsleep(unsigned int useconds) {
    LOG("DOS Usleep system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosNanosleep(const struct timespec* req, struct timespec* rem) {
    LOG("DOS Nanosleep system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetitimer(int which, struct itimerval* curr_value) {
    LOG("DOS Getitimer system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSetitimer(int which, const struct itimerval* new_value, struct itimerval* old_value) {
    LOG("DOS Setitimer system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSelect(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, 
                                   struct timeval* timeout) {
    LOG("DOS Select system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPoll(struct pollfd* fds, nfds_t nfds, int timeout) {
    LOG("DOS Poll system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosEpollCreate(int size) {
    LOG("DOS EpollCreate system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosEpollCtl(int epfd, int op, int fd, struct epoll_event* event) {
    LOG("DOS EpollCtl system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosEpollWait(int epfd, struct epoll_event* events, int maxevents, int timeout) {
    LOG("DOS EpollWait system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSocket(int domain, int type, int protocol) {
    LOG("DOS Socket system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosBind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    LOG("DOS Bind system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosConnect(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    LOG("DOS Connect system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosListen(int sockfd, int backlog) {
    LOG("DOS Listen system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosAccept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
    LOG("DOS Accept system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSend(int sockfd, const void* buf, size_t len, int flags) {
    LOG("DOS Send system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosRecv(int sockfd, void* buf, size_t len, int flags) {
    LOG("DOS Recv system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSendto(int sockfd, const void* buf, size_t len, int flags,
                                   const struct sockaddr* dest_addr, socklen_t addrlen) {
    LOG("DOS Sendto system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosRecvfrom(int sockfd, void* buf, size_t len, int flags,
                                     struct sockaddr* src_addr, socklen_t* addrlen) {
    LOG("DOS Recvfrom system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSendmsg(int sockfd, const struct msghdr* msg, int flags) {
    LOG("DOS Sendmsg system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosRecvmsg(int sockfd, struct msghdr* msg, int flags) {
    LOG("DOS Recvmsg system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosShutdown(int sockfd, int how) {
    LOG("DOS Shutdown system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetsockopt(int sockfd, int level, int optname, void* optval, socklen_t* optlen) {
    LOG("DOS Getsockopt system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSetsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen) {
    LOG("DOS Setsockopt system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetsockname(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
    LOG("DOS Getsockname system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetpeername(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
    LOG("DOS Getpeername system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSocketpair(int domain, int type, int protocol, int sv[2]) {
    LOG("DOS Socketpair system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosIoctl(int fd, unsigned long request, ...) {
    LOG("DOS Ioctl system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFcntl(int fd, int cmd, ...) {
    LOG("DOS Fcntl system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosOpenat(int dirfd, const char* pathname, int flags, mode_t mode) {
    LOG("DOS Openat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMkdirat(int dirfd, const char* pathname, mode_t mode) {
    LOG("DOS Mkdirat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMknodat(int dirfd, const char* pathname, mode_t mode, dev_t dev) {
    LOG("DOS Mknodat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFchownat(int dirfd, const char* pathname, uid_t owner, gid_t group, int flags) {
    LOG("DOS Fchownat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFutimesat(int dirfd, const char* pathname, const struct timeval times[2]) {
    LOG("DOS Futimesat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosNewfstatat(int dirfd, const char* pathname, struct stat* statbuf, int flags) {
    LOG("DOS Newfstatat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosUnlinkat(int dirfd, const char* pathname, int flags) {
    LOG("DOS Unlinkat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosRenameat(int olddirfd, const char* oldpath, int newdirfd, const char* newpath) {
    LOG("DOS Renameat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosLinkat(int olddirfd, const char* oldpath, int newdirfd, const char* newpath, int flags) {
    LOG("DOS Linkat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSymlinkat(const char* target, int newdirfd, const char* linkpath) {
    LOG("DOS Symlinkat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosReadlinkat(int dirfd, const char* pathname, char* buf, size_t bufsiz) {
    LOG("DOS Readlinkat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFchmodat(int dirfd, const char* pathname, mode_t mode, int flags) {
    LOG("DOS Fchmodat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFaccessat(int dirfd, const char* pathname, int mode, int flags) {
    LOG("DOS Faccessat system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPselect(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds,
                                    const struct timespec* timeout, const sigset_t* sigmask) {
    LOG("DOS Pselect system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPpoll(struct pollfd* fds, nfds_t nfds, const struct timespec* timeout,
                                  const sigset_t* sigmask) {
    LOG("DOS Ppoll system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosUnshare(int flags) {
    LOG("DOS Unshare system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSetns(int fd, int nstype) {
    LOG("DOS Setns system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSplice(int fd_in, loff_t* off_in, int fd_out, loff_t* off_out, size_t len, unsigned int flags) {
    LOG("DOS Splice system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosVmsplice(int fd, const struct iovec* iov, unsigned long nr_segs, unsigned int flags) {
    LOG("DOS Vmsplice system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosTee(int fd_in, int fd_out, size_t len, unsigned int flags) {
    LOG("DOS Tee system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSyncFileRange(int fd, off64_t offset, off64_t nbytes, unsigned int flags) {
    LOG("DOS SyncFileRange system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosIoSetup(unsigned nr_events, aio_context_t* ctx) {
    LOG("DOS IoSetup system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosIoDestroy(aio_context_t ctx) {
    LOG("DOS IoDestroy system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosIoSubmit(aio_context_t ctx, long nr, struct iocb** iocbpp) {
    LOG("DOS IoSubmit system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosIoCancel(aio_context_t ctx, struct iocb* iocb, struct io_event* result) {
    LOG("DOS IoCancel system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosIoGetEvents(aio_context_t ctx, long min_nr, long nr, struct io_event* events, struct timespec* timeout) {
    LOG("DOS IoGetEvents system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosIoPgetevents(aio_context_t ctx, long min_nr, long nr, struct io_event* events,
                                         const struct timespec* timeout, const struct __aio_sigset* sigmask) {
    LOG("DOS IoPgetevents system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosReadahead(int fd, off64_t offset, size_t count) {
    LOG("DOS Readahead system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosKexecLoad(unsigned long entry, unsigned long nr_segments, struct kexec_segment* segments, unsigned long flags) {
    LOG("DOS KexecLoad system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosKexecFileLoad(int kernel_fd, int initrd_fd, unsigned long cmdline_len, const char* cmdline, unsigned long flags) {
    LOG("DOS KexecFileLoad system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosInitModule(void* module_image, unsigned long len, const char* param_values) {
    LOG("DOS InitModule system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosDeleteModule(const char* name, unsigned int flags) {
    LOG("DOS DeleteModule system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSyslog(int type, char* bufp, int len) {
    LOG("DOS Syslog system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosAdjtimex(struct timex* buf) {
    LOG("DOS Adjtimex system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosClockSettime(clockid_t clk_id, const struct timespec* tp) {
    LOG("DOS ClockSettime system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosClockGettime(clockid_t clk_id, struct timespec* tp) {
    LOG("DOS ClockGettime system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosClockGetres(clockid_t clk_id, struct timespec* res) {
    LOG("DOS ClockGetres system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosClockNanosleep(clockid_t clock_id, int flags, const struct timespec* request, struct timespec* remain) {
    LOG("DOS ClockNanosleep system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetrandom(void* buf, size_t buflen, unsigned int flags) {
    LOG("DOS Getrandom system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMemfdCreate(const char* name, unsigned int flags) {
    LOG("DOS MemfdCreate system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMbind(void* addr, unsigned long len, int mode, const unsigned long* nodemask,
                                  unsigned long maxnode, unsigned flags) {
    LOG("DOS Mbind system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSetMempolicy(int mode, const unsigned long* nodemask, unsigned long maxnode) {
    LOG("DOS SetMempolicy system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetMempolicy(int* mode, unsigned long* nodemask, unsigned long maxnode, void* addr, unsigned long flags) {
    LOG("DOS GetMempolicy system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMigratePages(int pid, unsigned long maxnode, const unsigned long* old_nodes, const unsigned long* new_nodes) {
    LOG("DOS MigratePages system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMovePages(int pid, unsigned long count, void** pages, const int* nodes, int* status, int flags) {
    LOG("DOS MovePages system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosAddKey(const char* type, const char* description, const void* payload, size_t plen, int ringid) {
    LOG("DOS AddKey system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosRequestKey(const char* type, const char* description, const char* callout_info, int destringid) {
    LOG("DOS RequestKey system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosKeyctl(int cmd, ...) {
    LOG("DOS Keyctl system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSeccomp(unsigned int operation, unsigned int flags, void* args) {
    LOG("DOS Seccomp system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosLandlockCreateRuleset(const struct landlock_ruleset_attr* attr, size_t size, __u32 flags) {
    LOG("DOS LandlockCreateRuleset system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosLandlockAddRule(int ruleset_fd, enum landlock_rule_type rule_type, const void* rule_attr, __u32 flags) {
    LOG("DOS LandlockAddRule system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosLandlockRestrictSelf(int ruleset_fd, __u32 flags) {
    LOG("DOS LandlockRestrictSelf system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPerfEventOpen(struct perf_event_attr* attr, pid_t pid, int cpu, int group_fd, unsigned long flags) {
    LOG("DOS PerfEventOpen system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFanotifyInit(unsigned int flags, unsigned int event_f_flags) {
    LOG("DOS FanotifyInit system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFanotifyMark(int fanotify_fd, unsigned int flags, uint64_t mask, int dirfd, const char* pathname) {
    LOG("DOS FanotifyMark system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPrctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {
    LOG("DOS Prctl system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosArchPrctl(int code, unsigned long addr) {
    LOG("DOS ArchPrctl system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPersonality(unsigned long persona) {
    LOG("DOS Personality system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosCapget(cap_user_header_t hdrp, cap_user_data_t datap) {
    LOG("DOS Capget system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosCapset(cap_user_header_t hdrp, const cap_user_data_t datap) {
    LOG("DOS Capset system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosIopl(int level) {
    LOG("DOS Iopl system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosIoperm(unsigned long from, unsigned long num, int turn_on) {
    LOG("DOS Ioperm system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosCreateModule(const char* name, size_t size) {
    LOG("DOS CreateModule system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosQueryModule(const char* name, int which, void* buf, size_t bufsize, size_t* ret) {
    LOG("DOS QueryModule system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosGetKernelSyms(struct kernel_sym* table) {
    LOG("DOS GetKernelSyms system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosLookupDcookie(uint64_t cookie64, char* buf, size_t len) {
    LOG("DOS LookupDcookie system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosKcmp(pid_t pid1, pid_t pid2, int type, unsigned long idx1, unsigned long idx2) {
    LOG("DOS Kcmp system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosProcessVmReadv(pid_t pid, const struct iovec* liov, unsigned long liovcnt,
                                           const struct iovec* riov, unsigned long riovcnt, unsigned long flags) {
    LOG("DOS ProcessVmReadv system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosProcessVmWritev(pid_t pid, const struct iovec* liov, unsigned long liovcnt,
                                            const struct iovec* riov, unsigned long riovcnt, unsigned long flags) {
    LOG("DOS ProcessVmWritev system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPkeyMprotect(void* addr, size_t len, int prot, int pkey) {
    LOG("DOS PkeyMprotect system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPkeyAlloc(unsigned long flags, unsigned long access_rights) {
    LOG("DOS PkeyAlloc system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPkeyFree(int pkey) {
    LOG("DOS PkeyFree system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosStatx(int dirfd, const char* pathname, int flags, unsigned int mask, struct statx* statxbuf) {
    LOG("DOS Statx system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosRseq(struct rseq* rseq, uint32 rseq_len, int flags, uint32 sig) {
    LOG("DOS Rseq system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPidfdSendSignal(int pidfd, int sig, siginfo_t* info, unsigned int flags) {
    LOG("DOS PidfdSendSignal system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosOpenTree(int dfd, const char* pathname, unsigned int flags) {
    LOG("DOS OpenTree system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMoveMount(int from_dfd, const char* from_pathname, int to_dfd, const char* to_pathname, unsigned int flags) {
    LOG("DOS MoveMount system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFsopen(const char* fs_name, unsigned int flags) {
    LOG("DOS Fsopen system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFsconfig(int fs_fd, unsigned int cmd, const char* key, const void* value, int aux) {
    LOG("DOS Fsconfig system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFsmount(int fs_fd, unsigned int flags, unsigned int mount_attrs) {
    LOG("DOS Fsmount system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFspick(int dfd, const char* path, unsigned int flags) {
    LOG("DOS Fspick system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPidfdOpen(pid_t pid, unsigned int flags) {
    LOG("DOS PidfdOpen system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosClone3(struct clone_args* cl_args, size_t size) {
    LOG("DOS Clone3 system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosCloseRange(unsigned int fd, unsigned int max_fd, unsigned int flags) {
    LOG("DOS CloseRange system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosOpenat2(int dirfd, const char* pathname, struct open_how* how, size_t size) {
    LOG("DOS Openat2 system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosPidfdGetfd(int pidfd, int targetfd, unsigned int flags) {
    LOG("DOS PidfdGetfd system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFaccessat2(int dirfd, const char* pathname, int mode, int flags) {
    LOG("DOS Faccessat2 system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosProcessMadvise(int pidfd, const struct iovec* iov, size_t iovcnt, int advice, unsigned long flags) {
    LOG("DOS ProcessMadvise system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosEpollPwait2(int epfd, struct epoll_event* events, int maxevents, const struct timespec* timeout,
                                        const sigset_t* sigmask, size_t sigsetsize) {
    LOG("DOS EpollPwait2 system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMountSetattr(int dfd, const char* path, unsigned int flags, struct mount_attr* uattr, size_t usize) {
    LOG("DOS MountSetattr system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosQuotactlFd(unsigned int fd, unsigned int cmd, int id, void* addr) {
    LOG("DOS QuotactlFd system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosMemfdSecret(unsigned int flags) {
    LOG("DOS MemfdSecret system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosProcessMrelease(int pidfd, unsigned int flags) {
    LOG("DOS ProcessMrelease system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosFutexWaitv(struct futex_waitv* waiters, unsigned int nr_futexes, unsigned int flags,
                                       struct timespec* timeout, clockid_t clockid) {
    LOG("DOS FutexWaitv system call not implemented yet");
    return -1;
}

int DosSyscallInterface::DosSetMempolicyHomeNode(unsigned long start, unsigned long len, unsigned long home_node, unsigned long flags) {
    LOG("DOS SetMempolicyHomeNode system call not implemented yet");
    return -1;
}

// Internal helper functions
int DosSyscallInterface::TranslateLinuxToDosError(int linux_errno) {
    // Convert Linux errno to DOS error code
    switch (linux_errno) {
        case 0: return DOS_ERROR_NONE;
        case ENOENT: return DOS_ERROR_FILE_NOT_FOUND;
        case EACCES: return DOS_ERROR_ACCESS_DENIED;
        case ENOMEM: return DOS_ERROR_INSUFFICIENT_MEMORY;
        case EEXIST: return DOS_ERROR_CURRENT_DIRECTORY_ATTEMPT_TO_REMOVE; // Not exact match
        case EINVAL: return DOS_ERROR_INVALID_ACCESS_CODE;
        case EISDIR: return DOS_ERROR_ACCESS_DENIED; // Can't open directory as file
        case ENOTDIR: return DOS_ERROR_PATH_NOT_FOUND;
        case ENOSPC: return DOS_ERROR_WRITE_PROTECTED;
        case EROFS: return DOS_ERROR_WRITE_PROTECTED;
        default: return DOS_ERROR_GENERAL_FAILURE;
    }
}

int DosSyscallInterface::TranslateDosToLinuxError(int dos_error) {
    // Convert DOS error code to Linux errno
    switch (dos_error) {
        case DOS_ERROR_NONE: return 0;
        case DOS_ERROR_FILE_NOT_FOUND: return ENOENT;
        case DOS_ERROR_ACCESS_DENIED: return EACCES;
        case DOS_ERROR_INSUFFICIENT_MEMORY: return ENOMEM;
        case DOS_ERROR_PATH_NOT_FOUND: return ENOTDIR;
        case DOS_ERROR_INVALID_ACCESS_CODE: return EINVAL;
        default: return EIO;
    }
}

bool DosSyscallInterface::IsValidDosPath(const char* path) {
    if (!path) {
        return false;
    }
    
    // Basic validation for DOS-style path
    // Should start with drive letter followed by colon, or be a relative path
    if (strlen(path) > DOS_MAX_PATH_LENGTH) {
        return false;
    }
    
    // Check that the path doesn't contain invalid characters for DOS
    const char* invalid_chars = "<>\"|?*";
    for (int i = 0; i < strlen(path); i++) {
        if (strchr(invalid_chars, path[i])) {
            return false;  // Invalid character found
        }
    }
    
    return true;
}

bool DosSyscallInterface::ConvertDosPathToUnix(const char* dos_path, char* unix_path, uint32 max_len) {
    if (!dos_path || !unix_path || max_len == 0) {
        return false;
    }
    
    // Make a copy of the DOS path to work with
    char temp_path[DOS_MAX_PATH_LENGTH];
    if (strlen(dos_path) >= sizeof(temp_path)) {
        return false;
    }
    strcpy_safe(temp_path, dos_path, sizeof(temp_path));
    
    // Check if it's a drive letter path (e.g., "C:\path")
    if (temp_path[1] == ':' && temp_path[2] == '\\') {
        // Extract drive letter
        char drive_letter = temp_path[0];
        
        // Convert drive letter to lowercase for consistency
        if (drive_letter >= 'A' && drive_letter <= 'Z') {
            drive_letter = drive_letter + 32;  // Convert to lowercase
        }
        
        // Map DOS drive to Unix path
        switch (drive_letter) {
            case 'a':  // A drive (RAM disk)
                snprintf(unix_path, max_len, "/A/%s", temp_path + 3);  // Skip "C:\"
                break;
            case 'c':  // C drive (main disk)
                snprintf(unix_path, max_len, "/HardDisk/%s", temp_path + 3);  // Skip "C:\"
                break;
            default:   // Other drives
                snprintf(unix_path, max_len, "/%c/%s", drive_letter, temp_path + 3);
                break;
        }
    } else {
        // It's a relative path or Unix-style path, just convert backslashes
        strcpy_safe(unix_path, temp_path, max_len);
    }
    
    // Convert backslashes to forward slashes
    for (char* p = unix_path; *p; p++) {
        if (*p == '\\') {
            *p = '/';
        }
    }
    
    return true;
}

bool DosSyscallInterface::ConvertUnixPathToDos(const char* unix_path, char* dos_path, uint32 max_len) {
    if (!unix_path || !dos_path || max_len == 0) {
        return false;
    }
    
    // For now, just convert forward slashes to backslashes
    // A more complete implementation would map Unix paths back to DOS drive letters
    uint32 i = 0;
    for (; i < max_len - 1 && unix_path[i] != '\0'; i++) {
        dos_path[i] = unix_path[i];
        if (dos_path[i] == '/') {
            dos_path[i] = '\\';
        }
    }
    dos_path[i] = '\0';
    
    return true;
}

uint8 DosSyscallInterface::GetDefaultDrive() {
    return current_drive;
}

bool DosSyscallInterface::SetDefaultDrive(uint8 drive) {
    if (drive < DOS_MAX_DRIVE_LETTERS) {
        current_drive = drive;
        return true;
    }
    return false;
}

const char* DosSyscallInterface::GetDosDrivePath(uint8 drive_letter) {
    // For now, return a simple mapping
    // A full implementation would use the registry or a mapping table
    if (drive_letter < DOS_MAX_DRIVE_LETTERS) {
        // This is a simplified implementation - in reality, we'd look up the actual path
        // associated with this drive letter
        static char temp_path[64];
        snprintf(temp_path, sizeof(temp_path), "/Drive%c", 'A' + drive_letter);
        return temp_path;
    }
    return nullptr;
}

bool DosSyscallInterface::SetDosDrivePath(uint8 drive_letter, const char* path) {
    // For now, this is a simplified implementation
    // A full implementation would update the registry or mapping table
    if (drive_letter < DOS_MAX_DRIVE_LETTERS && path) {
        return true;
    }
    return false;
}

DosPsp* DosSyscallInterface::CreatePsp(uint16_t parent_psp_segment, const char* program_name) {
    // Allocate memory for a new Program Segment Prefix
    DosPsp* psp = (DosPsp*)malloc(sizeof(DosPsp));
    if (!psp) {
        return nullptr;
    }
    
    // Initialize the PSP structure
    memset(psp, 0, sizeof(DosPsp));
    
    // Set up basic PSP fields
    psp->int_20h_instruction = 0xCD20;  // INT 20h instruction (0x20CD in little-endian)
    psp->parent_psp_segment = parent_psp_segment;
    
    // Initialize file handle table
    for (int i = 0; i < 20; i++) {
        psp->file_handles[i] = 0xFF;  // Mark as unused
    }
    
    // Set first three handles to standard input/output/error
    psp->file_handles[0] = 0;  // stdin
    psp->file_handles[1] = 1;  // stdout
    psp->file_handles[2] = 2;  // stderr
    
    // Copy program name if provided
    if (program_name) {
        strncpy((char*)psp->command_tail + 1, program_name, 8);
        psp->command_tail[0] = strlen(program_name);  // Length byte at start of command tail
    }
    
    return psp;
}

bool DosSyscallInterface::DestroyPsp(DosPsp* psp) {
    if (psp) {
        free(psp);
        return true;
    }
    return false;
}

DosDta* DosSyscallInterface::CreateDta() {
    // Allocate memory for a new Disk Transfer Area
    DosDta* dta = (DosDta*)malloc(sizeof(DosDta));
    if (!dta) {
        return nullptr;
    }
    
    // Initialize the DTA structure
    memset(dta, 0, sizeof(DosDta));
    
    return dta;
}

bool DosSyscallInterface::DestroyDta(DosDta* dta) {
    if (dta) {
        free(dta);
        return true;
    }
    return false;
}

DosMcb* DosSyscallInterface::CreateMcb(uint8 signature, uint16_t owner_psp, uint16_t size, const char* program_name) {
    // Allocate memory for a new Memory Control Block
    DosMcb* mcb = (DosMcb*)malloc(sizeof(DosMcb));
    if (!mcb) {
        return nullptr;
    }
    
    // Initialize the MCB structure
    memset(mcb, 0, sizeof(DosMcb));
    
    mcb->signature = signature;
    mcb->owner_psp = owner_psp;
    mcb->size = size;
    
    // Copy program name if provided
    if (program_name) {
        strncpy((char*)mcb->program_name, program_name, 8);
    }
    
    return mcb;
}

bool DosSyscallInterface::DestroyMcb(DosMcb* mcb) {
    if (mcb) {
        free(mcb);
        return true;
    }
    return false;
}

uint8* DosSyscallInterface::AllocateDosMemory(uint32 paragraphs) {
    uint32 bytes = paragraphs * 16;
    return (uint8*)malloc(bytes);
}

bool DosSyscallInterface::FreeDosMemory(uint8* address) {
    if (address) {
        free(address);
        return true;
    }
    return false;
}

bool DosSyscallInterface::ResizeDosMemory(uint8* address, uint32 new_paragraphs) {
    if (!address) {
        return false;
    }
    
    uint32 new_size = new_paragraphs * 16;
    // In a real implementation, this would resize the allocated memory block
    // For now, we'll just return true
    return true;
}

uint16_t DosSyscallInterface::GetDosMemoryBlockOwner(uint8* address) {
    // In a real implementation, this would look up the owner in the MCB
    // For now, return a dummy value
    return 0xFFFF;
}

bool DosSyscallInterface::SetDosMemoryBlockOwner(uint8* address, uint16_t owner_psp) {
    // In a real implementation, this would update the MCB
    // For now, return true
    return true;
}

uint16_t DosSyscallInterface::GetDosMemoryBlockSize(uint8* address) {
    // In a real implementation, this would look up the size in the MCB
    // For now, return a dummy value
    return 0;
}

bool DosSyscallInterface::SetDosMemoryBlockSize(uint8* address, uint16_t size) {
    // In a real implementation, this would update the MCB
    // For now, return true
    return true;
}

bool DosSyscallInterface::ValidateDosMemoryBlock(uint8* address) {
    // In a real implementation, this would validate the memory block
    // For now, return true
    return true;
}

bool DosSyscallInterface::SanitizeDosMemoryBlock(uint8* address) {
    // In a real implementation, this would sanitize the memory block
    // For now, return true
    return true;
}

bool DosSyscallInterface::NormalizeDosMemoryBlock(uint8* address) {
    // In a real implementation, this would normalize the memory block
    // For now, return true
    return true;
}

int DosSyscallInterface::CompareDosMemoryBlocks(uint8* address1, uint8* address2) {
    // In a real implementation, this would compare memory blocks
    // For now, return 0 (equal)
    return 0;
}

uint8* DosSyscallInterface::CloneDosMemoryBlock(uint8* source) {
    // In a real implementation, this would clone the memory block
    // For now, return nullptr
    return nullptr;
}

void DosSyscallInterface::FreeDosMemoryBlock(uint8* address) {
    FreeDosMemory(address);
}

uint8* DosSyscallInterface::AllocateDosMemoryBlock(uint32 size) {
    return (uint8*)malloc(size);
}

void DosSyscallInterface::DeallocateDosMemoryBlock(uint8* address) {
    free(address);
}

void DosSyscallInterface::PrintDosMemoryBlock(uint8* address) {
    if (address) {
        LOG("DOS Memory Block at: 0x" << hex << (uint32)address);
    }
}

void DosSyscallInterface::PrintDosMemoryBlocks() {
    LOG("DOS Memory Blocks: Not implemented");
}

bool DosSyscallInterface::PrintDosMemoryStatistics() {
    LOG("DOS Memory Statistics: Not implemented");
    return true;
}

bool DosSyscallInterface::PrintDosMemoryValidation() {
    LOG("DOS Memory Validation: Not implemented");
    return true;
}

bool DosSyscallInterface::PrintDosMemorySanitization() {
    LOG("DOS Memory Sanitization: Not implemented");
    return true;
}

bool DosSyscallInterface::PrintDosMemoryNormalization() {
    LOG("DOS Memory Normalization: Not implemented");
    return true;
}

bool DosSyscallInterface::PrintDosMemoryComparison(uint8* address1, uint8* address2) {
    LOG("DOS Memory Comparison: Not implemented");
    return true;
}

bool DosSyscallInterface::PrintDosMemoryCloning(uint8* source) {
    LOG("DOS Memory Cloning: Not implemented");
    return true;
}

bool DosSyscallInterface::PrintDosMemoryDeallocation(uint8* address) {
    LOG("DOS Memory Deallocation: Not implemented");
    return true;
}

bool DosSyscallInterface::PrintDosMemoryAllocation(uint32 size) {
    LOG("DOS Memory Allocation: Not implemented");
    return true;
}

// Initialize the DOS system call interface
bool InitializeDosSyscalls() {
    if (!g_dos_syscall_interface) {
        g_dos_syscall_interface = new DosSyscallInterface();
        if (!g_dos_syscall_interface) {
            LOG("Failed to create DOS syscall interface instance");
            return false;
        }
        
        if (!g_dos_syscall_interface->Initialize()) {
            LOG("Failed to initialize DOS syscall interface");
            delete g_dos_syscall_interface;
            g_dos_syscall_interface = nullptr;
            return false;
        }
        
        LOG("DOS system call interface initialized successfully");
    }
    
    return true;
}

// DOS interrupt handler for INT 21h and other DOS interrupts
extern "C" void DosInterruptHandler(Registers regs) {
    if (!g_dos_syscall_interface) {
        // If DOS interface isn't initialized, return
        return;
    }
    
    // Create a DOS system call context from the register state
    DosSyscallContext context;
    context.interrupt_number = regs.int_no;  // The interrupt number that was called
    context.function_number = regs.eax & 0xFF;  // AL register contains function number for INT 21h
    context.ax = regs.eax;
    context.bx = regs.ebx;
    context.cx = regs.ecx;
    context.dx = regs.edx;
    context.si = regs.esi;
    context.di = regs.edi;
    context.bp = regs.ebp;
    context.sp = regs.esp;
    context.ds = regs.ds;
    context.es = regs.es;
    context.flags = regs.eflags;
    context.cs = regs.cs;
    context.ip = regs.eip;
    context.ss = regs.ss;
    
    // Handle the DOS system call
    int result = g_dos_syscall_interface->HandleSyscall(&context);
    
    // Update registers if needed based on result
    // This is a simplified approach - a real implementation would properly handle
    // register state changes as needed by the DOS call
    
    // In a real implementation, we would update the return value in registers
    // For example, setting the return value in AX register
    // This requires assembly code to properly return with modified registers
}

// Handle DOS system calls from the kernel (wrapper function)
extern "C" int HandleDosSyscall(uint8 interrupt_number, uint8 function_number, 
                              uint32 ax, uint32 bx, uint32 cx, uint32 dx, 
                              uint32 si, uint32 di, uint32 bp, uint32 sp,
                              uint32 ds, uint32 es, uint32 flags,
                              uint32 cs, uint32 ip, uint32 ss) {
    if (!g_dos_syscall_interface) {
        return -1;
    }
    
    // Create a DOS system call context
    DosSyscallContext context;
    context.interrupt_number = interrupt_number;
    context.function_number = function_number;
    context.ax = ax;
    context.bx = bx;
    context.cx = cx;
    context.dx = dx;
    context.si = si;
    context.di = di;
    context.bp = bp;
    context.sp = sp;
    context.ds = ds;
    context.es = es;
    context.flags = flags;
    context.cs = cs;
    context.ip = ip;
    context.ss = ss;
    
    // Handle the DOS system call
    return g_dos_syscall_interface->HandleSyscall(&context);
}

// Load and run a DOS executable
bool RunDosExecutable(const char* filename, char* const argv[], char* const envp[]) {
    if (!filename || !g_dos_syscall_interface) {
        return false;
    }
    
    LOG("DOS executable loading not fully implemented yet: " << filename);
    
    // In a real implementation, we would:
    // 1. Load the DOS executable file
    // 2. Create a DOS environment (PSP, memory layout, etc.)
    // 3. Set up the necessary DOS data structures
    // 4. Start execution
    
    return false;
}

// Get the current DOS system call interface
DosSyscallInterface* GetDosSyscallInterface() {
    return g_dos_syscall_interface;
}