#ifndef _Kernel_SerialDriver_h_
#define _Kernel_SerialDriver_h_

// Don't include other headers in this file - only the package header should include other headers

#define SERIAL_COM1_BASE 0x3F8
#define SERIAL_COM2_BASE 0x2F8
#define SERIAL_COM3_BASE 0x3E8
#define SERIAL_COM4_BASE 0x2E8

#define SERIAL_DATA(port)              (port)
#define SERIAL_FIFO_COMMAND(port)      (port + 2)
#define SERIAL_LINE_COMMAND(port)      (port + 3)
#define SERIAL_MODEM_COMMAND(port)     (port + 4)
#define SERIAL_LINE_STATUS(port)       (port + 5)

class SerialDriver {
public:
    static void Initialize();
    static bool IsTransmitEmpty();
    static void WriteChar(char c);
    static void WriteString(const char* str);
    static bool IsReceiveEmpty();
    static char ReadChar();
    
    // Higher level functions
    static void WriteInteger(int32 value);
    static void WriteHex(uint32 value);
};

#endif