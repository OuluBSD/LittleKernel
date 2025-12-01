#include "Kernel.h"
#include "AbiMultiplexer.h"
#include "Logging.h"
#include "DosSyscalls.h"

// Global instance of the SCI multiplexer
SciMultiplexer* g_sci_multiplexer = nullptr;

SciMultiplexer::SciMultiplexer() {
    initialized = false;
    for (int i = 0; i < MAX_SCI_TYPES; i++) {
        sci_tables[i] = nullptr;
    }
}

SciMultiplexer::~SciMultiplexer() {
    // Cleanup handled by kernel shutdown
    for (int i = 0; i < MAX_SCI_TYPES; i++) {
        if (sci_tables[i]) {
            if (sci_tables[i]->handlers) {
                free(sci_tables[i]->handlers);
            }
            if (sci_tables[i]->names) {
                free(sci_tables[i]->names);
            }
            free(sci_tables[i]);
            sci_tables[i] = nullptr;
        }
    }
}

bool SciMultiplexer::Initialize() {
    LOG("Initializing SCI Multiplexer");

    // Initialize syscall tables for each SCI
    for (int i = 0; i < MAX_SCI_TYPES; i++) {
        sci_tables[i] = (SciSyscallTable*)malloc(sizeof(SciSyscallTable));
        if (!sci_tables[i]) {
            LOG("Failed to allocate SCI table for type: " << i);
            return false;
        }

        // Initialize each table
        sci_tables[i]->handlers = nullptr;
        sci_tables[i]->max_syscall_num = 0;
        sci_tables[i]->names = nullptr;
    }

    // Initialize default tables for known SCIs
    if (!InitializeDosSciV1()) {
        LOG("Failed to initialize DOS SCI v1");
    }

    if (!InitializeLinuxulatorSci()) {
        LOG("Failed to initialize Linuxulator SCI");
    }

    initialized = true;
    LOG("SCI Multiplexer initialized successfully");
    return true;
}

bool SciMultiplexer::RegisterSciSyscalls(SciType type, SciSyscallTable* table) {
    if (type <= SCI_UNKNOWN || type >= MAX_SCI_TYPES || !table) {
        return false;
    }

    // Store the syscall table for this SCI type
    if (sci_tables[type]) {
        // Free existing table if present
        if (sci_tables[type]->handlers) {
            free(sci_tables[type]->handlers);
        }
        if (sci_tables[type]->names) {
            free(sci_tables[type]->names);
        }
        free(sci_tables[type]);
    }

    // Create new table
    sci_tables[type] = (SciSyscallTable*)malloc(sizeof(SciSyscallTable));
    if (!sci_tables[type]) {
        return false;
    }

    // Copy the table
    sci_tables[type]->max_syscall_num = table->max_syscall_num;

    // Allocate handlers
    sci_tables[type]->handlers = (SyscallHandler*)malloc(table->max_syscall_num * sizeof(SyscallHandler));
    if (!sci_tables[type]->handlers) {
        free(sci_tables[type]);
        sci_tables[type] = nullptr;
        return false;
    }

    // Copy handlers
    for (uint32 i = 0; i < table->max_syscall_num; i++) {
        sci_tables[type]->handlers[i] = table->handlers[i];
    }

    // Allocate names if provided
    if (table->names) {
        sci_tables[type]->names = (const char**)malloc(table->max_syscall_num * sizeof(const char*));
        if (!sci_tables[type]->names) {
            free(sci_tables[type]->handlers);
            free(sci_tables[type]);
            sci_tables[type] = nullptr;
            return false;
        }

        // Copy names
        for (uint32 i = 0; i < table->max_syscall_num; i++) {
            sci_tables[type]->names[i] = table->names[i];
        }
    } else {
        sci_tables[type]->names = nullptr;
    }

    return true;
}

int SciMultiplexer::DispatchSyscall(SciType type, uint32 syscall_num,
                                   uint32 arg1, uint32 arg2, uint32 arg3,
                                   uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!initialized || type <= SCI_UNKNOWN || type >= MAX_SCI_TYPES) {
        LOG("SCI Multiplexer not initialized or invalid SCI type");
        return -1;
    }

    SciSyscallTable* table = sci_tables[type];
    if (!table || !table->handlers) {
        LOG("No syscall table for SCI type: " << type);
        return -1;
    }

    if (syscall_num >= table->max_syscall_num) {
        LOG("Syscall number out of range for SCI type: " << type << ", num: " << syscall_num);
        return -1;
    }

    SyscallHandler handler = table->handlers[syscall_num];
    if (!handler) {
        LOG("Unimplemented syscall for SCI type: " << type << ", num: " << syscall_num);
        return -1;
    }

    // Call the appropriate handler
    int result = handler(arg1, arg2, arg3, arg4, arg5, arg6);

    // Log the syscall for debugging (optional)
    if (table->names && table->names[syscall_num]) {
        DLOG("SCI " << type << " syscall " << table->names[syscall_num] << " returned: " << result);
    } else {
        DLOG("SCI " << type << " syscall " << syscall_num << " returned: " << result);
    }

    return result;
}

SciType SciMultiplexer::GetCurrentProcessSci() {
    if (!g_current_process) {
        return SCI_UNKNOWN;
    }
    return GetProcessSci(g_current_process);
}

SciType SciMultiplexer::GetProcessSci(ProcessControlBlock* pcb) {
    if (!pcb || !pcb->sci_context) {
        return SCI_UNKNOWN;
    }

    SciContext* context = (SciContext*)pcb->sci_context;
    return context->type;
}

bool SciMultiplexer::SetProcessSci(ProcessControlBlock* pcb, SciType type) {
    if (!pcb) {
        return false;
    }

    // Create or update the SCI context
    if (!pcb->sci_context) {
        pcb->sci_context = (void*)CreateSciContext(type);
        if (!pcb->sci_context) {
            return false;
        }
    } else {
        SciContext* context = (SciContext*)pcb->sci_context;
        context->type = type;
    }

    return true;
}

SciContext* SciMultiplexer::GetProcessSciContext(ProcessControlBlock* pcb) {
    if (!pcb || !pcb->sci_context) {
        return nullptr;
    }

    return (SciContext*)pcb->sci_context;
}

SciContext* SciMultiplexer::CreateSciContext(SciType type) {
    SciContext* context = (SciContext*)malloc(sizeof(SciContext));
    if (!context) {
        return nullptr;
    }

    context->type = type;
    context->context_data = nullptr;
    context->sci_flags = 0;

    return context;
}

void SciMultiplexer::DestroySciContext(SciContext* context) {
    if (context) {
        // Free any SCI-specific data
        if (context->context_data) {
            free(context->context_data);
        }
        free(context);
    }
}

bool SciMultiplexer::ConvertDosPathToUnix(const char* dos_path, char* unix_path, uint32 max_len) {
    if (!dos_path || !unix_path || max_len == 0) {
        return false;
    }
    
    // Make a copy of the DOS path to work with
    char temp_path[260];  // DOS_MAX_PATH_LENGTH
    if (strlen(dos_path) >= sizeof(temp_path)) {
        return false;
    }
    strcpy_safe(temp_path, dos_path, sizeof(temp_path));
    
    // Check if it's a drive letter path (e.g., "C:\path")
    if (temp_path[1] == ':' && temp_path[2] == '\\') {
        // Extract drive letter
        char drive_letter = temp_path[0];
        
        // Convert drive letter to lowercase for consistency
        if (drive_letter >= 'A' && drive_letter <= 'Z') {
            drive_letter = drive_letter + 32;  // Convert to lowercase
        }
        
        // Map DOS drive to Unix path
        switch (drive_letter) {
            case 'a':  // A drive (RAM disk)
                snprintf(unix_path, max_len, "/A/%s", temp_path + 3);  // Skip "C:\"
                break;
            case 'c':  // C drive (main disk)
                snprintf(unix_path, max_len, "/HardDisk/%s", temp_path + 3);  // Skip "C:\"
                break;
            default:   // Other drives
                snprintf(unix_path, max_len, "/%c/%s", drive_letter, temp_path + 3);
                break;
        }
    } else {
        // It's a relative path or Unix-style path, just convert backslashes
        strcpy_safe(unix_path, temp_path, max_len);
    }
    
    // Convert backslashes to forward slashes
    for (char* p = unix_path; *p; p++) {
        if (*p == '\\') {
            *p = '/';
        }
    }
    
    return true;
}

bool SciMultiplexer::ConvertUnixPathToDos(const char* unix_path, char* dos_path, uint32 max_len) {
    if (!unix_path || !dos_path || max_len == 0) {
        return false;
    }
    
    // For now, just convert forward slashes to backslashes
    // A more complete implementation would map Unix paths back to DOS drive letters
    uint32 i = 0;
    for (; i < max_len - 1 && unix_path[i] != '\0'; i++) {
        dos_path[i] = unix_path[i];
        if (dos_path[i] == '/') {
            dos_path[i] = '\\';
        }
    }
    dos_path[i] = '\0';
    
    return true;
}

bool SciMultiplexer::SetupDosDriveMappings() {
    // Set up default drive letter mappings in registry
    if (g_registry_manager) {
        RegistryWriteString("HKEY_LOCAL_MACHINE\\SYSTEM\\MountPoints", "A:", "/A", KEY_WRITE);
        RegistryWriteString("HKEY_LOCAL_MACHINE\\SYSTEM\\MountPoints", "C:", "/HardDisk", KEY_WRITE);
        LOG("DOS drive letter mappings registered");
    }
    
    return true;
}

ProcessControlBlock* SciMultiplexer::LoadDosExecutable(const char* filename, char* const argv[], char* const envp[]) {
    LOG("Loading DOS executable: " << filename);

    // For now, delegate to the existing DOS executable loading
    if (g_dos_syscall_interface) {
        if (g_dos_syscall_interface->RunDosExecutable(filename, argv, envp)) {
            // Get the newly created process and set its SCI type
            ProcessControlBlock* new_process = process_manager->GetCurrentProcess();
            if (new_process) {
                SetProcessSci(new_process, DOS_SCI_V1);
            }
            return new_process;
        }
    }

    LOG("Failed to load DOS executable: " << filename);
    return nullptr;
}

ProcessControlBlock* SciMultiplexer::LoadLinuxExecutable(const char* filename, char* const argv[], char* const envp[]) {
    LOG("Loading Linux executable: " << filename);

    // For now, delegate to the Linuxulator
    if (g_linuxulator) {
        ProcessControlBlock* new_process = g_linuxulator->LoadLinuxExecutable(filename, argv, envp);
        if (new_process) {
            SetProcessSci(new_process, LINUXULATOR);
        }
        return new_process;
    }

    LOG("Failed to load Linux executable: " << filename);
    return nullptr;
}

ProcessControlBlock* SciMultiplexer::LoadNativeExecutable(const char* filename, char* const argv[], char* const envp[]) {
    LOG("Loading native executable: " << filename);

    // This would be for native LittleKernel executables
    // For now, return null - this would be implemented as the native SCI
    return nullptr;
}

ProcessControlBlock* SciMultiplexer::LoadExecutable(const char* filename, char* const argv[], char* const envp[]) {
    // Determine executable type and load appropriately
    SciType sci_type = DetectSciTypeFromExecutable(filename);

    switch (sci_type) {
        case DOS_SCI_V1:
            return LoadDosExecutable(filename, argv, envp);
        case LINUXULATOR:
            return LoadLinuxExecutable(filename, argv, envp);
        case NATIVE:
            return LoadNativeExecutable(filename, argv, envp);
        default:
            LOG("Unknown executable type for: " << filename);
            return nullptr;
    }
}

SciType SciMultiplexer::DetectSciTypeFromExecutable(const char* filename) {
    // Determine the executable type by examining the file header
    // This is a simplified implementation

    // Check file extension as a simple heuristic
    const char* ext = strrchr(filename, '.');
    if (ext) {
        if (strcmp(ext, ".exe") == 0 || strcmp(ext, ".EXE") == 0) {
            // Could be DOS .exe, check more thoroughly in a real implementation
            return DOS_SCI_V1;
        } else if (strcmp(ext, ".com") == 0 || strcmp(ext, ".COM") == 0) {
            // DOS .com file
            return DOS_SCI_V1;
        } else if (strcmp(ext, ".elf") == 0 || strcmp(ext, ".ELF") == 0) {
            // ELF executable (Linux)
            return LINUXULATOR;
        } else if (strcmp(ext, ".out") == 0 || strcmp(ext, ".OUT") == 0) {
            // Likely Unix/Linux executable
            return LINUXULATOR;
        }
    }

    // If no extension match, try to determine by examining file contents
    // This would involve reading the file header in a real implementation

    // Default to native if nothing matches
    return NATIVE;
}

// Global functions
bool InitializeSciMultiplexer() {
    if (!g_sci_multiplexer) {
        g_sci_multiplexer = new SciMultiplexer();
        if (!g_sci_multiplexer) {
            LOG("Failed to create SCI multiplexer instance");
            return false;
        }

        if (!g_sci_multiplexer->Initialize()) {
            LOG("Failed to initialize SCI multiplexer");
            delete g_sci_multiplexer;
            g_sci_multiplexer = nullptr;
            return false;
        }

        LOG("SCI multiplexer initialized successfully");
    }

    return true;
}

extern "C" int HandleMultiplexedSyscall(uint32 syscall_num,
                                       uint32 arg1, uint32 arg2, uint32 arg3,
                                       uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_sci_multiplexer) {
        LOG("SCI multiplexer not initialized");
        return -1;
    }

    // Get current process SCI and dispatch accordingly
    SciType sci_type = g_sci_multiplexer->GetCurrentProcessSci();
    if (sci_type == SCI_UNKNOWN) {
        LOG("Unknown SCI type for current process");
        return -1;
    }

    return g_sci_multiplexer->DispatchSyscall(sci_type, syscall_num, arg1, arg2, arg3, arg4, arg5, arg6);
}

// ABI-specific initialization functions
bool InitializeDosKpiV1() {
    // This function would set up the DOS KPI v1 syscall table
    // For now, return true as it's already handled elsewhere
    LOG("DOS KPI v1 ABI initialized");
    return true;
}

bool InitializeDosSciV2() {
    // Initialize the DOS SCI v2 interface
    if (!g_dos_kpi_v2_interface) {
        g_dos_kpi_v2_interface = new DosKpiV2Interface();
        if (!g_dos_kpi_v2_interface) {
            LOG("Failed to create DOS-SCIV2 interface instance");
            return false;
        }

        if (!g_dos_kpi_v2_interface->Initialize()) {
            LOG("Failed to initialize DOS-SCIV2 interface");
            delete g_dos_kpi_v2_interface;
            g_dos_kpi_v2_interface = nullptr;
            return false;
        }
    }

    // Setup the syscall table for DOS-SCIV2
    if (!SetupDosKpiV2SyscallTable()) {
        LOG("Failed to setup DOS-SCIV2 syscall table");
        return false;
    }

    LOG("DOS SCI v2 initialized successfully");
    return true;
}

bool InitializeLinuxulatorSci() {
    // Initialize the Linuxulator SCI interface
    if (!g_linuxulator_abi) {
        g_linuxulator_abi = new LinuxulatorAbi();
        if (!g_linuxulator_abi) {
            LOG("Failed to create Linuxulator SCI instance");
            return false;
        }

        if (!g_linuxulator_abi->Initialize()) {
            LOG("Failed to initialize Linuxulator SCI");
            delete g_linuxulator_abi;
            g_linuxulator_abi = nullptr;
            return false;
        }
    }

    // Setup the syscall table for Linuxulator SCI
    if (!SetupLinuxulatorAbiSyscallTable()) {
        LOG("Failed to setup Linuxulator SCI syscall table");
        return false;
    }

    LOG("Linuxulator SCI initialized successfully");
    return true;
}