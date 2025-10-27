#ifndef _Kernel_h_
#define _Kernel_h_

// Package header for the New Kernel project
// All .cpp files in this project should include only this header

// Core definitions
#include "Defs.h"
#include "Common.h"

// Process Management
#include "ProcessControlBlock.h"

// System management
#include "Global.h"

// Hardware management
#include "Monitor.h"
#include "Timer.h"
#include "Interrupts.h"
#include "DescriptorTable.h"

// Memory management
#include "MemoryManager.h"
#include "Paging.h"
#include "SharedMemory.h"
#include "MemoryMappedFile.h"
#include "MemoryTracker.h"
#include "DriverFramework.h"
#include "ConsoleDriver.h"
#include "Vfs.h"
#include "RamFs.h"
#include "Registry.h"
#include "Syscalls.h"
#include "TestSuite.h"
#include "Debugging.h"
#include "Performance.h"
#include "StabilityTest.h"
#include "TestApplication.h"
#include "BootDrives.h"
#include "Linuxulator.h"
#include "LinuxSharedLib.h"
#include "DosSyscalls.h"
#include "AbiMultiplexer.h"
#include "SciMultiplexer.h"
#include "DosKpiV2.h"
#include "LinuxulatorAbi.h"
#include "NvidiaSupport.h"
#include "FloppyDriver.h"
#include "FloppyTest.h"

// I/O and drivers
#include "SerialDriver.h"

// System services
#include "Syscall.h"

// Boot and low-level
#include "Multiboot.h"
#include "Initrd.h"

// Configuration
#include "KernelConfig.h"
#include "BootProcess.h"
#include "HAL.h"
#include "RuntimeConfig.h"
#include "HardwareDiagnostics.h"
#include "EarlyMemory.h"
#include "ErrorHandler.h"
#include "Profiler.h"
#include "ModuleLoader.h"
#include "HardwareComponents.h"
#include "ConfigParser.h"
#include "ArchAbstraction.h"
#include "Thread.h"
#include "ProcessGroup.h"
// #include "ProcessDebugging.h"  // Already included above
// #include "RealTimeScheduling.h"  // Already included above
#include "ProcessAccounting.h"
#include "ProcessSuspension.h"
// Add missing driver includes
#include "KeyboardDriver.h"
#include "MouseDriver.h"

// Synchronization
#include "Synchronization.h"

// Inter-Process Communication
#include "Ipc.h"

// Logging
#include "Logging.h"

#endif