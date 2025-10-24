# LittleKernel Documentation

## Table of Contents
1. [Introduction](#introduction)
2. [Architecture Overview](#architecture-overview)
3. [Building the Kernel](#building-the-kernel)
4. [System Calls](#system-calls)
5. [Driver Framework](#driver-framework)
6. [File System](#file-system)
7. [Process Management](#process-management)
8. [Memory Management](#memory-management)
9. [Debugging Tools](#debugging-tools)
10. [Performance Profiling](#performance-profiling)
11. [Testing Framework](#testing-framework)
12. [Configuration](#configuration)

## Introduction
LittleKernel is a 32-bit x86 kernel written in C++ that implements basic OS functionality including memory management, task scheduling, interrupt handling, and system calls. The kernel follows the James Molloy OS development tutorial and provides a foundation for understanding kernel development concepts.

This document provides comprehensive information about the LittleKernel architecture, design principles, and usage.

## Architecture Overview
### Design Philosophy
LittleKernel follows a modular design with clear separation of concerns between different kernel subsystems. The architecture emphasizes:
- Modularity: Components are loosely coupled for easier maintenance
- Compatibility: Support for both Linux system calls and DOS applications
- Performance: Efficient scheduling and memory management
- Reliability: Robust error handling and recovery
- Ultimate++ Integration: Designed to work well with the Ultimate++ framework

### Core Components
1. **Global State (SVar)**: Centralized structure containing all kernel state
2. **Memory Management**: Heap and paging systems for dynamic memory allocation
3. **Task Management**: Basic process scheduling and context switching
4. **Interrupt System**: Handles hardware and software interrupts
5. **System Calls**: Interface between kernel and user space applications

### Device-Centric Approach
The kernel follows a device-centric design philosophy similar to Windows, where devices are first-class citizens in the system:
- Devices are prominently displayed in the system (like "My Computer", "My Documents")
- Primary storage devices use drive letters (C:, D:, etc.) similar to Windows
- Each device feels like "your own" component of the computer, not just an abstract filesystem entry
- Hierarchical organization rather than a single unified filesystem tree (like Unix)

## Building the Kernel
### Prerequisites
- Ultimate++ development environment
- Toolchain for x86 development
- QEMU for testing (optional)

### Build Process
1. Ensure all dependencies are installed
2. Navigate to the project root directory
3. Run `./build.sh` to compile the kernel
4. The resulting kernel image will be created

### Configuration
The kernel uses a Linux-style configuration system with `.config` file and `make menuconfig` for interactive configuration.

## System Calls
LittleKernel implements a Linux-compatible system call interface with the following major categories:

### File Operations
- `open`, `close`: Open and close files
- `read`, `write`: Read from and write to files
- `lseek`: Set file position indicator
- `stat`, `fstat`: Get file status information

### Process Operations
- `fork`: Create a copy of the current process
- `execve`: Execute a program
- `waitpid`: Wait for a child process to change state
- `getpid`, `exit`: Get process ID and exit process
- `kill`: Send a signal to a process

### Memory Management
- `mmap`, `munmap`: Map or unmap files or devices into memory
- `brk`: Change the location of the program break

### System Information
- `uname`: Get system information
- `gettimeofday`: Get time of day

## Driver Framework
### Overview
The kernel provides a comprehensive driver framework with a hierarchical class structure:

- `DriverBase`: Abstract base class for all drivers
- `BlockDeviceDriver`: For block devices like disks
- `CharacterDeviceDriver`: For character devices like serial ports
- `NetworkDriver`: For network devices
- `UsbDriver`: For USB devices

### Device Registration
All devices must be registered with the `DriverFramework` which manages the device lifecycle and provides a unified interface for device operations.

### Driver Loading
The kernel supports dynamic loading and unloading of drivers through the `DriverLoader` system.

## File System
### Virtual File System (VFS)
The kernel implements a VFS layer that provides a unified interface to different file systems. The VFS supports:
- Mounting and unmounting of different file systems
- File permissions and access control
- File system caching

### Supported File Systems
- FAT32: For DOS compatibility
- RAMFS: Initial RAM filesystem for boot configuration (A: drive)
- Registry-based path translation: Windows-compatible drive letter system

### Drive Letter System
- A: Drive: Initial RAM filesystem and boot configuration system (like /boot in Linux)
- B: Drive: EFI partition system when enabled (like /boot/efi in Linux)
- C: Drive: Primary storage device with pagefile.sys-like swap functionality

## Process Management
### Process Control Block (PCB)
Each process is represented by a PCB containing:
- Process ID (PID)
- Process state (running, waiting, etc.)
- Priority level
- Memory management information
- Register state for context switching

### Scheduling
The kernel supports multiple scheduling modes:
- Cooperative: Processes yield control explicitly
- Preemptive: Scheduler forcibly switches processes
- Round-robin: Time-sliced scheduling
- Real-time: For time-critical processes

### Inter-Process Communication (IPC)
The kernel provides several IPC mechanisms:
- Pipes: Unidirectional data flow between processes
- Shared memory: Memory regions shared between processes
- Signals: Asynchronous notifications between processes

## Memory Management
### Virtual Memory
The kernel provides virtual memory management with:
- Page-based memory allocation
- Memory protection between processes
- Demand paging capabilities

### Memory Allocation
- Kernel heap for dynamic memory allocation
- Page allocation for large memory blocks
- Memory-mapped file support

### Memory Protection
- Memory isolation between processes
- Read/write/execute permission controls
- Address space layout randomization (ASLR)

## Debugging Tools
### Kernel Debugger
The kernel includes a comprehensive debugging system with:
- Breakpoint management (execution, read, write, access)
- Memory inspection and modification
- Stack trace capabilities
- System state dumping
- Crash dump functionality

### Logging System
- Stream-like syntax for logging with `LOG()` macro
- Debug-specific logging with `DLOG()` macro
- Conditional logging based on debug flags

### Panic and Crash Handling
- Kernel panic function with system state dump
- Error reporting with file and line information
- Safe shutdown procedures

## Performance Profiling
### Performance Counters
The kernel includes performance profiling with:
- Time-based counters for measuring execution time
- Count-based counters for tracking events
- Memory usage monitoring
- Predefined counters for key kernel operations

### Optimization Utilities
- Scheduler optimization
- Memory management optimization
- Interrupt handling optimization
- Process switching optimization

## Testing Framework
### Test Suite
The kernel includes a comprehensive test suite with:
- Test registration and execution system
- Individual test functions for core kernel components
- Result tracking and reporting

### Stability Testing
- Memory stress testing
- Process creation/destruction stress testing
- Filesystem operation stress testing
- System state validation after tests

## Configuration
### Kernel Configuration
The kernel uses a Linux-style configuration system with:
- `.config` file to store kernel configuration options
- `make menuconfig` for interactive configuration
- Configuration values written as C++ defines to headers
- Makefile for building with various configuration options

### Runtime Configuration
The kernel supports runtime configuration changes through the registry system.

---

*This documentation is automatically generated from the kernel source code and represents the current state of the LittleKernel project.*