#ifndef _Kernel_h_
#define _Kernel_h_

// Package header for the New Kernel project
// All .cpp files in this project should include only this header

// System headers (if needed)
#include <stdarg.h>

// Core definitions
#include "Defs.h"
#include "Common.h"

// System management
#include "Global.h"

// Hardware management
#include "Monitor.h"
#include "Timer.h"
#include "Interrupts.h"
#include "DescriptorTable.h"

// Memory management
#include "MemoryManager.h"

// I/O and drivers
#include "SerialDriver.h"

// System services
#include "Syscall.h"

// Boot and low-level
#include "Multiboot.h"
#include "Initrd.h"

// Logging
#include "Logging.h"

#endif