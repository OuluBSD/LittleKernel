#include "Kernel.h"

DriverFramework::DriverFramework() {
    device_list = nullptr;
    next_device_id = 1;  // Start from 1, 0 is invalid
    lock.Initialize();

    DLOG("Driver framework initialized");
}

DriverFramework::~DriverFramework() {
    // Clean up all registered devices
    Device* current_dev = device_list;
    while (current_dev) {
        Device* next = current_dev->next;
        free(current_dev);
        current_dev = next;
    }

    DLOG("Driver framework destroyed");
}

bool DriverFramework::RegisterDevice(Device* device) {
    if (!device) {
        LOG("Cannot register null device");
        return false;
    }

    lock.Acquire();

    // Assign a unique ID if not already assigned
    if (device->id == 0) {
        device->id = next_device_id++;
    }

    // Check if device already exists
    Device* current = device_list;
    while (current) {
        if (current->id == device->id) {
            LOG("Device with ID " << device->id << " already registered");
            lock.Release();
            return false;
        }
        current = current->next;
    }

    // Add to the device list
    device->next = device_list;
    device_list = device;

    DLOG("Registered device ID " << device->id << " (" << device->name << ")");
    lock.Release();
    return true;
}

bool DriverFramework::UnregisterDevice(uint32 device_id) {
    lock.Acquire();

    Device* current = device_list;
    Device* prev = nullptr;

    while (current) {
        if (current->id == device_id) {
            if (prev) {
                prev->next = current->next;
            } else {
                device_list = current->next;
            }

            // Free the device
            free(current);

            DLOG("Unregistered device ID " << device_id);
            lock.Release();
            return true;
        }
        prev = current;
        current = current->next;
    }

    LOG("Device with ID " << device_id << " not found for unregistration");
    lock.Release();
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
    lock.Acquire();

    Device* current = device_list;
    bool all_success = true;
    while (current) {
        if (current->ops && current->ops->init) {
            if (!current->ops->init(current)) {
                LOG("Failed to initialize device: " << current->name);
                all_success = false;
            } else {
                DLOG("Initialized device: " << current->name);
            }
        }
        current = current->next;
    }

    lock.Release();
    DLOG("Device initialization completed");
    return all_success;
}

bool DriverFramework::Read(uint32 device_id, void* buffer, uint32 size, uint32 offset) {
    if (!buffer || size == 0) {
        LOG("Invalid parameters for reading from device");
        return false;
    }

    Device* device = FindDeviceById(device_id);
    if (!device) {
        LOG("Device with ID " << device_id << " not found for reading");
        return false;
    }

    if (!device->ops || !device->ops->read) {
        LOG("Device " << device->name << " does not support reading");
        return false;
    }

    return device->ops->read(device, buffer, size, offset);
}

bool DriverFramework::Write(uint32 device_id, const void* buffer, uint32 size, uint32 offset) {
    if (!buffer || size == 0) {
        LOG("Invalid parameters for writing to device");
        return false;
    }

    Device* device = FindDeviceById(device_id);
    if (!device) {
        LOG("Device with ID " << device_id << " not found for writing");
        return false;
    }

    if (!device->ops || !device->ops->write) {
        LOG("Device " << device->name << " does not support writing");
        return false;
    }

    return device->ops->write(device, buffer, size, offset);
}

bool DriverFramework::Ioctl(uint32 device_id, uint32 command, void* arg) {
    Device* device = FindDeviceById(device_id);
    if (!device) {
        LOG("Device with ID " << device_id << " not found for ioctl");
        return false;
    }

    if (!device->ops || !device->ops->ioctl) {
        LOG("Device " << device->name << " does not support ioctl commands");
        return false;
    }

    return device->ops->ioctl(device, command, arg);
}

bool DriverFramework::Close(uint32 device_id) {
    Device* device = FindDeviceById(device_id);
    if (!device) {
        LOG("Device with ID " << device_id << " not found for closing");
        return false;
    }

    if (!device->ops || !device->ops->close) {
        LOG("Device " << device->name << " does not support close operation");
        return false;
    }

    return device->ops->close(device);
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
    lock.Acquire();
    Device* first = device_list;
    lock.Release();
    return first;
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
    if (!device) return false;

    lock.Acquire();
    Device* current = device_list;
    while (current) {
        if (current == device) {
            lock.Release();
            return true;
        }
        current = current->next;
    }
    lock.Release();
    return false;
}

void DriverFramework::GenerateDeviceName(Device* device, const char* base_name) {
    if (!device || !base_name) return;

    // Simple approach - if name is empty, use the base name
    if (strlen(device->name) == 0) {
        strcpy_safe(device->name, base_name, sizeof(device->name));
    }
}