#ifndef _Kernel_Registry_h_
#define _Kernel_Registry_h_

#include "Common.h"
#include "Defs.h"
#include "Vfs.h"

// Registry constants
#define REGISTRY_MAX_KEY_LENGTH 256
#define REGISTRY_MAX_VALUE_NAME 16384  // Windows-compatible limit
#define REGISTRY_MAX_VALUE_LENGTH 65536
#define REGISTRY_MAX_SUBKEYS 1024
#define REGISTRY_MAX_VALUES 1024

// Registry value types
#define REG_NONE 0
#define REG_SZ 1
#define REG_EXPAND_SZ 2
#define REG_BINARY 3
#define REG_DWORD 4
#define REG_DWORD_BIG_ENDIAN 5
#define REG_LINK 6
#define REG_MULTI_SZ 7
#define REG_QWORD 11

// Registry access permissions
#define KEY_QUERY_VALUE 0x0001
#define KEY_SET_VALUE 0x0002
#define KEY_CREATE_SUB_KEY 0x0004
#define KEY_ENUMERATE_SUB_KEYS 0x0008
#define KEY_NOTIFY 0x0010
#define KEY_CREATE_LINK 0x0020
#define KEY_WOW64_64KEY 0x0100
#define KEY_WOW64_32KEY 0x0200
#define KEY_READ (KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_NOTIFY)
#define KEY_WRITE (KEY_SET_VALUE | KEY_CREATE_SUB_KEY)
#define KEY_ALL_ACCESS (KEY_READ | KEY_WRITE)

// Registry key structure
struct RegistryKey {
    char name[REGISTRY_MAX_KEY_LENGTH];
    char full_path[REGISTRY_MAX_KEY_LENGTH * 2];
    RegistryKey* parent;
    RegistryKey* subkeys[REGISTRY_MAX_SUBKEYS];
    uint32_t subkey_count;
    char* value_names[REGISTRY_MAX_VALUES];
    uint32_t value_types[REGISTRY_MAX_VALUES];
    void* value_data[REGISTRY_MAX_VALUES];
    uint32_t value_sizes[REGISTRY_MAX_VALUES];
    uint32_t value_count;
    uint32_t access_mask;  // Permissions
    uint32_t last_write_time;
    uint32_t ref_count;
    Spinlock key_lock;
};

// Registry handle structure
struct RegistryHandle {
    RegistryKey* key;
    uint32_t access;
    bool valid;
};

// Registry value structure for external use
struct RegistryValue {
    char name[REGISTRY_MAX_VALUE_NAME];
    uint32_t type;
    void* data;
    uint32_t size;
};

// Registry class for kernel-side registry operations
class Registry {
private:
    RegistryKey* root_key;  // HKEY_LOCAL_MACHINE equivalent
    RegistryKey* user_root; // HKEY_USERS equivalent
    RegistryKey* current_config; // HKEY_CURRENT_CONFIG equivalent
    Spinlock registry_lock;     // Global registry lock
    
public:
    Registry();
    ~Registry();
    
    // Initialize the registry system
    bool Initialize();
    
    // Create a new registry key
    bool CreateKey(const char* path, uint32_t access, RegistryKey** key);
    
    // Open an existing registry key
    bool OpenKey(const char* path, uint32_t access, RegistryKey** key);
    
    // Close a registry key
    bool CloseKey(RegistryKey* key);
    
    // Delete a registry key
    bool DeleteKey(const char* path);
    
    // Set a registry value
    bool SetValue(RegistryKey* key, const char* value_name, uint32_t type, const void* data, uint32_t size);
    
    // Get a registry value
    bool GetValue(RegistryKey* key, const char* value_name, void* data, uint32_t* size);
    
    // Delete a registry value
    bool DeleteValue(RegistryKey* key, const char* value_name);
    
    // Enumerate subkeys
    bool EnumerateKey(RegistryKey* key, uint32_t index, char* name, uint32_t name_size);
    
    // Enumerate values
    bool EnumerateValue(RegistryKey* key, uint32_t index, char* name, uint32_t name_size, uint32_t* type);
    
    // Get value information
    bool QueryValueInfo(RegistryKey* key, const char* value_name, uint32_t* type, uint32_t* size);
    
    // Registry-based path translation - translate device paths via registry mappings
    bool TranslatePath(const char* input_path, char* output_path, uint32_t max_len);
    
    // Add a path mapping to the registry
    bool AddPathMapping(const char* virtual_path, const char* physical_path);
    
    // Get the root key
    RegistryKey* GetRootKey() { return root_key; }

private:
    // Internal helper functions
    RegistryKey* CreateRegistryKey(const char* name, RegistryKey* parent);
    void DestroyRegistryKey(RegistryKey* key);
    RegistryKey* FindKey(const char* path);
    bool ParsePath(const char* path, char* root, char* subpath);
    bool SplitPath(const char* path, char* dir, char* key_name);
};

// Global registry instance
extern Registry* g_registry;

// Initialize the global registry
bool InitializeRegistry();

// Registry API for modules to safely access registry with proper permissions
bool RegistryReadValue(const char* key_path, const char* value_name, void* buffer, uint32_t* size, uint32_t access_mask);
bool RegistryWriteValue(const char* key_path, const char* value_name, uint32_t type, const void* buffer, uint32_t size, uint32_t access_mask);
bool RegistryReadString(const char* key_path, const char* value_name, char* buffer, uint32_t* size, uint32_t access_mask);
bool RegistryWriteString(const char* key_path, const char* value_name, const char* str, uint32_t access_mask);

#endif