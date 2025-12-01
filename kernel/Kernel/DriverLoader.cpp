#include "Kernel.h"
#include "DriverLoader.h"
#include "Logging.h"
#include "ModuleLoader.h"

// Global driver loader instance
DriverLoader* g_driver_loader = nullptr;

DriverLoader::DriverLoader() {
    driver_count = 0;
    loader_lock.Initialize();
    
    // Initialize all driver slots as empty
    for (uint32_t i = 0; i < MAX_LOADED_DRIVERS; i++) {
        loaded_drivers[i].is_loaded = false;
        loaded_drivers[i].name[0] = '\0';
    }
}

DriverLoader::~DriverLoader() {
    // Unload all loaded drivers
    for (uint32_t i = 0; i < driver_count; i++) {
        if (loaded_drivers[i].is_loaded) {
            InternalUnloadDriver(i, true);  // Force unload
        }
    }
}

bool DriverLoader::Initialize() {
    LOG("Initializing Driver Loader system");
    
    // Check if module loader is available
    if (!g_module_loader) {
        LOG("Module loader not available - required for driver loading");
        return false;
    }
    
    // Initialize the driver framework if not already done
    if (!driver_framework) {
        if (!InitializeDriverFramework()) {
            LOG("Failed to initialize driver framework");
            return false;
        }
    }
    
    LOG("Driver Loader system initialized successfully");
    return true;
}

DriverLoadResult DriverLoader::LoadDriver(const char* driver_name, const char* driver_path) {
    if (!driver_name || !driver_path) {
        return DriverLoadResult::FAILED;
    }
    
    loader_lock.Acquire();
    
    // Check if driver is already loaded
    if (FindDriverIndex(driver_name) != MAX_LOADED_DRIVERS) {
        loader_lock.Release();
        return DriverLoadResult::ALREADY_LOADED;
    }
    
    // Find a free slot
    uint32_t slot_idx = FindFreeSlot();
    if (slot_idx == MAX_LOADED_DRIVERS) {
        LOG("No free slots for new driver: " << driver_name);
        loader_lock.Release();
        return DriverLoadResult::INSUFFICIENT_RESOURCES;
    }
    
    // Load the module using the module loader
    ModuleInfo* module = g_module_loader->GetModuleInfo(driver_path);
    if (!module) {
        // If the module isn't already loaded, load it from file
        ModuleLoadResult load_result = g_module_loader->LoadModuleFromFile(driver_path);
        if (load_result != ModuleLoadResult::SUCCESS) {
            LOG("Failed to load driver module: " << driver_path << " (result: " << (int)load_result << ")");
            loader_lock.Release();
            return DriverLoadResult::FAILED;
        }

        // Try to get the module info again
        module = g_module_loader->GetModuleInfo(driver_path);
        if (!module) {
            LOG("Loaded module but couldn't get info: " << driver_path);
            loader_lock.Release();
            return DriverLoadResult::FAILED;
        }
    }

    // Get the module base address
    void* module_base = module->base_address;
    if (!module_base) {
        LOG("Failed to get module base for: " << driver_name);
        loader_lock.Release();
        return DriverLoadResult::FAILED;
    }

    // Validate the driver module
    if (!ValidateDriverModule(module_base, module->size)) {
        LOG("Driver module validation failed: " << driver_name);
        loader_lock.Release();
        return DriverLoadResult::INVALID_FORMAT;
    }
    
    // Load the driver
    DriverLoadResult result = InternalLoadDriver(driver_name, module_base);
    
    if (result != DriverLoadResult::SUCCESS) {
        LOG("Failed to load driver: " << driver_name << " (result: " << (int)result << ")");
        g_module_loader->UnloadModule(driver_path);
    }
    
    loader_lock.Release();
    return result;
}

DriverLoadResult DriverLoader::UnloadDriver(const char* driver_name) {
    if (!driver_name) {
        return DriverLoadResult::FAILED;
    }
    
    loader_lock.Acquire();
    
    uint32_t index = FindDriverIndex(driver_name);
    if (index == MAX_LOADED_DRIVERS) {
        loader_lock.Release();
        return DriverLoadResult::FAILED;  // Driver not found
    }
    
    // Check reference count
    if (loaded_drivers[index].ref_count > 0) {
        LOG("Cannot unload driver " << driver_name << ", ref_count: " << loaded_drivers[index].ref_count);
        loader_lock.Release();
        return DriverLoadResult::FAILED;  // Still in use
    }
    
    DriverLoadResult result = InternalUnloadDriver(index);
    
    loader_lock.Release();
    return result;
}

DriverLoadResult DriverLoader::UnloadDriverById(uint32_t device_id) {
    loader_lock.Acquire();
    
    uint32_t index = FindDriverIndexById(device_id);
    if (index == MAX_LOADED_DRIVERS) {
        loader_lock.Release();
        return DriverLoadResult::FAILED;  // Driver not found
    }
    
    // Check reference count
    if (loaded_drivers[index].ref_count > 0) {
        LOG("Cannot unload driver for device ID " << device_id << ", ref_count: " << loaded_drivers[index].ref_count);
        loader_lock.Release();
        return DriverLoadResult::FAILED;  // Still in use
    }
    
    DriverLoadResult result = InternalUnloadDriver(index);
    
    loader_lock.Release();
    return result;
}

bool DriverLoader::GetDriverInfo(const char* driver_name, LoadedDriverInfo& info) {
    if (!driver_name) {
        return false;
    }
    
    loader_lock.Acquire();
    
    uint32_t index = FindDriverIndex(driver_name);
    if (index == MAX_LOADED_DRIVERS) {
        loader_lock.Release();
        return false;
    }
    
    info = loaded_drivers[index];
    loader_lock.Release();
    return true;
}

bool DriverLoader::GetDriverInfoById(uint32_t device_id, LoadedDriverInfo& info) {
    loader_lock.Acquire();
    
    uint32_t index = FindDriverIndexById(device_id);
    if (index == MAX_LOADED_DRIVERS) {
        loader_lock.Release();
        return false;
    }
    
    info = loaded_drivers[index];
    loader_lock.Release();
    return true;
}

bool DriverLoader::IsDriverLoaded(const char* driver_name) {
    if (!driver_name) {
        return false;
    }
    
    loader_lock.Acquire();
    bool loaded = FindDriverIndex(driver_name) != MAX_LOADED_DRIVERS;
    loader_lock.Release();
    return loaded;
}

uint32_t DriverLoader::GetLoadedDriverCount() {
    loader_lock.Acquire();
    uint32_t count = driver_count;
    loader_lock.Release();
    return count;
}

bool DriverLoader::GetNextDriverInfo(uint32_t index, LoadedDriverInfo& info) {
    if (index >= MAX_LOADED_DRIVERS) {
        return false;
    }
    
    loader_lock.Acquire();
    
    uint32_t valid_drivers_found = 0;
    for (uint32_t i = 0; i < MAX_LOADED_DRIVERS; i++) {
        if (loaded_drivers[i].is_loaded) {
            if (valid_drivers_found == index) {
                info = loaded_drivers[i];
                loader_lock.Release();
                return true;
            }
            valid_drivers_found++;
        }
    }
    
    loader_lock.Release();
    return false;
}

bool DriverLoader::ValidateDriverModule(void* module_base, uint32_t size) {
    if (!module_base || size == 0) {
        return false;
    }
    
    // Basic validation:
    // 1. Check if the module has a valid ELF header (for ELF-based modules)
    // In a real implementation, we'd check the ELF header here
    
    // For now, we'll do a basic check for a potential driver entry point
    // This is a simplified implementation
    
    // Check if the module has basic driver symbols
    // In a real implementation, we'd use dynamic symbol lookup
    
    // For now, just return true - in a real system, this would be more thorough
    return CheckDriverSignature(module_base) && CheckDriverCompatibility(module_base);
}

bool DriverLoader::IncrementReferenceCount(const char* driver_name) {
    if (!driver_name) {
        return false;
    }
    
    loader_lock.Acquire();
    
    uint32_t index = FindDriverIndex(driver_name);
    if (index == MAX_LOADED_DRIVERS) {
        loader_lock.Release();
        return false;
    }
    
    loaded_drivers[index].ref_count++;
    loader_lock.Release();
    return true;
}

bool DriverLoader::DecrementReferenceCount(const char* driver_name) {
    if (!driver_name) {
        return false;
    }
    
    loader_lock.Acquire();
    
    uint32_t index = FindDriverIndex(driver_name);
    if (index == MAX_LOADED_DRIVERS) {
        loader_lock.Release();
        return false;
    }
    
    if (loaded_drivers[index].ref_count > 0) {
        loaded_drivers[index].ref_count--;
        loader_lock.Release();
        return true;
    }
    
    loader_lock.Release();
    return false;  // Already at 0
}

DriverLoadResult DriverLoader::RegisterDriver(DriverBase* driver, Device* device, const char* driver_name) {
    if (!driver || !device || !driver_name) {
        return DriverLoadResult::FAILED;
    }
    
    loader_lock.Acquire();
    
    // Check if driver is already registered
    if (FindDriverIndex(driver_name) != MAX_LOADED_DRIVERS) {
        loader_lock.Release();
        return DriverLoadResult::ALREADY_LOADED;
    }
    
    // Find a free slot
    uint32_t slot_idx = FindFreeSlot();
    if (slot_idx == MAX_LOADED_DRIVERS) {
        LOG("No free slots for new driver: " << driver_name);
        loader_lock.Release();
        return DriverLoadResult::INSUFFICIENT_RESOURCES;
    }
    
    // Initialize the driver info structure
    LoadedDriverInfo& driver_info = loaded_drivers[slot_idx];
    strcpy_safe(driver_info.name, driver_name, sizeof(driver_info.name));
    strcpy_safe(driver_info.version, "1.0", sizeof(driver_info.version));  // Default version
    driver_info.load_address = (uint32_t)driver;  // For built-in drivers, store the instance address
    driver_info.size = 0;  // Built-in drivers don't have a module size
    driver_info.driver_instance = driver;
    driver_info.device = device;
    driver_info.is_loaded = true;
    driver_info.ref_count = 0;
    driver_info.timestamp = global_timer ? global_timer->GetTickCount() : 0;
    
    // Register the device with the driver framework
    if (driver_framework && !driver_framework->RegisterDevice(device)) {
        LOG("Failed to register device with driver framework for: " << driver_name);
        memset(&driver_info, 0, sizeof(driver_info));
        driver_info.is_loaded = false;
        loader_lock.Release();
        return DriverLoadResult::FAILED;
    }
    
    driver_count++;
    
    LOG("Driver registered successfully: " << driver_name);
    loader_lock.Release();
    return DriverLoadResult::SUCCESS;
}

uint32_t DriverLoader::FindDriverIndex(const char* driver_name) {
    for (uint32_t i = 0; i < MAX_LOADED_DRIVERS; i++) {
        if (loaded_drivers[i].is_loaded && 
            strncmp(loaded_drivers[i].name, driver_name, sizeof(loaded_drivers[i].name)) == 0) {
            return i;
        }
    }
    return MAX_LOADED_DRIVERS;  // Not found
}

uint32_t DriverLoader::FindDriverIndexById(uint32_t device_id) {
    for (uint32_t i = 0; i < MAX_LOADED_DRIVERS; i++) {
        if (loaded_drivers[i].is_loaded && 
            loaded_drivers[i].device && 
            loaded_drivers[i].device->id == device_id) {
            return i;
        }
    }
    return MAX_LOADED_DRIVERS;  // Not found
}

uint32_t DriverLoader::FindFreeSlot() {
    for (uint32_t i = 0; i < MAX_LOADED_DRIVERS; i++) {
        if (!loaded_drivers[i].is_loaded) {
            return i;
        }
    }
    return MAX_LOADED_DRIVERS;  // No free slots
}

DriverLoadResult DriverLoader::InternalLoadDriver(const char* driver_name, void* module_base) {
    // This is a simplified implementation
    // In a real system, this would:
    // 1. Parse the driver module to find the driver's entry point
    // 2. Call the driver's initialization code
    // 3. Register the driver with the device framework
    // 4. Store information about the loaded driver
    
    // For this implementation, we'll log that we're attempting to load
    LOG("Loading driver: " << driver_name << " (at 0x" << (uint32_t)module_base << ")");
    
    // Find a free slot
    uint32_t slot_idx = FindFreeSlot();
    if (slot_idx == MAX_LOADED_DRIVERS) {
        return DriverLoadResult::INSUFFICIENT_RESOURCES;
    }
    
    // In a real implementation, we would extract the driver information from the module
    // For now, we'll create a placeholder entry
    LoadedDriverInfo& driver_info = loaded_drivers[slot_idx];
    strcpy_safe(driver_info.name, driver_name, sizeof(driver_info.name));
    strcpy_safe(driver_info.version, "1.0", sizeof(driver_info.version));
    driver_info.load_address = (uint32_t)module_base;
    driver_info.size = 0;  // Would be determined from the module
    driver_info.driver_instance = nullptr;  // Would be the actual driver instance
    driver_info.device = nullptr;  // Would be the registered device
    driver_info.is_loaded = true;
    driver_info.ref_count = 0;
    driver_info.timestamp = global_timer ? global_timer->GetTickCount() : 0;
    
    driver_count++;
    
    LOG("Driver loaded successfully: " << driver_name);
    return DriverLoadResult::SUCCESS;
}

DriverLoadResult DriverLoader::InternalUnloadDriver(uint32_t index, bool force_unload) {
    if (index >= MAX_LOADED_DRIVERS || !loaded_drivers[index].is_loaded) {
        return DriverLoadResult::FAILED;
    }
    
    // Check if we can unload (if not forced and ref_count > 0)
    if (!force_unload && loaded_drivers[index].ref_count > 0) {
        LOG("Cannot unload driver " << loaded_drivers[index].name << ", ref_count: " << loaded_drivers[index].ref_count);
        return DriverLoadResult::FAILED;
    }
    
    // If there's an associated device, unregister it
    if (loaded_drivers[index].device && driver_framework) {
        driver_framework->UnregisterDevice(loaded_drivers[index].device->id);
    }
    
    // If there's an associated driver instance and it has a shutdown method, call it
    if (loaded_drivers[index].driver_instance) {
        loaded_drivers[index].driver_instance->Shutdown();
    }
    
    // In a real system, we would also free the allocated memory for the module
    // and perform cleanup
    
    // Clear the driver info
    memset(&loaded_drivers[index], 0, sizeof(LoadedDriverInfo));
    loaded_drivers[index].is_loaded = false;
    
    if (driver_count > 0) {
        driver_count--;
    }
    
    LOG("Driver unloaded: " << loaded_drivers[index].name);
    return DriverLoadResult::SUCCESS;
}

bool DriverLoader::CheckDriverSignature(void* module_base) {
    // Placeholder implementation
    // In a real system, this would verify a cryptographic signature
    return true;
}

bool DriverLoader::CheckDriverCompatibility(void* module_base) {
    // Placeholder implementation
    // In a real system, this would check if the driver is compatible
    // with the current kernel version and architecture
    return true;
}

bool InitializeDriverLoader() {
    if (!g_driver_loader) {
        g_driver_loader = new DriverLoader();
        if (!g_driver_loader) {
            LOG("Failed to create driver loader instance");
            return false;
        }
        
        if (!g_driver_loader->Initialize()) {
            LOG("Failed to initialize driver loader");
            delete g_driver_loader;
            g_driver_loader = nullptr;
            return false;
        }
        
        LOG("Driver loader initialized successfully");
    }
    
    return true;
}