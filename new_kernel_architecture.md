# New Kernel Architecture Design

## Overview
This document outlines the architecture for a new kernel that combines Windows 98 features with modern OS concepts. The design maintains compatibility with legacy DOS applications while providing modern OS features like memory protection, multitasking, and hardware abstraction.

## Core Design Principles

### 1. Modularity
- Separate kernel components should be loosely coupled
- Implement a layered architecture with clear interfaces between components
- Support for loadable kernel modules (LKMs)

### 2. Compatibility
- Maintain DOS/Windows 98 compatibility through emulation
- Support for legacy DOS applications via DOS subsystem
- Windows 98 API compatibility layer (Win32K equivalent)

### 3. Performance
- Efficient scheduling and memory management
- Fast context switching
- Minimal overhead for common operations

### 4. Reliability
- Robust error handling and recovery mechanisms
- Memory protection between processes
- Kernel crash resilience where possible

### 5. Ultimate++ Integration
- Designed to work well with the Ultimate++ framework
- Use U++ idioms and patterns where appropriate

## Architecture Overview

### System Architecture Layers
```
+-------------------------------------+
|            Applications             |
+-------------------------------------+
|         System Call Layer           |
+-------------------------------------+
|        OS Subsystems (Win32, DOS)  |
+-------------------------------------+
|        Kernel Services Layer        |
|  (Process, Memory, File, Device)    |
+-------------------------------------+
|          Hardware Layer             |
|      (Interrupts, HAL, etc.)        |
+-------------------------------------+
```

## Key Components

### 1. Process Management (Tasking)

#### New Task Structure
```cpp
struct NewTask {
    int pid;                          // Process ID
    int parent_pid;                   // Parent Process ID
    ProcessState state;               // Running, Waiting, Suspended, etc.
    uint32 esp, ebp;                 // Stack pointers
    uint32 eip;                      // Instruction pointer
    uint32 kernel_stack;             // Kernel stack location
    PageDirectory* page_directory;   // Virtual memory space
    ProcessType type;                // User, Kernel, System, DOS
    Task* next, *prev;               // Linked list pointers
    ProcessContext context;          // CPU registers save area
    std::vector<Thread*> threads;    // Associated threads
    std::vector<Handle> handles;     // Open handles to resources
    SecurityContext security;        // Security tokens and privileges
    ProcessFlags flags;              // Various process flags
};
```

#### Task Scheduler
- Hybrid scheduler supporting both cooperative and preemptive scheduling
- Priority-based scheduling with round-robin for same priority
- Support for Windows 98 compatible scheduling (cooperative for Win16 apps)
- Real-time scheduling support for critical processes

### 2. Memory Management

#### Virtual Memory System
- Demand paging with copy-on-write
- Memory-mapped files
- Shared memory regions
- Support for both 32-bit and future 64-bit addressing
- Memory protection with NX bit and DEP support

#### Heap Management
- Multiple heap managers for different purposes
- Per-process heaps
- Kernel heap for kernel allocations
- Compatibility layer for Windows 98 segmented memory model
- Memory pools for frequently allocated objects

### 3. File System Layer

#### Virtual File System (VFS)
- Support for multiple file system types:
  - FAT32 (for DOS compatibility)
  - NTFS read-only (for Windows compatibility)
  - Custom journaling file system for modern use
- Mount point support
- File system caching and buffering
- POSIX-compliant and Windows API-compatible interfaces

#### Windows 98 File System Features
- Long file name support (with 8.3 compatibility)
- File sharing and locking mechanisms
- Named pipes and mailslots
- Registry equivalent

### 4. Device Driver Framework

#### Driver Model
- Loadable kernel modules (LKMs)
- Device object model similar to Windows NT
- Plug and play support
- Power management
- WDM-like architecture for compatibility

#### Hardware Abstraction Layer (HAL)
- Abstract hardware differences
- Interrupt handling abstraction
- Timer and synchronization primitives
- I/O port access

### 5. System Call Interface

#### Linux Compatibility Layer
- Linux system call interface for running Linux applications
- Linuxulator concept similar to FreeBSD's Linuxulator
- ELF loader for Linux binaries
- Linux-compatible process and memory management

#### Windows 98 API Layer
- Win32 API compatibility layer
- GDI equivalent for graphics
- Kernel32, User32, GDI32 API implementations
- DLL loading and management

#### DOS Compatibility Layer
- DOS API translation layer
- Real-mode emulation for DOS applications
- 16-bit application support
- DOS memory management (conventional, UMB, XMS, EMS)

### 6. Security Model

#### Process Isolation
- Memory protection between processes
- Separate address spaces
- Hardware-enforced protection rings
- Capability-based security for fine-grained access control

#### Access Control
- User and group management
- File and object permissions
- Security identifiers and access tokens

## Windows 98 Specific Features

### Compatibility Mode
- Windows 98 API compatibility layer that translates Win32 calls
- Support for 16-bit applications with virtualization
- Registry implementation to replace Windows 98 registry
- VMM (Virtual Memory Manager) equivalent
- VxD (Virtual Device Driver) support or emulation

### File System Integration
- Long file name support
- Windows 98 file attributes and timestamps
- Windows 98 networking stack compatibility

## Modern OS Features

### Memory Management
- Virtual memory with demand paging
- Copy-on-write for efficient process creation
- Memory-mapped files
- Advanced caching mechanisms

### Process Management
- True multitasking with memory protection
- Thread support with synchronization primitives
- Process groups and sessions
- Inter-process communication (pipes, shared memory, etc.)

### Security
- User authentication
- File and object permissions
- Secure inter-process communication
- Mandatory access control options

### Hardware Support
- Modern hardware abstraction
- Power management
- Hot-plug device support
- Advanced interrupt handling (APIC support)

### Networking
- TCP/IP stack integration
- POSIX socket API
- Network file systems support

## Implementation Strategy

### Phase 1: Core Infrastructure
1. Basic kernel entry point and initialization
2. Memory management (heap allocation, paging)
3. Basic interrupt handling
4. System call interface placeholder

### Phase 2: Process Management
1. Task structure and scheduler
2. Process creation and destruction
3. Context switching
4. Basic IPC mechanisms

### Phase 3: Memory Management
1. Virtual memory system
2. Memory protection
3. Demand paging
4. Shared memory

### Phase 4: File System
1. VFS layer
2. FAT32 implementation
3. File system caching
4. Mount/unmount system

### Phase 5: Windows 98 Compatibility
1. Win32 API compatibility layer
2. DOS subsystem
3. Registry equivalent
4. 16-bit application support

### Phase 6: Linuxulator
1. Linux system call translation
2. ELF loader
3. Linux-compatible process management
4. Linux shared library support

## Technical Considerations

### Performance
- Use of modern CPU features (MMX, SSE, etc.)
- Efficient data structures for kernel operations
- Caching strategies for frequently accessed data

### Reliability
- Kernel memory protection
- Stack overflow detection
- Hardware error handling
- Recovery mechanisms

### Scalability
- Support for multiple CPUs
- Efficient locking mechanisms
- Memory scaling

## U++ Integration

### U++ Library Utilization
- Use of U++ containers where appropriate
- Integration with U++ build system
- Use of U++ idioms and patterns
- Memory management integration

### Framework Compatibility
- Ensure kernel components can work with U++ applications
- Provide U++-style interfaces where appropriate
- Maintain performance with U++ abstractions

## Conclusion

This architecture combines the best of Windows 98's compatibility and user experience with modern OS design principles. The modular design allows for gradual implementation and testing of components, while maintaining the flexibility to add features as needed. The layered approach ensures that legacy applications continue to work while providing modern capabilities to new applications.