#include "Kernel.h"

// Initialize serial communication
void InitializeSerial() {
    outportb(0x3F8 + 1, 0x00);    // Disable all interrupts
    outportb(0x3F8 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outportb(0x3F8 + 0, 0x03);    // Set divisor to 3 (lo) // 38400 baud
    outportb(0x3F8 + 1, 0x00);    //                  (hi)
    outportb(0x3F8 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outportb(0x3F8 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outportb(0x3F8 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

// Internal function to send a character to serial
static void SerialWriteChar(char c) {
    while ((inportb(0x3F8 + 5) & 0x20) == 0);  // Wait until THR is empty
    outportb(0x3F8, c);
}

// Internal function to print a string to serial
static void SerialWriteString(const char* str) {
    while (*str) {
        if (*str == '\n') {
            SerialWriteChar('\r');
        }
        SerialWriteChar(*str++);
    }
}

// Write to both monitor and serial
static void WriteLogOutput(const char* msg) {
    // Write to monitor
    if (global && global->monitor) {
        global->monitor->Write(msg);
        
        // Add newline if not present
        if (msg[strlen(msg)-1] != '\n') {
            global->monitor->Write("\n");
        }
    }
    
    // Write to serial
    SerialWriteString(msg);
    
    // Add newline to serial if not present
    if (msg[strlen(msg)-1] != '\n') {
        SerialWriteChar('\n');
    }
}

// LogStream implementation
LogStream::LogStream() {
    pos = 0;
    buffer[0] = '\0';
}

LogStream& LogStream::operator<<(const char* str) {
    AppendString(str);
    return *this;
}

LogStream& LogStream::operator<<(int32 val) {
    AppendInt(val);
    return *this;
}

LogStream& LogStream::operator<<(uint32 val) {
    AppendUint32(val);
    return *this;
}

LogStream& LogStream::operator<<(uint64 val) {
    AppendUint64(val);
    return *this;
}

LogStream& LogStream::operator<<(char c) {
    AppendChar(c);
    return *this;
}

LogStream& LogStream::operator<<(void* ptr) {
    AppendHex((uint32)ptr);
    return *this;
}

void LogStream::AppendString(const char* str) {
    uint32 len = strlen(str);
    if (pos + len < 510) {
        for (uint32 i = 0; i < len; i++) {
            buffer[pos++] = str[i];
        }
        buffer[pos] = '\0';
    }
}

void LogStream::AppendChar(char c) {
    if (pos < 510) {
        buffer[pos++] = c;
        buffer[pos] = '\0';
    }
}

void LogStream::AppendInt(int32 val) {
    if (val == 0) {
        if (pos < 510) buffer[pos++] = '0';
        buffer[pos] = '\0';
        return;
    }
    
    char temp[16];
    uint32 i = 0;
    bool negative = false;
    
    if (val < 0) {
        negative = true;
        val = -val;
    }
    
    while (val > 0) {
        temp[i++] = '0' + (val % 10);
        val /= 10;
    }
    
    if (negative && pos < 510) {
        buffer[pos++] = '-';
    }
    
    for (int j = i - 1; j >= 0; j--) {
        if (pos < 510) {
            buffer[pos++] = temp[j];
        }
    }
    buffer[pos] = '\0';
}

void LogStream::AppendUint32(uint32 val) {
    if (val == 0) {
        if (pos < 510) buffer[pos++] = '0';
        buffer[pos] = '\0';
        return;
    }

    char temp[16];
    uint32 i = 0;

    while (val > 0) {
        temp[i++] = '0' + (val % 10);
        val /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {
        if (pos < 510) {
            buffer[pos++] = temp[j];
        }
    }
    buffer[pos] = '\0';
}

void LogStream::AppendUint64(uint64 val) {
    if (val == 0) {
        if (pos < 510) buffer[pos++] = '0';
        buffer[pos] = '\0';
        return;
    }

    char temp[24];
    uint32 i = 0;

    while (val > 0) {
        temp[i++] = '0' + (val % 10);
        val /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {
        if (pos < 510) {
            buffer[pos++] = temp[j];
        }
    }
    buffer[pos] = '\0';
}

void LogStream::AppendHex(uint32 val) {
    const char hex_chars[] = "0123456789ABCDEF";
    char temp[16];
    uint32 i = 0;
    
    if (val == 0) {
        temp[i++] = '0';
    } else {
        while (val > 0) {
            temp[i++] = hex_chars[val & 0xF];
            val >>= 4;
        }
    }
    
    if (pos < 508) {
        buffer[pos++] = '0';
        buffer[pos++] = 'x';
    }
    
    for (int j = i - 1; j >= 0; j--) {
        if (pos < 510) {
            buffer[pos++] = temp[j];
        }
    }
    buffer[pos] = '\0';
}

void LogStream::Flush(const char* prefix) {
    char output[512];
    uint32 i = 0;
    
    // Add prefix
    for (uint32 j = 0; prefix[j] != '\0'; j++) {
        output[i++] = prefix[j];
    }
    
    // Add the buffered content
    for (uint32 j = 0; j < pos && j < 500; j++) {
        output[i++] = buffer[j];
    }
    
    output[i] = '\0';
    WriteLogOutput(output);
}