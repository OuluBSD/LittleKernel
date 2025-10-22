#include "SerialDriver.h"
#include "Common.h"

SerialDriver serial(SERIAL_PORT_A);

SerialDriver::SerialDriver(uint16 port) {
    this->port = port;
}

void SerialDriver::Init() {
    outb(port + 1, 0x00);    // Disable all interrupts
    outb(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(port + 1, 0x00);    //                  (hi byte)
    outb(port + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

bool SerialDriver::IsTransmitEmpty() {
    return inb(port + 5) & 0x20;
}

void SerialDriver::WriteChar(char c) {
    while (!IsTransmitEmpty()) ;
    outb(port, c);
}

void SerialDriver::Write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        WriteChar(str[i]);
    }
}

void SerialDriver::WriteDec(int i) {
    if (i < 0) {
        WriteChar('-');
        i *= -1;
    }
    
    bool foundfirst = false;
    for (int j = 9; j >= 0; j--) {
        int pow = Pow10(j);
        int k = i / pow;
        i = i % pow;
        if (k < 0)
            k *= -1;
        if (!foundfirst && k)
            foundfirst = true;
        if (foundfirst)
            WriteChar('0' + k);
    }
    if (!foundfirst)
        WriteChar('0');
}

void SerialDriver::WriteHex(uint32 i) {
    Write("0x");
    for (int j = 0; j < 8; j++) {
        int k = (i & 0xF0000000) >> (32 - 4);
        if (k < 10)
            WriteChar('0' + k);
        else
            WriteChar('A' + k - 10);
        i = i << 4;
    }
}