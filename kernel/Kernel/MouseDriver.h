#ifndef _Kernel_MouseDriver_h_
#define _Kernel_MouseDriver_h_

#include "Common.h"
#include "DriverFramework.h"
#include "Defs.h"
#include "RingBuffer.h"  // Assuming we have a ring buffer implementation

// Mouse-specific constants
#define PS2_MOUSE_PORT_DATA 0x60
#define PS2_MOUSE_PORT_STATUS 0x64
#define PS2_MOUSE_PORT_COMMAND 0x64
#define PS2_MOUSE_IRQ 12  // PS/2 uses IRQ 12 for mouse

// Mouse commands
#define PS2_MOUSE_CMD_SET_SCALE_1_1 0xE6
#define PS2_MOUSE_CMD_SET_SCALE_2_1 0xE7
#define PS2_MOUSE_CMD_SET_RESOLUTION 0xE8
#define PS2_MOUSE_CMD_GET_STATUS 0xE9
#define PS2_MOUSE_CMD_SET_STREAM_MODE 0xEA
#define PS2_MOUSE_CMD_STATUS_REQUEST 0xEB
#define PS2_MOUSE_CMD_GET_ID 0xF2
#define PS2_MOUSE_CMD_SET_SAMPLE_RATE 0xF3
#define PS2_MOUSE_CMD_ENABLE_PACKET_STREAMING 0xF4
#define PS2_MOUSE_CMD_DISABLE_PACKET_STREAMING 0xF5
#define PS2_MOUSE_CMD_RESET 0xFF

// Mouse configuration commands
#define PS2_CMD_READ_MOUSE_OUTPUT 0x20
#define PS2_CMD_WRITE_MOUSE_OUTPUT 0x60
#define PS2_CMD_MOUSE_WRITE 0xD4

// Mouse packet structure
struct MousePacket {
    uint8_t status_byte;  // Contains button states and overflow flags
    int8_t x_movement;    // X-axis movement (-127 to +127)
    int8_t y_movement;    // Y-axis movement (-127 to +127)
    int8_t z_movement;    // Z-axis movement (wheel) if available
    uint32_t timestamp;   // Tick count when event occurred
};

// Mouse event structure
struct MouseEvent {
    int32_t x;            // Absolute X position (0 to screen width)
    int32_t y;            // Absolute Y position (0 to screen height)
    int8_t x_movement;    // Relative X movement (-127 to +127)
    int8_t y_movement;    // Relative Y movement (-127 to +127)
    bool left_button;     // Left button state
    bool right_button;    // Right button state
    bool middle_button;   // Middle button state
    int8_t wheel_movement; // Wheel movement (positive for up, negative for down)
    uint32_t timestamp;   // Tick count when event occurred
};

// Mouse-specific IOCTL commands
enum MouseIoctlCommands {
    MOUSE_GET_STATUS = 1,
    MOUSE_GET_RESOLUTION,
    MOUSE_SET_RESOLUTION,
    MOUSE_GET_SAMPLE_RATE,
    MOUSE_SET_SAMPLE_RATE,
    MOUSE_GET_ID,
    MOUSE_FLUSH_BUFFER,
    MOUSE_GET_EVENT_COUNT,
    MOUSE_SET_CURSOR_POSITION,
    MOUSE_GET_CURSOR_POSITION
};

// PS2 mouse driver class
class MouseDriver {
private:
    Device mouse_device;
    uint8_t mouse_id;                    // Mouse ID (0=PS/2, 3=Intellimouse, 4=Intellimouse Explorer)
    bool has_wheel;                      // Whether mouse supports wheel
    bool has_buttons_4_5;                // Whether mouse supports additional buttons
    uint8_t resolution;                  // Resolution (0=1, 1=1.5, 2=2, 3=8 counts/mm)
    uint8_t sample_rate;                 // Sample rate in reports per second
    int32_t cursor_x;                    // Current cursor X position
    int32_t cursor_y;                    // Current cursor Y position
    int32_t screen_width;                // Screen width
    int32_t screen_height;               // Screen height
    RingBuffer<MouseEvent, 256> event_buffer;  // Circular buffer for mouse events
    Spinlock buffer_lock;                // Lock for protecting the event buffer
    uint8_t packet_bytes[3];             // Temporary storage for mouse packet bytes
    uint8_t packet_byte_index;           // Index for building mouse packet
    bool packet_ready;                   // Whether a complete packet is ready
    
public:
    MouseDriver();
    ~MouseDriver();
    
    // Initialize the mouse driver
    bool Initialize();
    
    // Mouse-specific functions
    bool ReadPacket(uint8_t& data);
    void ProcessPacket();
    bool GetMouseEvent(MouseEvent& event);
    uint32_t GetEventCount();
    void FlushBuffer();
    
    // Configuration functions
    bool SetResolution(uint8_t res);
    uint8_t GetResolution();
    bool SetSampleRate(uint8_t rate);
    uint8_t GetSampleRate();
    uint8_t GetMouseId();
    bool GetStatus();
    
    // Cursor functions
    void SetCursorPosition(int32_t x, int32_t y);
    void GetCursorPosition(int32_t& x, int32_t& y);
    void UpdateCursorPosition(int8_t x_move, int8_t y_move);
    
    // Handle mouse-specific IOCTL commands
    bool HandleIoctl(uint32 command, void* arg);
    
    // Get the device structure for registration
    Device* GetDevice() { return &mouse_device; }

private:
    // Driver framework callbacks
    static bool MouseInit(Device* device);
    static bool MouseRead(Device* device, void* buffer, uint32 size, uint32 offset);
    static bool MouseWrite(Device* device, const void* buffer, uint32 size, uint32 offset);
    static bool MouseIoctl(Device* device, uint32 command, void* arg);
    static bool MouseClose(Device* device);
    
    // Internal helper functions
    bool SendCommand(uint8_t cmd);
    bool WriteData(uint8_t data);
    uint8_t ReadData();
    bool WaitForInputBuffer();
    bool WaitForOutputBuffer();
    bool EnablePacketStreaming();
    bool DisablePacketStreaming();
    bool MouseWrite(uint8_t data);
    bool MouseRead(uint8_t& data);
};

#endif