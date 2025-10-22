# LittleKernel Class Documentation

## UML Diagrams

Check out the visual representations of these classes in the UML diagrams:

- [LittleKernel Main Structures](LittleKernel_Main_Structures.png) - Shows the core data structures including SVar (global state), Monitor, Task, Heap, and related components
- [LittleKernel Callback System](LittleKernel_Callback_System.png) - Details the callback system implementation from LittleLibrary
- [LittleKernel File System and Interrupts](LittleKernel_FileSystem_Interrupts.png) - Shows filesystem structures and interrupt handling components

## Monitor Class

The Monitor class provides text-based output to the screen and serial port.

### Methods
- `Init()` - Initialize the monitor
- `MoveCursor()` - Update cursor position in video memory
- `Scroll()` - Scroll the display when reaching bottom
- `Put(char c)` - Write a single character to the screen
- `Clear()` - Clear the entire screen to black
- `Write(const char *c)` - Write a null-terminated string
- `WriteDec(int i)` - Write an integer in decimal format
- `WriteHex(void* p)` / `WriteHex(uint32 i)` - Write in hexadecimal format
- `NewLine()` - Move to the next line

### Global Access
The global monitor instance is accessed through the `MON` macro which refers to `global->monitor`.

## Heap Class

The Heap class provides dynamic memory allocation and deallocation in the kernel space.

### Methods
- `Create(uint32 start, uint32 end, uint32 max, uint8 supervisor, uint8 readonly)` - Initialize a new heap
- `Allocate(uint32 size, uint8 page_align)` - Allocate a block of memory
- `Free(void *p)` - Free an allocated block of memory

### Memory Management
- Uses an indexed approach with hole tracking
- Manages both allocated blocks and free holes
- Supports page-aligned allocations
- Maintains an ordered array of headers for efficient searching

### Constants
- `KHEAP_START`: 0xC0000000 - Default heap start address
- `KHEAP_INITIAL_SIZE`: 0x100000 - Initial heap size
- `HEAP_INDEX_SIZE`: 0x20000 - Size of the index array
- `HEAP_MAGIC`: 0x123890AB - Magic number for error checking
- `HEAP_MIN_SIZE`: 0x70000 - Minimum heap size

## Task Structure

The Task struct represents a single process in the system.

### Fields
- `id` - Process ID
- `esp`, `ebp` - Stack and base pointers
- `eip` - Instruction pointer (program counter)
- `page_directory` - Process-specific page directory
- `kernel_stack` - Location of kernel stack
- `next` - Pointer to the next task in the linked list

### Task Management Functions
- `InitialiseTasking()` - Initialize the tasking system
- `TaskSwitch()` - Switch to the next task in the ready queue
- `Fork()` - Create a new process by duplicating current process
- `MoveStack(void *new_stack_start, uint32 size)` - Move the current stack to a new location
- `GetPid()` - Get the process ID of current task
- `SwitchToUserMode()` - Switch execution to user mode

## Interrupt System

The interrupt system handles both software interrupts (ISRs) and hardware interrupts (IRQs).

### Constants
- `IRQ0` to `IRQ15` - Define interrupt vectors for hardware interrupts (32-47)

### Functions
- `RegisterInterruptHandler(uint8 n, Callback1<Registers> handler)` - Register a handler for a specific interrupt
- `ResetInterruptHandlers()` - Reset all interrupt handlers to defaults
- `EnableInterrupts()` / `DisableInterrupts()` - Enable/disable interrupts globally
- `isr_handler(Registers regs)` / `irq_handler(Registers regs)` - Low-level handlers for ISRs and IRQs

### Data Structures
- `Registers` - Structure containing CPU register state when interrupt occurred
- `isr_t` - Function pointer type for interrupt handlers: `typedef void (*isr_t)(Registers)`

## Page Management

The paging system provides virtual memory management.

### Key Components
- Page directories and page tables for virtual-to-physical address translation
- Memory mapping with different protection levels
- Support for both kernel and user space page tables

### Functions
- `InitialisePaging()` - Initialize the paging system
- Various functions for managing page tables and directories

## System Call Interface

The kernel provides a basic system call interface for user-space programs.

### Available System Calls
1. `syscall_MonitorWrite` - Write a string to the monitor
2. `syscall_MonitorWriteHex` - Write a hex value to the monitor
3. `syscall_MonitorWriteDec` - Write a decimal value to the monitor

### System Call Registration
- Stored in `global->syscalls` array
- `num_syscalls` indicates number of available system calls

## File System Interface

Basic filesystem support is provided through a virtual filesystem interface.

### Key Components
- `FsNode` - Represents a node in the filesystem (file or directory)
- `InitrdHeader`, `InitrdFileHeader` - Structures for initial ramdisk
- `InitialiseInitrd()` - Initialize the initial ramdisk

### File Operations
- Read, write, open, close operations
- Directory traversal and listing
- Support for different filesystem types

## Timer Interface

Provides system timer functionality.

### Methods
- `Init(int freq)` - Initialize timer with given frequency
- Support for timer-based task switching and time measurement

## Descriptor Table Management

Manages CPU descriptor tables (Global Descriptor Table and Interrupt Descriptor Table).

### Components
- `DescriptorTable` class with methods to initialize and manage descriptors
- Segment descriptors for code and data segments
- Interrupt gate descriptors for interrupt handlers

## Callback System

The kernel uses a sophisticated callback system from LittleLibrary for flexible function invocation.

### Key Templates
- `Callback1<ARG>` - Callback that accepts one argument
- `CallbackClass1<ARG, T>` - Member function callback
- Used extensively for interrupt handlers and other callback-based systems