#ifndef _Kernel_ConfigParser_h_
#define _Kernel_ConfigParser_h_

#include "Defs.h"

// Configuration option structure
struct ConfigOption {
    char name[64];
    bool is_bool;
    bool bool_value;
    int int_value;
    char str_value[128];
    bool is_set;
};

// Configuration parser class
class ConfigParser {
private:
    static const uint32_t MAX_CONFIG_OPTIONS = 256;
    ConfigOption options[MAX_CONFIG_OPTIONS];
    uint32_t option_count;
    char config_file_path[256];

public:
    ConfigParser();
    ~ConfigParser();
    
    // Initialize the configuration parser
    bool Initialize();
    
    // Load configuration from a file
    bool LoadConfig(const char* file_path);
    
    // Save configuration to a file
    bool SaveConfig(const char* file_path);
    
    // Parse configuration from a buffer
    bool ParseConfig(const char* buffer, uint32_t size);
    
    // Get boolean configuration value
    bool GetBool(const char* name, bool default_value = false);
    
    // Get integer configuration value
    int GetInt(const char* name, int default_value = 0);
    
    // Get string configuration value
    const char* GetString(const char* name, const char* default_value = "");
    
    // Set boolean configuration value
    bool SetBool(const char* name, bool value);
    
    // Set integer configuration value
    bool SetInt(const char* name, int value);
    
    // Set string configuration value
    bool SetString(const char* name, const char* value);
    
    // Check if a configuration option is set
    bool IsSet(const char* name);
    
    // Generate C++ header file with defines from configuration
    bool GenerateHeaderFile(const char* header_path);
    
    // Get all configuration options
    const ConfigOption* GetOptions(uint32_t* count);
    
    // Print configuration to console
    void PrintConfig();
    
    // Validate configuration values
    bool ValidateConfig();
    
    // Reset to default values
    void Reset();
};

// Global configuration parser instance
extern ConfigParser* g_config_parser;

// Initialize configuration system
bool InitializeConfigSystem();

// Helper function to load .config file
bool LoadKernelConfigFile(const char* path);

// Helper function to generate configuration header
bool GenerateConfigHeader(const char* config_path, const char* header_path);

#endif // _Kernel_ConfigParser_h_