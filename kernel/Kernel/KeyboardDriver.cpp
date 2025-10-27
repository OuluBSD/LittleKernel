#include "Kernel.h"
#include "KeyboardDriver.h"
#include "Logging.h"
#include "Interrupts.h"  // For setting up interrupt handlers

KeyboardDriver::KeyboardDriver() {
    // Initialize the device structure
    keyboard_device.id = 0;  // Will be assigned by framework
    strcpy_safe(keyboard_device.name, "keyboard0", sizeof(keyboard_device.name));
    keyboard_device.type = DEVICE_TYPE_KEYBOARD;
    keyboard_device.private_data = this;  // Point to this object for device callbacks
    keyboard_device.flags = 0;
    keyboard_device.base_port = PS2_KEYBOARD_PORT_DATA;
    keyboard_device.irq_line = PS2_KEYBOARD_IRQ;
    keyboard_device.mmio_base = nullptr;
    keyboard_device.next = nullptr;
    
    // Set up driver operations
    static DriverOperations ops = {
        KeyboardInit,
        KeyboardRead,  // Reading would return key events
        KeyboardWrite, // Not typically used for keyboard, but we'll implement for LED control
        KeyboardIoctl,
        KeyboardClose
    };
    keyboard_device.ops = &ops;
    
    // Initialize driver-specific data
    current_scancode_set = SCANCODE_SET_1;
    led_status[0] = false;  // Num Lock
    led_status[1] = false;  // Caps Lock
    led_status[2] = false;  // Scroll Lock
    buffer_lock.Initialize();
}

KeyboardDriver::~KeyboardDriver() {
    // Keyboard driver cleanup (if needed)
    // Disable keyboard interrupts if still active
    SendCommand(PS2_CMD_DISABLE_FIRST_PORT);
}

bool KeyboardDriver::Initialize() {
    LOG("Initializing PS2 Keyboard driver");
    
    // Reset and self-test the keyboard
    if (!SelfTest()) {
        LOG("Keyboard self-test failed");
        return false;
    }
    
    // Disable first PS/2 port
    if (!SendCommand(PS2_CMD_DISABLE_FIRST_PORT)) {
        LOG("Failed to disable first PS/2 port for initialization");
        return false;
    }
    
    // Flush output buffer
    while (inportb(PS2_KEYBOARD_PORT_STATUS) & 1) {
        inportb(PS2_KEYBOARD_PORT_DATA);
    }
    
    // Enable first PS/2 port
    if (!SendCommand(PS2_CMD_ENABLE_FIRST_PORT)) {
        LOG("Failed to enable first PS/2 port");
        return false;
    }
    
    // Enable keyboard interrupts
    uint8 config;
    if (!WaitForOutputBuffer()) return false;
    outportb(PS2_KEYBOARD_PORT_COMMAND, PS2_CMD_READ_CONFIG);
    if (!WaitForInputBuffer()) return false;
    config = inportb(PS2_KEYBOARD_PORT_DATA);
    
    // Enable first port interrupt
    config |= PS2_CFG_FIRST_PORT_INT;
    config |= PS2_CFG_TRANSLATION;  // Enable translation to scan code set 1
    
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_KEYBOARD_PORT_COMMAND, PS2_CMD_WRITE_CONFIG);
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_KEYBOARD_PORT_DATA, config);
    
    // Set scancode set to 1
    SetScancodeSet(SCANCODE_SET_1);
    
    // Set LEDs
    SetLeds(false, false, false);
    
    // Clear the event buffer
    FlushBuffer();
    
    LOG("PS2 Keyboard driver initialized successfully");
    return true;
}

bool KeyboardDriver::ReadScancode(uint8& scancode) {
    if (inportb(PS2_KEYBOARD_PORT_STATUS) & 1) {  // Check if output buffer has data
        scancode = inportb(PS2_KEYBOARD_PORT_DATA);
        return true;
    }
    return false;
}

void KeyboardDriver::ProcessScancode(uint8 scancode) {
    KeyboardEvent event;
    event.scancode = scancode;
    event.is_pressed = !(scancode & 0x80);  // High bit indicates key release
    event.timestamp = global_timer ? global_timer->GetTickCount() : 0;
    
    // Add event to buffer
    buffer_lock.Acquire();
    if (!event_buffer.IsFull()) {
        event_buffer.Push(event);
    } else {
        // Buffer full, drop the oldest event
        event_buffer.Pop();
        event_buffer.Push(event);
    }
    buffer_lock.Release();
}

bool KeyboardDriver::GetKeyEvent(KeyboardEvent& event) {
    buffer_lock.Acquire();
    if (!event_buffer.IsEmpty()) {
        event = event_buffer.Pop();
        buffer_lock.Release();
        return true;
    }
    buffer_lock.Release();
    return false;
}

uint32 KeyboardDriver::GetEventCount() {
    buffer_lock.Acquire();
    uint32 count = event_buffer.Count();
    buffer_lock.Release();
    return count;
}

void KeyboardDriver::FlushBuffer() {
    buffer_lock.Acquire();
    event_buffer.Clear();
    buffer_lock.Release();
}

bool KeyboardDriver::SetLeds(bool num_lock, bool caps_lock, bool scroll_lock) {
    // Update our internal state
    led_status[0] = num_lock;
    led_status[1] = caps_lock;
    led_status[2] = scroll_lock;
    
    // Calculate LED byte: bit 0 = scroll lock, bit 1 = num lock, bit 2 = caps lock
    uint8 led_byte = 0;
    if (scroll_lock) led_byte |= 1;
    if (num_lock) led_byte |= 2;
    if (caps_lock) led_byte |= 4;
    
    // Send command to set LEDs
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_KEYBOARD_PORT_DATA, 0xED);  // Set LEDs command
    
    // Wait for ACK
    uint8 response;
    int retries = 10000;  // Prevent infinite loop
    while (retries-- > 0) {
        if (inportb(PS2_KEYBOARD_PORT_STATUS) & 1) {  // Check output buffer
            response = inportb(PS2_KEYBOARD_PORT_DATA);
            if (response == 0xFA) break;  // ACK received
        }
    }
    
    // Send LED states
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_KEYBOARD_PORT_DATA, led_byte);
    
    // Wait for ACK
    retries = 10000;
    while (retries-- > 0) {
        if (inportb(PS2_KEYBOARD_PORT_STATUS) & 1) {  // Check output buffer
            response = inportb(PS2_KEYBOARD_PORT_DATA);
            if (response == 0xFA) return true;  // ACK received
        }
    }
    
    LOG("Failed to set keyboard LEDs");
    return false;
}

bool KeyboardDriver::GetLeds(bool& num_lock, bool& caps_lock, bool& scroll_lock) {
    num_lock = led_status[0];
    caps_lock = led_status[1];
    scroll_lock = led_status[2];
    return true;
}

bool KeyboardDriver::SetScancodeSet(uint8 set) {
    if (set != SCANCODE_SET_1 && set != SCANCODE_SET_2) {
        return false;
    }
    
    // Send command to set scancode set
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_KEYBOARD_PORT_DATA, 0xF0);  // Set scancode set command
    
    // Wait for ACK
    uint8 response;
    int retries = 10000;
    while (retries-- > 0) {
        if (inportb(PS2_KEYBOARD_PORT_STATUS) & 1) {  // Check output buffer
            response = inportb(PS2_KEYBOARD_PORT_DATA);
            if (response == 0xFA) break;  // ACK received
        }
    }
    
    // Send the new set number
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_KEYBOARD_PORT_DATA, set);
    
    // Wait for ACK
    retries = 10000;
    while (retries-- > 0) {
        if (inportb(PS2_KEYBOARD_PORT_STATUS) & 1) {  // Check output buffer
            response = inportb(PS2_KEYBOARD_PORT_DATA);
            if (response == 0xFA) {
                current_scancode_set = set;
                return true;  // ACK received
            }
        }
    }
    
    LOG("Failed to set scancode set");
    return false;
}

uint8 KeyboardDriver::GetScancodeSet() {
    return current_scancode_set;
}

bool KeyboardDriver::HandleIoctl(uint32 command, void* arg) {
    switch (command) {
        case KEYBOARD_GET_SCANCODE_SET: {
            uint8* set = (uint8*)arg;
            if (set) {
                *set = GetScancodeSet();
            }
            break;
        }
        
        case KEYBOARD_SET_SCANCODE_SET: {
            uint8* new_set = (uint8*)arg;
            if (new_set) {
                return SetScancodeSet(*new_set);
            }
            return false;
        }
        
        case KEYBOARD_GET_LEDS: {
            bool* leds = (bool*)arg;
            if (leds) {
                bool num_lock, caps_lock, scroll_lock;
                if (GetLeds(num_lock, caps_lock, scroll_lock)) {
                    leds[0] = num_lock;
                    leds[1] = caps_lock;
                    leds[2] = scroll_lock;
                    return true;
                }
            }
            return false;
        }
        
        case KEYBOARD_SET_LEDS: {
            bool* leds = (bool*)arg;
            if (leds) {
                return SetLeds(leds[0], leds[1], leds[2]);  // num_lock, caps_lock, scroll_lock
            }
            return false;
        }
        
        case KEYBOARD_FLUSH_BUFFER: {
            FlushBuffer();
            break;
        }
        
        case KEYBOARD_GET_EVENT_COUNT: {
            uint32* count = (uint32*)arg;
            if (count) {
                *count = GetEventCount();
            }
            break;
        }
        
        default:
            return false;
    }
    
    return true;
}

// Driver framework callbacks
bool KeyboardDriver::KeyboardInit(Device* device) {
    if (!device) {
        return false;
    }
    
    // Get the keyboard driver instance from private_data
    KeyboardDriver* driver = (KeyboardDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    bool result = driver->Initialize();
    if (result) {
        device->flags |= DRIVER_INITIALIZED;
        DLOG("Keyboard device initialized");
    } else {
        device->flags |= DRIVER_ERROR;
    }
    
    return result;
}

bool KeyboardDriver::KeyboardRead(Device* device, void* buffer, uint32 size, uint32 offset) {
    if (!device || !buffer || size == 0) {
        return false;
    }
    
    KeyboardDriver* driver = (KeyboardDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    // This function reads keyboard events
    // Size should be at least sizeof(KeyboardEvent)
    if (size < sizeof(KeyboardEvent)) {
        return false;
    }
    
    // Read one or more keyboard events (if available)
    uint32 num_events = size / sizeof(KeyboardEvent);
    KeyboardEvent* events = (KeyboardEvent*)buffer;
    
    for (uint32 i = 0; i < num_events; i++) {
        if (!driver->GetKeyEvent(events[i])) {
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

bool KeyboardDriver::KeyboardWrite(Device* device, const void* buffer, uint32 size, uint32 offset) {
    if (!device || !buffer || size == 0) {
        return false;
    }
    
    KeyboardDriver* driver = (KeyboardDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    // Writing to keyboard is typically for LED control
    // We'll interpret the data as a boolean array [num_lock, caps_lock, scroll_lock]
    if (size >= 3 * sizeof(bool)) {
        bool* leds = (bool*)buffer;
        return driver->SetLeds(leds[0], leds[1], leds[2]);
    }
    
    return false;
}

bool KeyboardDriver::KeyboardIoctl(Device* device, uint32 command, void* arg) {
    if (!device) {
        return false;
    }
    
    KeyboardDriver* driver = (KeyboardDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    return driver->HandleIoctl(command, arg);
}

bool KeyboardDriver::KeyboardClose(Device* device) {
    if (!device) {
        return false;
    }
    
    // For keyboard, just mark as inactive
    device->flags &= ~DRIVER_ACTIVE;
    return true;
}

bool KeyboardDriver::SendCommand(uint8 cmd) {
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_KEYBOARD_PORT_COMMAND, cmd);
    return true;
}

bool KeyboardDriver::WriteData(uint8 data) {
    if (!WaitForInputBuffer()) return false;
    outportb(PS2_KEYBOARD_PORT_DATA, data);
    return true;
}

uint8 KeyboardDriver::ReadData() {
    return inportb(PS2_KEYBOARD_PORT_DATA);
}

bool KeyboardDriver::WaitForInputBuffer() {
    // Wait until input buffer is empty (bit 1 of status register)
    for (int i = 0; i < 0xFFFF; i++) {
        if (!(inportb(PS2_KEYBOARD_PORT_STATUS) & 2)) {
            return true;
        }
    }
    return false;
}

bool KeyboardDriver::WaitForOutputBuffer() {
    // Wait until output buffer is full (bit 0 of status register)
    for (int i = 0; i < 0xFFFF; i++) {
        if (inportb(PS2_KEYBOARD_PORT_STATUS) & 1) {
            return true;
        }
    }
    return false;
}

bool KeyboardDriver::SelfTest() {
    // Disable first PS/2 port
    if (!SendCommand(PS2_CMD_DISABLE_FIRST_PORT)) return false;
    
    // Flush output buffer
    while (inportb(PS2_KEYBOARD_PORT_STATUS) & 1) {
        inportb(PS2_KEYBOARD_PORT_DATA);
    }
    
    // Enable first PS/2 port
    if (!SendCommand(PS2_CMD_ENABLE_FIRST_PORT)) return false;
    
    return true;
}