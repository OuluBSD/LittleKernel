#ifndef _LittleKernel_LogStream_h_
#define _LittleKernel_LogStream_h_

#include "GenericOutput.h"
#include "Common.h"

// A simple stream-like class for formatted logging
// This class allows syntax like: LOG("Value is " << value << ".")
class LogStream {
private:
    char buffer[256];  // Buffer for building messages
    size_t pos;        // Current position in buffer

    // Helper to append string to buffer
    void append(const char* str) {
        if (!str) return;
        while (*str && pos < sizeof(buffer) - 1) {
            buffer[pos++] = *str++;
        }
        buffer[pos] = '\0';  // Ensure null termination
    }

    // Helper to append integer as string
    void append(int value) {
        char temp[16];  // Buffer for integer conversion
        int temp_pos = 0;
        
        // Handle negative numbers
        if (value < 0) {
            if (pos < sizeof(buffer) - 1) {
                buffer[pos++] = '-';
            }
            value = -value;
        }
        
        // Convert to string (in reverse first)
        if (value == 0) {
            temp[temp_pos++] = '0';
        } else {
            while (value > 0) {
                temp[temp_pos++] = '0' + (value % 10);
                value /= 10;
            }
        }
        
        // Now append in correct order
        for (int i = temp_pos - 1; i >= 0; i--) {
            if (pos < sizeof(buffer) - 1) {
                buffer[pos++] = temp[i];
            }
        }
        buffer[pos] = '\0';
    }
    
    // Helper to append uint32 as hex string
    void appendHex(uint32 value) {
        append("0x");  // Add hex prefix
        char temp[9];  // 8 hex digits + null terminator
        temp[8] = '\0';
        
        // Convert to hex (in reverse first)
        for (int i = 7; i >= 0; i--) {
            int digit = value & 0xF;
            if (digit < 10) {
                temp[i] = '0' + digit;
            } else {
                temp[i] = 'A' + (digit - 10);
            }
            value >>= 4;
        }
        
        append(temp);
    }

public:
    LogStream() {
        pos = 0;
        buffer[0] = '\0';
    }
    
    // Operator for const char* 
    LogStream& operator<<(const char* str) {
        append(str);
        return *this;
    }
    
    // Operator for int
    LogStream& operator<<(int value) {
        append(value);
        return *this;
    }
    
    // Operator for uint32 (for hex conversion)
    LogStream& operator<<(uint32 value) {
        appendHex(value);
        return *this;
    }
    
    // Operator for single character
    LogStream& operator<<(char c) {
        if (pos < sizeof(buffer) - 1) {
            buffer[pos++] = c;
            buffer[pos] = '\0';
        }
        return *this;
    }
    
    // Finalize and output the message
    ~LogStream() {
        if (pos > 0) {
            // Add newline and output to generic write
            GenericWrite(buffer, true);
        }
    }
};

// Define the LOG macro to use LogStream
#define LOG(x) do { LogStream _log_stream; _log_stream << x; } while(0)

#endif