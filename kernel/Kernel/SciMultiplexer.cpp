#include "Kernel.h"
#include "SciMultiplexer.h"
#include "Logging.h"
#include "Vfs.h"
#include "ProcessControlBlock.h"
#include "DosSyscalls.h"
#include "DosKpiV2.h"
#include "LinuxulatorAbi.h"

// Global instance of the SCI multiplexer
SciMultiplexer* g_sci_multiplexer = nullptr;

SciMultiplexer::SciMultiplexer() {
    for (int i = 0; i < MAX_SCI_TYPES; i++) {
        sci_tables[i] = nullptr;
    }
    initialized = false;
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
    
    // Initialize all SCI tables to null
    for (int i = 0; i < MAX_SCI_TYPES; i++) {
        sci_tables[i] = nullptr;
    }
    
    // Initialize default tables for known SCIs
    if (!InitializeDosSciV1()) {
        LOG("Failed to initialize DOS SCI v1");
    }
    
    if (!InitializeDosSciV2()) {
        LOG("Failed to initialize DOS SCI v2");
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
    
    // Register the new table
    sci_tables[type] = table;
    return true;
}

int SciMultiplexer::DispatchSyscall(SciType type, uint32 syscall_num, 
                                   uint32 arg1, uint32 arg2, uint32 arg3, 
                                   uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!initialized || type <= SCI_UNKNOWN || type >= MAX_SCI_TYPES) {
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
    if (!pcb) {
        return SCI_UNKNOWN;
    }
    
    // Get the ABI context from the process and extract the type
    AbiContext* abi_context = (AbiContext*)pcb->abi_context;
    if (!abi_context) {
        return SCI_UNKNOWN;
    }
    
    // Convert the ABI context to SCI type
    switch (abi_context->type) {
        case DOS_KPI_V1:
            return DOS_SCI_V1;
        case DOS_KPI_V2:
            return DOS_SCI_V2;
        case LINUXULATOR:
            return LINUXULATOR;
        case NATIVE:
            return NATIVE;
        default:
            return SCI_UNKNOWN;
    }
}

bool SciMultiplexer::SetProcessSci(ProcessControlBlock* pcb, SciType type) {
    if (!pcb) {
        return false;
    }
    
    // Create or update the ABI context
    AbiContext* abi_context = (AbiContext*)pcb->abi_context;
    if (!abi_context) {
        abi_context = (AbiContext*)malloc(sizeof(AbiContext));
        if (!abi_context) {
            return false;
        }
        memset(abi_context, 0, sizeof(AbiContext));
        pcb->abi_context = (void*)abi_context;
    }
    
    // Convert SCI type to ABI type
    switch (type) {
        case DOS_SCI_V1:
            abi_context->type = DOS_KPI_V1;
            break;
        case DOS_SCI_V2:
            abi_context->type = DOS_KPI_V2;
            break;
        case LINUXULATOR:
            abi_context->type = LINUXULATOR;
            break;
        case NATIVE:
            abi_context->type = NATIVE;
            break;
        default:
            abi_context->type = ABI_UNKNOWN;
            break;
    }
    
    return true;
}

SciContext* SciMultiplexer::GetProcessSciContext(ProcessControlBlock* pcb) {
    if (!pcb || !pcb->abi_context) {
        return nullptr;
    }
    
    // Cast the ABI context to SCI context (they're compatible for now)
    return (SciContext*)pcb->abi_context;
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

// Initialize the SCI multiplexer
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

// Main syscall dispatcher function
extern "C" int HandleMultiplexedSyscall(uint32 syscall_num, 
                                       uint32 arg1, uint32 arg2, uint32 arg3, 
                                       uint32 arg4, uint32 arg5, uint32 arg6) {
    if (!g_sci_multiplexer) {
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

// SCI-specific initialization functions
bool InitializeDosSciV1() {
    // This function prepares the DOS SCI v1 (interrupt-based)
    LOG("DOS SCI v1 prepared");
    return true;
}

bool InitializeDosSciV2() {
    // This function prepares the DOS SCI v2 (syscall instruction-based)
    LOG("DOS SCI v2 prepared");
    return true;
}

bool InitializeLinuxulatorSci() {
    // This function integrates with the Linuxulator
    LOG("Linuxulator SCI prepared");
    return true;
}