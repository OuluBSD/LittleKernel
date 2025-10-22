#include "Kernel.h"

Monitor::Monitor() {
    cursor_x = 0;
    cursor_y = 0;
    default_attribute = WHITE_ON_BLACK;
}

void Monitor::Initialize() {
    cursor_x = 0;
    cursor_y = 0;
    Clear();
}

// Set cursor position
void Monitor::MoveCursor() {
    uint16 cursor_location = cursor_y * MAX_COLS + cursor_x;
    outportb(REG_SCREEN_CTRL, 14);                  // cursor position high
    outportb(REG_SCREEN_DATA, cursor_location >> 8);
    outportb(REG_SCREEN_CTRL, 15);                  // cursor position low
    outportb(REG_SCREEN_DATA, cursor_location);
}

// Scroll the screen
void Monitor::Scroll() {
    // Get a space character with the default color attributes
    uint8 attribute_byte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
    uint16 blank = 0x20 /* space */ | (attribute_byte << 8);

    // Row 25 is the end, this means we need to scroll up
    if (cursor_y >= MAX_ROWS) {
        // Move the current text chunk that makes up the screen
        // back in the buffer by a line
        uint16* video = (uint16*)VIDEO_ADDRESS;
        for (int i = 0; i < (MAX_ROWS - 1) * MAX_COLS; i++) {
            video[i] = video[i + MAX_COLS];
        }

        // Blank the last line by filling it with spaces
        for (int i = (MAX_ROWS - 1) * MAX_COLS; i < MAX_ROWS * MAX_COLS; i++) {
            video[i] = blank;
        }

        // Move the cursor to the last line
        cursor_y = MAX_ROWS - 1;
    }
}

// Write to the monitor
void Monitor::Write(const char* str) {
    int i = 0;
    while (str[i] != '\0') {
        WriteChar(str[i++]);
    }
}

// Write a single character
void Monitor::WriteChar(char c) {
    uint8 back_colour = 0;
    uint8 fore_colour = 15;
    uint8 attribute_byte = (back_colour << 4) | (fore_colour & 0x0F);
    uint16 attribute = attribute_byte << 8;
    uint16* video = (uint16*)VIDEO_ADDRESS;

    // Handle backspace
    if (c == 0x08 && cursor_x) {
        cursor_x--;
        video[cursor_y * MAX_COLS + cursor_x] = ' ' | attribute;
    }
    // Handle tab
    else if (c == 0x09) {
        cursor_x = (cursor_x + 8) & ~(8 - 1);
    }
    // Handle carriage return
    else if (c == '\r') {
        cursor_x = 0;
    }
    // Handle newline
    else if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    }
    // Handle printable characters
    else if (c >= ' ') {
        video[cursor_y * MAX_COLS + cursor_x] = c | attribute;
        cursor_x++;
    }

    // Check if cursor has reached the end of the line
    if (cursor_x >= MAX_COLS) {
        cursor_x = 0;
        cursor_y++;
    }

    Scroll();
    MoveCursor();
}

// Clear the screen
void Monitor::Clear() {
    uint8 attribute_byte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
    uint16 blank = 0x20 /* space */ | (attribute_byte << 8);

    for (int i = 0; i < MAX_ROWS * MAX_COLS; i++) {
        ((uint16*)VIDEO_ADDRESS)[i] = blank;
    }

    cursor_x = 0;
    cursor_y = 0;
    MoveCursor();
}

// Set text color
void Monitor::SetColor(uint8 color) {
    default_attribute = color;
}

// Set cursor position
void Monitor::SetPosition(int x, int y) {
    if (x >= 0 && x < MAX_COLS && y >= 0 && y < MAX_ROWS) {
        cursor_x = x;
        cursor_y = y;
        MoveCursor();
    }
}

// Formatted write (simple implementation)
void Monitor::WriteFormat(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    const char* ptr = format;
    while (*ptr) {
        if (*ptr == '%' && *(ptr + 1)) {
            ptr++;
            switch (*ptr) {
                case 'd': {
                    int num = va_arg(args, int);
                    // Convert number to string and print
                    char num_str[12];  // Enough for 32-bit int
                    int i = 0, temp = num;
                    
                    if (num == 0) {
                        WriteChar('0');
                    } else {
                        if (num < 0) {
                            WriteChar('-');
                            temp = -temp;
                        }
                        
                        // Extract digits
                        char digits[12];
                        int digit_count = 0;
                        while (temp > 0) {
                            digits[digit_count++] = (temp % 10) + '0';
                            temp /= 10;
                        }
                        
                        // Print in reverse order
                        for (int j = digit_count - 1; j >= 0; j--) {
                            WriteChar(digits[j]);
                        }
                    }
                    break;
                }
                case 'x': {
                    uint32 num = va_arg(args, uint32);
                    // Print hex number
                    WriteChar('0');
                    WriteChar('x');
                    
                    char hex_chars[] = "0123456789ABCDEF";
                    bool leading_zero = true;
                    
                    for (int i = 7; i >= 0; i--) {
                        uint32 nibble = (num >> (i * 4)) & 0xF;
                        if (nibble != 0) leading_zero = false;
                        if (!leading_zero || i == 0) {
                            WriteChar(hex_chars[nibble]);
                        }
                    }
                    break;
                }
                case 's': {
                    const char* str = va_arg(args, const char*);
                    if (str) Write(str);
                    else Write("(null)");
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    WriteChar(c);
                    break;
                }
                default:
                    WriteChar(*ptr);  // Print literal character
                    break;
            }
        } else {
            WriteChar(*ptr);
        }
        ptr++;
    }
    
    va_end(args);
}