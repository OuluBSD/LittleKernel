#include "RuntimeConfig.h"
#include "Kernel.h"

// Global runtime configuration manager instance
RuntimeConfigManager* g_runtime_config = nullptr;

RuntimeConfigManager::RuntimeConfigManager() : entry_count(0) {
    // Initialize all entries to zero
    memset(entries, 0, sizeof(entries));
}

RuntimeConfigManager::~RuntimeConfigManager() {
    // Clean up any allocated resources for entries
    for (uint32_t i = 0; i < entry_count; i++) {
        if (entries[i].default_value) {
            kfree(entries[i].default_value);
        }
    }
}

bool RuntimeConfigManager::Initialize() {
    entry_count = 0;
    
    // Register the main kernel configuration values as runtime-configurable entries
    if (!RegisterConfig("kernel_heap_size", &g_kernel_config->kernel_heap_size, 
                        sizeof(g_kernel_config->kernel_heap_size), nullptr, false)) {
        LOG("Error: Failed to register kernel_heap_size config");
        return false;
    }
    
    if (!RegisterConfig("max_processes", &g_kernel_config->max_processes, 
                        sizeof(g_kernel_config->max_processes), nullptr, false)) {
        LOG("Error: Failed to register max_processes config");
        return false;
    }
    
    if (!RegisterConfig("timer_frequency", &g_kernel_config->timer_frequency, 
                        sizeof(g_kernel_config->timer_frequency), nullptr, false)) {
        LOG("Error: Failed to register timer_frequency config");
        return false;
    }
    
    if (!RegisterConfig("scheduler_quantum_ms", &g_kernel_config->scheduler_quantum_ms, 
                        sizeof(g_kernel_config->scheduler_quantum_ms), nullptr, false)) {
        LOG("Error: Failed to register scheduler_quantum_ms config");
        return false;
    }
    
    if (!RegisterConfig("enable_preemptive_scheduling", &g_kernel_config->enable_preemptive_scheduling, 
                        sizeof(g_kernel_config->enable_preemptive_scheduling), nullptr, false)) {
        LOG("Error: Failed to register enable_preemptive_scheduling config");
        return false;
    }
    
    if (!RegisterConfig("enable_cooperative_scheduling", &g_kernel_config->enable_cooperative_scheduling, 
                        sizeof(g_kernel_config->enable_cooperative_scheduling), nullptr, false)) {
        LOG("Error: Failed to register enable_cooperative_scheduling config");
        return false;
    }
    
    if (!RegisterConfig("page_size", &g_kernel_config->page_size, 
                        sizeof(g_kernel_config->page_size), nullptr, false)) {
        LOG("Error: Failed to register page_size config");
        return false;
    }
    
    if (!RegisterConfig("console_buffer_size", &g_kernel_config->console_buffer_size, 
                        sizeof(g_kernel_config->console_buffer_size), nullptr, false)) {
        LOG("Error: Failed to register console_buffer_size config");
        return false;
    }
    
    if (!RegisterConfig("enable_serial_logging", &g_kernel_config->enable_serial_logging, 
                        sizeof(g_kernel_config->enable_serial_logging), nullptr, false)) {
        LOG("Error: Failed to register enable_serial_logging config");
        return false;
    }
    
    if (!RegisterConfig("enable_vga_logging", &g_kernel_config->enable_vga_logging, 
                        sizeof(g_kernel_config->enable_vga_logging), nullptr, false)) {
        LOG("Error: Failed to register enable_vga_logging config");
        return false;
    }
    
    if (!RegisterConfig("max_open_files", &g_kernel_config->max_open_files, 
                        sizeof(g_kernel_config->max_open_files), nullptr, false)) {
        LOG("Error: Failed to register max_open_files config");
        return false;
    }
    
    LOG("Runtime configuration system initialized with " << entry_count << " entries");
    return true;
}

bool RuntimeConfigManager::RegisterConfig(const char* name, void* value, uint32_t size, 
                                          ConfigChangeCallback callback, bool is_readonly) {
    if (!name || !value || entry_count >= MAX_CONFIG_ENTRIES) {
        return false;
    }
    
    // Check if a config with this name already exists
    for (uint32_t i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].name, name) == 0) {
            LOG("Warning: Configuration " << name << " already registered, overwriting");
            // Update the existing entry
            entries[i].value = value;
            entries[i].size = size;
            entries[i].callback = callback;
            entries[i].is_readonly = is_readonly;
            return true;
        }
    }
    
    // Add new configuration entry
    RuntimeConfigEntry& entry = entries[entry_count];
    
    // Copy name
    strncpy(entry.name, name, sizeof(entry.name) - 1);
    entry.name[sizeof(entry.name) - 1] = 0;  // Ensure null termination
    
    entry.value = value;
    entry.size = size;
    entry.callback = callback;
    entry.is_readonly = is_readonly;
    
    // Store default value
    entry.default_value = kmalloc(size);
    if (entry.default_value) {
        memcpy(entry.default_value, value, size);
    } else {
        LOG("Warning: Failed to allocate memory for default value of " << name);
    }
    
    entry_count++;
    LOG("Registered runtime configuration: " << name << ", readonly: " << is_readonly);
    
    return true;
}

bool RuntimeConfigManager::GetConfig(const char* name, void* out_value, uint32_t max_size) {
    if (!name || !out_value) {
        return false;
    }
    
    for (uint32_t i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].name, name) == 0) {
            if (max_size < entries[i].size) {
                return false;  // Output buffer too small
            }
            
            memcpy(out_value, entries[i].value, entries[i].size);
            return true;
        }
    }
    
    return false;  // Config not found
}

bool RuntimeConfigManager::SetConfig(const char* name, const void* new_value, uint32_t size) {
    if (!name || !new_value) {
        return false;
    }
    
    for (uint32_t i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].name, name) == 0) {
            if (entries[i].is_readonly) {
                LOG("Error: Attempt to change read-only configuration: " << name);
                return false;
            }
            
            if (size != entries[i].size) {
                LOG("Error: Size mismatch when setting configuration: " << name);
                return false;
            }
            
            // Validate the configuration change before applying it
            if (!ValidateConfigChange(name, new_value, size)) {
                LOG("Error: Configuration change validation failed for: " << name);
                return false;
            }
            
            // Store old value for callback
            void* old_value = kmalloc(entries[i].size);
            if (old_value) {
                memcpy(old_value, entries[i].value, entries[i].size);
            }
            
            // Apply the change
            memcpy(entries[i].value, new_value, size);
            
            // Call the callback if defined
            if (entries[i].callback) {
                entries[i].callback(name, old_value, (void*)new_value);
            }
            
            // Free the old value storage
            if (old_value) {
                kfree(old_value);
            }
            
            LOG("Runtime configuration changed: " << name);
            return true;
        }
    }
    
    return false;  // Config not found
}

const char* RuntimeConfigManager::GetConfigString(const char* name) {
    static char buffer[256];  // Static buffer for string representation
    
    for (uint32_t i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].name, name) == 0) {
            // Convert the configuration value to a string representation
            if (entries[i].size == sizeof(uint32_t)) {
                uint32_t value = *(uint32_t*)entries[i].value;
                snprintf(buffer, sizeof(buffer), "%u", value);
            } else if (entries[i].size == sizeof(bool)) {
                bool value = *(bool*)entries[i].value;
                snprintf(buffer, sizeof(buffer), "%s", value ? "true" : "false");
            } else if (entries[i].size == sizeof(uint8_t)) {
                uint8_t value = *(uint8_t*)entries[i].value;
                snprintf(buffer, sizeof(buffer), "%u", value);
            } else if (entries[i].size == sizeof(uint16_t)) {
                uint16_t value = *(uint16_t*)entries[i].value;
                snprintf(buffer, sizeof(buffer), "%u", value);
            } else {
                // For other sizes, just print as hex
                snprintf(buffer, sizeof(buffer), "0x%p", entries[i].value);
            }
            return buffer;
        }
    }
    
    return nullptr;  // Config not found
}

bool RuntimeConfigManager::SetConfigFromString(const char* name, const char* value_str) {
    if (!name || !value_str) {
        return false;
    }
    
    for (uint32_t i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].name, name) == 0) {
            // Parse the string value based on the configuration type
            if (entries[i].size == sizeof(uint32_t)) {
                uint32_t value = 0;
                const char* str = value_str;
                
                // Parse the number (simple implementation)
                while (*str && (*str == ' ' || *str == '\t')) str++;  // Skip whitespace
                while (*str >= '0' && *str <= '9') {
                    value = value * 10 + (*str - '0');
                    str++;
                }
                
                return SetConfig(name, &value, sizeof(value));
            } else if (entries[i].size == sizeof(bool)) {
                bool value = (strcmp(value_str, "true") == 0 || 
                              strcmp(value_str, "1") == 0 || 
                              strcmp(value_str, "yes") == 0 ||
                              strcmp(value_str, "on") == 0);
                return SetConfig(name, &value, sizeof(value));
            } else if (entries[i].size == sizeof(uint8_t)) {
                uint8_t value = 0;
                const char* str = value_str;
                
                while (*str && (*str == ' ' || *str == '\t')) str++;  // Skip whitespace
                while (*str >= '0' && *str <= '9') {
                    value = value * 10 + (*str - '0');
                    str++;
                }
                
                return SetConfig(name, &value, sizeof(value));
            } else if (entries[i].size == sizeof(uint16_t)) {
                uint16_t value = 0;
                const char* str = value_str;
                
                while (*str && (*str == ' ' || *str == '\t')) str++;  // Skip whitespace
                while (*str >= '0' && *str <= '9') {
                    value = value * 10 + (*str - '0');
                    str++;
                }
                
                return SetConfig(name, &value, sizeof(value));
            }
            
            // For other types, we can't parse them from string
            return false;
        }
    }
    
    return false;  // Config not found
}

bool RuntimeConfigManager::SaveConfigToFile(const char* filename) {
    // For now, we'll just log that we're saving the configuration
    // In a real implementation, this would write to a file
    LOG("Saving runtime configuration to file: " << filename);
    
    for (uint32_t i = 0; i < entry_count; i++) {
        const char* value_str = GetConfigString(entries[i].name);
        if (value_str) {
            LOG("Config: " << entries[i].name << " = " << value_str);
        }
    }
    
    return true;  // Placeholder implementation
}

bool RuntimeConfigManager::LoadConfigFromFile(const char* filename) {
    // For now, we'll just log that we're loading the configuration
    // In a real implementation, this would read from a file
    LOG("Loading runtime configuration from file: " << filename);
    
    // Placeholder implementation - return true for now
    return true;
}

uint32_t RuntimeConfigManager::GetConfigNames(char** names, uint32_t max_names) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < entry_count && count < max_names; i++) {
        names[count] = entries[i].name;
        count++;
    }
    return count;
}

bool RuntimeConfigManager::UpdateFromStaticConfig() {
    // This function would update the runtime configuration values from the static 
    // configuration loaded at boot time. For now, we'll just return true.
    LOG("Updated runtime configuration from static config");
    return true;
}

bool RuntimeConfigManager::ApplyPendingChanges() {
    // This function would apply any configuration changes that may require special handling
    // For example, changes to timer frequency might require reprogramming the hardware timer
    LOG("Applied pending configuration changes");
    return true;
}

bool RuntimeConfigManager::ValidateConfigChange(const char* name, const void* new_value, uint32_t size) {
    // Basic validation for some common configuration values
    if (strcmp(name, "timer_frequency") == 0 && size == sizeof(uint32_t)) {
        uint32_t freq = *(uint32_t*)new_value;
        if (freq < 1 || freq > 10000) {  // 1 Hz to 10 kHz seems reasonable
            LOG("Validation failed: timer_frequency out of range (1-10000): " << freq);
            return false;
        }
    } else if (strcmp(name, "kernel_heap_size") == 0 && size == sizeof(uint32_t)) {
        uint32_t heap_size = *(uint32_t*)new_value;
        if (heap_size == 0 || heap_size > 1024 * 1024 * 1024) {  // Max 1GB
            LOG("Validation failed: kernel_heap_size out of range (1 - 1GB): " << heap_size);
            return false;
        }
    } else if (strcmp(name, "max_processes") == 0 && size == sizeof(uint32_t)) {
        uint32_t max_procs = *(uint32_t*)new_value;
        if (max_procs == 0 || max_procs > 10000) {  // Arbitrary upper limit
            LOG("Validation failed: max_processes out of range (1-10000): " << max_procs);
            return false;
        }
    }
    
    // Add more validation rules as needed
    return true;
}

// Function to initialize runtime configuration system
bool InitializeRuntimeConfig() {
    g_runtime_config = new RuntimeConfigManager();
    if (!g_runtime_config) {
        LOG("Error: Failed to allocate runtime configuration manager");
        return false;
    }
    
    if (!g_runtime_config->Initialize()) {
        LOG("Error: Failed to initialize runtime configuration manager");
        delete g_runtime_config;
        g_runtime_config = nullptr;
        return false;
    }
    
    LOG("Runtime configuration system initialized successfully");
    return true;
}