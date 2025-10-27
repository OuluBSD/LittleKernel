/*
 * FloppyTest.h - Header file for floppy disk driver test functions
 */

#ifndef FLOPPYTEST_H
#define FLOPPYTEST_H

#include "Common.h"
#include "Defs.h"

// Test function declarations
int TestFloppyDriver();
extern "C" int run_floppy_tests();

#endif // FLOPPYTEST_H