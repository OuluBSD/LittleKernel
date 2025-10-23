# Kernel Rewrite Tasks

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

## Phase 3: Process Management
- [ ] Implement process scheduler with both cooperative and preemptive modes
- [ ] Create process control block (PCB) structures
- [ ] Implement fork/exec/vfork system calls
- [ ] Implement process synchronization primitives (semaphores, mutexes, events)
- [ ] Implement inter-process communication (pipes, shared memory, signals)
- [ ] Implement process state management

## Phase 4: Memory Management
- [ ] Implement virtual memory management
- [ ] Create page allocation and deallocation systems
- [ ] Implement memory protection between processes
- [ ] Implement shared memory regions
- [ ] Create memory-mapped file support
- [ ] Implement demand paging if needed

## Phase 5: Device Drivers and I/O
- [ ] Create device driver framework
- [ ] Implement basic console/terminal driver
- [ ] Create block device interface for storage
- [ ] Implement keyboard and mouse drivers
- [ ] Design network stack interface (for future expansion)
- [ ] Create driver loading/unloading system

## Phase 6: File System
- [ ] Implement virtual file system layer
- [ ] Create FAT32 driver for DOS compatibility
- [ ] Create basic ext-like file system
- [ ] Implement file system caching
- [ ] Create file system mounting/unmounting system
- [ ] Implement file permissions and access control

## Phase 7: System Calls Interface
- [ ] Implement Linux-compatible system calls (open, read, write, etc.)
- [ ] Create system call dispatcher
- [ ] Implement process control system calls (fork, exec, wait, etc.)
- [ ] Create signal handling system
- [ ] Implement memory management system calls (mmap, brk, etc.)
- [ ] Create networking system calls (for future)

## Phase 8: Testing and Integration
- [ ] Create kernel test suite
- [ ] Implement kernel debugging tools
- [ ] Test with basic applications
- [ ] Performance optimization
- [ ] Stability testing and bug fixes
- [ ] Documentation

## Phase 9: Linux Emulation Layer (Linuxulator)
- [ ] Research FreeBSD's Linuxulator implementation as reference
- [ ] Design Linux system call translation layer
- [ ] Implement basic Linux binary loading and execution
- [ ] Create Linux-compatible ELF loader
- [ ] Implement Linux system call interface mapping to kernel services
- [ ] Design Linux process emulation structure
- [ ] Implement Linux signal handling translation
- [ ] Create Linux-compatible memory management interface
- [ ] Implement Linux file descriptor and VFS translation
- [ ] Add support for Linux-specific features (proc, sysfs, etc.)
- [ ] Test with static Linux executables
- [ ] Implement Linux shared library support
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