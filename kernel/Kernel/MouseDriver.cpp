#include "Kernel.h"
#include "MouseDriver.h"
#include "Logging.h"
#include "Interrupts.h"  // For setting up interrupt handlers

MouseDriver::MouseDriver() {
    // Initialize the device structure
    mouse_device.id = 0;  // Will be assigned by framework
    strcpy_safe(mouse_device.name, "mouse0", sizeof(mouse_device.name));
    mouse_device.type = DEVICE_TYPE_MOUSE;
    mouse_device.private_data = this;  // Point to this object for device callbacks
    mouse_device.flags = 0;
    mouse_device.base_port = PS2_MOUSE_PORT_DATA;
    mouse_device.irq_line = PS2_MOUSE_IRQ;
    mouse_device.mmio_base = nullptr;
    mouse_device.next = nullptr;
    
    // Set up driver operations
    static DriverOperations ops = {
        MouseInit,
        MouseRead,   // Reading would return mouse events
        MouseWrite,  // Not typically used for mouse
        MouseIoctl,
        MouseClose
    };
    mouse_device.ops = &ops;
    
    // Initialize driver-specific data
    mouse_id = 0;
    has_wheel = false;
    has_buttons_4_5 = false;
    resolution = 2;  // Default to 2 counts/mm
    sample_rate = 100;  // Default to 100 samples per second
    cursor_x = 0;
    cursor_y = 0;
    screen_width = 800;  // Default screen width - should be set by display driver
    screen_height = 600; // Default screen height - should be set by display driver
    packet_byte_index = 0;
    packet_ready = false;
    buffer_lock.Initialize();
}

MouseDriver::~MouseDriver() {
    // Mouse driver cleanup (if needed)
    // Disable mouse interrupts if still active
    DisablePacketStreaming();
}

bool MouseDriver::Initialize() {
    LOG("Initializing PS2 Mouse driver");
    
    // Enable the auxiliary mouse device
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_COMMAND, 0xA8);  // Enable auxiliary device
    
    // Enable interrupts for the auxiliary device
    uint8 config;
    if (!WaitForOutputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_COMMAND, 0x20);  // Read configuration
    if (!WaitForOutputBuffer()) return false;
    config = inportb(PS2_MOUSE_PORT_DATA);
    
    config |= 0x02;  // Enable interrupt for auxiliary device
    config |= 0x40;  // Enable translation for auxiliary device
    
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_COMMAND, 0x60);  // Write configuration
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_DATA, config);
    
    // Set default sample rate and resolution
    SetSampleRate(100);
    SetResolution(2);
    
    // Get mouse ID to detect capabilities
    SendCommand(PS2_MOUSE_CMD_GET_ID);
    if (!WaitForOutputBuffer()) return false;
    mouse_id = inportb(PS2_MOUSE_PORT_DATA);
    
    // Detect mouse type based on ID
    switch (mouse_id) {
        case 0x00:
            LOG("Standard PS/2 mouse detected");
            break;
        case 0x03:
            LOG("Mouse with scroll wheel detected");
            has_wheel = true;
            break;
        case 0x04:
            LOG("5-button mouse detected");
            has_wheel = true;
            has_buttons_4_5 = true;
            break;
        default:
            LOG("Unknown mouse type ID: " << (uint32)mouse_id);
            break;
    }
    
    // Enable packet streaming
    if (!EnablePacketStreaming()) {
        LOG("Failed to enable mouse packet streaming");
        return false;
    }
    
    // Clear the event buffer
    FlushBuffer();
    
    LOG("PS2 Mouse driver initialized successfully (ID: " << (uint32)mouse_id << ")");
    return true;
}

bool MouseDriver::ReadPacket(uint8& data) {
    if (inportb(PS2_MOUSE_PORT_STATUS) & 1) {  // Check if output buffer has data
        data = inportb(PS2_MOUSE_PORT_DATA);
        return true;
    }
    return false;
}

void MouseDriver::ProcessPacket() {
    if (packet_byte_index < 3) return;  // Need at least 3 bytes for a complete packet
    
    // Create mouse event from packet
    MouseEvent event;
    event.x_movement = packet_bytes[1];
    event.y_movement = -(int8_t)packet_bytes[2];  // Y is inverted
    event.left_button = (packet_bytes[0] & 0x01) != 0;
    event.right_button = (packet_bytes[0] & 0x02) != 0;
    event.middle_button = (packet_bytes[0] & 0x04) != 0;
    
    // Handle wheel if available
    if (has_wheel) {
        event.wheel_movement = (int8_t)((packet_bytes[0] & 0x0C) << 4) >> 4;  // Sign-extend 4-bit to 8-bit
    } else {
        event.wheel_movement = 0;
    }
    
    // Update cursor position
    UpdateCursorPosition(event.x_movement, event.y_movement);
    event.x = cursor_x;
    event.y = cursor_y;
    
    // Set timestamp
    event.timestamp = global_timer ? global_timer->GetTickCount() : 0;
    
    // Add event to buffer
    buffer_lock.Acquire();
    if (!event_buffer.IsFull()) {
        event_buffer.Push(event);
    } else {
        // Buffer full, drop the oldest event
        MouseEvent dummy;
        event_buffer.Pop(dummy);
        event_buffer.Push(event);
    }
    buffer_lock.Release();
    
    // Reset packet index for next packet
    packet_byte_index = 0;
    packet_ready = false;
}

bool MouseDriver::GetMouseEvent(MouseEvent& event) {
    buffer_lock.Acquire();
    if (!event_buffer.IsEmpty()) {
        bool result = event_buffer.Pop(event);
        buffer_lock.Release();
        return result;
    }
    buffer_lock.Release();
    return false;
}

uint32 MouseDriver::GetEventCount() {
    buffer_lock.Acquire();
    uint32 count = event_buffer.Count();
    buffer_lock.Release();
    return count;
}

void MouseDriver::FlushBuffer() {
    buffer_lock.Acquire();
    event_buffer.Clear();
    buffer_lock.Release();
}

bool MouseDriver::SetResolution(uint8 res) {
    if (res > 3) res = 3;  // Max resolution value is 3
    
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_DATA, PS2_MOUSE_CMD_SET_RESOLUTION);
    
    // Wait for ACK
    uint8 response;
    int retries = 10000;
    while (retries-- > 0) {
        if (inportb(PS2_MOUSE_PORT_STATUS) & 1) {
            response = inportb(PS2_MOUSE_PORT_DATA);
            if (response == 0xFA) break;  // ACK received
        }
    }
    
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_DATA, res);
    
    // Wait for ACK
    retries = 10000;
    while (retries-- > 0) {
        if (inportb(PS2_MOUSE_PORT_STATUS) & 1) {
            response = inportb(PS2_MOUSE_PORT_DATA);
            if (response == 0xFA) {
                resolution = res;
                return true;  // ACK received
            }
        }
    }
    
    LOG("Failed to set mouse resolution");
    return false;
}

uint8 MouseDriver::GetResolution() {
    return resolution;
}

bool MouseDriver::SetSampleRate(uint8 rate) {
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_DATA, PS2_MOUSE_CMD_SET_SAMPLE_RATE);
    
    // Wait for ACK
    uint8 response;
    int retries = 10000;
    while (retries-- > 0) {
        if (inportb(PS2_MOUSE_PORT_STATUS) & 1) {
            response = inportb(PS2_MOUSE_PORT_DATA);
            if (response == 0xFA) break;  // ACK received
        }
    }
    
    // Some sample rates need to be set multiple times due to PS/2 protocol
    // Common rates: 10, 20, 40, 60, 80, 100, 200
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_DATA, rate);
    
    // Wait for ACK
    retries = 10000;
    while (retries-- > 0) {
        if (inportb(PS2_MOUSE_PORT_STATUS) & 1) {
            response = inportb(PS2_MOUSE_PORT_DATA);
            if (response == 0xFA) {
                sample_rate = rate;
                return true;  // ACK received
            }
        }
    }
    
    LOG("Failed to set mouse sample rate");
    return false;
}

uint8 MouseDriver::GetSampleRate() {
    return sample_rate;
}

uint8 MouseDriver::GetMouseId() {
    return mouse_id;
}

bool MouseDriver::GetStatus() {
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_DATA, PS2_MOUSE_CMD_GET_STATUS);
    
    // Wait for response (3 bytes)
    uint8 status_bytes[3];
    for (int i = 0; i < 3; i++) {
        int retries = 10000;
        while (retries-- > 0) {
            if (inportb(PS2_MOUSE_PORT_STATUS) & 1) {
                status_bytes[i] = inportb(PS2_MOUSE_PORT_DATA);
                break;
            }
        }
    }
    
    return true;
}

void MouseDriver::SetCursorPosition(int32_t x, int32_t y) {
    if (x < 0) x = 0;
    if (x >= screen_width) x = screen_width - 1;
    if (y < 0) y = 0;
    if (y >= screen_height) y = screen_height - 1;
    
    cursor_x = x;
    cursor_y = y;
}

void MouseDriver::GetCursorPosition(int32_t& x, int32_t& y) {
    x = cursor_x;
    y = cursor_y;
}

void MouseDriver::UpdateCursorPosition(int8_t x_move, int8_t y_move) {
    cursor_x += x_move;
    cursor_y += y_move;
    
    // Keep cursor within screen bounds
    if (cursor_x < 0) cursor_x = 0;
    if (cursor_x >= screen_width) cursor_x = screen_width - 1;
    if (cursor_y < 0) cursor_y = 0;
    if (cursor_y >= screen_height) cursor_y = screen_height - 1;
}

bool MouseDriver::HandleIoctl(uint32 command, void* arg) {
    switch (command) {
        case MOUSE_GET_STATUS: {
            bool result = GetStatus();
            if (arg) {
                *(bool*)arg = result;
            }
            return result;
        }
        
        case MOUSE_GET_RESOLUTION: {
            uint8* res = (uint8*)arg;
            if (res) {
                *res = GetResolution();
            }
            break;
        }
        
        case MOUSE_SET_RESOLUTION: {
            uint8* new_res = (uint8*)arg;
            if (new_res) {
                return SetResolution(*new_res);
            }
            return false;
        }
        
        case MOUSE_GET_SAMPLE_RATE: {
            uint8* rate = (uint8*)arg;
            if (rate) {
                *rate = GetSampleRate();
            }
            break;
        }
        
        case MOUSE_SET_SAMPLE_RATE: {
            uint8* new_rate = (uint8*)arg;
            if (new_rate) {
                return SetSampleRate(*new_rate);
            }
            return false;
        }
        
        case MOUSE_GET_ID: {
            uint8* id = (uint8*)arg;
            if (id) {
                *id = GetMouseId();
            }
            break;
        }
        
        case MOUSE_FLUSH_BUFFER: {
            FlushBuffer();
            break;
        }
        
        case MOUSE_GET_EVENT_COUNT: {
            uint32* count = (uint32*)arg;
            if (count) {
                *count = GetEventCount();
            }
            break;
        }
        
        case MOUSE_SET_CURSOR_POSITION: {
            int32_t* pos = (int32_t*)arg;
            if (pos && screen_width > 0 && screen_height > 0) {
                SetCursorPosition(pos[0], pos[1]);
                return true;
            }
            return false;
        }
        
        case MOUSE_GET_CURSOR_POSITION: {
            int32_t* pos = (int32_t*)arg;
            if (pos) {
                GetCursorPosition(pos[0], pos[1]);
                return true;
            }
            return false;
        }
        
        default:
            return false;
    }
    
    return true;
}

// Driver framework callbacks
bool MouseDriver::MouseInit(Device* device) {
    if (!device) {
        return false;
    }
    
    // Get the mouse driver instance from private_data
    MouseDriver* driver = (MouseDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    bool result = driver->Initialize();
    if (result) {
        device->flags |= DRIVER_INITIALIZED;
        DLOG("Mouse device initialized");
    } else {
        device->flags |= DRIVER_ERROR;
    }
    
    return result;
}

bool MouseDriver::MouseRead(Device* device, void* buffer, uint32 size, uint32 offset) {
    if (!device || !buffer || size == 0) {
        return false;
    }
    
    MouseDriver* driver = (MouseDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    // This function reads mouse events
    // Size should be at least sizeof(MouseEvent)
    if (size < sizeof(MouseEvent)) {
        return false;
    }
    
    // Read one or more mouse events (if available)
    uint32 num_events = size / sizeof(MouseEvent);
    MouseEvent* events = (MouseEvent*)buffer;
    
    for (uint32 i = 0; i < num_events; i++) {
        if (!driver->GetMouseEvent(events[i])) {
            // No more events available
            if (i == 0) {
                // If we couldn't read even the first event, return false
                return false;
            }
            break;
        }
    }
    
    return true;
}

bool MouseDriver::MouseWrite(Device* device, const void* buffer, uint32 size, uint32 offset) {
    // Writing to mouse is not typically supported
    return false;
}

bool MouseDriver::MouseIoctl(Device* device, uint32 command, void* arg) {
    if (!device) {
        return false;
    }
    
    MouseDriver* driver = (MouseDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    return driver->HandleIoctl(command, arg);
}

bool MouseDriver::MouseClose(Device* device) {
    if (!device) {
        return false;
    }
    
    MouseDriver* driver = (MouseDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    // Disable mouse packet streaming
    driver->DisablePacketStreaming();
    
    // Mark as inactive
    device->flags &= ~DRIVER_ACTIVE;
    return true;
}

bool MouseDriver::SendCommand(uint8 cmd) {
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_COMMAND, cmd);
    return true;
}

bool MouseDriver::WriteData(uint8 data) {
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_DATA, data);
    return true;
}

uint8 MouseDriver::ReadData() {
    return inportb(PS2_MOUSE_PORT_DATA);
}

bool MouseDriver::WaitForInputBuffer() {
    // Wait until input buffer is empty (bit 1 of status register)
    for (int i = 0; i < 0xFFFF; i++) {
        if (!(inportb(PS2_MOUSE_PORT_STATUS) & 2)) {
            return true;
        }
    }
    return false;
}

bool MouseDriver::WaitForOutputBuffer() {
    // Wait until output buffer is full (bit 0 of status register)
    for (int i = 0; i < 0xFFFF; i++) {
        if (inportb(PS2_MOUSE_PORT_STATUS) & 1) {
            return true;
        }
    }
    return false;
}

bool MouseDriver::EnablePacketStreaming() {
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_DATA, PS2_MOUSE_CMD_ENABLE_PACKET_STREAMING);
    
    // Wait for ACK
    uint8 response;
    int retries = 10000;
    while (retries-- > 0) {
        if (inportb(PS2_MOUSE_PORT_STATUS) & 1) {
            response = inportb(PS2_MOUSE_PORT_DATA);
            if (response == 0xFA) {
                return true;  // ACK received
            }
        }
    }
    
    LOG("Failed to enable mouse packet streaming");
    return false;
}

bool MouseDriver::DisablePacketStreaming() {
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_DATA, PS2_MOUSE_CMD_DISABLE_PACKET_STREAMING);
    
    // Wait for ACK
    uint8 response;
    int retries = 10000;
    while (retries-- > 0) {
        if (inportb(PS2_MOUSE_PORT_STATUS) & 1) {
            response = inportb(PS2_MOUSE_PORT_DATA);
            if (response == 0xFA) {
                return true;  // ACK received
            }
        }
    }
    
    LOG("Failed to disable mouse packet streaming");
    return false;
}

bool MouseDriver::MouseWrite(uint8 data) {
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_COMMAND, PS2_CMD_MOUSE_WRITE);
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_MOUSE_PORT_DATA, data);
    return true;
}

bool MouseDriver::MouseRead(uint8& data) {
    // Wait for data from mouse
    int retries = 10000;
    while (retries-- > 0) {
        if (inportb(PS2_MOUSE_PORT_STATUS) & 1) {
            data = inportb(PS2_MOUSE_PORT_DATA);
            return true;
        }
    }
    return false;
}