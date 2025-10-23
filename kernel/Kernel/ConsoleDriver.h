#ifndef _Kernel_ConsoleDriver_h_
#define _Kernel_ConsoleDriver_h_

#include "Common.h"
#include "DriverFramework.h"

// Console-specific constants
const uint32 CONSOLE_WIDTH = 80;
const uint32 CONSOLE_HEIGHT = 25;
const uint32 CONSOLE_BUFFER_SIZE = CONSOLE_WIDTH * CONSOLE_HEIGHT;

// Console driver private data
struct ConsoleDriverData {
    uint16* video_memory;         // Pointer to video memory
    uint32 cursor_x;              // Current cursor X position
    uint32 cursor_y;              // Current cursor Y position
    uint8 attribute;              // Current text attribute (color)
    bool cursor_enabled;          // Whether cursor is visible
};

// Console-specific IOCTL commands
enum ConsoleIoctlCommands {
    CONSOLE_GET_SIZE = 1,
    CONSOLE_GET_CURSOR_POS,
    CONSOLE_SET_CURSOR_POS,
    CONSOLE_CLEAR_SCREEN,
    CONSOLE_SET_COLOR,
    CONSOLE_SCROLL
};

// Console driver class
class ConsoleDriver {
private:
    Device console_device;
    ConsoleDriverData data;
    
public:
    ConsoleDriver();
    ~ConsoleDriver();
    
    // Initialize the console driver
    bool Initialize();
    
    // Console-specific functions
    void PutChar(char c);
    void PutString(const char* str);
    void ClearScreen();
    void SetCursorPosition(uint32 x, uint32 y);
    void GetCursorPosition(uint32* x, uint32* y);
    void SetColor(uint8 foreground, uint8 background);
    void Scroll();
    
    // Handle console-specific IOCTL commands
    bool HandleIoctl(uint32 command, void* arg);
    
    // Get the device structure for registration
    Device* GetDevice() { return &console_device; }
    
private:
    // Driver framework callbacks
    static bool ConsoleInit(Device* device);
    static bool ConsoleRead(Device* device, void* buffer, uint32 size, uint32 offset);
    static bool ConsoleWrite(Device* device, const void* buffer, uint32 size, uint32 offset);
    static bool ConsoleIoctl(Device* device, uint32 command, void* arg);
    static bool ConsoleClose(Device* device);
    
    // Internal helper functions
    void UpdateCursorPosition();
    void NewLine();
    void Backspace();
};

#endif