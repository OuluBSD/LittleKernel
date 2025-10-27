#ifndef _Kernel_RuntimeConfig_h_
#define _Kernel_RuntimeConfig_h_

#include "Defs.h"
#include "KernelConfig.h"

// Configuration change notification callback
typedef void (*ConfigChangeCallback)(const char* config_name, void* old_value, void* new_value);

// Runtime configuration entry
struct RuntimeConfigEntry {
    char name[64];                    // Configuration name
    void* value;                      // Pointer to the configuration value
    uint32 size;                    // Size of the configuration value in bytes
    ConfigChangeCallback callback;    // Callback for when the value changes
    bool is_readonly;                 // Whether this configuration can be changed at runtime
    void* default_value;              // Default value for this configuration
};

// Runtime configuration manager
class RuntimeConfigManager {
private:
    static const uint32 MAX_CONFIG_ENTRIES = 256;
    RuntimeConfigEntry entries[MAX_CONFIG_ENTRIES];
    uint32 entry_count;

public:
    RuntimeConfigManager();
    ~RuntimeConfigManager();

    // Initialize the runtime configuration system
    bool Initialize();

    // Register a configuration value that can be changed at runtime
    bool RegisterConfig(const char* name, void* value, uint32 size, 
                       ConfigChangeCallback callback = nullptr, bool is_readonly = false);

    // Get a configuration value by name
    bool GetConfig(const char* name, void* out_value, uint32 max_size);

    // Set a configuration value by name (runtime change)
    bool SetConfig(const char* name, const void* new_value, uint32 size);

    // Get configuration as string
    const char* GetConfigString(const char* name);

    // Set configuration from string (parses the string to appropriate type)
    bool SetConfigFromString(const char* name, const char* value_str);

    // Save current configuration to a file (if filesystem available)
    bool SaveConfigToFile(const char* filename);

    // Load configuration from a file (if filesystem available)
    bool LoadConfigFromFile(const char* filename);

    // Get a list of all configuration names
    uint32 GetConfigNames(char** names, uint32 max_names);

    // Update all configuration values based on the current g_kernel_config
    bool UpdateFromStaticConfig();

    // Apply configuration changes that require special handling
    bool ApplyPendingChanges();
    
    // Validate a configuration change before applying it
    bool ValidateConfigChange(const char* name, const void* new_value, uint32 size);
};

// Global runtime configuration manager instance
extern RuntimeConfigManager* g_runtime_config;

// Helper macros for common configuration operations
#define RUNTIME_CONFIG() (g_runtime_config)

// Function to initialize runtime configuration system
bool InitializeRuntimeConfig();

#endif // _Kernel_RuntimeConfig_h_