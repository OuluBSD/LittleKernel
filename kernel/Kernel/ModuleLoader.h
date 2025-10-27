#ifndef _Kernel_ModuleLoader_h_
#define _Kernel_ModuleLoader_h_

#include "Defs.h"
#include "Kernel.h"

// Module header structure
struct ModuleHeader {
    char signature[8];        // "LKMOD" signature + null + version info
    uint32 version;         // Module format version
    uint32 header_size;     // Size of this header
    uint32 module_size;     // Total size of the module
    uint32 code_size;       // Size of code section
    uint32 data_size;       // Size of data section
    uint32 bss_size;        // Size of BSS section
    uint32 entry_point;     // Entry point offset from module start
    uint32 init_function;   // Initialization function offset
    uint32 cleanup_function; // Cleanup function offset
    uint32 export_table_offset; // Offset to export table
    uint32 export_count;    // Number of exported symbols
    uint32 import_table_offset; // Offset to import table
    uint32 import_count;    // Number of imported symbols
    char module_name[64];     // Name of the module
    char author[64];          // Author of the module
    char description[256];    // Description of the module
    uint32 checksum;        // Simple checksum for integrity
};

// Module information structure
struct ModuleInfo {
    char name[64];
    void* base_address;
    uint32 size;
    ModuleHeader* header;
    bool loaded;
    bool initialized;
    uint32 reference_count;
    ModuleInfo* next;
};

// Symbol information for export/import
struct SymbolInfo {
    char name[128];
    void* address;
    uint32 size;
};

// Module loading result codes
enum class ModuleLoadResult {
    SUCCESS = 0,
    INVALID_FORMAT = -1,
    INVALID_SIGNATURE = -2,
    INVALID_CHECKSUM = -3,
    ALREADY_LOADED = -4,
    INSUFFICIENT_MEMORY = -5,
    INIT_FAILED = -6,
    MISSING_IMPORTS = -7,
    INVALID_ENTRY_POINT = -8
};

// Module loading callback function type
typedef ModuleLoadResult (*ModuleInitFn)();
typedef void (*ModuleCleanupFn)();

// Module loader interface
class ModuleLoader {
private:
    static const uint32 MAX_LOADED_MODULES = 64;
    static const uint32 MAX_SYMBOLS = 1024;
    
    ModuleInfo* loaded_modules;
    uint32 module_count;
    
    // Symbol table for exports from all modules
    SymbolInfo* symbol_table;
    uint32 symbol_count;
    uint32 max_symbols;
    
    // For serial module loading (optional enhancement)
    bool serial_loading_enabled;
    
public:
    ModuleLoader();
    ~ModuleLoader();
    
    // Initialize the module loading system
    bool Initialize();
    
    // Load a module from memory
    ModuleLoadResult LoadModule(void* module_data, uint32 size, const char* name = nullptr);
    
    // Load a module from a file (when file system is available)
    ModuleLoadResult LoadModuleFromFile(const char* filename);
    
    // Unload a module
    ModuleLoadResult UnloadModule(const char* module_name);
    
    // Unload a module by its info
    ModuleLoadResult UnloadModule(ModuleInfo* module);
    
    // Initialize a loaded module
    ModuleLoadResult InitializeModule(ModuleInfo* module);
    
    // Get module information
    ModuleInfo* GetModuleInfo(const char* name);
    
    // Get loaded module list
    ModuleInfo* GetLoadedModules(uint32* count);
    
    // Check if a module is loaded
    bool IsModuleLoaded(const char* name);
    
    // Get symbol address by name
    void* GetSymbolAddress(const char* symbol_name);
    
    // Register a symbol in the global symbol table
    bool RegisterSymbol(const char* name, void* address, uint32 size = 0);
    
    // Validate module header
    ModuleLoadResult ValidateModule(const void* module_data, uint32 size);
    
    // Relocate module (if needed for position-independent code)
    ModuleLoadResult RelocateModule(ModuleInfo* module, void* target_address);
    
    // Execute module entry point
    ModuleLoadResult ExecuteModule(ModuleInfo* module);
    
    // Print loaded modules information
    void PrintLoadedModules();
    
    // Enable/disable serial loading capability
    void EnableSerialLoading(bool enable);
    bool IsSerialLoadingEnabled() const;
    
    // Load module via serial connection (for optional enhancement)
    ModuleLoadResult LoadModuleViaSerial();
    
    // Verify module signatures (for optional security enhancement)
    bool VerifyModuleSignature(ModuleInfo* module);
    
    // Get module statistics
    void GetStatistics(uint32* module_count, uint32* symbol_count, uint32* total_memory);
    
    // Perform dependency resolution
    ModuleLoadResult ResolveDependencies(ModuleInfo* module);
    
    // Update module reference count
    void IncrementReferenceCount(const char* module_name);
    void DecrementReferenceCount(const char* module_name);
    
    // Security check for modules
    ModuleLoadResult SecurityCheck(ModuleInfo* module);
};

// Macro for modules to define their entry points
#define MODULE_ENTRY_POINT() \
    extern "C" __attribute__((section(".module_init"))) ModuleLoadResult module_init(); \
    ModuleLoadResult module_init()

#define MODULE_CLEANUP_POINT() \
    extern "C" __attribute__((section(".module_cleanup"))) void module_cleanup(); \
    void module_cleanup()

#define DECLARE_MODULE(name, author, description) \
    static const ModuleHeader module_header = { \
        .signature = "LKMOD00", \
        .version = 1, \
        .header_size = sizeof(ModuleHeader), \
        .module_size = 0, /* Filled at build time */ \
        .code_size = 0, /* Filled at build time */ \
        .data_size = 0, /* Filled at build time */ \
        .bss_size = 0, /* Filled at build time */ \
        .entry_point = (uint32)module_entry_point, \
        .init_function = (uint32)module_init, \
        .cleanup_function = (uint32)module_cleanup, \
        .export_table_offset = 0, \
        .export_count = 0, \
        .import_table_offset = 0, \
        .import_count = 0, \
        .module_name = name, \
        .author = author, \
        .description = description, \
        .checksum = 0 /* Calculated at runtime */ \
    };

// Global module loader instance
extern ModuleLoader* g_module_loader;

// Initialize module loading system
bool InitializeModuleLoader();

// Utility function to calculate simple checksum
uint32 CalculateModuleChecksum(const void* data, uint32 size);

#endif // _Kernel_ModuleLoader_h_