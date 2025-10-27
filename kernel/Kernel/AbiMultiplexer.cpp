#include "Kernel.h"
#include "AbiMultiplexer.h"
#include "Logging.h"
#include "DosSyscalls.h"

// Global instance of the ABI multiplexer
AbiMultiplexer* g_abi_multiplexer = nullptr;

AbiMultiplexer::AbiMultiplexer() {
    initialized = false;
    for (int i = 0; i < MAX_ABI_TYPES; i++) {
        abi_tables[i] = nullptr;
    }
}

AbiMultiplexer::~AbiMultiplexer() {
    // Cleanup handled by kernel shutdown
    for (int i = 0; i < MAX_ABI_TYPES; i++) {
        if (abi_tables[i]) {
            if (abi_tables[i]->handlers) {
                free(abi_tables[i]->handlers);
            }
            if (abi_tables[i]->names) {
                free(abi_tables[i]->names);
            }
            free(abi_tables[i]);
            abi_tables[i] = nullptr;
        }
    }
}

bool AbiMultiplexer::Initialize() {
    LOG("Initializing ABI Multiplexer");
    
    // Initialize syscall tables for each ABI
    for (int i = 0; i < MAX_ABI_TYPES; i++) {
        abi_tables[i] = (AbiSyscallTable*)malloc(sizeof(AbiSyscallTable));
        if (!abi_tables[i]) {
            LOG("Failed to allocate ABI table for type: " << i);
            return false;
        }
        
        // Initialize each table
        abi_tables[i]->handlers = nullptr;
        abi_tables[i]->max_syscall_num = 0;
        abi_tables[i]->names = nullptr;
    }
    
    // Initialize default tables for known ABIs
    if (!InitializeDosKpiV1()) {
        LOG("Failed to initialize DOS KPI v1 ABI");
    }
    
    if (!InitializeLinuxulatorAbi()) {
        LOG("Failed to initialize Linuxulator ABI");
    }
    
    initialized = true;
    LOG("ABI Multiplexer initialized successfully");
    return true;
}

bool AbiMultiplexer::RegisterAbiSyscalls(AbiType type, AbiSyscallTable* table) {
    if (type <= ABI_UNKNOWN || type >= MAX_ABI_TYPES || !table) {
        return false;
    }
    
    // Store the syscall table for this ABI type
    if (abi_tables[type]) {
        // Free existing table if present
        if (abi_tables[type]->handlers) {
            free(abi_tables[type]->handlers);
        }
        if (abi_tables[type]->names) {
            free(abi_tables[type]->names);
        }
        free(abi_tables[type]);
    }
    
    // Create new table
    abi_tables[type] = (AbiSyscallTable*)malloc(sizeof(AbiSyscallTable));
    if (!abi_tables[type]) {
        return false;
    }
    
    // Copy the table
    abi_tables[type]->max_syscall_num = table->max_syscall_num;
    
    // Allocate handlers
    abi_tables[type]->handlers = (SyscallHandler*)malloc(table->max_syscall_num * sizeof(SyscallHandler));
    if (!abi_tables[type]->handlers) {
        free(abi_tables[type]);
        abi_tables[type] = nullptr;
        return false;
    }
    
    // Copy handlers
    for (uint32 i = 0; i < table->max_syscall_num; i++) {
        abi_tables[type]->handlers[i] = table->handlers[i];
    }
    
    // Allocate names if provided
    if (table->names) {
        abi_tables[type]->names = (const char**)malloc(table->max_syscall_num * sizeof(const char*));
        if (!abi_tables[type]->names) {
            free(abi_tables[type]->handlers);
            free(abi_tables[type]);
            abi_tables[type] = nullptr;
            return false;
        }
        
        // Copy names
        for (uint32 i = 0; i < table->max_syscall_num; i++) {
            abi_tables[type]->names[i] = table->names[i];
        }
    } else {
        abi_tables[type]->names = nullptr;
    }
    
    return true;
}

int AbiMultiplexer::DispatchSyscall(AbiType type, uint32 syscall_num, 
                                   uint32 arg1, uint32 arg2, uint32 arg3, 
                                   uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!initialized || type <= ABI_UNKNOWN || type >= MAX_ABI_TYPES) {
        LOG("ABI Multiplexer not initialized or invalid ABI type");
        return -1;
    }
    
    AbiSyscallTable* table = abi_tables[type];
    if (!table || !table->handlers) {
        LOG("No syscall table for ABI type: " << type);
        return -1;
    }
    
    if (syscall_num >= table->max_syscall_num) {
        LOG("Syscall number out of range for ABI type: " << type << ", num: " << syscall_num);
        return -1;
    }
    
    SyscallHandler handler = table->handlers[syscall_num];
    if (!handler) {
        LOG("Unimplemented syscall for ABI type: " << type << ", num: " << syscall_num);
        return -1;
    }
    
    // Call the appropriate handler
    int result = handler(arg1, arg2, arg3, arg4, arg5, arg6);
    
    // Log the syscall for debugging (optional)
    if (table->names && table->names[syscall_num]) {
        DLOG("ABI " << type << " syscall " << table->names[syscall_num] << " returned: " << result);
    } else {
        DLOG("ABI " << type << " syscall " << syscall_num << " returned: " << result);
    }
    
    return result;
}

AbiType AbiMultiplexer::GetCurrentProcessAbi() {
    if (!g_current_process) {
        return ABI_UNKNOWN;
    }
    return GetProcessAbi(g_current_process);
}

AbiType AbiMultiplexer::GetProcessAbi(ProcessControlBlock* pcb) {
    if (!pcb || !pcb->abi_context) {
        return ABI_UNKNOWN;
    }
    
    AbiContext* context = (AbiContext*)pcb->abi_context;
    return context->type;
}

bool AbiMultiplexer::SetProcessAbi(ProcessControlBlock* pcb, AbiType type) {
    if (!pcb) {
        return false;
    }
    
    // Create or update the ABI context
    if (!pcb->abi_context) {
        pcb->abi_context = (void*)CreateAbiContext(type);
        if (!pcb->abi_context) {
            return false;
        }
    } else {
        AbiContext* context = (AbiContext*)pcb->abi_context;
        context->type = type;
    }
    
    return true;
}

AbiContext* AbiMultiplexer::GetProcessAbiContext(ProcessControlBlock* pcb) {
    if (!pcb || !pcb->abi_context) {
        return nullptr;
    }
    
    return (AbiContext*)pcb->abi_context;
}

AbiContext* AbiMultiplexer::CreateAbiContext(AbiType type) {
    AbiContext* context = (AbiContext*)malloc(sizeof(AbiContext));
    if (!context) {
        return nullptr;
    }
    
    context->type = type;
    context->context_data = nullptr;
    context->abi_flags = 0;
    
    return context;
}

void AbiMultiplexer::DestroyAbiContext(AbiContext* context) {
    if (context) {
        // Free any ABI-specific data
        if (context->context_data) {
            free(context->context_data);
        }
        free(context);
    }
}

bool AbiMultiplexer::ConvertDosPathToUnix(const char* dos_path, char* unix_path, uint32 max_len) {
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

bool AbiMultiplexer::ConvertUnixPathToDos(const char* unix_path, char* dos_path, uint32 max_len) {
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

bool AbiMultiplexer::SetupDosDriveMappings() {
    // Set up default drive letter mappings in registry
    if (g_registry_manager) {
        RegistryWriteString("HKEY_LOCAL_MACHINE\\SYSTEM\\MountPoints", "A:", "/A", KEY_WRITE);
        RegistryWriteString("HKEY_LOCAL_MACHINE\\SYSTEM\\MountPoints", "C:", "/HardDisk", KEY_WRITE);
        LOG("DOS drive letter mappings registered");
    }
    
    return true;
}

ProcessControlBlock* AbiMultiplexer::LoadDosExecutable(const char* filename, char* const argv[], char* const envp[]) {
    LOG("Loading DOS executable: " << filename);
    
    // For now, delegate to the existing DOS executable loading
    if (g_dos_syscall_interface) {
        if (g_dos_syscall_interface->RunDosExecutable(filename, argv, envp)) {
            // Get the newly created process and set its ABI type
            ProcessControlBlock* new_process = process_manager->GetCurrentProcess();
            if (new_process) {
                SetProcessAbi(new_process, DOS_KPI_V1);
            }
            return new_process;
        }
    }
    
    LOG("Failed to load DOS executable: " << filename);
    return nullptr;
}

ProcessControlBlock* AbiMultiplexer::LoadLinuxExecutable(const char* filename, char* const argv[], char* const envp[]) {
    LOG("Loading Linux executable: " << filename);
    
    // For now, delegate to the Linuxulator
    if (g_linuxulator) {
        ProcessControlBlock* new_process = g_linuxulator->LoadLinuxExecutable(filename, argv, envp);
        if (new_process) {
            SetProcessAbi(new_process, LINUXULATOR);
        }
        return new_process;
    }
    
    LOG("Failed to load Linux executable: " << filename);
    return nullptr;
}

ProcessControlBlock* AbiMultiplexer::LoadNativeExecutable(const char* filename, char* const argv[], char* const envp[]) {
    LOG("Loading native executable: " << filename);
    
    // This would be for native LittleKernel executables
    // For now, return null - this would be implemented as the native ABI
    return nullptr;
}

ProcessControlBlock* AbiMultiplexer::LoadExecutable(const char* filename, char* const argv[], char* const envp[]) {
    // Determine executable type and load appropriately
    AbiType abi_type = DetectAbiTypeFromExecutable(filename);
    
    switch (abi_type) {
        case DOS_KPI_V1:
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

AbiType AbiMultiplexer::DetectAbiTypeFromExecutable(const char* filename) {
    // Determine the executable type by examining the file header
    // This is a simplified implementation
    
    // Check file extension as a simple heuristic
    const char* ext = strrchr(filename, '.');
    if (ext) {
        if (strcmp(ext, ".exe") == 0 || strcmp(ext, ".EXE") == 0) {
            // Could be DOS .exe, check more thoroughly in a real implementation
            return DOS_KPI_V1;
        } else if (strcmp(ext, ".com") == 0 || strcmp(ext, ".COM") == 0) {
            // DOS .com file
            return DOS_KPI_V1;
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
bool InitializeAbiMultiplexer() {
    if (!g_abi_multiplexer) {
        g_abi_multiplexer = new AbiMultiplexer();
        if (!g_abi_multiplexer) {
            LOG("Failed to create ABI multiplexer instance");
            return false;
        }
        
        if (!g_abi_multiplexer->Initialize()) {
            LOG("Failed to initialize ABI multiplexer");
            delete g_abi_multiplexer;
            g_abi_multiplexer = nullptr;
            return false;
        }
        
        LOG("ABI multiplexer initialized successfully");
    }
    
    return true;
}

extern "C" int HandleMultiplexedSyscall(uint32 syscall_num, 
                                       uint32 arg1, uint32 arg2, uint32 arg3, 
                                       uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_abi_multiplexer) {
        LOG("ABI multiplexer not initialized");
        return -1;
    }
    
    // Get current process ABI and dispatch accordingly
    AbiType abi_type = g_abi_multiplexer->GetCurrentProcessAbi();
    if (abi_type == ABI_UNKNOWN) {
        LOG("Unknown ABI type for current process");
        return -1;
    }
    
    return g_abi_multiplexer->DispatchSyscall(abi_type, syscall_num, arg1, arg2, arg3, arg4, arg5, arg6);
}

// ABI-specific initialization functions
bool InitializeDosKpiV1() {
    // This function would set up the DOS KPI v1 syscall table
    // For now, return true as it's already handled elsewhere
    LOG("DOS KPI v1 ABI initialized");
    return true;
}

bool InitializeDosKpiV2() {
    // Initialize the DOS KPI v2 interface
    if (!g_dos_kpi_v2_interface) {
        g_dos_kpi_v2_interface = new DosKpiV2Interface();
        if (!g_dos_kpi_v2_interface) {
            LOG("Failed to create DOS-KPIv2 interface instance");
            return false;
        }
        
        if (!g_dos_kpi_v2_interface->Initialize()) {
            LOG("Failed to initialize DOS-KPIv2 interface");
            delete g_dos_kpi_v2_interface;
            g_dos_kpi_v2_interface = nullptr;
            return false;
        }
    }
    
    // Setup the syscall table for DOS-KPIv2
    if (!SetupDosKpiV2SyscallTable()) {
        LOG("Failed to setup DOS-KPIv2 syscall table");
        return false;
    }
    
    LOG("DOS KPI v2 ABI initialized successfully");
    return true;
}

bool InitializeLinuxulatorAbi() {
    // Initialize the Linuxulator ABI interface
    if (!g_linuxulator_abi) {
        g_linuxulator_abi = new LinuxulatorAbi();
        if (!g_linuxulator_abi) {
            LOG("Failed to create Linuxulator ABI instance");
            return false;
        }
        
        if (!g_linuxulator_abi->Initialize()) {
            LOG("Failed to initialize Linuxulator ABI");
            delete g_linuxulator_abi;
            g_linuxulator_abi = nullptr;
            return false;
        }
    }
    
    // Setup the syscall table for Linuxulator ABI
    if (!SetupLinuxulatorAbiSyscallTable()) {
        LOG("Failed to setup Linuxulator ABI syscall table");
        return false;
    }
    
    LOG("Linuxulator ABI initialized successfully");
    return true;
}