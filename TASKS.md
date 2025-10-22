# Kernel Rewrite Tasks

## Phase 1: Analysis and Planning
- [ ] Analyze current LittleKernel structure and identify core components
- [ ] Design new kernel architecture combining Windows 98 features with modern OS concepts
- [ ] Plan system call interface to be Linux-compatible
- [ ] Design memory management system for both kernel and user space
- [ ] Plan process/thread management with cooperative and preemptive options
- [ ] Design device driver interface architecture
- [ ] Plan file system architecture with FAT32/DOS compatibility

## Phase 2: Core Infrastructure
- [ ] Set up new kernel project structure
- [ ] Implement basic kernel entry point and initialization
- [ ] Implement basic memory management (heap allocation)
- [ ] Implement basic interrupt handling
- [ ] Create placeholder system call interface
- [ ] Implement logging infrastructure with LOG macro (stream-like syntax)
- [ ] Write kernel configuration system

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

## Phase 9: Legacy Code Migration
- [ ] Rename multiboot_main to multiboot_main_old (with "deprecated - to be removed" comment)
- [ ] Flag all existing code as deprecated using #if 0 ... #endif if problematic
- [ ] Identify reusable components from old codebase
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