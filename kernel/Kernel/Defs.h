#ifndef _Kernel_Defs_h_
#define _Kernel_Defs_h_

// Type definitions following Ultimate++ conventions

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long long uint64;

typedef signed char  int8;
typedef signed short int16;
typedef signed int   int32;
typedef signed long long int64;

// Boolean type
typedef unsigned char bool;
#define true 1
#define false 0

// Null pointer
#define NULL 0

// Common constants
#define PAGE_SIZE 4096
#define SECTOR_SIZE 512

// Memory layout
#define KERNEL_VIRTUAL_BASE 0xC0000000
#define KERNEL_HEAP_START 0xD0000000
#define KERNEL_HEAP_SIZE 0x1000000  // 16MB

// Useful macros
#define HIGH_BYTE_OF_WORD(x) ((x & 0xFF00) >> 8)
#define LOW_BYTE_OF_WORD(x)  (x & 0x00FF)
#define CONCAT_BYTES(h, l)   ((h << 8) | l)

// Alignment macros
#define ALIGN_UP(addr, size)   (((addr) + (size) - 1) & ~((size) - 1))
#define ALIGN_DOWN(addr, size) ((addr) & ~((size) - 1))

// Bit manipulation macros
#define SET_BIT(value, bit)    ((value) |= (1 << (bit)))
#define CLEAR_BIT(value, bit)  ((value) &= ~(1 << (bit)))
#define TEST_BIT(value, bit)   (((value) >> (bit)) & 1)

#endif