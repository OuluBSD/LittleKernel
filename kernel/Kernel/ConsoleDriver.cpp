#include "Kernel.h"
#include "ConsoleDriver.h"
#include "Logging.h"

ConsoleDriver::ConsoleDriver() {
    // Initialize the device structure
    console_device.id = 0;  // Will be assigned by framework
    strcpy_safe(console_device.name, "console0", sizeof(console_device.name));
    console_device.type = DEVICE_TYPE_CONSOLE;
    console_device.private_data = &data;
    console_device.flags = 0;
    console_device.base_port = 0x3D4;  // VGA cursor port
    console_device.irq_line = 0;
    console_device.mmio_base = nullptr;
    console_device.next = nullptr;
    
    // Set up driver operations
    static DriverOperations ops = {
        ConsoleInit,
        ConsoleRead,
        ConsoleWrite,
        ConsoleIoctl,
        ConsoleClose
    };
    console_device.ops = &ops;
    
    // Initialize driver data
    data.video_memory = (uint16*)0xB8000;  // VGA text mode buffer
    data.cursor_x = 0;
    data.cursor_y = 0;
    data.attribute = 0x07;  // White on black
    data.cursor_enabled = true;
}

ConsoleDriver::~ConsoleDriver() {
    // Console driver cleanup (if needed)
}

bool ConsoleDriver::Initialize() {
    ClearScreen();
    LOG("Console driver initialized");
    return true;
}

void ConsoleDriver::PutChar(char c) {
    uint16* video = data.video_memory;
    uint8 att = data.attribute;
    
    switch (c) {
        case '\n':  // Newline
            data.cursor_x = 0;
            data.cursor_y++;
            break;
            
        case '\b':  // Backspace
            if (data.cursor_x > 0) {
                data.cursor_x--;
                video[data.cursor_y * CONSOLE_WIDTH + data.cursor_x] = 
                    (' ' | att << 8);
            }
            break;
            
        case '\t':  // Tab
            // Move to next tab stop (every 8 characters)
            data.cursor_x = (data.cursor_x + 8) & ~(8 - 1);
            if (data.cursor_x >= CONSOLE_WIDTH) {
                data.cursor_x = 0;
                data.cursor_y++;
            }
            break;
            
        default:
            video[data.cursor_y * CONSOLE_WIDTH + data.cursor_x] = 
                (c | att << 8);
            data.cursor_x++;
            break;
    }
    
    // Handle line wrapping
    if (data.cursor_x >= CONSOLE_WIDTH) {
        data.cursor_x = 0;
        data.cursor_y++;
    }
    
    // Handle scrolling if needed
    if (data.cursor_y >= CONSOLE_HEIGHT) {
        Scroll();
        data.cursor_y = CONSOLE_HEIGHT - 1;
    }
    
    UpdateCursorPosition();
}

void ConsoleDriver::PutString(const char* str) {
    if (!str) return;
    
    const char* c = str;
    while (*c) {
        PutChar(*c);
        c++;
    }
}

void ConsoleDriver::ClearScreen() {
    uint16* video = data.video_memory;
    uint8 att = data.attribute;
    
    for (uint32 i = 0; i < CONSOLE_BUFFER_SIZE; i++) {
        video[i] = (' ' | att << 8);
    }
    
    data.cursor_x = 0;
    data.cursor_y = 0;
    UpdateCursorPosition();
}

void ConsoleDriver::SetCursorPosition(uint32 x, uint32 y) {
    if (x >= CONSOLE_WIDTH) x = CONSOLE_WIDTH - 1;
    if (y >= CONSOLE_HEIGHT) y = CONSOLE_HEIGHT - 1;
    
    data.cursor_x = x;
    data.cursor_y = y;
    UpdateCursorPosition();
}

void ConsoleDriver::GetCursorPosition(uint32* x, uint32* y) {
    if (x) *x = data.cursor_x;
    if (y) *y = data.cursor_y;
}

void ConsoleDriver::SetColor(uint8 foreground, uint8 background) {
    data.attribute = (background << 4) | (foreground & 0x0F);
}

void ConsoleDriver::Scroll() {
    uint16* video = data.video_memory;
    uint8 att = data.attribute;
    
    // Move lines up by one
    for (uint32 y = 0; y < CONSOLE_HEIGHT - 1; y++) {
        for (uint32 x = 0; x < CONSOLE_WIDTH; x++) {
            uint32 src = (y + 1) * CONSOLE_WIDTH + x;
            uint32 dst = y * CONSOLE_WIDTH + x;
            video[dst] = video[src];
        }
    }
    
    // Clear the last line
    for (uint32 x = 0; x < CONSOLE_WIDTH; x++) {
        uint32 idx = (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH + x;
        video[idx] = (' ' | att << 8);
    }
}

bool ConsoleDriver::HandleIoctl(uint32 command, void* arg) {
    switch (command) {
        case CONSOLE_GET_SIZE: {
            uint32* size = (uint32*)arg;
            if (size) {
                size[0] = CONSOLE_WIDTH;
                size[1] = CONSOLE_HEIGHT;
            }
            break;
        }
        
        case CONSOLE_GET_CURSOR_POS: {
            uint32* pos = (uint32*)arg;
            if (pos) {
                pos[0] = data.cursor_x;
                pos[1] = data.cursor_y;
            }
            break;
        }
        
        case CONSOLE_SET_CURSOR_POS: {
            uint32* pos = (uint32*)arg;
            if (pos) {
                SetCursorPosition(pos[0], pos[1]);
            }
            break;
        }
        
        case CONSOLE_CLEAR_SCREEN: {
            ClearScreen();
            break;
        }
        
        case CONSOLE_SET_COLOR: {
            uint32* color = (uint32*)arg;
            if (color) {
                SetColor(color[0], color[1]);  // foreground, background
            }
            break;
        }
        
        case CONSOLE_SCROLL: {
            Scroll();
            break;
        }
        
        default:
            return false;
    }
    
    return true;
}

void ConsoleDriver::UpdateCursorPosition() {
    uint32 pos = data.cursor_y * CONSOLE_WIDTH + data.cursor_x;
    
    // Send position to VGA controller
    outportb(0x3D4, 0x0F);  // Cursor location low byte
    outportb(0x3D5, (uint8)(pos & 0xFF));
    outportb(0x3D4, 0x0E);  // Cursor location high byte
    outportb(0x3D5, (uint8)((pos >> 8) & 0xFF));
}

void ConsoleDriver::NewLine() {
    data.cursor_x = 0;
    data.cursor_y++;
    
    if (data.cursor_y >= CONSOLE_HEIGHT) {
        Scroll();
        data.cursor_y = CONSOLE_HEIGHT - 1;
    }
    
    UpdateCursorPosition();
}

void ConsoleDriver::Backspace() {
    if (data.cursor_x > 0) {
        data.cursor_x--;
        uint16* video = data.video_memory;
        video[data.cursor_y * CONSOLE_WIDTH + data.cursor_x] = 
            (' ' | data.attribute << 8);
    }
    
    UpdateCursorPosition();
}

// Driver framework callbacks
bool ConsoleDriver::ConsoleInit(Device* device) {
    if (!device || !device->private_data) {
        return false;
    }
    
    ConsoleDriverData* data = (ConsoleDriverData*)device->private_data;
    // Initialize video memory and clear screen
    uint16* video = data->video_memory;
    uint8 att = data->attribute;
    
    for (uint32 i = 0; i < CONSOLE_BUFFER_SIZE; i++) {
        video[i] = (' ' | att << 8);
    }
    
    data->cursor_x = 0;
    data->cursor_y = 0;
    
    // Update hardware cursor position
    uint32 pos = 0;
    outportb(0x3D4, 0x0F);
    outportb(0x3D5, (uint8)(pos & 0xFF));
    outportb(0x3D4, 0x0E);
    outportb(0x3D5, (uint8)((pos >> 8) & 0xFF));
    
    device->flags |= DRIVER_INITIALIZED;
    DLOG("Console device initialized");
    return true;
}

bool ConsoleDriver::ConsoleRead(Device* device, void* buffer, uint32 size, uint32 offset) {
    // For a console, reading would typically mean reading keyboard input
    // which we'll handle through a separate keyboard driver
    // For now, return false indicating read is not supported
    return false;
}

bool ConsoleDriver::ConsoleWrite(Device* device, const void* buffer, uint32 size, uint32 offset) {
    if (!device || !buffer || size == 0) {
        return false;
    }
    
    ConsoleDriverData* data = (ConsoleDriverData*)device->private_data;
    if (!data) {
        return false;
    }
    
    const char* str = (const char*)buffer;
    for (uint32 i = 0; i < size && str[i] != '\0'; i++) {
        ConsoleDriver temp_driver;
        // Temporarily set up the data to use the device's data
        memcpy(&temp_driver.data, data, sizeof(ConsoleDriverData));
        
        temp_driver.PutChar(str[i]);
        
        // Copy the modified data back
        memcpy(data, &temp_driver.data, sizeof(ConsoleDriverData));
    }
    
    return true;
}

bool ConsoleDriver::ConsoleIoctl(Device* device, uint32 command, void* arg) {
    if (!device) {
        return false;
    }
    
    ConsoleDriverData* data = (ConsoleDriverData*)device->private_data;
    if (!data) {
        return false;
    }
    
    // Create a temporary driver instance to handle the ioctl
    ConsoleDriver temp_driver;
    memcpy(&temp_driver.data, data, sizeof(ConsoleDriverData));
    
    bool result = temp_driver.HandleIoctl(command, arg);
    
    // Copy the data back in case it was modified
    memcpy(data, &temp_driver.data, sizeof(ConsoleDriverData));
    
    return result;
}

bool ConsoleDriver::ConsoleClose(Device* device) {
    // Console doesn't really have a "close" operation
    // Just mark it as inactive
    if (device) {
        device->flags &= ~DRIVER_ACTIVE;
    }
    return true;
}