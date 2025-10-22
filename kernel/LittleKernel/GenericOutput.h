#ifndef GENERIC_OUTPUT_H
#define GENERIC_OUTPUT_H

#include "Common.h"
#include "Monitor.h"

void init_serial();

// Generic output function that writes to both monitor and serial
void GenericWrite(const char* str, bool newline=false);
void GenericWriteDec(int i);
void GenericWriteHex(uint32 i);

#endif