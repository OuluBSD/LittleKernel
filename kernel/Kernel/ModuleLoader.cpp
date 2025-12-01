#include "ModuleLoader.h"
#include "Kernel.h"

// Global module loader instance
ModuleLoader* g_module_loader = nullptr;

ModuleLoader::ModuleLoader() 
    : loaded_modules(nullptr), module_count(0), 
      symbol_table(nullptr), symbol_count(0), max_symbols(0),
      serial_loading_enabled(false) {
}

ModuleLoader::~ModuleLoader() {
    // Unload all modules and clean up
    ModuleInfo* current = loaded_modules;
    while (current) {
        ModuleInfo* next = current->next;
        UnloadModule(current);
        current = next;
    }
    
    if (symbol_table) {
        free(symbol_table);
    }
}

bool ModuleLoader::Initialize() {
    // Initialize linked list
    loaded_modules = nullptr;
    module_count = 0;
    
    // Allocate symbol table
    max_symbols = MAX_SYMBOLS;
    symbol_table = (SymbolInfo*)malloc(max_symbols * sizeof(SymbolInfo));
    if (!symbol_table) {
        LOG("Error: Failed to allocate symbol table for module loader");
        return false;
    }
    symbol_count = 0;
    memset(symbol_table, 0, max_symbols * sizeof(SymbolInfo));
    
    LOG("Module loading system initialized");
    LOG("Module loading framework ready to load kernel modules");
    
    return true;
}

ModuleLoadResult ModuleLoader::LoadModule(void* module_data, uint32 size, const char* name) {
    if (!module_data || size < sizeof(ModuleHeader)) {
        return ModuleLoadResult::INVALID_FORMAT;
    }
    
    // Validate module format
    ModuleLoadResult validation_result = ValidateModule(module_data, size);
    if (validation_result != ModuleLoadResult::SUCCESS) {
        LOG("Module validation failed: " << (int)validation_result);
        return validation_result;
    }
    
    // Get header
    ModuleHeader* header = (ModuleHeader*)module_data;
    char module_name[64];
    
    if (name) {
        strncpy(module_name, name, sizeof(module_name) - 1);
        module_name[sizeof(module_name) - 1] = 0;
    } else {
        strncpy(module_name, header->module_name, sizeof(module_name) - 1);
        module_name[sizeof(module_name) - 1] = 0;
    }
    
    // Check if module is already loaded
    if (IsModuleLoaded(module_name)) {
        LOG("Module already loaded: " << module_name);
        return ModuleLoadResult::ALREADY_LOADED;
    }
    
    // Allocate memory for the module
    void* module_base = malloc(size);
    if (!module_base) {
        LOG("Failed to allocate memory for module: " << module_name);
        return ModuleLoadResult::INSUFFICIENT_MEMORY;
    }
    
    // Copy module to allocated memory
    memcpy(module_base, module_data, size);
    
    // Create module info structure
    ModuleInfo* module_info = (ModuleInfo*)malloc(sizeof(ModuleInfo));
    if (!module_info) {
        LOG("Failed to allocate module info for: " << module_name);
        free(module_base);
        return ModuleLoadResult::INSUFFICIENT_MEMORY;
    }
    
    // Initialize module info
    memset(module_info, 0, sizeof(ModuleInfo));
    strncpy(module_info->name, module_name, sizeof(module_info->name) - 1);
    module_info->name[sizeof(module_info->name) - 1] = 0;
    module_info->base_address = module_base;
    module_info->size = size;
    module_info->header = (ModuleHeader*)module_base;
    module_info->loaded = false;
    module_info->initialized = false;
    module_info->reference_count = 1;
    
    // Add to loaded modules list
    module_info->next = loaded_modules;
    loaded_modules = module_info;
    module_count++;
    
    LOG("Module loaded successfully: " << module_name << " at 0x" << (uint32)module_base);
    
    // Perform security checks
    ModuleLoadResult security_result = SecurityCheck(module_info);
    if (security_result != ModuleLoadResult::SUCCESS) {
        LOG("Security check failed for module: " << module_name);
        UnloadModule(module_info);
        return security_result;
    }
    
    // Resolve dependencies
    ModuleLoadResult dep_result = ResolveDependencies(module_info);
    if (dep_result != ModuleLoadResult::SUCCESS) {
        LOG("Dependency resolution failed for module: " << module_name);
        UnloadModule(module_info);
        return dep_result;
    }
    
    // Relocate module if necessary
    ModuleLoadResult reloc_result = RelocateModule(module_info, module_base);
    if (reloc_result != ModuleLoadResult::SUCCESS) {
        LOG("Module relocation failed for: " << module_name);
        UnloadModule(module_info);
        return reloc_result;
    }
    
    // Module loaded successfully but not yet initialized
    module_info->loaded = true;
    
    // Try to initialize the module
    ModuleLoadResult init_result = InitializeModule(module_info);
    if (init_result != ModuleLoadResult::SUCCESS) {
        LOG("Module initialization failed for: " << module_name);
        // Don't unload here, as the module might be in a partially initialized state
        module_info->initialized = false;
        return init_result;
    }
    
    module_info->initialized = true;
    LOG("Module initialized successfully: " << module_name);
    
    return ModuleLoadResult::SUCCESS;
}

ModuleLoadResult ModuleLoader::LoadModuleFromFile(const char* filename) {
    // Note: This would require file system support which may not be available during early boot
    // For now, return not supported; in a fully implemented system, this would load from file
    LOG("Load module from file not implemented: " << filename);
    return ModuleLoadResult::INVALID_FORMAT;
}

ModuleLoadResult ModuleLoader::UnloadModule(const char* module_name) {
    ModuleInfo* current = loaded_modules;
    ModuleInfo* prev = nullptr;
    
    while (current) {
        if (strcmp(current->name, module_name) == 0) {
            // Check reference count
            if (current->reference_count > 1) {
                DecrementReferenceCount(module_name);
                LOG("Module " << module_name << " has references, decreasing count to " << current->reference_count);
                return ModuleLoadResult::SUCCESS; // Success but not actually unloaded
            }
            
            // Execute cleanup if initialized
            if (current->initialized) {
                ModuleCleanupFn cleanup_fn = (ModuleCleanupFn)((uint8*)current->base_address + 
                                                              current->header->cleanup_function);
                if (cleanup_fn) {
                    LOG("Executing cleanup for module: " << module_name);
                    cleanup_fn();
                }
            }
            
            // Remove from linked list
            if (prev) {
                prev->next = current->next;
            } else {
                loaded_modules = current->next;
            }
            
            // Free allocated memory
            free(current->base_address);
            free(current);
            
            module_count--;
            LOG("Module unloaded successfully: " << module_name);
            
            return ModuleLoadResult::SUCCESS;
        }
        
        prev = current;
        current = current->next;
    }
    
    LOG("Module not found for unloading: " << module_name);
    return ModuleLoadResult::INVALID_FORMAT;
}

ModuleLoadResult ModuleLoader::UnloadModule(ModuleInfo* module) {
    if (!module) return ModuleLoadResult::INVALID_FORMAT;
    return UnloadModule(module->name);
}

ModuleLoadResult ModuleLoader::InitializeModule(ModuleInfo* module) {
    if (!module || !module->loaded) {
        return ModuleLoadResult::INVALID_FORMAT;
    }
    
    // Execute the module's initialization function
    ModuleInitFn init_fn = (ModuleInitFn)((uint8*)module->base_address + 
                                         module->header->init_function);
    if (init_fn) {
        LOG("Initializing module: " << module->name);
        ModuleLoadResult result = init_fn();
        
        if (result == ModuleLoadResult::SUCCESS) {
            LOG("Module initialized successfully: " << module->name);
        } else {
            LOG("Module initialization failed: " << module->name << ", result: " << (int)result);
        }
        
        return result;
    }
    
    LOG("No initialization function found for module: " << module->name);
    return ModuleLoadResult::SUCCESS; // Not an error if no init function
}

ModuleInfo* ModuleLoader::GetModuleInfo(const char* name) {
    ModuleInfo* current = loaded_modules;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

ModuleInfo* ModuleLoader::GetLoadedModules(uint32* count) {
    *count = module_count;
    return loaded_modules;
}

bool ModuleLoader::IsModuleLoaded(const char* name) {
    return GetModuleInfo(name) != nullptr;
}

void* ModuleLoader::GetSymbolAddress(const char* symbol_name) {
    if (!symbol_name) return nullptr;
    
    // Check in the global symbol table first
    for (uint32 i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, symbol_name) == 0) {
            return symbol_table[i].address;
        }
    }
    
    // If not found in global table, could search in module exports
    // For this implementation, we'll just return nullptr
    return nullptr;
}

bool ModuleLoader::RegisterSymbol(const char* name, void* address, uint32 size) {
    if (!name || !address || symbol_count >= max_symbols) {
        return false;
    }
    
    // Check if symbol already exists
    for (uint32 i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            // Update existing symbol
            symbol_table[i].address = address;
            symbol_table[i].size = size;
            return true;
        }
    }
    
    // Add new symbol
    strncpy(symbol_table[symbol_count].name, name, sizeof(symbol_table[symbol_count].name) - 1);
    symbol_table[symbol_count].name[sizeof(symbol_table[symbol_count].name) - 1] = 0;
    symbol_table[symbol_count].address = address;
    symbol_table[symbol_count].size = size;
    
    symbol_count++;
    return true;
}

ModuleLoadResult ModuleLoader::ValidateModule(const void* module_data, uint32 size) {
    if (!module_data || size < sizeof(ModuleHeader)) {
        LOG("Module validation: Invalid size or null data");
        return ModuleLoadResult::INVALID_FORMAT;
    }
    
    const ModuleHeader* header = (const ModuleHeader*)module_data;
    
    // Check signature
    if (strncmp(header->signature, "LKMOD", 5) != 0) {
        LOG("Module validation: Invalid signature: " << header->signature);
        return ModuleLoadResult::INVALID_SIGNATURE;
    }
    
    // Check version (for now, just accept version 1)
    if (header->version != 1) {
        LOG("Module validation: Unsupported version: " << header->version);
        return ModuleLoadResult::INVALID_FORMAT;
    }
    
    // Check total size matches header info
    if (header->module_size != size) {
        LOG("Module validation: Size mismatch - header says " << header->module_size << ", actual is " << size);
        return ModuleLoadResult::INVALID_FORMAT;
    }
    
    // Check if code and data sections fit within total size
    if (header->code_size + header->data_size + header->bss_size > size - sizeof(ModuleHeader)) {
        LOG("Module validation: Sections exceed available space");
        return ModuleLoadResult::INVALID_FORMAT;
    }
    
    // Calculate and verify checksum
    uint32 expected_checksum = CalculateModuleChecksum(module_data, size);
    if (expected_checksum != header->checksum) {
        LOG("Module validation: Checksum mismatch - calculated: 0x" << expected_checksum << 
            ", header has: 0x" << header->checksum);
        return ModuleLoadResult::INVALID_CHECKSUM;
    }
    
    LOG("Module validation: Module is valid");
    return ModuleLoadResult::SUCCESS;
}

ModuleLoadResult ModuleLoader::RelocateModule(ModuleInfo* module, void* target_address) {
    // For a basic implementation, we'll assume position-independent code
    // In a more complex system, we would adjust addresses in the module
    LOG("Module relocation not required (assuming position-independent code)");
    return ModuleLoadResult::SUCCESS;
}

ModuleLoadResult ModuleLoader::ExecuteModule(ModuleInfo* module) {
    if (!module || !module->loaded) {
        return ModuleLoadResult::INVALID_FORMAT;
    }
    
    // Execute the module's entry point
    void (*entry_fn)() = (void(*)())((uint8*)module->base_address + 
                                    module->header->entry_point);
    if (entry_fn) {
        LOG("Executing module: " << module->name);
        entry_fn();
        return ModuleLoadResult::SUCCESS;
    }
    
    LOG("No entry point found for module: " << module->name);
    return ModuleLoadResult::INVALID_ENTRY_POINT;
}

void ModuleLoader::PrintLoadedModules() {
    LOG("=== Loaded Modules ===");
    
    ModuleInfo* current = loaded_modules;
    uint32 count = 0;
    
    while (current) {
        LOG(count << ": " << current->name
            << " at 0x" << (uint32)current->base_address
            << ", size: " << current->size
            << ", refs: " << current->reference_count
            << ", loaded: " << (current->loaded ? "yes" : "no")
            << ", init: " << (current->initialized ? "yes" : "no"));
        current = current->next;
        count++;
    }
    
    LOG("Total modules: " << module_count);
    LOG("====================");
}

void ModuleLoader::EnableSerialLoading(bool enable) {
    serial_loading_enabled = enable;
    LOG("Serial module loading " << (enable ? "enabled" : "disabled"));
}

bool ModuleLoader::IsSerialLoadingEnabled() const {
    return serial_loading_enabled;
}

ModuleLoadResult ModuleLoader::LoadModuleViaSerial() {
    if (!serial_loading_enabled) {
        LOG("Serial loading not enabled");
        return ModuleLoadResult::INVALID_FORMAT;
    }
    
    // For this implementation, we'll just return not supported
    // In a real implementation, this would read a module from serial connection
    LOG("Load module via serial not implemented");
    return ModuleLoadResult::INVALID_FORMAT;
}

bool ModuleLoader::VerifyModuleSignature(ModuleInfo* module) {
    // For this basic implementation, we'll just return true
    // A real implementation would verify cryptographic signatures
    return true;
}

void ModuleLoader::GetStatistics(uint32* module_count, uint32* symbol_count, uint32* total_memory) {
    if (module_count) *module_count = this->module_count;
    if (symbol_count) *symbol_count = this->symbol_count;
    if (total_memory) {
        *total_memory = 0;
        ModuleInfo* current = loaded_modules;
        while (current) {
            *total_memory += current->size;
            current = current->next;
        }
    }
}

ModuleLoadResult ModuleLoader::ResolveDependencies(ModuleInfo* module) {
    // For this basic implementation, we don't have sophisticated dependency resolution
    // In a real system, we would check the import table and ensure all dependencies are met
    LOG("Dependency resolution not implemented for: " << module->name);
    return ModuleLoadResult::SUCCESS;
}

void ModuleLoader::IncrementReferenceCount(const char* module_name) {
    ModuleInfo* module = GetModuleInfo(module_name);
    if (module) {
        module->reference_count++;
    }
}

void ModuleLoader::DecrementReferenceCount(const char* module_name) {
    ModuleInfo* module = GetModuleInfo(module_name);
    if (module && module->reference_count > 0) {
        module->reference_count--;
    }
}

ModuleLoadResult ModuleLoader::SecurityCheck(ModuleInfo* module) {
    // Basic security check - for now, just verify the module header is valid
    // A real implementation would do more sophisticated checks
    if (!module || !module->header) {
        return ModuleLoadResult::INVALID_FORMAT;
    }
    
    LOG("Security check passed for module: " << module->name);
    return ModuleLoadResult::SUCCESS;
}

// Initialize module loading system
bool InitializeModuleLoader() {
    g_module_loader = new ModuleLoader();
    if (!g_module_loader) {
        LOG("Error: Failed to allocate module loader");
        return false;
    }
    
    if (!g_module_loader->Initialize()) {
        LOG("Error: Failed to initialize module loader");
        delete g_module_loader;
        g_module_loader = nullptr;
        return false;
    }
    
    LOG("Module loading system initialized successfully");
    LOG("Kernel module loading framework ready");
    return true;
}

// Utility function to calculate simple checksum
uint32 CalculateModuleChecksum(const void* data, uint32 size) {
    const uint8* bytes = (const uint8*)data;
    uint32 checksum = 0;
    
    for (uint32 i = 0; i < size; i++) {
        checksum += bytes[i];
    }
    
    return checksum;
}