#ifndef _Kernel_KeyboardDriver_h_
#define _Kernel_KeyboardDriver_h_

#include "Common.h"
#include "DriverFramework.h"
#include "Defs.h"
#include "RingBuffer.h"  // Assuming we have a ring buffer implementation

// Keyboard-specific constants
#define PS2_KEYBOARD_PORT_DATA 0x60
#define PS2_KEYBOARD_PORT_STATUS 0x64
#define PS2_KEYBOARD_PORT_COMMAND 0x64
#define PS2_KEYBOARD_IRQ 1

// Keyboard commands
#define PS2_CMD_READ_CONFIG 0x20
#define PS2_CMD_WRITE_CONFIG 0x60
#define PS2_CMD_DISABLE_FIRST_PORT 0xAD
#define PS2_CMD_ENABLE_FIRST_PORT 0xAE
#define PS2_CMD_DISABLE_SECOND_PORT 0xA7
#define PS2_CMD_ENABLE_SECOND_PORT 0xA8

// Keyboard configuration bits
#define PS2_CFG_FIRST_PORT_INT 0x01
#define PS2_CFG_SECOND_PORT_INT 0x02
#define PS2_CFG_SYSTEM_FLAG 0x04
#define PS2_CFG_RESERVED 0x08
#define PS2_CFG_FIRST_PORT_CLK 0x10
#define PS2_CFG_SECOND_PORT_CLK 0x20
#define PS2_CFG_TRANSLATION 0x40

// Scancode set constants
#define SCANCODE_SET_1 1
#define SCANCODE_SET_2 2

// Keyboard event structure
struct KeyboardEvent {
    uint8_t scancode;
    bool is_pressed;  // true for key press, false for key release
    uint32_t timestamp;  // Tick count when event occurred
};

// Keyboard-specific IOCTL commands
enum KeyboardIoctlCommands {
    KEYBOARD_GET_SCANCODE_SET = 1,
    KEYBOARD_SET_SCANCODE_SET,
    KEYBOARD_GET_LEDS,
    KEYBOARD_SET_LEDS,
    KEYBOARD_FLUSH_BUFFER,
    KEYBOARD_GET_EVENT_COUNT
};

// PS2 keyboard driver class
class KeyboardDriver {
private:
    Device keyboard_device;
    uint8_t current_scancode_set;
    bool led_status[3];  // Num Lock, Caps Lock, Scroll Lock
    RingBuffer<KeyboardEvent, 256> event_buffer;  // Circular buffer for keyboard events
    Spinlock buffer_lock;  // Lock for protecting the event buffer
    
public:
    KeyboardDriver();
    ~KeyboardDriver();
    
    // Initialize the keyboard driver
    bool Initialize();
    
    // Keyboard-specific functions
    bool ReadScancode(uint8_t& scancode);
    void ProcessScancode(uint8_t scancode);
    bool GetKeyEvent(KeyboardEvent& event);
    uint32_t GetEventCount();
    void FlushBuffer();
    
    // LED control functions
    bool SetLeds(bool num_lock, bool caps_lock, bool scroll_lock);
    bool GetLeds(bool& num_lock, bool& caps_lock, bool& scroll_lock);
    
    // Scancode set functions
    bool SetScancodeSet(uint8_t set);
    uint8_t GetScancodeSet();
    
    // Handle keyboard-specific IOCTL commands
    bool HandleIoctl(uint32 command, void* arg);
    
    // Get the device structure for registration
    Device* GetDevice() { return &keyboard_device; }

private:
    // Driver framework callbacks
    static bool KeyboardInit(Device* device);
    static bool KeyboardRead(Device* device, void* buffer, uint32 size, uint32 offset);
    static bool KeyboardWrite(Device* device, const void* buffer, uint32 size, uint32 offset);
    static bool KeyboardIoctl(Device* device, uint32 command, void* arg);
    static bool KeyboardClose(Device* device);
    
    // Internal helper functions
    bool SendCommand(uint8_t cmd);
    bool WriteData(uint8_t data);
    uint8_t ReadData();
    bool WaitForInputBuffer();
    bool WaitForOutputBuffer();
    bool SelfTest();
};

#endif