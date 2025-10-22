#include "GenericOutput.h"
#include "Common.h"

// Serial port definitions
#define SERIAL_PORT_A 0x3F8

// Serial port helper functions
static void serial_init() {
    outb(SERIAL_PORT_A + 1, 0x00);    // Disable all interrupts
    outb(SERIAL_PORT_A + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(SERIAL_PORT_A + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(SERIAL_PORT_A + 1, 0x00);    //                  (hi byte)
    outb(SERIAL_PORT_A + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(SERIAL_PORT_A + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(SERIAL_PORT_A + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

static bool serial_is_transmit_empty() {
    return inb(SERIAL_PORT_A + 5) & 0x20;
}

static void serial_write_char(char c) {
    while (!serial_is_transmit_empty()) ;
    outb(SERIAL_PORT_A, c);
}

static void serial_write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        serial_write_char(str[i]);
    }
}

// Forward declarations for monitor functions
extern "C" void monitor_write(const char* str);
extern "C" void monitor_write_dec(int i);
extern "C" void monitor_write_hex(uint32 i);

// Initialize serial port
void init_serial() {
    serial_init();
    serial_write("Serial port initialized\n");
}

// Generic output function that writes to both monitor and serial
void GenericWrite(const char* str, bool new_line) {
    // Write to monitor
    monitor_write(str);
    
    // Write to serial
    serial_write(str);
    
    if (new_line) {
        monitor_write("\n");
        serial_write("\n");
    }
}

void GenericWriteDec(int i) {
    // Write to monitor
    monitor_write_dec(i);
    
    // Convert to string and write to serial
    if (i < 0) {
        serial_write_char('-');
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
            serial_write_char('0' + k);
    }
    if (!foundfirst)
        serial_write_char('0');
}

void GenericWriteHex(uint32 i) {
    // Write to monitor
    monitor_write_hex(i);
    
    // Convert to hex string and write to serial
    serial_write("0x");
    for (int j = 0; j < 8; j++) {
        int k = (i & 0xF0000000) >> (32 - 4);
        if (k < 10)
            serial_write_char('0' + k);
        else
            serial_write_char('A' + k - 10);
        i = i << 4;
    }
}