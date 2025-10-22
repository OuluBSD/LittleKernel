#ifndef SERIAL_DRIVER_H
#define SERIAL_DRIVER_H

#include "Common.h"

#define SERIAL_PORT_A 0x3F8

class SerialDriver {
private:
    uint16 port;
    
public:
    SerialDriver(uint16 port = SERIAL_PORT_A);
    void Init();
    bool IsTransmitEmpty();
    void WriteChar(char c);
    void Write(const char* str);
    void WriteDec(int i);
    void WriteHex(uint32 i);
};

// Global serial driver instance
extern SerialDriver serial;

#endif