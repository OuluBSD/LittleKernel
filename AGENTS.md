# LittleKernel OS Analysis Agents

This file documents the agents that can be used to analyze and work with the LittleKernel OS project.

## Overview

LittleKernel is a 32-bit x86 kernel following the James Molloy OS development tutorial. It implements basic OS functionality including memory management, task scheduling, interrupt handling, and system calls.

## Analysis Tools

### AST Analysis
- **dump_ast.sh**: Creates AST dumps of source files for detailed structural analysis
- **UMKA build system**: Custom build system that compiles the kernel without standard libraries

### UML Diagrams
The following UML diagrams help visualize the kernel architecture:

1. [Main Structures Diagram](LittleKernel_Main_Structures.puml) - Core kernel structures including global state, memory management, and task management
2. [Callback System Diagram](LittleKernel_Callback_System.puml) - Callback and function object system implementation
3. [File System and Interrupts Diagram](LittleKernel_FileSystem_Interrupts.puml) - File system abstractions and interrupt handling

### Diagram Verification
**Important**: All PlantUML files should be regularly checked for syntax errors using the `plantuml` command. When modifying these files, always verify they can be processed without errors by running:
```bash
plantuml <diagram_file.puml>
```

## Code Structure

### Kernel Components
- **SVar (Global.h)**: Central structure containing all kernel state
- **Monitor (Monitor.h)**: Text output and I/O interface
- **Heap (Heap.h)**: Dynamic memory management system
- **Task (Task.h)**: Process/task scheduling and management
- **Interrupts (Interrupts.h)**: Hardware and software interrupt handling
- **Paging (Paging.h)**: Virtual memory management
- **FileSystem (FileSystem.h)**: Basic VFS implementation

### Library Components
- **LittleLibrary**: Supporting utilities including callback system
- **FixedArray**: Compile-time sized array template
- **OrderedArray**: Template for maintaining sorted arrays

## Working with the Project

1. Build: `./build.sh`
2. Run: `./run_qemu.sh`
3. Generate AST: `./dump_ast.sh`
4. Generate documentation: View files in `docs/` directory

## Key Concepts

- **Monolithic kernel design**: All kernel services run in privileged mode
- **Preemptive multitasking**: Time-sliced task scheduling
- **Virtual memory**: Paging-based memory management
- **Interrupt-driven I/O**: Asynchronous hardware handling
- **System calls**: Controlled interface to kernel services

Please refer to QWEN.md for all development guidelines, conventions, and documentation.