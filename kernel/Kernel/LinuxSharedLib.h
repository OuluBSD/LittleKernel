#ifndef _Kernel_LinuxSharedLib_h_
#define _Kernel_LinuxSharedLib_h_

#include "Common.h"
#include "Defs.h"
#include "Linuxulator.h"

// Linux shared library constants
#define LINUX_SO_MAX_PATH 4096
#define LINUX_SO_MAX_DEPS 32
#define LINUX_SO_HASH_TABLE_SIZE 256

// Linux shared library types
#define LINUX_SO_TYPE_EXECUTABLE 1
#define LINUX_SO_TYPE_LIBRARY 2
#define LINUX_SO_TYPE_INTERPRETER 3

// Linux ELF section header types for shared libraries
#define SHT_LINUX_DYNAMIC 6
#define SHT_LINUX_DYNSYM 11
#define SHT_LINUX_DYNSTR 12
#define SHT_LINUX_RELA 4
#define SHT_LINUX_REL 9
#define SHT_LINUX_HASH 5
#define SHT_LINUX_GNU_HASH 0x6ffffff6
#define SHT_LINUX_GNU_VERDEF 0x6ffffffd
#define SHT_LINUX_GNU_VERNEED 0x6ffffffe
#define SHT_LINUX_GNU_VERSYM 0x6fffffff

// Linux dynamic entry types
#define DT_LINUX_NULL 0
#define DT_LINUX_NEEDED 1
#define DT_LINUX_PLTRELSZ 2
#define DT_LINUX_PLTGOT 3
#define DT_LINUX_HASH 4
#define DT_LINUX_STRTAB 5
#define DT_LINUX_SYMTAB 6
#define DT_LINUX_RELA 7
#define DT_LINUX_RELASZ 8
#define DT_LINUX_RELAENT 9
#define DT_LINUX_STRSZ 10
#define DT_LINUX_SYMENT 11
#define DT_LINUX_INIT 12
#define DT_LINUX_FINI 13
#define DT_LINUX_SONAME 14
#define DT_LINUX_RPATH 15
#define DT_LINUX_SYMBOLIC 16
#define DT_LINUX_REL 17
#define DT_LINUX_RELSZ 18
#define DT_LINUX_RELENT 19
#define DT_LINUX_PLTREL 20
#define DT_LINUX_DEBUG 21
#define DT_LINUX_TEXTREL 22
#define DT_LINUX_JMPREL 23
#define DT_LINUX_BIND_NOW 24
#define DT_LINUX_INIT_ARRAY 25
#define DT_LINUX_FINI_ARRAY 26
#define DT_LINUX_INIT_ARRAYSZ 27
#define DT_LINUX_FINI_ARRAYSZ 28
#define DT_LINUX_RUNPATH 29
#define DT_LINUX_FLAGS 30
#define DT_LINUX_ENCODING 32
#define DT_LINUX_PREINIT_ARRAY 32
#define DT_LINUX_PREINIT_ARRAYSZ 33
#define DT_LINUX_GNU_HASH 0x6ffffef5
#define DT_LINUX_TLSDESC_PLT 0x6ffffef6
#define DT_LINUX_TLSDESC_GOT 0x6ffffef7
#define DT_LINUX_GNU_CONFLICT 0x6ffffef8
#define DT_LINUX_GNU_LIBLIST 0x6ffffef9
#define DT_LINUX_CONFIG 0x6ffffefa
#define DT_LINUX_DEPAUDIT 0x6ffffefb
#define DT_LINUX_AUDIT 0x6ffffefc
#define DT_LINUX_PLTPAD 0x6ffffefd
#define DT_LINUX_MOVETAB 0x6ffffefe
#define DT_LINUX_SYMINFO 0x6ffffeff
#define DT_LINUX_GNU_VERSYM 0x6ffffff0
#define DT_LINUX_GNU_VERDEF 0x6ffffffd
#define DT_LINUX_GNU_VERDEFNUM 0x6ffffffe
#define DT_LINUX_GNU_VERNEED 0x6fffffff
#define DT_LINUX_GNU_VERNEEDNUM 0x6ffffff

// Linux relocation types
#define R_LINUX_386_32 1
#define R_LINUX_386_PC32 2
#define R_LINUX_386_GOT32 3
#define R_LINUX_386_PLT32 4
#define R_LINUX_386_COPY 5
#define R_LINUX_386_GLOB_DAT 6
#define R_LINUX_386_JMP_SLOT 7
#define R_LINUX_386_RELATIVE 8
#define R_LINUX_386_GOTOFF 9
#define R_LINUX_386_GOTPC 10

// Linux shared library hash table structure
struct LinuxSoHashTable {
    uint32_t nbucket;    // Number of hash buckets
    uint32_t nchain;     // Number of hash chains
    uint32_t bucket[1]; // Hash buckets
    uint32_t chain[1];   // Hash chain table
};

// Linux GNU hash table structure
struct LinuxGnuHashTable {
    uint32_t nbuckets;   // Number of hash buckets
    uint32_t symoffset;  // Index of first symbol in hash table
    uint32_t bloom_size;  // Bloom filter size (in words)
    uint32_t bloom_shift; // Bloom filter shift count
    uint32_t bloom[1];    // Bloom filter
    uint32_t buckets[1];  // Hash buckets
    uint32_t chain[1];   // Hash chain table
};

// Linux dynamic entry structure
struct LinuxDynEntry {
    uint32_t d_tag;      // Dynamic entry tag
    union {
        uint32_t d_val;  // Integer value
        uint32_t d_ptr;  // Pointer value
    } d_un;
};

// Linux symbol table entry
struct LinuxElfSym {
    uint32_t st_name;    // Symbol name (string table index)
    uint32_t st_value;   // Symbol value
    uint32_t st_size;    // Symbol size
    uint8_t st_info;     // Symbol type and binding
    uint8_t st_other;    // Symbol visibility
    uint16_t st_shndx;   // Section index
};

// Linux relocation entry
struct LinuxElfRel {
    uint32_t r_offset;    // Address of reference
    uint32_t r_info;     // Symbol index and type
};

// Linux relocation entry with addend
struct LinuxElfRela {
    uint32_t r_offset;    // Address of reference
    uint32_t r_info;     // Symbol index and type
    int32_t r_addend;    // Constant part of expression
};

// Linux version definition structure
struct LinuxVerDef {
    uint16_t vd_version; // Version revision
    uint16_t vd_flags;   // Version information
    uint16_t vd_ndx;     // Version index
    uint16_t vd_cnt;     // Number of associated aux entries
    uint32_t vd_hash;    // Version name hash value
    uint32_t vd_aux;     // Offset in bytes to verdaux array
    uint32_t vd_next;    // Offset in bytes to next verdef entry
};

// Linux version definition auxiliary structure
struct LinuxVerDefAux {
    uint32_t vda_name;   // Version or dependency name
    uint32_t vda_next;   // Offset in bytes to next verdaux entry
};

// Linux version need structure
struct LinuxVerNeed {
    uint16_t vn_version; // Version of structure
    uint16_t vn_cnt;     // Number of associated aux entries
    uint32_t vn_file;    // Offset of filename for this dependency
    uint32_t vn_aux;     // Offset in bytes to vernaux array
    uint32_t vn_next;    // Offset in bytes to next verneed entry
};

// Linux version need auxiliary structure
struct LinuxVerNeedAux {
    uint32_t vna_hash;   // Hash value of dependency name
    uint16_t vna_flags;  // Dependency specific information
    uint16_t vna_other;  // Unused
    uint32_t vna_name;   // Dependency name string offset
    uint32_t vna_next;   // Offset in bytes to next vernaux entry
};

// Linux shared library structure
struct LinuxSharedLibrary {
    char name[LINUX_SO_MAX_PATH];     // Library name
    char path[LINUX_SO_MAX_PATH];     // Full path to library
    uint32_t base_address;           // Base address where library is loaded
    uint32_t size;                    // Size of library in memory
    uint32_t type;                    // Type of shared object
    uint32_t ref_count;               // Reference count
    bool loaded;                      // Whether library is loaded
    bool relocated;                  // Whether library has been relocated
    uint32_t entry_point;             // Library entry point
    uint32_t init_func;              // Initialization function
    uint32_t fini_func;               // Finalization function
    uint32_t* init_array;            // Initialization function array
    uint32_t init_array_size;        // Size of initialization array
    uint32_t* fini_array;            // Finalization function array
    uint32_t fini_array_size;         // Size of finalization array
    LinuxSharedLibrary* dependencies[LINUX_SO_MAX_DEPS]; // Dependencies
    uint32_t dep_count;               // Number of dependencies
    LinuxElfHeader elf_header;        // ELF header
    LinuxDynEntry* dynamic_section;   // Dynamic section
    uint32_t dynamic_section_size;   // Size of dynamic section
    LinuxElfSym* symbol_table;        // Symbol table
    uint32_t symbol_count;            // Number of symbols
    char* string_table;               // String table
    uint32_t string_table_size;       // Size of string table
    LinuxElfRela* rela_table;         // RELA relocation table
    uint32_t rela_count;              // Number of RELA relocations
    LinuxElfRel* rel_table;           // REL relocation table
    uint32_t rel_count;               // Number of REL relocations
    LinuxElfRela* jmprel_table;       // PLT relocation table
    uint32_t jmprel_count;            // Number of PLT relocations
    uint32_t pltrel_type;             // Type of PLT relocations
    LinuxSoHashTable* hash_table;    // Hash table
    LinuxGnuHashTable* gnu_hash_table; // GNU hash table
    uint32_t* got;                    // Global offset table
    uint32_t got_size;                // Size of GOT
    uint32_t plt_base;                // Base address of PLT
    uint32_t plt_size;                // Size of PLT
    LinuxVerDef* verdef;             // Version definition section
    uint32_t verdef_count;            // Number of version definitions
    LinuxVerNeed* verneed;           // Version needed section
    uint32_t verneed_count;           // Number of version needs
    uint16_t* versym;                // Version symbol table
    uint32_t versym_count;           // Number of version symbols
    uint32_t load_time;               // Time when library was loaded
    uint32_t last_used;               // Last time library was used
    uint32_t load_order;              // Load order (for dependency resolution)
    LinuxSharedLibrary* next;         // Next library in linked list
    LinuxSharedLibrary* prev;         // Previous library in linked list
};

// Linux shared library manager class
class LinuxSoManager {
private:
    LinuxSharedLibrary* libraries;        // Linked list of loaded libraries
    uint32_t library_count;                // Number of loaded libraries
    Spinlock so_manager_lock;              // Lock for thread safety
    LinuxSharedLibrary* hash_table[LINUX_SO_HASH_TABLE_SIZE]; // Hash table for fast lookup
    
public:
    LinuxSoManager();
    ~LinuxSoManager();
    
    // Initialize the shared library manager
    bool Initialize();
    
    // Load a shared library
    LinuxSharedLibrary* LoadLibrary(const char* name, const char* path = nullptr);
    
    // Unload a shared library
    bool UnloadLibrary(const char* name);
    
    // Find a loaded shared library by name
    LinuxSharedLibrary* FindLibrary(const char* name);
    
    // Get library by base address
    LinuxSharedLibrary* GetLibraryByAddress(uint32_t address);
    
    // Resolve a symbol in a library
    uint32_t ResolveSymbol(LinuxSharedLibrary* library, const char* symbol_name);
    
    // Resolve a symbol globally (across all libraries)
    uint32_t ResolveSymbolGlobal(const char* symbol_name);
    
    // Perform relocations for a library
    bool RelocateLibrary(LinuxSharedLibrary* library);
    
    // Call initialization functions for a library
    bool InitializeLibrary(LinuxSharedLibrary* library);
    
    // Call finalization functions for a library
    bool FinalizeLibrary(LinuxSharedLibrary* library);
    
    // Get library statistics
    uint32_t GetLibraryCount();
    void PrintLibraryList();
    void PrintLibraryInfo(LinuxSharedLibrary* library);
    
    // Memory management
    void* AllocateLibraryMemory(uint32_t size);
    bool FreeLibraryMemory(void* address);
    
private:
    // Internal helper functions
    uint32_t HashName(const char* name);
    LinuxSharedLibrary* CreateLibraryEntry();
    void DestroyLibraryEntry(LinuxSharedLibrary* library);
    bool ParseElfHeaders(LinuxSharedLibrary* library, const char* filename);
    bool LoadElfSegments(LinuxSharedLibrary* library, const char* filename);
    bool ParseDynamicSection(LinuxSharedLibrary* library);
    bool ParseSymbolTable(LinuxSharedLibrary* library);
    bool ParseRelocationTables(LinuxSharedLibrary* library);
    bool ParseHashTables(LinuxSharedLibrary* library);
    bool ParseVersionSections(LinuxSharedLibrary* library);
    bool LoadDependencies(LinuxSharedLibrary* library);
    bool ApplyRelocations(LinuxSharedLibrary* library);
    bool ApplyRelaRelocations(LinuxSharedLibrary* library);
    bool ApplyRelRelocations(LinuxSharedLibrary* library);
    bool ApplyPltRelocations(LinuxSharedLibrary* library);
    bool SetupGOT(LinuxSharedLibrary* library);
    bool SetupPLT(LinuxSharedLibrary* library);
    char* GetStringFromTable(LinuxSharedLibrary* library, uint32_t offset);
    LinuxElfSym* GetSymbolFromTable(LinuxSharedLibrary* library, uint32_t index);
    uint32_t CalculateRelocation(LinuxSharedLibrary* library, uint32_t type, uint32_t symbol_value, uint32_t addend, uint32_t address);
    bool AddToHashTable(LinuxSharedLibrary* library);
    bool RemoveFromHashTable(LinuxSharedLibrary* library);
    LinuxSharedLibrary* FindInHashTable(const char* name);
};

// Global shared library manager instance
extern LinuxSoManager* g_so_manager;

// Initialize the shared library manager
bool InitializeSoManager();

// Load a Linux shared library
LinuxSharedLibrary* LoadLinuxSharedLibrary(const char* name, const char* path = nullptr);

// Unload a Linux shared library
bool UnloadLinuxSharedLibrary(const char* name);

// Resolve a symbol in loaded libraries
uint32_t ResolveLinuxSymbol(const char* symbol_name);

#endif