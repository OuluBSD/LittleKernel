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
    uint32_t load_address;            // Address where driver is loaded
    uint32_t size;                    // Size of driver in memory
    DriverBase* driver_instance;      // Pointer to driver instance
    Device* device;                   // Associated device
    bool is_loaded;                   // Whether currently loaded
    uint32_t ref_count;               // Reference count
    uint32_t timestamp;               // When loaded
};

// Driver loader class that manages dynamic loading/unloading of drivers
class DriverLoader {
private:
    static const uint32_t MAX_LOADED_DRIVERS = 64;
    LoadedDriverInfo loaded_drivers[MAX_LOADED_DRIVERS];
    uint32_t driver_count;
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
    DriverLoadResult UnloadDriverById(uint32_t device_id);
    
    // Get information about a loaded driver
    bool GetDriverInfo(const char* driver_name, LoadedDriverInfo& info);
    bool GetDriverInfoById(uint32_t device_id, LoadedDriverInfo& info);
    
    // Check if a driver is currently loaded
    bool IsDriverLoaded(const char* driver_name);
    
    // Get loaded driver count
    uint32_t GetLoadedDriverCount();
    
    // Enumerate loaded drivers
    bool GetNextDriverInfo(uint32_t index, LoadedDriverInfo& info);
    
    // Validate a driver module before loading
    bool ValidateDriverModule(void* module_base, uint32_t size);
    
    // Update driver reference count
    bool IncrementReferenceCount(const char* driver_name);
    bool DecrementReferenceCount(const char* driver_name);
    
    // Register a pre-loaded driver (for built-in drivers)
    DriverLoadResult RegisterDriver(DriverBase* driver, Device* device, const char* driver_name);

private:
    // Internal helper functions
    uint32_t FindDriverIndex(const char* driver_name);
    uint32_t FindDriverIndexById(uint32_t device_id);
    uint32_t FindFreeSlot();
    
    // Driver loading/unloading functions
    DriverLoadResult InternalLoadDriver(const char* driver_name, void* module_base);
    DriverLoadResult InternalUnloadDriver(uint32_t index, bool force_unload = false);
    
    // Security and validation functions
    bool CheckDriverSignature(void* module_base);
    bool CheckDriverCompatibility(void* module_base);
};

// Global driver loader instance
extern DriverLoader* g_driver_loader;

// Initialize the global driver loader
bool InitializeDriverLoader();

#endif