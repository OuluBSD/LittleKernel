# LittleKernel Documentation

LittleKernel is a 32-bit x86 kernel written in C++ that follows the James Molloy OS development tutorial. It provides basic kernel functionality including memory management, process management, interrupt handling, and basic I/O.

## Project Structure

```
LittleKernel/
├── alt/                    # Alternative GUI framework components
├── kernel/
│   ├── LittleKernel/      # Main kernel implementation
│   │   ├── Common.h       # Basic type definitions and common utilities
│   │   ├── Kernel.h       # Main kernel header including all components
│   │   ├── Global.h       # Global kernel state (SVar structure)
│   │   ├── Monitor.h      # Text display and I/O interface
│   │   ├── DescriptorTable.h # CPU descriptor tables (GDT/IDT)
│   │   ├── Heap.h         # Memory management system
│   │   ├── Paging.h       # Virtual memory management
│   │   ├── Task.h         # Process/task management
│   │   ├── Timer.h        # System timer
│   │   ├── Interrupts.h   # Interrupt handling
│   │   ├── FileSystem.h   # Basic filesystem support
│   │   ├── Syscall.h      # System call interface
│   │   ├── OrderedArray.h # Template for ordered arrays
│   │   ├── Initrd.h       # Initial ramdisk support
│   │   └── main.cpp       # Kernel entry point
│   └── LittleLibrary/     # Supporting utility library
│       ├── LittleLibrary.h # Main library header
│       ├── Callback.h     # Function object implementation
│       ├── FixedArray.h   # Compile-time sized array template
│       ├── Memory.h       # Memory utilities
│       └── icxxabi.h      # C++ ABI implementation
├── build.sh              # Build script
├── run_qemu.sh           # Run in QEMU emulator
├── run_bochs.sh          # Run in Bochs emulator
└── ...
```

## Core Components

### Global State (SVar)
The SVar struct contains all global kernel state:

## UML Diagrams

The following UML diagrams are available in the docs directory:

### Architecture Diagrams
- [LittleKernel Main Structures](LittleKernel_Main_Structures.png) - Core kernel structures
- [LittleKernel Callback System](LittleKernel_Callback_System.png) - Callback system implementation
- [LittleKernel File System and Interrupts](LittleKernel_FileSystem_Interrupts.png) - File system and interrupt handling

### Function Flow Diagrams
- [InitialiseTasking Activity Flow](InitialiseTasking_Flow.png) - Activity diagram of the InitialiseTasking function
- [InitialiseTasking Call Flow](InitialiseTasking_Call_Flow.png) - Call sequence for InitialiseTasking
- [Main Execution Flow](Main_InitialiseTasking_Flow.png) - Main execution flow showing where InitialiseTasking is called

```cpp
struct SVar {
    DescriptorTable dt;          // Descriptor tables (GDT/IDT)
    Monitor monitor;             // Text display interface
    Timer timer;                 // System timer
    Heap kheap;                  // Kernel heap
    uint32 *frames;              // Frame management
    uint32 nframes;              // Number of frames
    PageDirectory* kernel_directory;  // Kernel page directory
    PageDirectory* current_directory; // Current page directory
    uint32 placement_address;    // Current memory allocation point
    FixedArray<Callback1<int>, 1> cbtestarr;            // Test callback array
    FixedArray<Callback1<Registers>, 256> interrupt_handlers; // Interrupt handlers
    // Initrd fields for initial ramdisk
    // Task management fields
    volatile Task *current_task;    // Currently running task
    volatile Task *ready_queue;     // Task ready queue
    uint32 next_pid;                // Next process ID to assign
    void *syscalls[3];              // System call table
    uint32 num_syscalls;            // Number of system calls
};
```

### Monitor Class
Handles text output to screen and serial port:

- `Put(char c)` - Write a single character
- `Write(const char *c)` - Write a string
- `Clear()` - Clear the screen
- `WriteHex()`, `WriteDec()` - Number formatting
- `MoveCursor()`, `Scroll()` - Screen cursor management

### Heap Class
Dynamic memory management system:

- `Allocate(uint32 size, uint8 page_align)` - Allocate memory
- `Free(void *p)` - Free memory
- Uses a sophisticated algorithm with hole tracking
- Supports both page-aligned and regular allocations

### Task Management
Provides basic process management:

- `InitialiseTasking()` - Initialize tasking system
- `TaskSwitch()` - Switch between processes
- `Fork()` - Fork the current process
- `SwitchToUserMode()` - Switch to user mode execution

### Interrupt System
Handles hardware interrupts:

- `RegisterInterruptHandler()` - Register interrupt handlers
- Supports both software interrupts (ISRs) and hardware interrupts (IRQs)
- Provides callback-based interrupt handling

### Paging System
Virtual memory management:

- Page directory and page table management
- Memory mapping and protection
- Supports both kernel and user page directories

### System Calls
Basic system call interface:

- `syscall_MonitorWrite` - Write to monitor
- `syscall_MonitorWriteHex` - Write hex to monitor
- `syscall_MonitorWriteDec` - Write decimal to monitor

## Build System

The project uses a custom build system via the UMKA framework:

- `build.sh` - Main build script
- Build method files (.bm) define compilation parameters
- Cross-compilation for 32-bit x86 with no standard libraries
- Custom linker script (link.ld)

## Running the Kernel

- Use `run_qemu.sh` to run in QEMU emulator
- Use `run_bochs.sh` to run in Bochs emulator
- Kernel expects multiboot-compliant bootloader

## Dependencies

- GCC cross-compiler for 32-bit x86 targets
- QEMU or Bochs emulator
- UMKA build framework (https://github.com/OuluBSD/umka)