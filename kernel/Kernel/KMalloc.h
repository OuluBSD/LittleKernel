#ifndef _Kernel_KMalloc_h_
#define _Kernel_KMalloc_h_

#include "MemoryManager.h"

// Kernel memory allocation functions (aliases to standard functions)
#define kmalloc(size) malloc(size)
#define kfree(ptr) free(ptr)

#endif