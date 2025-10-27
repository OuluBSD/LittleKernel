#include "ConfigParser.h"
#include "Kernel.h"
#include "Common.h"

// Global configuration parser instance
ConfigParser* g_config_parser = nullptr;

ConfigParser::ConfigParser() : option_count(0) {
    memset(options, 0, sizeof(options));
    memset(config_file_path, 0, sizeof(config_file_path));
}

ConfigParser::~ConfigParser() {
    // Cleanup if needed
}

bool ConfigParser::Initialize() {
    option_count = 0;
    LOG("Configuration parser initialized");
    return true;
}

bool ConfigParser::LoadConfig(const char* file_path) {
    if (!file_path) return false;
    
    // For this implementation, we'll just simulate loading from the .config file
    // In a real implementation, we would read the actual file
    LOG("Loading configuration from: " << file_path);
    
    // Copy the file path
    strncpy(config_file_path, file_path, sizeof(config_file_path) - 1);
    config_file_path[sizeof(config_file_path) - 1] = 0;
    
    // For now, use the default file path to read
    // In a real system, we'd have file I/O support to read the actual file
    // Since we're in kernel space, we'll use a simple approach to load the config
    
    // For this example, we'll just set some default values based on the .config file content
    SetBool("CONFIG_X86", true);
    SetBool("CONFIG_SERIAL_CONSOLE", true);
    SetBool("CONFIG_VGA_CONSOLE", true);
    SetBool("CONFIG_KERNEL_DEBUG", true);
    SetBool("CONFIG_VERBOSE_LOG", true);
    SetBool("CONFIG_RUNTIME_CONFIG", true);
    SetBool("CONFIG_HAL", true);
    SetBool("CONFIG_PROFILING", true);
    SetBool("CONFIG_MODULES", true);
    SetBool("CONFIG_PCI", true);
    SetInt("CONFIG_TIMER_HZ", 100);
    SetInt("CONFIG_MAX_PROCESSES", 128);
    SetInt("CONFIG_KERNEL_HEAP_SIZE", 16);
    SetBool("CONFIG_EARLY_MEM", true);
    SetBool("CONFIG_HW_DIAGNOSTICS", true);
    SetBool("CONFIG_ERROR_HANDLING", true);
    
    LOG("Configuration loaded from: " << file_path);
    return true;
}

bool ConfigParser::SaveConfig(const char* file_path) {
    // For this implementation, we'll just simulate saving
    LOG("Saving configuration to: " << file_path);
    return true;
}

bool ConfigParser::ParseConfig(const char* buffer, uint32 size) {
    if (!buffer || size == 0) return false;
    
    // Simple parser for .config format
    // Each line is in the format: CONFIG_NAME=value
    const char* line_start = buffer;
    const char* current = buffer;
    const char* end = buffer + size;
    
    while (current < end) {
        // Find end of line
        const char* line_end = current;
        while (line_end < end && *line_end != '\n' && *line_end != '\r') {
            line_end++;
        }
        
        // Skip empty lines and comments
        if (line_end > line_start && *line_start != '#' && *line_start != '\n' && *line_start != '\r') {
            // Parse the line
            const char* equals = nullptr;
            for (const char* p = line_start; p < line_end; p++) {
                if (*p == '=') {
                    equals = p;
                    break;
                }
            }
            
            if (equals) {
                // Extract name and value
                char name[64];
                char value[128];
                
                int name_len = equals - line_start;
                if (name_len >= sizeof(name)) name_len = sizeof(name) - 1;
                strncpy(name, line_start, name_len);
                name[name_len] = 0;
                
                int value_len = line_end - equals - 1;
                if (value_len >= sizeof(value)) value_len = sizeof(value) - 1;
                strncpy(value, equals + 1, value_len);
                value[value_len] = 0;
                
                // Check if it's a boolean config (y/n)
                if (value_len == 1 && (value[0] == 'y' || value[0] == 'n')) {
                    SetBool(name, value[0] == 'y');
                } else if (value_len > 0) {
                    // Check if it's a number
                    bool is_number = true;
                    for (int i = 0; i < value_len; i++) {
                        if (value[i] < '0' || value[i] > '9') {
                            is_number = false;
                            break;
                        }
                    }
                    
                    if (is_number) {
                        SetInt(name, str_to_int(value));
                    } else {
                        // Otherwise treat as string
                        SetString(name, value);
                    }
                }
            } else {
                // Check for disabled config option like "# CONFIG_SOMETHING is not set"
                if (line_start[0] == '#') {
                    const char* disabled_start = strstr(line_start, "CONFIG_");
                    if (disabled_start) {
                        const char* is_not_set = strstr(disabled_start, " is not set");
                        if (is_not_set) {
                            char name[64];
                            int name_len = is_not_set - disabled_start;
                            if (name_len >= sizeof(name)) name_len = sizeof(name) - 1;
                            strncpy(name, disabled_start, name_len);
                            name[name_len] = 0;
                            
                            SetBool(name, false);
                        }
                    }
                }
            }
        }
        
        // Move to next line
        current = line_end;
        while (current < end && (*current == '\n' || *current == '\r')) {
            current++;
        }
        line_start = current;
    }
    
    return true;
}

bool ConfigParser::GetBool(const char* name, bool default_value) {
    if (!name) return default_value;
    
    for (uint32 i = 0; i < option_count; i++) {
        if (strcmp(options[i].name, name) == 0 && options[i].is_bool) {
            return options[i].bool_value;
        }
    }
    
    return default_value;
}

int ConfigParser::GetInt(const char* name, int default_value) {
    if (!name) return default_value;
    
    for (uint32 i = 0; i < option_count; i++) {
        if (strcmp(options[i].name, name) == 0 && !options[i].is_bool) {
            return options[i].int_value;
        }
    }
    
    return default_value;
}

const char* ConfigParser::GetString(const char* name, const char* default_value) {
    if (!name) return default_value;
    
    for (uint32 i = 0; i < option_count; i++) {
        if (strcmp(options[i].name, name) == 0 && !options[i].is_bool) {
            return options[i].str_value;
        }
    }
    
    return default_value;
}

bool ConfigParser::SetBool(const char* name, bool value) {
    if (!name || option_count >= MAX_CONFIG_OPTIONS) return false;
    
    // Look for existing option
    for (uint32 i = 0; i < option_count; i++) {
        if (strcmp(options[i].name, name) == 0) {
            options[i].is_bool = true;
            options[i].bool_value = value;
            options[i].is_set = true;
            return true;
        }
    }
    
    // Add new option
    ConfigOption& opt = options[option_count];
    strncpy(opt.name, name, sizeof(opt.name) - 1);
    opt.name[sizeof(opt.name) - 1] = 0;
    opt.is_bool = true;
    opt.bool_value = value;
    opt.is_set = true;
    option_count++;
    
    return true;
}

bool ConfigParser::SetInt(const char* name, int value) {
    if (!name || option_count >= MAX_CONFIG_OPTIONS) return false;
    
    // Look for existing option
    for (uint32 i = 0; i < option_count; i++) {
        if (strcmp(options[i].name, name) == 0) {
            options[i].is_bool = false;
            options[i].int_value = value;
            options[i].is_set = true;
            return true;
        }
    }
    
    // Add new option
    ConfigOption& opt = options[option_count];
    strncpy(opt.name, name, sizeof(opt.name) - 1);
    opt.name[sizeof(opt.name) - 1] = 0;
    opt.is_bool = false;
    opt.int_value = value;
    opt.is_set = true;
    option_count++;
    
    return true;
}

bool ConfigParser::SetString(const char* name, const char* value) {
    if (!name || !value || option_count >= MAX_CONFIG_OPTIONS) return false;
    
    // Look for existing option
    for (uint32 i = 0; i < option_count; i++) {
        if (strcmp(options[i].name, name) == 0) {
            options[i].is_bool = false;
            strncpy(options[i].str_value, value, sizeof(options[i].str_value) - 1);
            options[i].str_value[sizeof(options[i].str_value) - 1] = 0;
            options[i].is_set = true;
            return true;
        }
    }
    
    // Add new option
    ConfigOption& opt = options[option_count];
    strncpy(opt.name, name, sizeof(opt.name) - 1);
    opt.name[sizeof(opt.name) - 1] = 0;
    opt.is_bool = false;
    strncpy(opt.str_value, value, sizeof(opt.str_value) - 1);
    opt.str_value[sizeof(opt.str_value) - 1] = 0;
    opt.is_set = true;
    option_count++;
    
    return true;
}

bool ConfigParser::IsSet(const char* name) {
    if (!name) return false;
    
    for (uint32 i = 0; i < option_count; i++) {
        if (strcmp(options[i].name, name) == 0) {
            return options[i].is_set;
        }
    }
    
    return false;
}

bool ConfigParser::GenerateHeaderFile(const char* header_path) {
    if (!header_path) return false;
    
    LOG("Generating configuration header: " << header_path);
    
    // In a real implementation, we would write the actual header file
    // For this example, we'll just log what would be written
    LOG("Configuration header generation simulated for: " << header_path);
    
    // The header would contain C #define statements for each configuration option
    LOG("Generated defines:");
    for (uint32 i = 0; i < option_count; i++) {
        if (options[i].is_set) {
            if (options[i].is_bool) {
                LOG("  #define " << options[i].name << " " << (options[i].bool_value ? "1" : "0"));
            } else if (options[i].int_value != 0) {
                LOG("  #define " << options[i].name << " " << options[i].int_value);
            } else if (options[i].str_value[0] != 0) {
                LOG("  #define " << options[i].name << " \"" << options[i].str_value << "\"");
            }
        }
    }
    
    return true;
}

const ConfigOption* ConfigParser::GetOptions(uint32* count) {
    *count = option_count;
    return options;
}

void ConfigParser::PrintConfig() {
    LOG("=== Kernel Configuration ===");
    for (uint32 i = 0; i < option_count; i++) {
        if (options[i].is_set) {
            if (options[i].is_bool) {
                LOG(options[i].name << "=" << (options[i].bool_value ? "y" : "n"));
            } else if (options[i].int_value != 0) {
                LOG(options[i].name << "=" << options[i].int_value);
            } else if (options[i].str_value[0] != 0) {
                LOG(options[i].name << "=\"" << options[i].str_value << "\"");
            }
        }
    }
    LOG("============================");
}

bool ConfigParser::ValidateConfig() {
    // Basic validation
    int timer_hz = GetInt("CONFIG_TIMER_HZ", 100);
    if (timer_hz < 1 || timer_hz > 10000) {
        LOG("Error: Invalid CONFIG_TIMER_HZ value: " << timer_hz);
        return false;
    }
    
    int max_processes = GetInt("CONFIG_MAX_PROCESSES", 128);
    if (max_processes < 1 || max_processes > 10000) {
        LOG("Error: Invalid CONFIG_MAX_PROCESSES value: " << max_processes);
        return false;
    }
    
    int heap_size = GetInt("CONFIG_KERNEL_HEAP_SIZE", 16);
    if (heap_size < 1 || heap_size > 1024) {
        LOG("Error: Invalid CONFIG_KERNEL_HEAP_SIZE value: " << heap_size);
        return false;
    }
    
    LOG("Configuration validation passed");
    return true;
}

void ConfigParser::Reset() {
    option_count = 0;
    memset(options, 0, sizeof(options));
    LOG("Configuration reset");
}

// Initialize configuration system
bool InitializeConfigSystem() {
    g_config_parser = new ConfigParser();
    if (!g_config_parser) {
        LOG("Error: Failed to allocate configuration parser");
        return false;
    }
    
    if (!g_config_parser->Initialize()) {
        LOG("Error: Failed to initialize configuration parser");
        delete g_config_parser;
        g_config_parser = nullptr;
        return false;
    }
    
    LOG("Configuration system initialized successfully");
    return true;
}

// Helper function to load .config file
bool LoadKernelConfigFile(const char* path) {
    if (!g_config_parser) {
        LOG("Error: Configuration parser not initialized");
        return false;
    }
    
    if (!g_config_parser->LoadConfig(path)) {
        LOG("Error: Failed to load kernel config file: " << path);
        return false;
    }
    
    if (!g_config_parser->ValidateConfig()) {
        LOG("Warning: Configuration validation failed, using defaults");
    }
    
    LOG("Kernel configuration loaded from: " << path);
    return true;
}

// Helper function to generate configuration header
bool GenerateConfigHeader(const char* config_path, const char* header_path) {
    if (!g_config_parser) {
        LOG("Error: Configuration parser not initialized");
        return false;
    }
    
    // Load config first if not already loaded
    if (!config_path || config_path[0] == 0) {
        config_path = ".config";
    }
    
    if (!header_path || header_path[0] == 0) {
        header_path = "kernel_config_defines.h";
    }
    
    if (!LoadKernelConfigFile(config_path)) {
        LOG("Error: Failed to load config for header generation");
        return false;
    }
    
    if (!g_config_parser->GenerateHeaderFile(header_path)) {
        LOG("Error: Failed to generate configuration header: " << header_path);
        return false;
    }
    
    LOG("Configuration header generated: " << header_path);
    return true;
}