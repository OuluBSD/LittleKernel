#include "Kernel.h"

ConsoleDriver::ConsoleDriver() {
    // Initialize the device structure
    console_device.id = 0;  // Will be assigned by framework
    strcpy_safe(console_device.name, "Console", sizeof(console_device.name));
    console_device.type = DEVICE_TYPE_CONSOLE;
    console_device.flags = 0;
    console_device.private_data = &data;
    console_device.ops = nullptr;  // Will be set below
    console_device.next = nullptr;
    console_device.base_port = 0x3F8;  // Default serial port for debugging
    console_device.irq_line = 4;       // Default IRQ for serial
    console_device.mmio_base = nullptr;

    // Set up the driver operations
    static DriverOperations console_ops = {
        ConsoleInit,
        ConsoleRead,
        ConsoleWrite,
        ConsoleIoctl,
        ConsoleClose
    };
    console_device.ops = &console_ops;

    // Initialize driver data
    data.video_memory = (uint16*)0xB8000;  // VGA text buffer
    data.cursor_x = 0;
    data.cursor_y = 0;
    data.attribute = 0x07;  // Light gray on black
    data.cursor_enabled = true;

    DLOG("ConsoleDriver initialized");
}

ConsoleDriver::~ConsoleDriver() {
    DLOG("ConsoleDriver destroyed");
}

bool ConsoleDriver::Initialize() {
    // Initialize the console
    ClearScreen();
    SetCursorPosition(0, 0);
    
    DLOG("ConsoleDriver initialized and ready");
    return true;
}

void ConsoleDriver::PutChar(char c) {
    uint16* video_memory = data.video_memory;
    uint8 attribute = data.attribute;
    uint32 pos = data.cursor_y * CONSOLE_WIDTH + data.cursor_x;

    switch (c) {
        case '\n':
            NewLine();
            break;
        case '\r':
            data.cursor_x = 0;
            break;
        case '\b':
            if (data.cursor_x > 0) {
                data.cursor_x--;
                pos = data.cursor_y * CONSOLE_WIDTH + data.cursor_x;
                video_memory[pos] = (attribute << 8) | ' ';
            }
            break;
        default:
            video_memory[pos] = (attribute << 8) | c;
            data.cursor_x++;
            break;
    }

    // Handle line wrapping
    if (data.cursor_x >= CONSOLE_WIDTH) {
        data.cursor_x = 0;
        data.cursor_y++;
    }

    // Scroll if needed
    if (data.cursor_y >= CONSOLE_HEIGHT) {
        Scroll();
        data.cursor_y = CONSOLE_HEIGHT - 1;
    }

    UpdateCursorPosition();
}

void ConsoleDriver::PutString(const char* str) {
    if (!str) return;

    for (uint32 i = 0; str[i]; i++) {
        PutChar(str[i]);
    }
}

void ConsoleDriver::ClearScreen() {
    uint16* video_memory = data.video_memory;
    uint16 blank = (data.attribute << 8) | ' ';

    for (uint32 i = 0; i < CONSOLE_BUFFER_SIZE; i++) {
        video_memory[i] = blank;
    }

    data.cursor_x = 0;
    data.cursor_y = 0;
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
    uint16* video_memory = data.video_memory;

    // Move all lines up by one
    for (uint32 y = 0; y < CONSOLE_HEIGHT - 1; y++) {
        for (uint32 x = 0; x < CONSOLE_WIDTH; x++) {
            video_memory[y * CONSOLE_WIDTH + x] = 
                video_memory[(y + 1) * CONSOLE_WIDTH + x];
        }
    }

    // Clear the last line
    uint16 blank = (data.attribute << 8) | ' ';
    for (uint32 x = 0; x < CONSOLE_WIDTH; x++) {
        video_memory[(CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH + x] = blank;
    }
}

bool ConsoleDriver::HandleIoctl(uint32 command, void* arg) {
    switch (command) {
        case CONSOLE_GET_SIZE:
            if (arg) {
                uint32* size = (uint32*)arg;
                size[0] = CONSOLE_WIDTH;
                size[1] = CONSOLE_HEIGHT;
                return true;
            }
            break;
        case CONSOLE_GET_CURSOR_POS:
            if (arg) {
                uint32* pos = (uint32*)arg;
                pos[0] = data.cursor_x;
                pos[1] = data.cursor_y;
                return true;
            }
            break;
        case CONSOLE_SET_CURSOR_POS:
            if (arg) {
                uint32* pos = (uint32*)arg;
                SetCursorPosition(pos[0], pos[1]);
                return true;
            }
            break;
        case CONSOLE_CLEAR_SCREEN:
            ClearScreen();
            return true;
        case CONSOLE_SET_COLOR:
            if (arg) {
                uint8* color = (uint8*)arg;
                SetColor(color[0], color[1]);  // foreground, background
                return true;
            }
            break;
        case CONSOLE_SCROLL:
            Scroll();
            return true;
        default:
            LOG("Unknown console ioctl command: " << command);
            break;
    }
    return false;
}

void ConsoleDriver::UpdateCursorPosition() {
    uint16 pos = data.cursor_y * CONSOLE_WIDTH + data.cursor_x;

    // Use the outportb function from Common.h
    outportb(0x3D4, 14);  // Cursor position high
    outportb(0x3D5, pos >> 8);
    outportb(0x3D4, 15);  // Cursor position low
    outportb(0x3D5, pos);
}

void ConsoleDriver::NewLine() {
    data.cursor_x = 0;
    data.cursor_y++;
    
    if (data.cursor_y >= CONSOLE_HEIGHT) {
        Scroll();
        data.cursor_y = CONSOLE_HEIGHT - 1;
    }
}

void ConsoleDriver::Backspace() {
    if (data.cursor_x > 0) {
        data.cursor_x--;
    } else if (data.cursor_y > 0) {
        data.cursor_y--;
        data.cursor_x = CONSOLE_WIDTH - 1;
    }
    
    uint16* video_memory = data.video_memory;
    uint32 pos = data.cursor_y * CONSOLE_WIDTH + data.cursor_x;
    video_memory[pos] = (data.attribute << 8) | ' ';
}

// Static driver framework callback implementations

bool ConsoleDriver::ConsoleInit(Device* device) {
    if (!device || !device->private_data) {
        return false;
    }

    // Since the constructor already initialized the private data, 
    // we just return true to indicate success
    return true;
}

bool ConsoleDriver::ConsoleRead(Device* device, void* buffer, uint32 size, uint32 offset) {
    // Console doesn't support reading in this implementation
    return false;
}

bool ConsoleDriver::ConsoleWrite(Device* device, const void* buffer, uint32 size, uint32 offset) {
    if (!device || !buffer || size == 0) {
        return false;
    }

    // Get the console driver instance
    ConsoleDriverData* data = (ConsoleDriverData*)device->private_data;
    if (!data) {
        return false;
    }

    // Cast buffer to char* and write to console
    const char* str = (const char*)buffer;
    ConsoleDriver* driver = reinterpret_cast<ConsoleDriver*>(
        reinterpret_cast<char*>(data) - 
        reinterpret_cast<char*>(&((ConsoleDriver*)nullptr)->data)
    );

    for (uint32 i = 0; i < size && str[i] != '\0'; i++) {
        driver->PutChar(str[i]);
    }

    return true;
}

bool ConsoleDriver::ConsoleIoctl(Device* device, uint32 command, void* arg) {
    if (!device || !device->private_data) {
        return false;
    }

    // Get the console driver instance
    ConsoleDriverData* data = (ConsoleDriverData*)device->private_data;
    ConsoleDriver* driver = reinterpret_cast<ConsoleDriver*>(
        reinterpret_cast<char*>(data) - 
        reinterpret_cast<char*>(&((ConsoleDriver*)nullptr)->data)
    );

    return driver->HandleIoctl(command, arg);
}

bool ConsoleDriver::ConsoleClose(Device* device) {
    // Console doesn't need any special closing in this implementation
    return true;
}