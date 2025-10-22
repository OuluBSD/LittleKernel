#include "Kernel.h"

void SerialDriver::Initialize() {
    outportb(SERIAL_LINE_COMMAND(SERIAL_COM1_BASE), 0x80);    // Enable DLAB (set baud rate divisor)
    outportb(SERIAL_COM1_BASE, 0x03);                         // Set divisor to 3 (lo byte) 38400 baud
    outportb(SERIAL_COM1_BASE + 1, 0x00);                     //                  (hi byte)
    outportb(SERIAL_LINE_COMMAND(SERIAL_COM1_BASE), 0x03);    // 8 bits, no parity, one stop bit
    outportb(SERIAL_FIFO_COMMAND(SERIAL_COM1_BASE), 0xC7);    // Enable FIFO, clear them, 14-byte threshold
    outportb(SERIAL_MODEM_COMMAND(SERIAL_COM1_BASE), 0x0B);   // IRQs enabled, RTS/DSR set
}

bool SerialDriver::IsTransmitEmpty() {
    return inportb(SERIAL_LINE_STATUS(SERIAL_COM1_BASE)) & 0x20;
}

void SerialDriver::WriteChar(char c) {
    while (!IsTransmitEmpty()) {
        // Wait for the serial port to be ready
    }
    outportb(SERIAL_COM1_BASE, c);
}

void SerialDriver::WriteString(const char* str) {
    while (*str) {
        if (*str == '\n') {
            WriteChar('\r');  // Send carriage return before newline
        }
        WriteChar(*str++);
    }
}

bool SerialDriver::IsReceiveEmpty() {
    return inportb(SERIAL_LINE_STATUS(SERIAL_COM1_BASE)) & 1;
}

char SerialDriver::ReadChar() {
    while (IsReceiveEmpty()) {
        // Wait for data to be available
    }
    return inportb(SERIAL_COM1_BASE);
}

void SerialDriver::WriteInteger(int32 value) {
    if (value == 0) {
        WriteChar('0');
        return;
    }
    
    char buffer[12]; // Enough for 32-bit integer
    uint32 len = 0;
    bool negative = false;
    
    if (value < 0) {
        negative = true;
        value = -value;
    }
    
    // Extract digits
    while (value > 0) {
        buffer[len++] = '0' + (value % 10);
        value /= 10;
    }
    
    // Add negative sign if needed
    if (negative) {
        buffer[len++] = '-';
    }
    
    // Print in reverse order
    for (int i = len - 1; i >= 0; i--) {
        WriteChar(buffer[i]);
    }
}

void SerialDriver::WriteHex(uint32 value) {
    WriteString("0x");
    
    if (value == 0) {
        WriteChar('0');
        return;
    }
    
    char buffer[16]; // Enough for 32-bit hex value
    uint32 len = 0;
    const char* hex_chars = "0123456789ABCDEF";
    
    // Extract hex digits
    while (value > 0) {
        buffer[len++] = hex_chars[value & 0xF];
        value >>= 4;
    }
    
    // Print in reverse order
    for (int i = len - 1; i >= 0; i--) {
        WriteChar(buffer[i]);
    }
}