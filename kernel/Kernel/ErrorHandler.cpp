#include "ErrorHandler.h"
#include "Kernel.h"

// Global error handler instance
ErrorHandlerManager* g_error_handler = nullptr;

ErrorHandlerManager::ErrorHandlerManager() 
    : handler_count(0), history_count(0), history_index(0), error_recovery_enabled(true) {
    // Initialize arrays to zero
    memset(handlers, 0, sizeof(handlers));
    memset(error_history, 0, sizeof(error_history));
}

ErrorHandlerManager::~ErrorHandlerManager() {
    // Clean up if needed
}

bool ErrorHandlerManager::Initialize() {
    handler_count = 0;
    history_count = 0;
    history_index = 0;
    error_recovery_enabled = true;
    
    LOG("Error handling system initialized");
    return true;
}

bool ErrorHandlerManager::RegisterErrorHandler(ErrorHandler handler) {
    if (!handler || handler_count >= MAX_ERROR_HANDLERS) {
        return false;
    }
    
    // Check if handler is already registered
    for (uint32_t i = 0; i < handler_count; i++) {
        if (handlers[i] == handler) {
            return true; // Already registered
        }
    }
    
    // Add the new handler
    handlers[handler_count] = handler;
    handler_count++;
    
    LOG("Error handler registered");
    return true;
}

bool ErrorHandlerManager::UnregisterErrorHandler(ErrorHandler handler) {
    for (uint32_t i = 0; i < handler_count; i++) {
        if (handlers[i] == handler) {
            // Shift remaining handlers down
            for (uint32_t j = i; j < handler_count - 1; j++) {
                handlers[j] = handlers[j + 1];
            }
            handler_count--;
            LOG("Error handler unregistered");
            return true;
        }
    }
    return false; // Handler not found
}

void ErrorHandlerManager::ReportError(KernelError error, const char* context, const char* file, uint32_t line) {
    ReportErrorWithInfo(error, context, file, line, nullptr);
}

void ErrorHandlerManager::ReportErrorWithInfo(KernelError error, const char* context, const char* file, 
                                             uint32_t line, void* additional_info) {
    // Create error information
    ErrorInfo& info = error_history[history_index];
    info.error_code = error;
    info.context = context ? context : "Unknown";
    info.description = GetErrorDescription(error);
    info.line_number = line;
    info.file_name = file ? file : "Unknown";
    info.timestamp = global_timer ? global_timer->GetTickCount() : 0;
    info.additional_info = additional_info;
    
    // Update history tracking
    history_index = (history_index + 1) % MAX_ERROR_HISTORY;
    if (history_count < MAX_ERROR_HISTORY) {
        history_count++;
    }
    
    // Log the error
    LOG("ERROR: [" << GetErrorName(error) << "] " << info.context 
        << " at " << info.file_name << ":" << info.line_number
        << " - " << info.description);
    
    // Call registered error handlers
    for (uint32_t i = 0; i < handler_count; i++) {
        if (handlers[i]) {
            handlers[i](context, error, additional_info);
        }
    }
    
    // For critical errors, take special action
    if (error == KernelError::ERROR_OUT_OF_MEMORY || 
        error == KernelError::ERROR_HARDWARE_FAILURE ||
        error == KernelError::ERROR_MEMORY_CORRUPTED) {
        HandleCriticalError(error, context);
    }
}

const char* ErrorHandlerManager::GetErrorDescription(KernelError error) {
    switch (error) {
        case KernelError::SUCCESS:
            return "Success";
        case KernelError::ERROR_GENERAL:
            return "General error";
        case KernelError::ERROR_INVALID_PARAMETER:
            return "Invalid parameter";
        case KernelError::ERROR_OUT_OF_MEMORY:
            return "Out of memory";
        case KernelError::ERROR_NOT_IMPLEMENTED:
            return "Not implemented";
        case KernelError::ERROR_NOT_SUPPORTED:
            return "Not supported";
        case KernelError::ERROR_ACCESS_DENIED:
            return "Access denied";
        case KernelError::ERROR_FILE_NOT_FOUND:
            return "File not found";
        case KernelError::ERROR_DEVICE_ERROR:
            return "Device error";
        case KernelError::ERROR_TIMEOUT:
            return "Timeout";
        case KernelError::ERROR_ALREADY_EXISTS:
            return "Already exists";
        case KernelError::ERROR_NOT_INITIALIZED:
            return "Not initialized";
        case KernelError::ERROR_BUFFER_TOO_SMALL:
            return "Buffer too small";
        case KernelError::ERROR_NO_MORE_ENTRIES:
            return "No more entries";
        case KernelError::ERROR_INVALID_PROCESS:
            return "Invalid process";
        case KernelError::ERROR_PROCESS_LIMIT_EXCEEDED:
            return "Process limit exceeded";
        case KernelError::ERROR_PROCESS_NOT_FOUND:
            return "Process not found";
        case KernelError::ERROR_INVALID_ADDRESS:
            return "Invalid address";
        case KernelError::ERROR_PAGE_FAULT:
            return "Page fault";
        case KernelError::ERROR_MEMORY_CORRUPTED:
            return "Memory corrupted";
        case KernelError::ERROR_STACK_OVERFLOW:
            return "Stack overflow";
        case KernelError::ERROR_HARDWARE_FAILURE:
            return "Hardware failure";
        case KernelError::ERROR_INVALID_DEVICE:
            return "Invalid device";
        case KernelError::ERROR_DEVICE_BUSY:
            return "Device busy";
        case KernelError::ERROR_IRQ_FAILURE:
            return "IRQ failure";
        case KernelError::ERROR_FS_CORRUPTED:
            return "File system corrupted";
        case KernelError::ERROR_FS_FULL:
            return "File system full";
        case KernelError::ERROR_FS_ACCESS_DENIED:
            return "File system access denied";
        case KernelError::ERROR_FS_INVALID_PATH:
            return "File system invalid path";
        case KernelError::ERROR_NETWORK_UNREACHABLE:
            return "Network unreachable";
        case KernelError::ERROR_CONNECTION_REFUSED:
            return "Connection refused";
        case KernelError::ERROR_NETWORK_TIMEOUT:
            return "Network timeout";
        default:
            return "Unknown error";
    }
}

const char* ErrorHandlerManager::GetErrorName(KernelError error) {
    switch (error) {
        case KernelError::SUCCESS:
            return "SUCCESS";
        case KernelError::ERROR_GENERAL:
            return "ERROR_GENERAL";
        case KernelError::ERROR_INVALID_PARAMETER:
            return "ERROR_INVALID_PARAMETER";
        case KernelError::ERROR_OUT_OF_MEMORY:
            return "ERROR_OUT_OF_MEMORY";
        case KernelError::ERROR_NOT_IMPLEMENTED:
            return "ERROR_NOT_IMPLEMENTED";
        case KernelError::ERROR_NOT_SUPPORTED:
            return "ERROR_NOT_SUPPORTED";
        case KernelError::ERROR_ACCESS_DENIED:
            return "ERROR_ACCESS_DENIED";
        case KernelError::ERROR_FILE_NOT_FOUND:
            return "ERROR_FILE_NOT_FOUND";
        case KernelError::ERROR_DEVICE_ERROR:
            return "ERROR_DEVICE_ERROR";
        case KernelError::ERROR_TIMEOUT:
            return "ERROR_TIMEOUT";
        case KernelError::ERROR_ALREADY_EXISTS:
            return "ERROR_ALREADY_EXISTS";
        case KernelError::ERROR_NOT_INITIALIZED:
            return "ERROR_NOT_INITIALIZED";
        case KernelError::ERROR_BUFFER_TOO_SMALL:
            return "ERROR_BUFFER_TOO_SMALL";
        case KernelError::ERROR_NO_MORE_ENTRIES:
            return "ERROR_NO_MORE_ENTRIES";
        case KernelError::ERROR_INVALID_PROCESS:
            return "ERROR_INVALID_PROCESS";
        case KernelError::ERROR_PROCESS_LIMIT_EXCEEDED:
            return "ERROR_PROCESS_LIMIT_EXCEEDED";
        case KernelError::ERROR_PROCESS_NOT_FOUND:
            return "ERROR_PROCESS_NOT_FOUND";
        case KernelError::ERROR_INVALID_ADDRESS:
            return "ERROR_INVALID_ADDRESS";
        case KernelError::ERROR_PAGE_FAULT:
            return "ERROR_PAGE_FAULT";
        case KernelError::ERROR_MEMORY_CORRUPTED:
            return "ERROR_MEMORY_CORRUPTED";
        case KernelError::ERROR_STACK_OVERFLOW:
            return "ERROR_STACK_OVERFLOW";
        case KernelError::ERROR_HARDWARE_FAILURE:
            return "ERROR_HARDWARE_FAILURE";
        case KernelError::ERROR_INVALID_DEVICE:
            return "ERROR_INVALID_DEVICE";
        case KernelError::ERROR_DEVICE_BUSY:
            return "ERROR_DEVICE_BUSY";
        case KernelError::ERROR_IRQ_FAILURE:
            return "ERROR_IRQ_FAILURE";
        case KernelError::ERROR_FS_CORRUPTED:
            return "ERROR_FS_CORRUPTED";
        case KernelError::ERROR_FS_FULL:
            return "ERROR_FS_FULL";
        case KernelError::ERROR_FS_ACCESS_DENIED:
            return "ERROR_FS_ACCESS_DENIED";
        case KernelError::ERROR_FS_INVALID_PATH:
            return "ERROR_FS_INVALID_PATH";
        case KernelError::ERROR_NETWORK_UNREACHABLE:
            return "ERROR_NETWORK_UNREACHABLE";
        case KernelError::ERROR_CONNECTION_REFUSED:
            return "ERROR_CONNECTION_REFUSED";
        case KernelError::ERROR_NETWORK_TIMEOUT:
            return "ERROR_NETWORK_TIMEOUT";
        default:
            return "UNKNOWN_ERROR";
    }
}

bool ErrorHandlerManager::IsSuccess(KernelError error) {
    return (int)error >= 0;
}

bool ErrorHandlerManager::IsError(KernelError error) {
    return (int)error < 0;
}

void ErrorHandlerManager::SetRecoveryEnabled(bool enabled) {
    error_recovery_enabled = enabled;
}

bool ErrorHandlerManager::IsRecoveryEnabled() const {
    return error_recovery_enabled;
}

const ErrorInfo* ErrorHandlerManager::GetErrorHistory(uint32_t* count) {
    *count = history_count;
    return error_history;
}

void ErrorHandlerManager::ClearErrorHistory() {
    history_count = 0;
    history_index = 0;
    memset(error_history, 0, sizeof(error_history));
    LOG("Error history cleared");
}

void ErrorHandlerManager::HandleCriticalError(KernelError error, const char* context) {
    LOG("CRITICAL ERROR: " << GetErrorName(error) << " in " << context);
    
    // For certain critical errors, we might want to halt the system
    switch (error) {
        case KernelError::ERROR_OUT_OF_MEMORY:
            LOG("CRITICAL: Out of memory - attempting emergency recovery");
            // In a real implementation, we might try to free up critical memory
            break;
        case KernelError::ERROR_MEMORY_CORRUPTED:
            LOG("CRITICAL: Memory corruption detected - system integrity compromised");
            // Could attempt to enter safe mode or halt
            break;
        case KernelError::ERROR_HARDWARE_FAILURE:
            LOG("CRITICAL: Hardware failure - system may be unstable");
            // Log the error but continue operation if possible
            break;
        default:
            LOG("CRITICAL: Unhandled critical error occurred");
            break;
    }
    
    // In a real implementation, we might trigger a kernel panic or safe mode here
    // For now, we'll just log and continue
}

bool ErrorHandlerManager::AttemptRecovery(KernelError error) {
    if (!error_recovery_enabled) {
        return false;
    }
    
    switch (error) {
        case KernelError::ERROR_DEVICE_BUSY:
            // Wait and retry after a delay
            if (global_timer) {
                global_timer->Sleep(10); // Wait 10ms
            }
            return true;
            
        case KernelError::ERROR_TIMEOUT:
            // Try with different parameters or longer timeout
            return true;
            
        case KernelError::ERROR_OUT_OF_MEMORY:
            // Try to free up some non-critical memory
            // This is a simplified approach
            return true;
            
        case KernelError::ERROR_NOT_INITIALIZED:
            // Try to initialize the component
            return true;
            
        default:
            // For most errors, we cannot automatically recover
            return false;
    }
}

// Initialize error handling framework
bool InitializeErrorHandling() {
    g_error_handler = new ErrorHandlerManager();
    if (!g_error_handler) {
        LOG("Error: Failed to allocate error handler manager");
        return false;
    }
    
    if (!g_error_handler->Initialize()) {
        LOG("Error: Failed to initialize error handler manager");
        delete g_error_handler;
        g_error_handler = nullptr;
        return false;
    }
    
    LOG("Error handling framework initialized successfully");
    return true;
}

// Convert a kernel error to a standard integer code
int32_t KernelErrorToInt(KernelError error) {
    return (int32_t)error;
}

// Create a kernel error from an integer code
KernelError IntToKernelError(int32_t code) {
    return (KernelError)code;
}