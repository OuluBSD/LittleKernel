#ifndef _Kernel_DriverFramework_h_
#define _Kernel_DriverFramework_h_

#include "Common.h"

// Device types enumeration
enum DeviceType {
    DEVICE_TYPE_UNKNOWN = 0,
    DEVICE_TYPE_CONSOLE,
    DEVICE_TYPE_KEYBOARD,
    DEVICE_TYPE_MOUSE,
    DEVICE_TYPE_DISK,
    DEVICE_TYPE_NETWORK,
    DEVICE_TYPE_GRAPHICS,
    DEVICE_TYPE_SOUND
};

// Driver flags
enum DriverFlags {
    DRIVER_INITIALIZED = 1,
    DRIVER_ACTIVE = 2,
    DRIVER_ERROR = 4
};

// Forward declaration
struct Device;

// Function pointer types for driver operations
typedef bool (*DriverInitializeFunc)(Device* device);
typedef bool (*DriverReadFunc)(Device* device, void* buffer, uint32 size, uint32 offset);
typedef bool (*DriverWriteFunc)(Device* device, const void* buffer, uint32 size, uint32 offset);
typedef bool (*DriverIoctlFunc)(Device* device, uint32 command, void* arg);
typedef bool (*DriverCloseFunc)(Device* device);

// Driver operations structure
struct DriverOperations {
    DriverInitializeFunc init;
    DriverReadFunc read;
    DriverWriteFunc write;
    DriverIoctlFunc ioctl;
    DriverCloseFunc close;
};

// Device structure
struct Device {
    uint32 id;                    // Unique device ID
    char name[64];                // Device name
    DeviceType type;              // Device type
    void* private_data;           // Driver-specific data
    uint32 flags;                 // Device flags
    DriverOperations* ops;        // Driver operations
    Device* next;                 // Next device in the list
    
    // Device-specific fields
    uint32 base_port;             // Base I/O port (if applicable)
    uint32 irq_line;              // IRQ line (if applicable)
    void* mmio_base;              // Memory-mapped I/O base address (if applicable)
};

// Driver framework class
class DriverFramework {
private:
    Device* device_list;
    uint32 next_device_id;
    Spinlock lock;

public:
    DriverFramework();
    ~DriverFramework();
    
    // Register a new device with the framework
    bool RegisterDevice(Device* device);
    
    // Unregister a device from the framework
    bool UnregisterDevice(uint32 device_id);
    
    // Find a device by ID
    Device* FindDeviceById(uint32 device_id);
    
    // Find a device by name
    Device* FindDeviceByName(const char* name);
    
    // Find a device by type
    Device* FindDeviceByType(DeviceType type);
    
    // Initialize all registered devices
    bool InitializeAllDevices();
    
    // Read from a device
    bool Read(uint32 device_id, void* buffer, uint32 size, uint32 offset = 0);
    
    // Write to a device
    bool Write(uint32 device_id, const void* buffer, uint32 size, uint32 offset = 0);
    
    // Send an IOCTL command to a device
    bool Ioctl(uint32 device_id, uint32 command, void* arg);
    
    // Close a device
    bool Close(uint32 device_id);
    
    // Enumerate devices
    uint32 GetDeviceCount();
    Device* GetFirstDevice();
    
    // Get device information
    const char* GetDeviceName(uint32 device_id);
    DeviceType GetDeviceType(uint32 device_id);
    
private:
    // Helper functions
    bool IsValidDevice(Device* device);
    void GenerateDeviceName(Device* device, const char* base_name);
};

// Global driver operations
extern DriverFramework* driver_framework;

// Common driver initialization function
bool InitializeDriverFramework();

#endif