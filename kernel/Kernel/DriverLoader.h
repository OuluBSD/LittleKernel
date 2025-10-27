#ifndef _Kernel_DriverLoader_h_
#define _Kernel_DriverLoader_h_

#include "Common.h"
#include "DriverFramework.h"
#include "ModuleLoader.h"  // Use existing module loading infrastructure

// Driver loading result codes
enum class DriverLoadResult {
    SUCCESS = 0,
    FAILED = -1,
    ALREADY_LOADED = -2,
    NOT_SUPPORTED = -3,
    INSUFFICIENT_RESOURCES = -4,
    INVALID_FORMAT = -5,
    DEPENDENCY_MISSING = -6
};

// Information about a loaded driver
struct LoadedDriverInfo {
    char name[64];                    // Driver name
    char version[16];                 // Driver version
    uint32 load_address;            // Address where driver is loaded
    uint32 size;                    // Size of driver in memory
    DriverBase* driver_instance;      // Pointer to driver instance
    Device* device;                   // Associated device
    bool is_loaded;                   // Whether currently loaded
    uint32 ref_count;               // Reference count
    uint32 timestamp;               // When loaded
};

// Driver loader class that manages dynamic loading/unloading of drivers
class DriverLoader {
private:
    static const uint32 MAX_LOADED_DRIVERS = 64;
    LoadedDriverInfo loaded_drivers[MAX_LOADED_DRIVERS];
    uint32 driver_count;
    Spinlock loader_lock;             // Lock for thread safety
    
public:
    DriverLoader();
    ~DriverLoader();
    
    // Initialize the driver loader system
    bool Initialize();
    
    // Load a driver from a file/module
    DriverLoadResult LoadDriver(const char* driver_name, const char* driver_path);
    
    // Unload a driver by name
    DriverLoadResult UnloadDriver(const char* driver_name);
    
    // Unload a driver by device ID
    DriverLoadResult UnloadDriverById(uint32 device_id);
    
    // Get information about a loaded driver
    bool GetDriverInfo(const char* driver_name, LoadedDriverInfo& info);
    bool GetDriverInfoById(uint32 device_id, LoadedDriverInfo& info);
    
    // Check if a driver is currently loaded
    bool IsDriverLoaded(const char* driver_name);
    
    // Get loaded driver count
    uint32 GetLoadedDriverCount();
    
    // Enumerate loaded drivers
    bool GetNextDriverInfo(uint32 index, LoadedDriverInfo& info);
    
    // Validate a driver module before loading
    bool ValidateDriverModule(void* module_base, uint32 size);
    
    // Update driver reference count
    bool IncrementReferenceCount(const char* driver_name);
    bool DecrementReferenceCount(const char* driver_name);
    
    // Register a pre-loaded driver (for built-in drivers)
    DriverLoadResult RegisterDriver(DriverBase* driver, Device* device, const char* driver_name);

private:
    // Internal helper functions
    uint32 FindDriverIndex(const char* driver_name);
    uint32 FindDriverIndexById(uint32 device_id);
    uint32 FindFreeSlot();
    
    // Driver loading/unloading functions
    DriverLoadResult InternalLoadDriver(const char* driver_name, void* module_base);
    DriverLoadResult InternalUnloadDriver(uint32 index, bool force_unload = false);
    
    // Security and validation functions
    bool CheckDriverSignature(void* module_base);
    bool CheckDriverCompatibility(void* module_base);
};

// Global driver loader instance
extern DriverLoader* g_driver_loader;

// Initialize the global driver loader
bool InitializeDriverLoader();

#endif