# LittleKernel Architecture Documentation

## Overview

LittleKernel is a modern operating system kernel that combines the familiarity of Windows 98-style features with the compatibility of Linux system calls. The kernel is designed to be modular, portable, and efficient while maintaining compatibility with both legacy and modern applications.

## Design Philosophy

### Device-Centric Approach

Unlike traditional Unix-like systems that emphasize a single unified filesystem tree, LittleKernel follows a device-centric design philosophy similar to Windows:

1. **Device Visibility**: Devices are prominently displayed in the system (like "My Computer", "My Documents")
2. **Drive Letters**: Primary storage devices use drive letters (C:, D:, etc.) similar to Windows
3. **Device Identity**: Each device feels like "your own" component of the computer, not just an abstract filesystem entry
4. **Hierarchical Organization**: Devices and storage are organized hierarchically rather than as a single unified filesystem tree (like Unix)
5. **Container Integration**: Virtual containers and virtualized storage appear as "My Computer"-like devices; they feel like real devices to the user even though they are virtualized, providing a consistent device-centric experience

### Ultimate++ Integration

The kernel is designed to work seamlessly with the Ultimate++ framework, leveraging its strengths in GUI development and cross-platform compatibility.

### Modularity

Components are designed to be loosely coupled for easier maintenance and extensibility. The architecture supports both monolithic and microkernel approaches.

## System Architecture

### Core Components

#### 1. Global State Management (SVar)

The kernel maintains a centralized global state structure that contains references to all major kernel subsystems:

- Memory management
- Process management
- Interrupt handling
- Device driver framework
- File system layer
- System call interface
- Configuration system

#### 2. Memory Management

The memory management subsystem provides:

- Virtual memory management with paging
- Heap allocation for dynamic memory
- Memory protection between processes
- Shared memory regions
- Memory-mapped file support
- Demand paging capabilities

#### 3. Process Management

Process management includes:

- Process scheduler with both cooperative and preemptive modes
- Process control block (PCB) structures
- Process synchronization primitives (semaphores, mutexes, events)
- Inter-process communication (pipes, shared memory, signals)
- Thread support for lightweight concurrency

#### 4. Device Driver Framework

The device driver framework provides:

- Abstract base classes for different driver types
- Device registration and management
- Driver loading and unloading
- Unified device interface for kernel services

#### 5. File System Layer

The virtual file system (VFS) layer:

- Provides unified interface to different filesystems
- Implements FAT32 driver for DOS compatibility
- Supports registry-based path translation
- Implements drive letter assignment system

#### 6. System Call Interface

The system call interface:

- Provides Linux-compatible system calls
- Translates Linux syscalls to kernel services
- Implements process control, file operations, and memory management syscalls
- Supports signal handling and inter-process communication

### Component Diagram

```
+-------------------------------------------------------------+
|                    LittleKernel Kernel                      |
+-------------------------------------------------------------+
| Global State (SVar)                                         |
|  +------------------+  +------------------+  +-----------+ |
|  | Memory Manager   |  | Process Manager  |  | Interrupt | |
|  |                  |  |                  |  | Manager   | |
|  | - Paging         |  | - Scheduler      |  | - IDT     | |
|  | - Heap           |  | - PCB            |  | - Handlers| |
|  | - Allocation     |  | - Synchronization|  | - PIC     | |
|  +------------------+  +------------------+  +-----------+ |
+-------------------------------------------------------------+
|            Device Driver Framework                         |
|  +------------------+  +------------------+  +-----------+ |
|  | Driver Base      |  | Block Drivers    |  | Char Dev  | |
|  |                  |  |                  |  | Drivers   | |
|  | - Registration   |  | - Storage        |  | - Console | |
|  | - Lifecycle      |  | - File Systems   |  | - Serial  | |
|  | - Interfaces     |  | - Caching        |  | - Input   | |
|  +------------------+  +------------------+  +-----------+ |
+-------------------------------------------------------------+
|              Virtual File System (VFS)                      |
|  +------------------+  +------------------+  +-----------+ |
|  | VFS Layer        |  | FAT32 Driver     |  | Registry  | |
|  |                  |  |                  |  |           | |
|  | - Mount Points   |  | - DOS Compatible |  | - Path    | |
|  | - File Ops       |  | - Long Names     |  |   Translation|
|  | - Permissions    |  | - Attributes     |  | - Keys    | |
|  +------------------+  +------------------+  +-----------+ |
+-------------------------------------------------------------+
|              System Call Interface                          |
|  +------------------+  +------------------+  +-----------+ |
|  | Syscall Dispatch |  | Process Syscalls |  | File      | |
|  |                  |  |                  |  | Syscalls  | |
|  | - Translation    |  | - fork/exec      |  | - open/   | |
|  | - Error Handling |  | - wait/exit      |  |   read/   | |
|  | - ABI Mapping    |  | - clone          |  |   write   | |
|  +------------------+  +------------------+  +-----------+ |
+-------------------------------------------------------------+
|              Hardware Abstraction Layer                     |
|  +------------------+  +------------------+  +-----------+ |
|  | Hardware Base    |  | CPU Abstraction  |  | Device    | |
|  |                  |  |                  |  | Abstraction|
|  | - Port I/O       |  | - Instructions   |  | - Bus     | |
|  | - Memory Access  |  | - Interrupts     |  |   Support | |
|  | - Timing         |  | - Paging         |  | - DMA     | |
|  +------------------+  +------------------+  +-----------+ |
+-------------------------------------------------------------+
```

## Memory Layout

The kernel uses a segmented memory layout optimized for both kernel and user space:

- **0x00000000 - 0x000FFFFF**: Reserved (NULL pointer detection, interrupt vectors)
- **0x00100000 - 0x0FFFFFFF**: Kernel space (kernel code, data, heap)
- **0x10000000 - 0xBFFFFFFF**: User space (per process)
- **0xC0000000 - 0xFFFFFFFF**: Shared kernel space (page tables, etc.)

## Boot Process

The kernel follows a multiboot-compliant boot process:

1. **Boot Loader**: Loads kernel image and passes multiboot information
2. **Kernel Entry**: Initializes basic hardware and sets up initial environment
3. **Memory Management**: Sets up paging and heap allocation
4. **Interrupt System**: Initializes interrupt descriptor table and handlers
5. **Device Drivers**: Initializes and registers core device drivers
6. **File System**: Mounts root filesystem and initializes VFS
7. **Process Management**: Initializes scheduler and creates initial processes
8. **System Services**: Starts system services and user applications

## System Call Interface

The kernel provides a Linux-compatible system call interface with:

- File operations (open, read, write, close, etc.)
- Process control (fork, exec, wait, etc.)
- Memory management (mmap, brk, etc.)
- Signal handling (signal, sigaction, etc.)
- Inter-process communication (pipe, socket, etc.)

## Device Driver Architecture

Drivers are implemented using C++ inheritance with a base `DriverBase` class and specialized classes for different device types:

- `DriverBase`: Abstract base class for all drivers
- `BlockDeviceDriver`: For block devices like disks
- `CharacterDeviceDriver`: For character devices like serial ports
- `NetworkDriver`: For network devices
- `UsbDriver`: For USB devices

## File System Architecture

The kernel implements a virtual file system layer that supports:

- Mount point management
- File permission and access control
- File system caching
- Registry-based path translation
- Drive letter assignment system

## Security Model

The kernel implements a security model based on:

- Process isolation through memory protection
- File permissions and access control
- Device driver security contexts
- Registry permission controls
- Module authentication and verification

## Performance Characteristics

The kernel is designed for optimal performance with:

- Efficient scheduling algorithms
- Fast memory management operations
- Optimized interrupt handling
- Minimal system call overhead
- Efficient context switching
- Cache-friendly data structures

## Portability

The kernel is designed to be portable across different architectures through:

- Hardware abstraction layer (HAL)
- Architecture-specific implementations
- Cross-compilation support
- Configuration-driven feature selection
- Platform-independent interfaces

## Compatibility

The kernel maintains compatibility with:

- Linux system calls for application compatibility
- DOS applications through DOS compatibility layer
- Windows-style drive letter assignments
- Standard POSIX interfaces where applicable
- Industry-standard device interfaces