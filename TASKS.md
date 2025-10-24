# Kernel Rewrite Tasks

## Kernel configuration
- [ ] Use /usr/bin/dialog for configuring kernel compilation (when calling make menuconfig)

## Project Restructure Completed:
- [x] Renamed old kernel directory to kernel_old (read-only)
- [x] Created new kernel structure: kernel/{Kernel,Library}
- [x] Created new U++ project files: kernel/Kernel/Kernel.upp and kernel/Library/Library.upp
- [x] Put kernel_old in read-only state for reference only

## Phase 1: Analysis and Planning
- [x] Analyze current LittleKernel structure in kernel_old and identify core components
- [x] Design new kernel architecture combining Windows 98 features with modern OS concepts
- [x] Plan system call interface to be Linux-compatible
- [x] Design memory management system for both kernel and user space
- [x] Plan process/thread management with cooperative and preemptive options
- [x] Design device driver interface architecture
- [x] Plan file system architecture with FAT32/DOS compatibility
- [x] Identify and resolve kernel rapid reboot issue in run.sh (interrupt handler setup order)

## Phase 1: Analysis and Planning - Optional Enhancements
- [ ] Architecture Documentation: Create detailed architectural diagrams and documentation for the new kernel design decisions
- [ ] Requirements Traceability: Document how each new feature addresses specific requirements identified in the analysis phase
- [ ] Risk Assessment: Add detailed risk analysis for each major component design decision
- [ ] Performance Benchmarks: Define performance goals and metrics based on the analysis of existing systems
- [ ] Security Analysis: Conduct a security analysis of the planned architecture to identify potential vulnerabilities early
- [ ] Business Plan: Conduct comprehensive market analysis of existing OS solutions and define business models for the kernel
- [ ] C++ Inheritance for Drivers and System Modules: Create comprehensive base Driver class with common functionality and inheritance hierarchies
- [ ] Microkernel Modularity Perspectives: Design component-based architecture to support microkernel implementation for improved marketplace desirability

## Solution Summary for Kernel Reboot Issue:
The kernel rapid reboot issue was caused by timer interrupts firing before interrupt handlers were registered. The fix involved ensuring that all interrupt handlers were set up before enabling interrupts in the main.cpp initialization sequence. This prevents unhandled timer interrupts that were causing the system to reset approximately every 10ms.

## Phase 2: Core Infrastructure
- [x] Set up new kernel project structure in kernel/Kernel
- [x] Implement basic kernel entry point and initialization
- [x] Implement basic memory management (heap allocation)
- [x] Implement basic interrupt handling
- [x] Create placeholder system call interface
- [x] Implement logging infrastructure with LOG macro (stream-like syntax)
- [x] Successfully build the kernel
- [x] Fix kernel reboot issue (correct interrupt handler setup order)
- [x] Implement proper initialization sequence (paging before interrupts)
- [x] Write kernel configuration system

## Phase 2: Core Infrastructure - Optional Enhancements
- [ ] Enhanced Boot Process: Implement a more sophisticated multiboot-compliant boot process with configuration loading
- [ ] Hardware Abstraction Layer: Create a more comprehensive HAL to improve portability
- [ ] Advanced Configuration System: Expand the configuration system to support runtime configuration changes
- [ ] Boot-Time Diagnostics: Add hardware detection and diagnostics during the initialization phase
- [ ] Memory Initialization Improvements: Implement more sophisticated early memory management before heap initialization
- [ ] Error Handling Framework: Create structured error handling mechanisms for initialization failures
- [ ] Kernel Profiling Infrastructure: Add basic profiling capabilities to measure system performance
- [ ] Module Loading System: Design a framework for dynamically loading kernel modules
- [ ] C++ Inheritance in HAL: Create abstract base classes for hardware components with architecture-specific implementations
- [ ] Linux-Style Configuration System: Implement .config file and make menuconfig for kernel configuration
- [ ] Build System Integration: Implement Makefile that works with .config system and integrates with build.sh
- [ ] Configuration Header Generation: Write configuration as C++ defines to kernel header from .config file
- [ ] Project File Synchronization: Keep UPP project files and Makefile synchronized with new/deleted files
- [ ] Git Ignore Management: Maintain .gitignore to prevent build residues from dirtying the repository

## Phase 3: Process Management
- [x] Implement process scheduler with both cooperative and preemptive modes
- [x] Create process control block (PCB) structures
- [x] Implement fork/exec/vfork system calls
- [x] Implement process synchronization primitives (semaphores, mutexes, events)
- [x] Implement inter-process communication (pipes, shared memory, signals)
- [x] Implement process state management

## Phase 3: Process Management - Optional Enhancements
- [x] Thread Implementation: Add lightweight thread support in addition to process management
- [ ] Process Priority Scheduling: Implement more sophisticated scheduling algorithms (Round-robin, Priority-based, etc.)
- [ ] Process Groups and Sessions: Add support for process groups and session management
- [ ] Real-time Scheduling: Add real-time process scheduling capabilities
- [ ] Process Accounting: Implement process resource accounting and monitoring
- [ ] Process Debugging Support: Add basic debugging capabilities for processes
- [ ] Process Suspension/Resumption: Implement process suspension and resumption functionality
- [ ] Process Migration: Add support for moving processes between different execution contexts
- [ ] Priority Inheritance: Implement priority inheritance for synchronization primitives to avoid priority inversion
- [ ] Process Resource Limits: Add support for controlling resource usage per process
- [ ] CPU Affinity: Implement CPU affinity for processes to control which CPUs they run on
- [ ] Solaris Exclusive Features - Process Tracing (ptrace): Implement ptrace functionality for process debugging and control
- [ ] Kernel Registry: Implement kernel-side registry system with permission controls
- [ ] Registry Permissions: Implement secure registry access with per-module permissions
- [ ] Registry API: Create API for modules to safely access registry with proper permissions
- [ ] Registry Security: Prevent modules from reading/writing unauthorized registry sections

## Phase 4: Memory Management
- [x] Implement virtual memory management
- [x] Create page allocation and deallocation systems
- [x] Implement memory protection between processes
- [x] Implement shared memory regions
- [x] Create memory-mapped file support
- [x] Implement demand paging if needed

## Phase 4: Memory Management - Optional Enhancements
- [ ] Memory Leak Detection and Prevention: Add reference counting for shared memory regions, implement garbage collection for unused memory pages, and create memory allocation tracking system to detect memory leaks
- [ ] Enhanced Demand Paging: Add support for page swapping to disk when physical memory is low, implement LRU (Least Recently Used) or other page replacement algorithms, and add page aging and dirty bit tracking for better page management
- [ ] Memory Protection Enhancements: Add more granular permission controls (read, write, execute), implement ASLR (Address Space Layout Randomization) for security, and add memory access validation and bounds checking
- [ ] Performance Optimizations: Implement TLB (Translation Lookaside Buffer) flush optimization, add support for large pages (4MB) for better performance, and optimize page fault handling to reduce overhead
- [ ] Memory-Mapped Files Enhancement: Add proper file-backed page caching, implement proper synchronization when multiple processes access the same file mapping, and add support for different file mapping types (private vs shared)
- [ ] Memory Pool Allocation: Create fixed-size memory pools for common allocations, implement slab allocation for frequently used objects, and add memory cache for frequently allocated/deallocated pages
- [ ] Virtual Memory Extensions: Add support for memory-mapped I/O operations, implement memory protection keys (if supported by hardware), and add support for huge TLB support for large allocations
- [ ] Debugging and Monitoring: Add memory usage tracking and reporting, implement memory leak detection tools, and create visualization of memory layout and usage

## Phase 5: Device Drivers and I/O
- [x] Create device driver framework
- [x] Implement basic console/terminal driver
- [x] Create block device interface for storage
- [x] Implement keyboard and mouse drivers
- [x] Design network stack interface (for future expansion)
- [x] Create driver loading/unloading system

## Phase 5: Device Drivers and I/O - Optional Enhancements
- [ ] Microkernel Module: Implement ability to compile and load modules via serial connection
- [ ] Module Authentication: Add cryptographic signature verification for loaded modules
- [ ] Dynamic Linking Support: Implement dynamic linking for serial-loaded modules
- [ ] Module Dependency Management: Create system for managing dependencies between modules
- [ ] Module Hot-Swapping: Enable loading/unloading of modules without system restart
- [ ] Module Security Context: Implement security contexts for loaded modules
- [ ] Module Resource Management: Track and limit resources used by loaded modules
- [ ] Module Communication Channel: Create dedicated communication channels for modules
- [ ] Serial Module Loader: Implement module loading via serial connection with proper error checking

## Phase 6: File System
- [x] Implement virtual file system layer
- [x] Create FAT32 driver for DOS compatibility
- [ ] Import UFS from FreeBSD
- [ ] Import BTRFS from Linux
- [x] Implement file system caching
- [x] Create file system mounting/unmounting system
- [x] Implement file permissions and access control
- [x] A: Drive Implementation: Create initial RAM filesystem and boot configuration system (like /boot in Linux)
- [x] B: Drive Implementation: Create EFI partition system when enabled (like /boot/efi in Linux)
- [x] C: Drive Swap File: Implement pagefile.sys-like swap functionality on primary drive
- [x] Drive Letter Management: Implement Windows-compatible drive letter assignment system
- [x] Registry-Based Path Translation: Implement registry-based translation of device paths

## Phase 7: System Calls Interface
- [x] Implement Linux-compatible system calls (open, read, write, etc.)
- [x] Create system call dispatcher
- [x] Implement process control system calls (fork, exec, wait, etc.)
- [x] Create signal handling system
- [x] Implement memory management system calls (mmap, brk, etc.)
- [ ] Create networking system calls (for future)

## Phase 8: Testing and Integration
- [x] Create kernel test suite
- [x] Implement kernel debugging tools
- [x] Test with basic applications
- [x] Performance optimization
- [x] Stability testing and bug fixes
- [x] Documentation

## Phase 8: Testing and Integration - Optional Enhancements
- [ ] GDB Integration: Implement GDB stub in kernel to enable external debugging using Bochs' GDB support
- [ ] Remote Debugging Protocol: Implement custom remote debugging protocol for better kernel inspection
- [ ] Debug Symbols Support: Add support for debug symbols and backtrace functionality
- [ ] Kernel Memory Inspector: Create debugging tools to inspect kernel memory structures
- [ ] Breakpoint Framework: Implement software and hardware breakpoint handling
- [ ] Watchpoint Support: Add memory watchpoint capabilities for debugging
- [ ] Kernel Profiling Interface: Integrate with profiling tools for performance analysis
- [ ] Crash Analysis Tools: Create tools for post-mortem crash analysis
- [ ] GUI Monitor Application: Create external GUI application that connects via serial to visualize kernel state in real-time
- [ ] Real-time Process Visualization: Create GUI that shows live process states and relationships
- [ ] System Resource Monitor: Visualize CPU, memory, I/O usage in real-time in the GUI
- [ ] UML Diagram Generation: Generate and update UML diagrams showing kernel components in the GUI
- [ ] Event Timeline: Show timeline of kernel events and system activity in the GUI
- [ ] Memory Layout Visualizer: Display real-time memory layout with allocated regions in the GUI
- [ ] Serial Communication Protocol: Define robust serial protocol for kernel-to-GUI communication
- [ ] Event Logging and Playback: Record system events and allow playback/replay functionality in the GUI
- [ ] QEMU Integration: Integrate GUI with QEMU's monitor interface for advanced control
- [ ] Data Export: Export system data in various formats for further analysis from the GUI
- [ ] Remote System Monitoring: Allow monitoring of remote kernel instances via the GUI
- [ ] Alert and Notification System: Create alerts for system anomalies or critical events in the GUI

## Phase 9: Linux Emulation Layer (Linuxulator)
- [x] Research FreeBSD's Linuxulator implementation as reference
- [x] Design Linux system call translation layer
- [x] Implement basic Linux binary loading and execution
- [x] Create Linux-compatible ELF loader
- [x] Implement Linux system call interface mapping to kernel services
- [x] Design Linux process emulation structure
- [x] Implement Linux signal handling translation
- [x] Create Linux-compatible memory management interface
- [x] Implement Linux file descriptor and VFS translation
- [x] Add support for Linux-specific features (proc, sysfs, etc.)
- [x] Test with static Linux executables
- [x] Implement Linux shared library support
- [ ] Bootstrap CentOS userspace environment
- [ ] Integrate Gentoo Prefix compilation support
- [ ] Add X11 forwarding support for Linux applications
- [ ] Implement Sound support for Linux applications
- [ ] Optimize Linuxulator performance
- [ ] Test with real-world Linux applications
- [ ] Document Linuxulator usage and limitations

## Phase 10: Legacy Code Migration
- [ ] Rename multiboot_main to multiboot_main_old (with "deprecated - to be removed" comment)
- [ ] Flag all existing code as deprecated using #if 0 ... #endif if problematic
- [ ] Identify reusable components from old codebase in kernel_old
- [ ] Gradually replace old components with new implementations
- [ ] Verify that new components work correctly with Ultimate++ framework
- [ ] Remove old code once new implementations are stable

## Phase 10: DOS-Shell Integration
- [ ] Implement DOS-compatible system call layer
- [ ] Support legacy DOS applications
- [ ] Create DOS file system compatibility layer
- [ ] Implement DOS interrupt system (for compatibility)
- [ ] Test with existing DOS-shell application
- [ ] Optimize for DOS application performance

## Phase 11: Virtio Device Support
### Basic Virtio Devices (Without GPU/OpenGL)
- [ ] Implement virtio-block driver for storage devices
- [ ] Implement virtio-net driver for network devices
- [ ] Implement virtio-console driver for terminal I/O
- [ ] Implement virtio-serial driver for communication
- [ ] Implement virtio-input drivers (keyboard, mouse)
- [ ] Implement virtio-rng driver for random number generation
- [ ] Implement virtio-balloon driver for memory management
- [ ] Create unified device driver framework for virtio devices
- [ ] Implement virtio-mmio driver for ARM platforms

### Advanced Virtio Devices (GPU/OpenGL Support)
- [ ] Implement virtio-gpu driver for basic graphics
- [ ] Implement virtio-gpu-gl driver for OpenGL acceleration
- [ ] Design GPU resource management system
- [ ] Implement 2D/3D acceleration support
- [ ] Add multimedia support (audio, video)
- [ ] Implement virtualized display and rendering

## Architecture Notes:

The new kernel should follow these principles:
1. **Modularity**: Components should be loosely coupled for easier maintenance
2. **Compatibility**: Support for both Linux system calls and DOS applications
3. **Performance**: Efficient scheduling and memory management
4. **Reliability**: Robust error handling and recovery
5. **Ultimate++ Integration**: Designed to work well with the Ultimate++ framework

## Logging Standards:

All new code must use the LOG() macro with stream-like syntax for consistency:
- Use: LOG("Message with value " << value << " and more text")
- Do NOT use: GenericWrite() functions directly
- The LOG macro should handle automatic newlines
- For debugging, use DLOG() which adds "[DEBUG]" prefix

## Memory Layout:
- 0x00000000 - 0x000FFFFF: Reserved (NULL pointer detection, interrupt vectors)
- 0x00100000 - 0x0FFFFFFF: Kernel space
- 0x10000000 - 0xBFFFFFFF: User space (per process)
- 0xC0000000 - 0xFFFFFFFF: Shared kernel space (page tables, etc.)

## Development Strategy:
- Work incrementally, implementing and testing one component at a time
- Maintain compatibility with Ultimate++ framework
- Use existing proven algorithms where possible
- Focus on stability and maintainability

## Solaris Exclusive Features - Optional Enhancements
- [ ] Process Tracing (ptrace): Implement ptrace functionality for process debugging and control
- [ ] DTrace-like Instrumentation: Create in-kernel dynamic tracing capabilities
- [ ] Zones/Containers Support: Implement lightweight virtualization similar to Solaris Zones
- [ ] Solaris IPC Mechanisms: Implement Solaris-specific IPC mechanisms (doors, extended file attributes)
- [ ] Adaptive Mutex: Implement adaptive mutex mechanisms that switch between spinning and sleeping
- [ ] Kernel Memory Pools: Implement Solaris-style memory pools for efficient allocation
- [ ] Fair Share Scheduler: Implement resource sharing scheduler similar to Solaris FSS

## Development Tooling - Optional Enhancements
- [ ] GUI Monitor Application Outside QEMU: Create GUI application that connects via serial to visualize kernel state
- [ ] Real-time Process Visualization: Show live process states and relationships in external GUI
- [ ] System Resource Monitor: Visualize CPU, memory, I/O usage in real-time in external GUI
- [ ] UML Diagram Generation: Generate and update UML diagrams showing kernel components in GUI
- [ ] Event Timeline: Show timeline of kernel events and system activity in GUI
- [ ] Memory Layout Visualizer: Display real-time memory layout with allocated regions in GUI
- [ ] QEMU Integration: Allow the GUI application to run QEMU and proxy its output
- [ ] Interactive Kernel Control: Allow some interactive control of kernel operations through the GUI
- [ ] Process Tree Visualization: Show parent-child relationships between processes in GUI
- [ ] Linux-Style Menuconfig: Implement make menuconfig interface for kernel configuration
- [ ] Configuration Validation: Create tool to validate .config file options
- [ ] Build Script Integration: Integrate .config system with build.sh for C++ header generation
- [ ] Project File Synchronization: Tool to keep UPP project files synchronized with Makefile
- [ ] Regedit Tool: Create registry editor for managing kernel registry
- [ ] Configuration Documentation: Generate documentation from configuration options## Design Philosophy: Device-Centric Approach

- [ ] Implement drive letter system (C:, D:, etc.) for primary storage devices
- [ ] Create "My Computer" system concept for device visibility
- [ ] Design device manager interface for organizing hardware components
- [ ] Implement container virtualization that appears as "My Computer"-like devices; they feel like real devices to users despite being virtualized
- [ ] Create hierarchical device organization instead of flat Unix-like filesystem
- [ ] Design virtual devices that appear as regular system devices to users
- [ ] Implement "My Documents", "My Pictures", etc. user-centric directory structure
- [ ] Create network drive mapping system similar to Windows
- [ ] Design device permission system that emphasizes ownership ("My" devices)
- [ ] Implement device status monitoring and health reporting
- [ ] Create unified device interface for both physical and virtual devices
- [ ] Design device topology visualization showing relationships between components
- [ ] A: Drive - Initial RAM filesystem and boot configuration (like /boot in Linux)
- [ ] B: Drive - EFI partition system (when enabled, like /boot/efi in Linux)
- [ ] C: Drive - Pagefile.sys-like swap functionality on primary drive
- [ ] Windows-Compatible Drive Mapping: Implement Windows-like drive letter assignments
