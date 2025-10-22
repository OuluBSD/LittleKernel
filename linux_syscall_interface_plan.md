# Linux-Compatible System Call Interface Plan

## Overview
This document outlines the plan for implementing a Linux-compatible system call interface that will allow the kernel to run Linux applications through the Linuxulator feature. The interface will translate Linux system calls into kernel services.

## Objectives
1. Implement Linux system call numbers and semantics
2. Create a translation layer between Linux syscalls and kernel services
3. Ensure compatibility with common Linux applications
4. Provide Linux ABI compatibility for static executables

## Core Linux System Calls to Implement

### Process Management
- `sys_exit` (1) - Terminate current process
- `sys_fork` (2) - Create new process
- `sys_execve` (11) - Execute program
- `sys_wait4` (114) - Wait for process termination
- `sys_clone` (120) - Create new process/thread

### File Operations
- `sys_open` (5) - Open file
- `sys_close` (6) - Close file
- `sys_read` (3) - Read from file
- `sys_write` (4) - Write to file
- `sys_lseek` (19) - Set file position
- `sys_unlink` (10) - Delete file
- `sys_mkdir` (39) - Create directory
- `sys_rmdir` (40) - Remove directory

### Memory Management
- `sys_brk` (45) - Change data segment size
- `sys_mmap` (90) - Map memory region
- `sys_munmap` (91) - Unmap memory region
- `sys_mprotect` (125) - Set memory protection
- `sys_madvise` (219) - Give advice about memory usage

### Process Control
- `sys_getpid` (20) - Get process ID
- `sys_getppid` (64) - Get parent process ID
- `sys_getuid` (24) - Get user ID
- `sys_geteuid` (49) - Get effective user ID
- `sys_kill` (37) - Send signal to process
- `sys_signal` (48) - Install signal handler
- `sys_sigaction` (67) - Change signal action

### Time Functions
- `sys_gettimeofday` (78) - Get time of day
- `sys_time` (13) - Get time
- `sys_settimeofday` (79) - Set time of day

## Implementation Strategy

### 1. System Call Dispatcher
- Implement a main system call dispatcher function
- Use a jump table or function pointer array for efficient dispatch
- Handle invalid system call numbers
- Preserve registers appropriately

```cpp
int32_t handle_system_call(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
```

### 2. Linuxulator System Call Layer
- Create a Linux-specific syscall layer that translates Linux semantics to kernel services
- Implement Linux-specific data structures (linux_dirent, linux_stat, etc.)
- Handle Linux-specific calling conventions

### 3. Virtual File System Integration
- Map Linux file operations to the kernel's VFS layer
- Handle Linux-specific file flags and permissions
- Implement Linux-style path resolution

### 4. Process Management Integration
- Map Linux process creation to kernel task management
- Implement Linux-style process relationships
- Handle Linux process states and signals

## System Call Number Mapping

The kernel will maintain a mapping between Linux system call numbers and kernel-specific function implementations:

```
Linux syscall number -> Kernel syscall handler
5 (sys_open) -> kernel_fs_open()
4 (sys_write) -> kernel_fs_write()
3 (sys_read) -> kernel_fs_read()
11 (sys_execve) -> kernel_process_execve()
```

## Data Structure Compatibility

### Linux Data Types
- Implement Linux-compatible data structures (struct stat, struct timeval, etc.)
- Handle architecture-specific differences (32-bit vs 64-bit)
- Ensure proper packing and alignment for binary compatibility

### File Descriptors
- Implement Linux-style file descriptor table per process
- Map to kernel's internal file handle system
- Support for pipes, sockets, and special files

## Error Code Mapping

Linux system calls return negative errno values on error. The implementation must map kernel error codes to Linux errno values:

```
Kernel error -> Linux errno
-1 (general error) -> -EFAULT
-2 (no such file) -> -ENOENT
-13 (access denied) -> -EACCES
```

## Security Considerations

- Implement Linux-style permission checks
- Map Linux user/group IDs to kernel security contexts
- Handle Linux capabilities if needed
- Ensure privilege separation between processes

## Testing Strategy

1. Start with simple syscalls like sys_getpid
2. Test with basic programs like "hello world"
3. Gradually add more complex syscalls
4. Test with static binaries first
5. Test with actual Linux applications

## Integration Points

### With Kernel Services
- File system layer for file operations
- Process management for process operations
- Memory management for memory operations
- Signal handling for signal operations

### With Linuxulator
- ELF loader to identify Linux binaries
- Compatibility library for userspace functions
- System call translation layer

## Implementation Timeline

### Phase 1: Basic System Call Framework
- Implement system call dispatcher
- Implement 5-10 most basic system calls (getpid, write, etc.)
- Create basic testing infrastructure

### Phase 2: File Operations
- Implement file-related system calls (open, read, write, close)
- Integrate with kernel VFS
- Test with simple file operations

### Phase 3: Process Management
- Implement process creation and control calls
- Implement exec and exit calls
- Test with process creation scenarios

### Phase 4: Advanced Features
- Implement memory management calls (mmap, brk)
- Implement signal handling
- Test with more complex applications

## Linuxulator Architecture

The Linuxulator will work as follows:
1. Identify Linux binaries (via ELF header analysis)
2. Create a Linux-compatible process environment
3. Intercept system calls and translate them
4. Provide Linux ABI compatibility
5. Use kernel services for actual operations

```
Linux Application -> Linux System Call -> Syscall Translation -> Kernel Service -> Hardware
```

## Expected Challenges

1. Linux-specific behaviors that differ from POSIX
2. Signal handling complexity
3. Memory management differences
4. Thread implementation (LinuxThreads vs NPTL)
5. File system semantics differences