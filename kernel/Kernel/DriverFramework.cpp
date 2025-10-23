#include "Kernel.h"
#include "DriverFramework.h"
#include "Logging.h"

// Global driver framework instance
DriverFramework* driver_framework = nullptr;

DriverFramework::DriverFramework() {
    device_list = nullptr;
    next_device_id = 1;
    lock.Initialize();
}

DriverFramework::~DriverFramework() {
    // Unregister all devices
    Device* current = device_list;
    while (current) {
        Device* next = current->next;
        if (current->ops && current->ops->close) {
            current->ops->close(current);
        }
        free(current);
        current = next;
    }
    device_list = nullptr;
}

bool DriverFramework::RegisterDevice(Device* device) {
    if (!device) {
        LOG("Cannot register null device");
        return false;
    }
    
    if (!IsValidDevice(device)) {
        LOG("Invalid device provided for registration");
        return false;
    }
    
    lock.Acquire();
    
    // Assign a unique ID if not already assigned
    if (device->id == 0) {
        device->id = next_device_id++;
    }
    
    // Add to the beginning of the device list
    device->next = device_list;
    device_list = device;
    
    DLOG("Registered device ID " << device->id << " (" << device->name << ")");
    
    lock.Release();
    
    // Initialize the device if it has an init function
    if (device->ops && device->ops->init) {
        if (device->ops->init(device)) {
            device->flags |= DRIVER_INITIALIZED;
            LOG("Device ID " << device->id << " initialized successfully");
        } else {
            LOG("Failed to initialize device ID " << device->id);
            device->flags |= DRIVER_ERROR;
            return false;
        }
    }
    
    return true;
}

bool DriverFramework::UnregisterDevice(uint32 device_id) {
    lock.Acquire();
    
    Device* current = device_list;
    Device* prev = nullptr;
    
    while (current) {
        if (current->id == device_id) {
            // Remove from the list
            if (prev) {
                prev->next = current->next;
            } else {
                device_list = current->next;
            }
            
            // Close the device if it has a close function
            if (current->ops && current->ops->close) {
                current->ops->close(current);
            }
            
            DLOG("Unregistered device ID " << device_id << " (" << current->name << ")");
            
            // Free the device structure (but not private_data, that's up to the driver)
            free(current);
            
            lock.Release();
            return true;
        }
        
        prev = current;
        current = current->next;
    }
    
    lock.Release();
    LOG("Device ID " << device_id << " not found for unregistration");
    return false;
}

Device* DriverFramework::FindDeviceById(uint32 device_id) {
    lock.Acquire();
    
    Device* current = device_list;
    while (current) {
        if (current->id == device_id) {
            lock.Release();
            return current;
        }
        current = current->next;
    }
    
    lock.Release();
    return nullptr;
}

Device* DriverFramework::FindDeviceByName(const char* name) {
    if (!name) return nullptr;
    
    lock.Acquire();
    
    Device* current = device_list;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            lock.Release();
            return current;
        }
        current = current->next;
    }
    
    lock.Release();
    return nullptr;
}

Device* DriverFramework::FindDeviceByType(DeviceType type) {
    lock.Acquire();
    
    Device* current = device_list;
    while (current) {
        if (current->type == type) {
            lock.Release();
            return current;
        }
        current = current->next;
    }
    
    lock.Release();
    return nullptr;
}

bool DriverFramework::InitializeAllDevices() {
    bool all_success = true;
    
    lock.Acquire();
    
    Device* current = device_list;
    while (current) {
        if (current->ops && current->ops->init && !(current->flags & DRIVER_INITIALIZED)) {
            if (current->ops->init(current)) {
                current->flags |= DRIVER_INITIALIZED;
                DLOG("Device ID " << current->id << " (" << current->name << ") initialized");
            } else {
                LOG("Failed to initialize device ID " << current->id << " (" << current->name << ")");
                current->flags |= DRIVER_ERROR;
                all_success = false;
            }
        }
        current = current->next;
    }
    
    lock.Release();
    
    return all_success;
}

bool DriverFramework::Read(uint32 device_id, void* buffer, uint32 size, uint32 offset) {
    Device* device = FindDeviceById(device_id);
    if (!device) {
        LOG("Device ID " << device_id << " not found for read operation");
        return false;
    }
    
    if (!(device->flags & DRIVER_INITIALIZED)) {
        LOG("Device ID " << device_id << " is not initialized");
        return false;
    }
    
    if (!device->ops || !device->ops->read) {
        LOG("Device ID " << device_id << " does not support read operations");
        return false;
    }
    
    return device->ops->read(device, buffer, size, offset);
}

bool DriverFramework::Write(uint32 device_id, const void* buffer, uint32 size, uint32 offset) {
    Device* device = FindDeviceById(device_id);
    if (!device) {
        LOG("Device ID " << device_id << " not found for write operation");
        return false;
    }
    
    if (!(device->flags & DRIVER_INITIALIZED)) {
        LOG("Device ID " << device_id << " is not initialized");
        return false;
    }
    
    if (!device->ops || !device->ops->write) {
        LOG("Device ID " << device_id << " does not support write operations");
        return false;
    }
    
    return device->ops->write(device, buffer, size, offset);
}

bool DriverFramework::Ioctl(uint32 device_id, uint32 command, void* arg) {
    Device* device = FindDeviceById(device_id);
    if (!device) {
        LOG("Device ID " << device_id << " not found for IOCTL operation");
        return false;
    }
    
    if (!(device->flags & DRIVER_INITIALIZED)) {
        LOG("Device ID " << device_id << " is not initialized");
        return false;
    }
    
    if (!device->ops || !device->ops->ioctl) {
        LOG("Device ID " << device_id << " does not support IOCTL operations");
        return false;
    }
    
    return device->ops->ioctl(device, command, arg);
}

bool DriverFramework::Close(uint32 device_id) {
    Device* device = FindDeviceById(device_id);
    if (!device) {
        LOG("Device ID " << device_id << " not found for close operation");
        return false;
    }
    
    if (!device->ops || !device->ops->close) {
        LOG("Device ID " << device_id << " does not support close operations");
        return false;
    }
    
    bool result = device->ops->close(device);
    if (result) {
        device->flags &= ~DRIVER_ACTIVE;
    }
    
    return result;
}

uint32 DriverFramework::GetDeviceCount() {
    lock.Acquire();
    
    uint32 count = 0;
    Device* current = device_list;
    while (current) {
        count++;
        current = current->next;
    }
    
    lock.Release();
    return count;
}

Device* DriverFramework::GetFirstDevice() {
    return device_list;
}

const char* DriverFramework::GetDeviceName(uint32 device_id) {
    Device* device = FindDeviceById(device_id);
    return device ? device->name : nullptr;
}

DeviceType DriverFramework::GetDeviceType(uint32 device_id) {
    Device* device = FindDeviceById(device_id);
    return device ? device->type : DEVICE_TYPE_UNKNOWN;
}

bool DriverFramework::IsValidDevice(Device* device) {
    // Basic validation
    if (!device) return false;
    if (device->name[0] == '\0') return false;  // Must have a name
    
    // If device has operations, check if required ones are valid
    if (device->ops) {
        // For now, we'll accept devices without read/write operations
        // since some devices might only have ioctl operations
    }
    
    return true;
}

void DriverFramework::GenerateDeviceName(Device* device, const char* base_name) {
    if (!device || !base_name) return;
    
    // Simple generation: append the ID to the base name
    snprintf_s(device->name, sizeof(device->name), "%s_%d", base_name, device->id);
}

// Initialize the driver framework
bool InitializeDriverFramework() {
    if (!driver_framework) {
        driver_framework = new DriverFramework();
        if (!driver_framework) {
            LOG("Failed to create driver framework instance");
            return false;
        }
        LOG("Driver framework initialized successfully");
    }
    
    return true;
}