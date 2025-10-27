#include "Kernel.h"
#include "Registry.h"
#include "Vfs.h"
#include "Logging.h"

// Global registry instance
Registry* g_registry = nullptr;

Registry::Registry() {
    root_key = nullptr;
    user_root = nullptr;
    current_config = nullptr;
    registry_lock.Initialize();
}

Registry::~Registry() {
    // Clean up all registry keys
    if (root_key) {
        DestroyRegistryKey(root_key);
    }
    if (user_root) {
        DestroyRegistryKey(user_root);
    }
    if (current_config) {
        DestroyRegistryKey(current_config);
    }
}

bool Registry::Initialize() {
    LOG("Initializing kernel registry system");
    
    // Create the root keys
    root_key = CreateRegistryKey("HKEY_LOCAL_MACHINE", nullptr);
    if (!root_key) {
        LOG("Failed to create HKEY_LOCAL_MACHINE");
        return false;
    }
    
    user_root = CreateRegistryKey("HKEY_USERS", nullptr);
    if (!user_root) {
        LOG("Failed to create HKEY_USERS");
        return false;
    }
    
    current_config = CreateRegistryKey("HKEY_CURRENT_CONFIG", nullptr);
    if (!current_config) {
        LOG("Failed to create HKEY_CURRENT_CONFIG");
        return false;
    }
    
    // Create a path translation subkey
    RegistryKey* path_trans_key = nullptr;
    if (!CreateKey("HKEY_LOCAL_MACHINE\\SYSTEM\\MountPoints", KEY_ALL_ACCESS, &path_trans_key)) {
        LOG("Warning: Could not create MountPoints key");
    }
    
    LOG("Kernel registry system initialized successfully");
    return true;
}

bool Registry::CreateKey(const char* path, uint32 access, RegistryKey** key) {
    if (!path || !key) {
        return false;
    }
    
    registry_lock.Acquire();
    
    // Parse the path to find parent and new key name
    char root[REGISTRY_MAX_KEY_LENGTH];
    char subpath[REGISTRY_MAX_KEY_LENGTH * 2];
    if (!ParsePath(path, root, subpath)) {
        registry_lock.Release();
        return false;
    }
    
    // Find the root key
    RegistryKey* root_key_ptr = nullptr;
    if (strcmp(root, "HKEY_LOCAL_MACHINE") == 0) {
        root_key_ptr = this->root_key;
    } else if (strcmp(root, "HKEY_USERS") == 0) {
        root_key_ptr = this->user_root;
    } else if (strcmp(root, "HKEY_CURRENT_CONFIG") == 0) {
        root_key_ptr = this->current_config;
    } else {
        registry_lock.Release();
        return false;
    }
    
    if (!root_key_ptr) {
        registry_lock.Release();
        return false;
    }
    
    // Find the parent key by traversing the subpath
    RegistryKey* parent_key = root_key_ptr;
    char temp_path[REGISTRY_MAX_KEY_LENGTH * 2];
    strcpy_safe(temp_path, subpath, sizeof(temp_path));
    
    char* token = strtok(temp_path, "\\");
    while (token != nullptr) {
        // Look for the subkey under the current parent
        RegistryKey* found_child = nullptr;
        for (uint32 i = 0; i < parent_key->subkey_count; i++) {
            if (parent_key->subkeys[i] && strcmp(parent_key->subkeys[i]->name, token) == 0) {
                found_child = parent_key->subkeys[i];
                break;
            }
        }
        
        if (!found_child) {
            // Create the missing key
            found_child = CreateRegistryKey(token, parent_key);
            if (!found_child) {
                registry_lock.Release();
                return false;
            }
        }
        
        parent_key = found_child;
        token = strtok(nullptr, "\\");
    }
    
    *key = parent_key;
    registry_lock.Release();
    return true;
}

bool Registry::OpenKey(const char* path, uint32 access, RegistryKey** key) {
    if (!path || !key) {
        return false;
    }
    
    registry_lock.Acquire();
    
    RegistryKey* found_key = FindKey(path);
    if (!found_key) {
        registry_lock.Release();
        return false;
    }
    
    // Check if the access is allowed
    if ((found_key->access_mask & access) != access) {
        registry_lock.Release();
        return false;
    }
    
    *key = found_key;
    found_key->ref_count++;
    
    registry_lock.Release();
    return true;
}

bool Registry::CloseKey(RegistryKey* key) {
    if (!key) {
        return false;
    }
    
    if (key->ref_count > 0) {
        key->ref_count--;
    }
    
    return true;
}

bool Registry::DeleteKey(const char* path) {
    if (!path) {
        return false;
    }
    
    registry_lock.Acquire();
    
    // Find the key to delete
    char dir_path[REGISTRY_MAX_KEY_LENGTH * 2];
    char key_name[REGISTRY_MAX_KEY_LENGTH];
    SplitPath(path, dir_path, key_name);
    
    RegistryKey* parent = FindKey(dir_path);
    if (!parent) {
        registry_lock.Release();
        return false;
    }
    
    // Find the key to delete in parent's subkeys
    for (uint32 i = 0; i < parent->subkey_count; i++) {
        if (parent->subkeys[i] && strcmp(parent->subkeys[i]->name, key_name) == 0) {
            // Remove from the array
            DestroyRegistryKey(parent->subkeys[i]);
            
            // Shift the remaining keys down
            for (uint32 j = i; j < parent->subkey_count - 1; j++) {
                parent->subkeys[j] = parent->subkeys[j + 1];
            }
            parent->subkey_count--;
            parent->subkeys[parent->subkey_count] = nullptr;
            
            registry_lock.Release();
            return true;
        }
    }
    
    registry_lock.Release();
    return false;
}

bool Registry::SetValue(RegistryKey* key, const char* value_name, uint32 type, const void* data, uint32 size) {
    if (!key || !value_name || !data || size > REGISTRY_MAX_VALUE_LENGTH) {
        return false;
    }
    
    key->key_lock.Acquire();
    
    // Check if value already exists
    int existing_index = -1;
    for (uint32 i = 0; i < key->value_count; i++) {
        if (key->value_names[i] && strcmp(key->value_names[i], value_name) == 0) {
            existing_index = i;
            break;
        }
    }
    
    if (existing_index >= 0) {
        // Update existing value
        uint32 old_size = key->value_sizes[existing_index];
        
        // Free old data if needed
        if (key->value_data[existing_index]) {
            free(key->value_data[existing_index]);
        }
        
        // Allocate new data
        key->value_data[existing_index] = malloc(size);
        if (!key->value_data[existing_index]) {
            key->key_lock.Release();
            return false;
        }
        
        memcpy(key->value_data[existing_index], data, size);
        key->value_types[existing_index] = type;
        key->value_sizes[existing_index] = size;
        
        // Update the name if it changed
        if (key->value_names[existing_index]) {
            free(key->value_names[existing_index]);
        }
        key->value_names[existing_index] = (char*)malloc(strlen(value_name) + 1);
        if (key->value_names[existing_index]) {
            strcpy(key->value_names[existing_index], value_name);
        }
    } else {
        // Add new value
        if (key->value_count >= REGISTRY_MAX_VALUES) {
            key->key_lock.Release();
            return false;  // No more space for values
        }
        
        uint32 idx = key->value_count;
        
        // Allocate value name
        key->value_names[idx] = (char*)malloc(strlen(value_name) + 1);
        if (!key->value_names[idx]) {
            key->key_lock.Release();
            return false;
        }
        strcpy(key->value_names[idx], value_name);
        
        // Allocate value data
        key->value_data[idx] = malloc(size);
        if (!key->value_data[idx]) {
            free(key->value_names[idx]);
            key->value_names[idx] = nullptr;
            key->key_lock.Release();
            return false;
        }
        
        memcpy(key->value_data[idx], data, size);
        key->value_types[idx] = type;
        key->value_sizes[idx] = size;
        key->value_count++;
    }
    
    key->key_lock.Release();
    return true;
}

bool Registry::GetValue(RegistryKey* key, const char* value_name, void* data, uint32* size) {
    if (!key || !value_name || !data || !size) {
        return false;
    }
    
    key->key_lock.Acquire();
    
    // Find the value
    for (uint32 i = 0; i < key->value_count; i++) {
        if (key->value_names[i] && strcmp(key->value_names[i], value_name) == 0) {
            // Check if the provided buffer is large enough
            if (*size < key->value_sizes[i]) {
                *size = key->value_sizes[i];  // Set required size
                key->key_lock.Release();
                return false;
            }
            
            // Copy the data
            memcpy(data, key->value_data[i], key->value_sizes[i]);
            *size = key->value_sizes[i];
            
            key->key_lock.Release();
            return true;
        }
    }
    
    key->key_lock.Release();
    return false;  // Value not found
}

bool Registry::DeleteValue(RegistryKey* key, const char* value_name) {
    if (!key || !value_name) {
        return false;
    }
    
    key->key_lock.Acquire();
    
    // Find the value
    for (uint32 i = 0; i < key->value_count; i++) {
        if (key->value_names[i] && strcmp(key->value_names[i], value_name) == 0) {
            // Free the data and name
            if (key->value_data[i]) {
                free(key->value_data[i]);
            }
            if (key->value_names[i]) {
                free(key->value_names[i]);
            }
            
            // Shift remaining values down
            for (uint32 j = i; j < key->value_count - 1; j++) {
                key->value_names[j] = key->value_names[j + 1];
                key->value_data[j] = key->value_data[j + 1];
                key->value_types[j] = key->value_types[j + 1];
                key->value_sizes[j] = key->value_sizes[j + 1];
            }
            
            key->value_count--;
            key->value_names[key->value_count] = nullptr;
            key->value_data[key->value_count] = nullptr;
            
            key->key_lock.Release();
            return true;
        }
    }
    
    key->key_lock.Release();
    return false;  // Value not found
}

bool Registry::EnumerateKey(RegistryKey* key, uint32 index, char* name, uint32 name_size) {
    if (!key || !name || index >= key->subkey_count) {
        return false;
    }
    
    key->key_lock.Acquire();
    
    if (index < key->subkey_count && key->subkeys[index]) {
        strncpy(name, key->subkeys[index]->name, name_size - 1);
        name[name_size - 1] = '\0';
        key->key_lock.Release();
        return true;
    }
    
    key->key_lock.Release();
    return false;
}

bool Registry::EnumerateValue(RegistryKey* key, uint32 index, char* name, uint32 name_size, uint32* type) {
    if (!key || !name || !type || index >= key->value_count) {
        return false;
    }
    
    key->key_lock.Acquire();
    
    if (index < key->value_count && key->value_names[index]) {
        strncpy(name, key->value_names[index], name_size - 1);
        name[name_size - 1] = '\0';
        *type = key->value_types[index];
        key->key_lock.Release();
        return true;
    }
    
    key->key_lock.Release();
    return false;
}

bool Registry::QueryValueInfo(RegistryKey* key, const char* value_name, uint32* type, uint32* size) {
    if (!key || !value_name) {
        return false;
    }
    
    key->key_lock.Acquire();
    
    for (uint32 i = 0; i < key->value_count; i++) {
        if (key->value_names[i] && strcmp(key->value_names[i], value_name) == 0) {
            if (type) *type = key->value_types[i];
            if (size) *size = key->value_sizes[i];
            key->key_lock.Release();
            return true;
        }
    }
    
    key->key_lock.Release();
    return false;
}

bool Registry::TranslatePath(const char* input_path, char* output_path, uint32 max_len) {
    if (!input_path || !output_path) {
        return false;
    }
    
    // Check if the path is a drive letter (X:)
    if (strlen(input_path) >= 2 && input_path[1] == ':') {
        char drive_letter[3] = { input_path[0], input_path[1], '\0' };
        
        // Look up the drive letter in the registry MountPoints key
        RegistryKey* mount_key = nullptr;
        if (OpenKey("HKEY_LOCAL_MACHINE\\SYSTEM\\MountPoints", KEY_READ, &mount_key)) {
            uint32 size = max_len;
            bool result = GetValue(mount_key, drive_letter, output_path, &size);
            CloseKey(mount_key);
            
            if (result) {
                // Append the rest of the input path after the drive specifier
                if (input_path[2] == '\\') {
                    strncat(output_path, input_path + 2, max_len - strlen(output_path) - 1);
                } else if (input_path[2] != '\0') {
                    strncat(output_path, input_path + 2, max_len - strlen(output_path) - 1);
                }
                return true;
            }
        }
    }
    
    // If no translation is found, just copy the input path to output path
    strncpy(output_path, input_path, max_len - 1);
    output_path[max_len - 1] = '\0';
    return true;
}

bool Registry::AddPathMapping(const char* virtual_path, const char* physical_path) {
    if (!virtual_path || !physical_path) {
        return false;
    }
    
    RegistryKey* mount_key = nullptr;
    if (!OpenKey("HKEY_LOCAL_MACHINE\\SYSTEM\\MountPoints", KEY_WRITE, &mount_key)) {
        // Create the key if it doesn't exist
        if (!CreateKey("HKEY_LOCAL_MACHINE\\SYSTEM\\MountPoints", KEY_WRITE, &mount_key)) {
            return false;
        }
    }
    
    bool result = SetValue(mount_key, virtual_path, REG_SZ, (void*)physical_path, strlen(physical_path) + 1);
    CloseKey(mount_key);
    
    return result;
}

RegistryKey* Registry::CreateRegistryKey(const char* name, RegistryKey* parent) {
    RegistryKey* key = (RegistryKey*)malloc(sizeof(RegistryKey));
    if (!key) {
        return nullptr;
    }
    
    memset(key, 0, sizeof(RegistryKey));
    
    if (name) {
        strncpy(key->name, name, sizeof(key->name) - 1);
        key->name[sizeof(key->name) - 1] = '\0';
    }
    
    if (parent) {
        strncpy(key->full_path, parent->full_path, sizeof(key->full_path) - 1);
        if (parent->full_path[strlen(parent->full_path) - 1] != '\\') {
            strncat(key->full_path, "\\", sizeof(key->full_path) - 1);
        }
        strncat(key->full_path, name, sizeof(key->full_path) - 1);
    } else {
        strncpy(key->full_path, name, sizeof(key->full_path) - 1);
    }
    
    key->parent = parent;
    key->access_mask = KEY_ALL_ACCESS;  // Default to full access
    key->ref_count = 0;
    key->key_lock.Initialize();
    
    // Initialize subkeys array
    for (int i = 0; i < REGISTRY_MAX_SUBKEYS; i++) {
        key->subkeys[i] = nullptr;
    }
    
    // Initialize values array
    for (int i = 0; i < REGISTRY_MAX_VALUES; i++) {
        key->value_names[i] = nullptr;
        key->value_data[i] = nullptr;
        key->value_types[i] = REG_NONE;
        key->value_sizes[i] = 0;
    }
    
    // If this is a child key, add it to parent's subkeys
    if (parent) {
        if (parent->subkey_count < REGISTRY_MAX_SUBKEYS) {
            parent->subkeys[parent->subkey_count] = key;
            parent->subkey_count++;
        } else {
            // No space for more subkeys
            free(key);
            return nullptr;
        }
    }
    
    return key;
}

void Registry::DestroyRegistryKey(RegistryKey* key) {
    if (!key) {
        return;
    }
    
    // Recursively destroy subkeys
    for (uint32 i = 0; i < key->subkey_count; i++) {
        if (key->subkeys[i]) {
            DestroyRegistryKey(key->subkeys[i]);
        }
    }
    
    // Free all value data
    for (uint32 i = 0; i < key->value_count; i++) {
        if (key->value_data[i]) {
            free(key->value_data[i]);
        }
        if (key->value_names[i]) {
            free(key->value_names[i]);
        }
    }
    
    // Free the key itself
    free(key);
}

RegistryKey* Registry::FindKey(const char* path) {
    if (!path) {
        return nullptr;
    }
    
    // Parse the path to find root and subpath
    char root[REGISTRY_MAX_KEY_LENGTH];
    char subpath[REGISTRY_MAX_KEY_LENGTH * 2];
    if (!ParsePath(path, root, subpath)) {
        return nullptr;
    }
    
    // Find the root key
    RegistryKey* current = nullptr;
    if (strcmp(root, "HKEY_LOCAL_MACHINE") == 0) {
        current = root_key;
    } else if (strcmp(root, "HKEY_USERS") == 0) {
        current = user_root;
    } else if (strcmp(root, "HKEY_CURRENT_CONFIG") == 0) {
        current = current_config;
    } else {
        return nullptr;
    }
    
    if (!current) {
        return nullptr;
    }
    
    // If no subpath, return the root
    if (strlen(subpath) == 0 || strcmp(subpath, "\\") == 0) {
        return current;
    }
    
    // Tokenize the subpath and traverse
    char temp_path[REGISTRY_MAX_KEY_LENGTH * 2];
    strcpy_safe(temp_path, subpath, sizeof(temp_path));
    
    char* token = strtok(temp_path, "\\");
    while (token != nullptr) {
        // Find the subkey with matching name
        RegistryKey* found_child = nullptr;
        for (uint32 i = 0; i < current->subkey_count; i++) {
            if (current->subkeys[i] && strcmp(current->subkeys[i]->name, token) == 0) {
                found_child = current->subkeys[i];
                break;
            }
        }
        
        if (!found_child) {
            return nullptr;  // Path not found
        }
        
        current = found_child;
        token = strtok(nullptr, "\\");
    }
    
    return current;
}

bool Registry::ParsePath(const char* path, char* root, char* subpath) {
    if (!path || !root || !subpath) {
        return false;
    }
    
    // Find the first backslash to separate root from subpath
    const char* separator = strchr(path, '\\');
    if (separator) {
        int root_len = separator - path;
        strncpy(root, path, root_len);
        root[root_len] = '\0';
        
        strncpy(subpath, separator, REGISTRY_MAX_KEY_LENGTH * 2 - 1);
        subpath[REGISTRY_MAX_KEY_LENGTH * 2 - 1] = '\0';
    } else {
        strncpy(root, path, REGISTRY_MAX_KEY_LENGTH - 1);
        root[REGISTRY_MAX_KEY_LENGTH - 1] = '\0';
        subpath[0] = '\0';
    }
    
    return true;
}

bool Registry::SplitPath(const char* path, char* dir, char* key_name) {
    if (!path || !dir || !key_name) {
        return false;
    }
    
    const char* last_separator = strrchr(path, '\\');
    if (last_separator) {
        int dir_len = last_separator - path;
        strncpy(dir, path, dir_len);
        dir[dir_len] = '\0';
        
        strcpy_safe(key_name, last_separator + 1, REGISTRY_MAX_KEY_LENGTH);
    } else {
        dir[0] = '\0';
        strcpy_safe(key_name, path, REGISTRY_MAX_KEY_LENGTH);
    }
    
    return true;
}

bool InitializeRegistry() {
    if (!g_registry) {
        g_registry = new Registry();
        if (!g_registry) {
            LOG("Failed to create registry instance");
            return false;
        }
        
        if (!g_registry->Initialize()) {
            LOG("Failed to initialize registry");
            delete g_registry;
            g_registry = nullptr;
            return false;
        }
        
        LOG("Registry system initialized successfully");
    }
    
    return true;
}

// Registry API for modules to safely access registry with proper permissions
bool RegistryReadValue(const char* key_path, const char* value_name, void* buffer, uint32* size, uint32 access_mask) {
    if (!g_registry) {
        return false;
    }
    
    RegistryKey* key = nullptr;
    if (!g_registry->OpenKey(key_path, access_mask & KEY_READ, &key)) {
        return false;
    }
    
    bool result = g_registry->GetValue(key, value_name, buffer, size);
    g_registry->CloseKey(key);
    
    return result;
}

bool RegistryWriteValue(const char* key_path, const char* value_name, uint32 type, const void* buffer, uint32 size, uint32 access_mask) {
    if (!g_registry) {
        return false;
    }
    
    RegistryKey* key = nullptr;
    if (!g_registry->OpenKey(key_path, access_mask & KEY_WRITE, &key)) {
        return false;
    }
    
    bool result = g_registry->SetValue(key, value_name, type, buffer, size);
    g_registry->CloseKey(key);
    
    return result;
}

bool RegistryReadString(const char* key_path, const char* value_name, char* buffer, uint32* size, uint32 access_mask) {
    if (!g_registry) {
        return false;
    }
    
    RegistryKey* key = nullptr;
    if (!g_registry->OpenKey(key_path, access_mask & KEY_READ, &key)) {
        return false;
    }
    
    bool result = g_registry->GetValue(key, value_name, buffer, size);
    g_registry->CloseKey(key);
    
    if (result && buffer && *size > 0) {
        // Ensure the string is null-terminated
        buffer[*size > 0 ? *size - 1 : 0] = '\0';
    }
    
    return result;
}

bool RegistryWriteString(const char* key_path, const char* value_name, const char* str, uint32 access_mask) {
    if (!g_registry) {
        return false;
    }
    
    RegistryKey* key = nullptr;
    if (!g_registry->OpenKey(key_path, access_mask & KEY_WRITE, &key)) {
        return false;
    }
    
    bool result = g_registry->SetValue(key, value_name, REG_SZ, (void*)str, strlen(str) + 1);
    g_registry->CloseKey(key);
    
    return result;
}