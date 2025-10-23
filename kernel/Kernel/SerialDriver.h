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
private:
    uint16 com_port;  // Store the COM port base address
    
public:
    SerialDriver();  // Constructor
    SerialDriver(uint16 port);  // Constructor with specific port
    void Initialize();
    bool IsTransmitEmpty();
    void WriteChar(char c);
    void WriteString(const char* str);
    bool IsReceiveEmpty();
    char ReadChar();
    
    // Higher level functions
    void WriteInteger(int32 value);
    void WriteHex(uint32 value);
    
    // Getter for the COM port
    uint16 GetComPort() const { return com_port; }
};

#endif