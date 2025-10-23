# New Kernel Architecture Documentation

This document describes the architecture of the new kernel that combines Windows 98 features with modern OS concepts.

## Architecture Overview

The new kernel follows a hybrid design that incorporates elements of both monolithic and microkernel architectures. It features:

- **Modular Design**: Components are loosely coupled for easier maintenance
- **Compatibility**: Support for both Linux system calls and DOS applications
- **Performance**: Efficient scheduling and memory management
- **Reliability**: Robust error handling and recovery
- **Ultimate++ Integration**: Designed to work well with the Ultimate++ framework

## Core Components

### Global State (SVar)
Centralized structure containing all kernel state:
- DescriptorTable: GDT and IDT management
- Monitor: Text mode display management
- Timer: System clock implementation
- KernelHeap: Dynamic memory allocation
- ProcessManager: Process management system
- MemoryManager: Virtual and physical memory management
- DeviceManager: Hardware device management
- VirtualFileSystem: File system abstraction layer
- KernelRegistry: System configuration database

### Process Management
- **ProcessManager**: Manages all processes in the system
- **Process**: Represents a running program with its own address space
- **Thread**: Light-weight execution unit within a process
- Supports both cooperative and preemptive scheduling
- Implements fork/exec/vfork system calls for process creation

### Memory Management
- **MemoryManager**: Centralized memory management system
- **KernelHeap**: Dynamic allocation in kernel space
- **Virtual Memory Allocator**: Virtual memory management
- **Page Frame Allocator**: Physical page management
- Implements memory protection between processes
- Supports shared memory regions

### Device Management
- **DeviceManager**: Centralized device management
- **DriverBase**: Abstract base class for all device drivers
- **BlockDeviceDriver**: Handles block-based storage devices
- **ConsoleDriver**: Manages console/terminal I/O
- Implements a driver registration system
- Supports interrupt-driven I/O

### File System
- **VirtualFileSystem**: Abstracts different file system types
- **FAT32FileSystem**: Implements FAT32 file system for DOS compatibility
- Implements mount/umount functionality
- Supports standard file operations (open, read, write, close)

### System Call Interface
- **LinuxSyscallInterface**: Implements Linux-compatible system calls
- **SyscallHandler**: Dispatches system calls to appropriate handlers
- Maintains compatibility with Linux applications
- Provides process control, file I/O, and memory management calls

### Registry System
- **KernelRegistry**: Centralized system configuration database
- **RegistryKey**: Represents a registry key with values and subkeys
- Implements permission-based access control
- Supports both kernel and user modules

## Design Decisions

### Driver Architecture
The kernel implements a C++ inheritance-based driver model:
- `DriverBase` abstract class provides common functionality
- Concrete drivers inherit from `DriverBase`
- Each driver implements standard interface methods
- Driver lifecycle management (Initialize, Shutdown, etc.)

### Microkernel Modularity
While primarily monolithic, the kernel supports some microkernel concepts:
- Component-based architecture allows for separation of concerns
- Services can potentially be run in user space if needed
- Driver modules can be dynamically loaded/unloaded

### Memory Layout
- 0x00000000 - 0x000FFFFF: Reserved (NULL pointer detection, interrupt vectors)
- 0x00100000 - 0x0FFFFFFF: Kernel space
- 0x10000000 - 0xBFFFFFFF: User space (per process)
- 0xC0000000 - 0xFFFFFFFF: Shared kernel space (page tables, etc.)

## Device-Centric Approach

The kernel follows a Windows-like device-centric design:
- Drive letters (C:, D:, etc.) for primary storage devices
- "My Computer" concept for device visibility
- Hierarchical device organization
- Registry-based device path translation
- Special drive assignments (A: for boot, B: for EFI, C: for primary storage)

## Linux Compatibility Layer

The kernel includes a Linux compatibility layer:
- Linux system call interface implementation
- ELF binary loader
- POSIX-compatible file operations
- Linux-style process management calls

## DOS-Shells Integration

The kernel maintains DOS compatibility:
- DOS interrupt system for legacy applications
- FAT32 file system support
- DOS-style file operations
- Real-mode to protected-mode transition support

## Security Considerations

- Memory protection between processes
- Permission-based registry access
- Protected kernel space
- Isolated user space processes

## Performance Optimizations

- Efficient interrupt handling
- Optimized memory allocation algorithms
- Fast context switching
- Caching mechanisms for frequently accessed data