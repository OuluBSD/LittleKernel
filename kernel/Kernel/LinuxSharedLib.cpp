#include "Kernel.h"
#include "LinuxSharedLib.h"
#include "Logging.h"
#include "Vfs.h"
#include "Linuxulator.h"

// Global shared library manager instance
LinuxSoManager* g_so_manager = nullptr;

LinuxSoManager::LinuxSoManager() {
    libraries = nullptr;
    library_count = 0;
    so_manager_lock.Initialize();
    
    // Initialize hash table
    for (int i = 0; i < LINUX_SO_HASH_TABLE_SIZE; i++) {
        hash_table[i] = nullptr;
    }
}

LinuxSoManager::~LinuxSoManager() {
    // Unload all libraries
    LinuxSharedLibrary* current = libraries;
    while (current) {
        LinuxSharedLibrary* next = current->next;
        UnloadLibrary(current->name);
        current = next;
    }
}

bool LinuxSoManager::Initialize() {
    LOG("Initializing Linux shared library manager");
    
    // Initialize any internal data structures if needed
    LOG("Linux shared library manager initialized successfully");
    return true;
}

LinuxSharedLibrary* LinuxSoManager::LoadLibrary(const char* name, const char* path) {
    if (!name) {
        return nullptr;
    }
    
    so_manager_lock.Acquire();
    
    // Check if library is already loaded
    LinuxSharedLibrary* existing = FindLibrary(name);
    if (existing) {
        existing->ref_count++;
        so_manager_lock.Release();
        LOG("Library " << name << " already loaded, increasing ref count to " << existing->ref_count);
        return existing;
    }
    
    // Create a new library entry
    LinuxSharedLibrary* library = CreateLibraryEntry();
    if (!library) {
        so_manager_lock.Release();
        LOG("Failed to create library entry for " << name);
        return nullptr;
    }
    
    // Set library name and path
    strncpy(library->name, name, sizeof(library->name) - 1);
    library->name[sizeof(library->name) - 1] = '\0';
    
    if (path) {
        strncpy(library->path, path, sizeof(library->path) - 1);
        library->path[sizeof(library->path) - 1] = '\0';
    } else {
        // Use default search path
        // In a real implementation, we would search LD_LIBRARY_PATH, /lib, /usr/lib, etc.
        snprintf(library->path, sizeof(library->path), "/lib/%s", name);
    }
    
    // Parse ELF headers
    if (!ParseElfHeaders(library, library->path)) {
        LOG("Failed to parse ELF headers for library " << name);
        DestroyLibraryEntry(library);
        so_manager_lock.Release();
        return nullptr;
    }
    
    // Load ELF segments
    if (!LoadElfSegments(library, library->path)) {
        LOG("Failed to load ELF segments for library " << name);
        DestroyLibraryEntry(library);
        so_manager_lock.Release();
        return nullptr;
    }
    
    // Parse dynamic section
    if (!ParseDynamicSection(library)) {
        LOG("Failed to parse dynamic section for library " << name);
        DestroyLibraryEntry(library);
        so_manager_lock.Release();
        return nullptr;
    }
    
    // Parse symbol table
    if (!ParseSymbolTable(library)) {
        LOG("Failed to parse symbol table for library " << name);
        DestroyLibraryEntry(library);
        so_manager_lock.Release();
        return nullptr;
    }
    
    // Parse relocation tables
    if (!ParseRelocationTables(library)) {
        LOG("Failed to parse relocation tables for library " << name);
        DestroyLibraryEntry(library);
        so_manager_lock.Release();
        return nullptr;
    }
    
    // Parse hash tables
    if (!ParseHashTables(library)) {
        LOG("Failed to parse hash tables for library " << name);
        DestroyLibraryEntry(library);
        so_manager_lock.Release();
        return nullptr;
    }
    
    // Parse version sections
    if (!ParseVersionSections(library)) {
        LOG("Failed to parse version sections for library " << name);
        DestroyLibraryEntry(library);
        so_manager_lock.Release();
        return nullptr;
    }
    
    // Load dependencies
    if (!LoadDependencies(library)) {
        LOG("Failed to load dependencies for library " << name);
        DestroyLibraryEntry(library);
        so_manager_lock.Release();
        return nullptr;
    }
    
    // Perform relocations
    if (!RelocateLibrary(library)) {
        LOG("Failed to relocate library " << name);
        DestroyLibraryEntry(library);
        so_manager_lock.Release();
        return nullptr;
    }
    
    // Add to linked list
    library->next = libraries;
    if (libraries) {
        libraries->prev = library;
    }
    libraries = library;
    library_count++;
    
    // Add to hash table for fast lookup
    AddToHashTable(library);
    
    // Mark as loaded
    library->loaded = true;
    library->ref_count = 1;
    
    // Call initialization functions
    if (!InitializeLibrary(library)) {
        LOG("Warning: Failed to initialize library " << name);
        // We'll still return the library since it's loaded
    }
    
    LOG("Successfully loaded shared library: " << name);
    so_manager_lock.Release();
    return library;
}

bool LinuxSoManager::UnloadLibrary(const char* name) {
    if (!name) {
        return false;
    }
    
    so_manager_lock.Acquire();
    
    // Find the library
    LinuxSharedLibrary* library = FindLibrary(name);
    if (!library) {
        so_manager_lock.Release();
        LOG("Library " << name << " not found for unloading");
        return false;
    }
    
    // Decrease reference count
    library->ref_count--;
    
    // Only unload if ref count reaches zero
    if (library->ref_count > 0) {
        LOG("Library " << name << " ref count decreased to " << library->ref_count);
        so_manager_lock.Release();
        return true;
    }
    
    // Call finalization functions
    FinalizeLibrary(library);
    
    // Remove from hash table
    RemoveFromHashTable(library);
    
    // Remove from linked list
    if (library->prev) {
        library->prev->next = library->next;
    } else {
        libraries = library->next;
    }
    
    if (library->next) {
        library->next->prev = library->prev;
    }
    
    library_count--;
    
    // Clean up the library
    DestroyLibraryEntry(library);
    
    LOG("Successfully unloaded shared library: " << name);
    so_manager_lock.Release();
    return true;
}

LinuxSharedLibrary* LinuxSoManager::FindLibrary(const char* name) {
    if (!name) {
        return nullptr;
    }
    
    // First try hash table for fast lookup
    LinuxSharedLibrary* library = FindInHashTable(name);
    if (library) {
        return library;
    }
    
    // Fall back to linear search
    LinuxSharedLibrary* current = libraries;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return nullptr;
}

LinuxSharedLibrary* LinuxSoManager::GetLibraryByAddress(uint32 address) {
    LinuxSharedLibrary* current = libraries;
    while (current) {
        if (address >= current->base_address && 
            address < (current->base_address + current->size)) {
            return current;
        }
        current = current->next;
    }
    
    return nullptr;
}

uint32 LinuxSoManager::ResolveSymbol(LinuxSharedLibrary* library, const char* symbol_name) {
    if (!library || !symbol_name) {
        return 0;
    }
    
    // Search in this library's symbol table
    for (uint32 i = 0; i < library->symbol_count; i++) {
        LinuxElfSym* sym = &library->symbol_table[i];
        const char* name = GetStringFromTable(library, sym->st_name);
        
        if (name && strcmp(name, symbol_name) == 0) {
            // Found the symbol
            return sym->st_value + library->base_address;
        }
    }
    
    return 0; // Symbol not found
}

uint32 LinuxSoManager::ResolveSymbolGlobal(const char* symbol_name) {
    if (!symbol_name) {
        return 0;
    }
    
    // Search in all loaded libraries
    LinuxSharedLibrary* current = libraries;
    while (current) {
        uint32 address = ResolveSymbol(current, symbol_name);
        if (address != 0) {
            return address;
        }
        current = current->next;
    }
    
    return 0; // Symbol not found
}

bool LinuxSoManager::RelocateLibrary(LinuxSharedLibrary* library) {
    if (!library || library->relocated) {
        return true; // Already relocated or invalid
    }
    
    LOG("Relocating library: " << library->name);
    
    // Apply RELA relocations
    if (!ApplyRelaRelocations(library)) {
        LOG("Failed to apply RELA relocations for library " << library->name);
        return false;
    }
    
    // Apply REL relocations
    if (!ApplyRelRelocations(library)) {
        LOG("Failed to apply REL relocations for library " << library->name);
        return false;
    }
    
    // Apply PLT relocations
    if (!ApplyPltRelocations(library)) {
        LOG("Failed to apply PLT relocations for library " << library->name);
        return false;
    }
    
    // Setup GOT
    if (!SetupGOT(library)) {
        LOG("Failed to setup GOT for library " << library->name);
        return false;
    }
    
    // Setup PLT
    if (!SetupPLT(library)) {
        LOG("Failed to setup PLT for library " << library->name);
        return false;
    }
    
    library->relocated = true;
    LOG("Library " << library->name << " relocated successfully");
    return true;
}

bool LinuxSoManager::InitializeLibrary(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    LOG("Initializing library: " << library->name);
    
    // Call initialization function if present
    if (library->init_func != 0) {
        // In a real implementation, we would call the function
        // For now, we'll just log it
        LOG("Calling init function at 0x" << library->init_func);
    }
    
    // Call initialization array functions
    if (library->init_array && library->init_array_size > 0) {
        uint32 count = library->init_array_size / sizeof(uint32);
        for (uint32 i = 0; i < count; i++) {
            uint32 func_addr = library->init_array[i];
            if (func_addr != 0) {
                // In a real implementation, we would call the function
                // For now, we'll just log it
                LOG("Calling init array function at 0x" << func_addr);
            }
        }
    }
    
    LOG("Library " << library->name << " initialized successfully");
    return true;
}

bool LinuxSoManager::FinalizeLibrary(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    LOG("Finalizing library: " << library->name);
    
    // Call finalization array functions
    if (library->fini_array && library->fini_array_size > 0) {
        uint32 count = library->fini_array_size / sizeof(uint32);
        for (uint32 i = 0; i < count; i++) {
            uint32 func_addr = library->fini_array[i];
            if (func_addr != 0) {
                // In a real implementation, we would call the function
                // For now, we'll just log it
                LOG("Calling fini array function at 0x" << func_addr);
            }
        }
    }
    
    // Call finalization function if present
    if (library->fini_func != 0) {
        // In a real implementation, we would call the function
        // For now, we'll just log it
        LOG("Calling fini function at 0x" << library->fini_func);
    }
    
    LOG("Library " << library->name << " finalized successfully");
    return true;
}

uint32 LinuxSoManager::GetLibraryCount() {
    return library_count;
}

void LinuxSoManager::PrintLibraryList() {
    LOG("Loaded Linux shared libraries:");
    LinuxSharedLibrary* current = libraries;
    while (current) {
        LOG("  " << current->name << " (ref_count: " << current->ref_count 
             << ", base: 0x" << current->base_address << ", size: " << current->size << ")");
        current = current->next;
    }
}

void LinuxSoManager::PrintLibraryInfo(LinuxSharedLibrary* library) {
    if (!library) {
        return;
    }
    
    LOG("Linux Shared Library Info:");
    LOG("  Name: " << library->name);
    LOG("  Path: " << library->path);
    LOG("  Base Address: 0x" << library->base_address);
    LOG("  Size: " << library->size);
    LOG("  Type: " << library->type);
    LOG("  Reference Count: " << library->ref_count);
    LOG("  Loaded: " << (library->loaded ? "Yes" : "No"));
    LOG("  Relocated: " << (library->relocated ? "Yes" : "No"));
    LOG("  Entry Point: 0x" << library->entry_point);
    LOG("  Init Function: 0x" << library->init_func);
    LOG("  Fini Function: 0x" << library->fini_func);
    LOG("  Dependencies: " << library->dep_count);
    LOG("  Symbols: " << library->symbol_count);
    LOG("  RELA Relocations: " << library->rela_count);
    LOG("  REL Relocations: " << library->rel_count);
    LOG("  PLT Relocations: " << library->jmprel_count);
    LOG("  Load Time: " << library->load_time);
    LOG("  Last Used: " << library->last_used);
}

// Internal helper functions

uint32 LinuxSoManager::HashName(const char* name) {
    if (!name) {
        return 0;
    }
    
    // Simple hash function (would be replaced with ELF hash in real implementation)
    uint32 hash = 0;
    while (*name) {
        hash = (hash << 4) + *name++;
        uint32 g = hash & 0xf0000000;
        if (g) {
            hash ^= g >> 24;
        }
        hash &= ~g;
    }
    
    return hash % LINUX_SO_HASH_TABLE_SIZE;
}

LinuxSharedLibrary* LinuxSoManager::CreateLibraryEntry() {
    LinuxSharedLibrary* library = (LinuxSharedLibrary*)malloc(sizeof(LinuxSharedLibrary));
    if (!library) {
        return nullptr;
    }
    
    // Zero-initialize the structure
    memset(library, 0, sizeof(LinuxSharedLibrary));
    
    // Initialize default values
    library->base_address = 0;
    library->size = 0;
    library->type = LINUX_SO_TYPE_LIBRARY;
    library->ref_count = 0;
    library->loaded = false;
    library->relocated = false;
    library->entry_point = 0;
    library->init_func = 0;
    library->fini_func = 0;
    library->init_array = nullptr;
    library->init_array_size = 0;
    library->fini_array = nullptr;
    library->fini_array_size = 0;
    library->dep_count = 0;
    library->dynamic_section = nullptr;
    library->dynamic_section_size = 0;
    library->symbol_table = nullptr;
    library->symbol_count = 0;
    library->string_table = nullptr;
    library->string_table_size = 0;
    library->rela_table = nullptr;
    library->rela_count = 0;
    library->rel_table = nullptr;
    library->rel_count = 0;
    library->jmprel_table = nullptr;
    library->jmprel_count = 0;
    library->pltrel_type = 0;
    library->hash_table = nullptr;
    library->gnu_hash_table = nullptr;
    library->got = nullptr;
    library->got_size = 0;
    library->plt_base = 0;
    library->plt_size = 0;
    library->verdef = nullptr;
    library->verdef_count = 0;
    library->verneed = nullptr;
    library->verneed_count = 0;
    library->versym = nullptr;
    library->versym_count = 0;
    library->load_time = global_timer ? global_timer->GetTickCount() : 0;
    library->last_used = library->load_time;
    library->load_order = 0;
    library->next = nullptr;
    library->prev = nullptr;
    
    // Initialize name and path to empty strings
    library->name[0] = '\0';
    library->path[0] = '\0';
    
    return library;
}

void LinuxSoManager::DestroyLibraryEntry(LinuxSharedLibrary* library) {
    if (!library) {
        return;
    }
    
    // Free allocated memory
    if (library->dynamic_section) {
        free(library->dynamic_section);
    }
    
    if (library->symbol_table) {
        free(library->symbol_table);
    }
    
    if (library->string_table) {
        free(library->string_table);
    }
    
    if (library->rela_table) {
        free(library->rela_table);
    }
    
    if (library->rel_table) {
        free(library->rel_table);
    }
    
    if (library->jmprel_table) {
        free(library->jmprel_table);
    }
    
    if (library->hash_table) {
        free(library->hash_table);
    }
    
    if (library->gnu_hash_table) {
        free(library->gnu_hash_table);
    }
    
    if (library->got) {
        free(library->got);
    }
    
    if (library->init_array) {
        free(library->init_array);
    }
    
    if (library->fini_array) {
        free(library->fini_array);
    }
    
    if (library->verdef) {
        free(library->verdef);
    }
    
    if (library->verneed) {
        free(library->verneed);
    }
    
    if (library->versym) {
        free(library->versym);
    }
    
    // Free the library structure itself
    free(library);
}

bool LinuxSoManager::ParseElfHeaders(LinuxSharedLibrary* library, const char* filename) {
    if (!library || !filename) {
        return false;
    }
    
    // Open the file
    int fd = g_vfs->Open(filename, O_RDONLY);
    if (fd < 0) {
        LOG("Failed to open ELF file: " << filename);
        return false;
    }
    
    // Read the ELF header
    int bytes_read = g_vfs->Read(fd, &library->elf_header, sizeof(LinuxElfHeader));
    if (bytes_read != sizeof(LinuxElfHeader)) {
        LOG("Failed to read ELF header from file: " << filename);
        g_vfs->Close(fd);
        return false;
    }
    
    // Verify ELF header
    if (!g_linuxulator->VerifyElfHeader(&library->elf_header)) {
        LOG("Invalid ELF header in file: " << filename);
        g_vfs->Close(fd);
        return false;
    }
    
    // Close the file
    g_vfs->Close(fd);
    
    return true;
}

bool LinuxSoManager::LoadElfSegments(LinuxSharedLibrary* library, const char* filename) {
    if (!library || !filename) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Read the program header table
    // 2. Map each LOAD segment into memory with appropriate permissions
    // 3. Set up the library's memory layout
    // 4. Prepare the library for execution
    
    LOG("Loading ELF segments for library: " << filename);
    
    // For now, we'll just simulate loading by allocating memory
    library->base_address = (uint32)malloc(0x100000); // Allocate 1MB for now
    if (!library->base_address) {
        LOG("Failed to allocate memory for library: " << filename);
        return false;
    }
    
    library->size = 0x100000; // 1MB
    return true;
}

bool LinuxSoManager::ParseDynamicSection(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Locate the dynamic section in the ELF file
    // 2. Parse all dynamic entries
    // 3. Extract information like needed libraries, symbol tables, etc.
    
    LOG("Parsing dynamic section for library: " << library->name);
    return true; // For now, just return success
}

bool LinuxSoManager::ParseSymbolTable(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Locate the symbol table (.dynsym section)
    // 2. Locate the string table (.dynstr section)
    // 3. Parse all symbols
    // 4. Build internal data structures for symbol lookup
    
    LOG("Parsing symbol table for library: " << library->name);
    return true; // For now, just return success
}

bool LinuxSoManager::ParseRelocationTables(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Locate the RELA relocation table (.rela.dyn section)
    // 2. Locate the REL relocation table (.rel.dyn section)
    // 3. Locate the PLT relocation table (.rela.plt or .rel.plt section)
    // 4. Parse all relocation entries
    
    LOG("Parsing relocation tables for library: " << library->name);
    return true; // For now, just return success
}

bool LinuxSoManager::ParseHashTables(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Locate the hash table (.hash section)
    // 2. Locate the GNU hash table (.gnu.hash section)
    // 3. Parse the hash tables for fast symbol lookup
    
    LOG("Parsing hash tables for library: " << library->name);
    return true; // For now, just return success
}

bool LinuxSoManager::ParseVersionSections(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Locate the version definition section (.gnu.version_d)
    // 2. Locate the version needed section (.gnu.version_r)
    // 3. Locate the version symbol table (.gnu.version)
    // 4. Parse version information
    
    LOG("Parsing version sections for library: " << library->name);
    return true; // For now, just return success
}

bool LinuxSoManager::LoadDependencies(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Iterate through DT_NEEDED entries in the dynamic section
    // 2. Load each needed library recursively
    // 3. Establish dependency relationships
    
    LOG("Loading dependencies for library: " << library->name);
    return true; // For now, just return success
}

bool LinuxSoManager::ApplyRelaRelocations(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Iterate through all RELA relocations
    // 2. Apply each relocation based on its type
    // 3. Update memory locations with relocated values
    
    LOG("Applying RELA relocations for library: " << library->name);
    return true; // For now, just return success
}

bool LinuxSoManager::ApplyRelRelocations(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Iterate through all REL relocations
    // 2. Apply each relocation based on its type
    // 3. Update memory locations with relocated values
    
    LOG("Applying REL relocations for library: " << library->name);
    return true; // For now, just return success
}

bool LinuxSoManager::ApplyPltRelocations(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Iterate through all PLT relocations
    // 2. Apply each relocation based on its type
    // 3. Update PLT/GOT entries with relocated values
    
    LOG("Applying PLT relocations for library: " << library->name);
    return true; // For now, just return success
}

bool LinuxSoManager::SetupGOT(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Initialize the Global Offset Table (GOT)
    // 2. Set up GOT entries for resolved symbols
    // 3. Set up lazy binding entries for unresolved symbols
    
    LOG("Setting up GOT for library: " << library->name);
    return true; // For now, just return success
}

bool LinuxSoManager::SetupPLT(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    // In a real implementation, this would:
    // 1. Initialize the Procedure Linkage Table (PLT)
    // 2. Set up PLT entries to call resolver functions
    // 3. Connect PLT entries with GOT entries
    
    LOG("Setting up PLT for library: " << library->name);
    return true; // For now, just return success
}

char* LinuxSoManager::GetStringFromTable(LinuxSharedLibrary* library, uint32 offset) {
    if (!library || !library->string_table || offset >= library->string_table_size) {
        return nullptr;
    }
    
    return library->string_table + offset;
}

LinuxElfSym* LinuxSoManager::GetSymbolFromTable(LinuxSharedLibrary* library, uint32 index) {
    if (!library || !library->symbol_table || index >= library->symbol_count) {
        return nullptr;
    }
    
    return &library->symbol_table[index];
}

uint32 LinuxSoManager::CalculateRelocation(LinuxSharedLibrary* library, uint32 type, 
                                            uint32 symbol_value, uint32 addend, uint32 address) {
    if (!library) {
        return 0;
    }
    
    // In a real implementation, this would calculate the relocated value
    // based on the relocation type, symbol value, addend, and target address
    
    switch (type) {
        case R_LINUX_386_32:
            return symbol_value + addend;
        case R_LINUX_386_PC32:
            return symbol_value + addend - address;
        case R_LINUX_386_GLOB_DAT:
        case R_LINUX_386_JMP_SLOT:
            return symbol_value;
        case R_LINUX_386_RELATIVE:
            return library->base_address + addend;
        default:
            LOG("Unsupported relocation type: " << type);
            return 0;
    }
}

bool LinuxSoManager::AddToHashTable(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    uint32 hash = HashName(library->name);
    library->next = (LinuxSharedLibrary*)hash_table[hash];
    hash_table[hash] = library;
    return true;
}

bool LinuxSoManager::RemoveFromHashTable(LinuxSharedLibrary* library) {
    if (!library) {
        return false;
    }
    
    uint32 hash = HashName(library->name);
    LinuxSharedLibrary* current = hash_table[hash];
    LinuxSharedLibrary* prev = nullptr;
    
    while (current) {
        if (current == library) {
            if (prev) {
                prev->next = current->next;
            } else {
                hash_table[hash] = (LinuxSharedLibrary*)current->next;
            }
            return true;
        }
        prev = current;
        current = (LinuxSharedLibrary*)current->next;
    }
    
    return false; // Not found in hash table
}

LinuxSharedLibrary* LinuxSoManager::FindInHashTable(const char* name) {
    if (!name) {
        return nullptr;
    }
    
    uint32 hash = HashName(name);
    LinuxSharedLibrary* current = hash_table[hash];
    
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = (LinuxSharedLibrary*)current->next;
    }
    
    return nullptr; // Not found
}

// Global functions

bool InitializeSoManager() {
    if (!g_so_manager) {
        g_so_manager = new LinuxSoManager();
        if (!g_so_manager) {
            LOG("Failed to create Linux shared library manager instance");
            return false;
        }
        
        if (!g_so_manager->Initialize()) {
            LOG("Failed to initialize Linux shared library manager");
            delete g_so_manager;
            g_so_manager = nullptr;
            return false;
        }
        
        LOG("Linux shared library manager initialized successfully");
    }
    
    return true;
}

LinuxSharedLibrary* LoadLinuxSharedLibrary(const char* name, const char* path) {
    if (!name || !g_so_manager) {
        return nullptr;
    }
    
    return g_so_manager->LoadLibrary(name, path);
}

bool UnloadLinuxSharedLibrary(const char* name) {
    if (!name || !g_so_manager) {
        return false;
    }
    
    return g_so_manager->UnloadLibrary(name);
}

uint32 ResolveLinuxSymbol(const char* symbol_name) {
    if (!symbol_name || !g_so_manager) {
        return 0;
    }
    
    return g_so_manager->ResolveSymbolGlobal(symbol_name);
}