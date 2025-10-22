# Analysis of LittleKernel Structure

## Overview
The LittleKernel is a 32-bit x86 kernel written in C++ that implements basic OS functionality including memory management, task scheduling, interrupt handling, and system calls. The kernel follows the James Molloy OS development tutorial and provides a foundation for understanding kernel development concepts.

## Core Components Analysis

### 1. Global State (SVar)
- Centralized structure containing all kernel state
- Located in `Global.h`
- Contains instances of:
  - DescriptorTable: Handles x86 descriptor tables
  - Monitor: Manages video output
  - Timer: Handles time-based operations
  - Heap: Kernel heap management
  - Interrupt handlers: Array of callbacks for interrupts
  - Task management: Current task, ready queue, etc.
  - File system: Root nodes and initialization data
  - System calls: Array of system call handlers

### 2. Memory Management
- **Heap Implementation** (`Heap.h`/`Heap.cpp`):
  - Dynamic memory allocator with support for aligned allocation
  - Uses a free-list approach with headers and footers
  - Supports page-aligned allocation
  - Has KMemoryAllocate functions for different allocation types
  - Manages contiguous memory blocks with hole tracking

### 3. Task Management
- **Task Structure** (`Task.h`/`Task.cpp`):
  - Process structure with ID, stack pointers, instruction pointer
  - Page directory for memory isolation
  - Kernel stack for system calls
  - Linked list implementation for task queue
  - Supports forking and task switching

### 4. Interrupt System
- **Interrupt Handling** (`Interrupts.h`/`Interrupts.cpp`):
  - Assembly and C handlers for interrupts
  - IRQ definitions for hardware interrupts (IRQ0-IRQ15)
  - Callback system for registering interrupt handlers
  - Supports both software interrupts and hardware IRQs

### 5. File System
- **Virtual File System** (`FileSystem.h`/`FileSystem.cpp`):
  - Abstract file system interface
  - Support for files, directories, character/block devices
  - Directory entity structure
  - Read/Write/Open/Close operations
  - Mount point support

### 6. System Calls
- **System Call Interface** (`Syscall.h`/`Syscall.cpp`):
  - Basic monitor write system calls
  - System call dispatcher
  - User mode switching functionality

### 7. Hardware Abstraction
- **Descriptor Table** (`DescriptorTable.h`/`DescriptorTable.cpp`):
  - Implements x86 GDT (Global Descriptor Table)
  - Handles segmentation and privilege levels
  - Supports user and kernel mode transitions

- **Timer** (`Timer.h`/`Timer.cpp`):
  - Programmable Interrupt Timer (PIT) functions
  - Tick counting and timer interrupts

- **Serial Driver** (`SerialDriver.h`/`SerialDriver.cpp`):
  - Serial communication functions
  - Used for kernel debugging output

### 8. Boot Process
- **Main Function** (`main.cpp`):
  - Entry point after multiboot
  - Initializes descriptor tables
  - Initializes subsystems in sequence:
    - Monitor
    - Serial driver
    - Paging
    - Tasking
    - Initrd (Initial RAM disk)
    - System calls
  - Switches to user mode at the end

### 9. Supporting Library (LittleLibrary)
- **Callback System** (`Callback.h`):
  - Template-based callback implementation
  - Supports callbacks with 0, 1, or 2 arguments
  - Used for interrupt handlers and other event systems
- **Memory Utilities** (`Memory.h`/`Memory.cpp`): 
  - Additional memory management functions

### 10. Build System
- **Ultimate++ Project Files**:
  - `.upp` files for U++ IDE integration
  - Build modules for GCC and Clang (32-bit)

### 11. Key Design Patterns
- Singleton pattern through global SVar structure
- Callback pattern for interrupt handling
- Virtual file system abstraction
- Linked list for task management

### 12. Important Files
- `Global.h`: Contains the global state structure (SVar)
- `main.cpp`: Kernel entry point
- `Kernel.h`: Includes all core kernel headers
- `Defs.h`: Type definitions and common macros
- `Common.h`: Common types and structures
- `LittleKernel.upp`: U++ project definition

## Deprecated Code
- Current implementation includes deprecated flag on `multiboot_main` function
- Uses `#if 0` blocks for disabled code
- Legacy code structure will be replaced with new implementation

## Architecture Notes
- 32-bit x86 architecture support only
- Multiboot compliant bootloader interface
- Memory layout with separate kernel and user spaces
- Implementation follows OS development tutorials but with custom extensions