#ifndef _Kernel_ErrorHandling_h_
#define _Kernel_ErrorHandling_h_

#include "Defs.h"

// Standard error codes for kernel operations
enum class KernelError {
    // Success codes
    SUCCESS = 0,
    
    // Generic errors
    ERROR_GENERAL = -1,
    ERROR_INVALID_PARAMETER = -2,
    ERROR_OUT_OF_MEMORY = -3,
    ERROR_NOT_IMPLEMENTED = -4,
    ERROR_NOT_SUPPORTED = -5,
    ERROR_ACCESS_DENIED = -6,
    ERROR_FILE_NOT_FOUND = -7,
    ERROR_DEVICE_ERROR = -8,
    ERROR_TIMEOUT = -9,
    ERROR_ALREADY_EXISTS = -10,
    ERROR_NOT_INITIALIZED = -11,
    ERROR_BUFFER_TOO_SMALL = -12,
    ERROR_NO_MORE_ENTRIES = -13,
    
    // Process-related errors
    ERROR_INVALID_PROCESS = -100,
    ERROR_PROCESS_LIMIT_EXCEEDED = -101,
    ERROR_PROCESS_NOT_FOUND = -102,
    
    // Memory-related errors
    ERROR_INVALID_ADDRESS = -200,
    ERROR_PAGE_FAULT = -201,
    ERROR_MEMORY_CORRUPTED = -202,
    ERROR_STACK_OVERFLOW = -203,
    
    // Hardware-related errors
    ERROR_HARDWARE_FAILURE = -300,
    ERROR_INVALID_DEVICE = -301,
    ERROR_DEVICE_BUSY = -302,
    ERROR_IRQ_FAILURE = -303,
    
    // File system errors
    ERROR_FS_CORRUPTED = -400,
    ERROR_FS_FULL = -401,
    ERROR_FS_ACCESS_DENIED = -402,
    ERROR_FS_INVALID_PATH = -403,
    
    // Network errors
    ERROR_NETWORK_UNREACHABLE = -500,
    ERROR_CONNECTION_REFUSED = -501,
    ERROR_NETWORK_TIMEOUT = -502
};

// Error handler function type
typedef void (*ErrorHandler)(const char* context, KernelError error, void* additional_info);

// Error information structure
struct ErrorInfo {
    KernelError error_code;
    const char* context;
    const char* description;
    uint32_t line_number;
    const char* file_name;
    uint32_t timestamp;
    void* additional_info;
};

// Error handling manager
class ErrorHandlerManager {
private:
    static const uint32_t MAX_ERROR_HANDLERS = 16;
    static const uint32_t MAX_ERROR_HISTORY = 64;
    
    ErrorHandler handlers[MAX_ERROR_HANDLERS];
    uint32_t handler_count;
    
    ErrorInfo error_history[MAX_ERROR_HISTORY];
    uint32_t history_count;
    uint32_t history_index;
    
    bool error_recovery_enabled;
    
public:
    ErrorHandlerManager();
    ~ErrorHandlerManager();
    
    // Initialize the error handling system
    bool Initialize();
    
    // Register an error handler
    bool RegisterErrorHandler(ErrorHandler handler);
    
    // Unregister an error handler
    bool UnregisterErrorHandler(ErrorHandler handler);
    
    // Report an error
    void ReportError(KernelError error, const char* context, const char* file, uint32_t line);
    
    // Report an error with additional info
    void ReportErrorWithInfo(KernelError error, const char* context, const char* file, 
                            uint32_t line, void* additional_info);
    
    // Get error description as string
    const char* GetErrorDescription(KernelError error);
    
    // Get error name as string
    const char* GetErrorName(KernelError error);
    
    // Check if an error is a success code
    static bool IsSuccess(KernelError error);
    
    // Check if an error is a failure code
    static bool IsError(KernelError error);
    
    // Enable/disable error recovery
    void SetRecoveryEnabled(bool enabled);
    bool IsRecoveryEnabled() const;
    
    // Get error history
    const ErrorInfo* GetErrorHistory(uint32_t* count);
    
    // Clear error history
    void ClearErrorHistory();
    
    // Handle critical errors that might require system halt
    void HandleCriticalError(KernelError error, const char* context);
    
    // Attempt to recover from an error
    bool AttemptRecovery(KernelError error);
};

// Macro for reporting errors with file and line information
#define REPORT_ERROR(error_code, context) \
    g_error_handler->ReportError(error_code, context, __FILE__, __LINE__)

#define REPORT_ERROR_INFO(error_code, context, info) \
    g_error_handler->ReportErrorWithInfo(error_code, context, __FILE__, __LINE__, info)

// Error handling helper macros
#define CHECK_AND_RETURN(expr, error_code, context) \
    do { \
        if (!(expr)) { \
            REPORT_ERROR(error_code, context); \
            return error_code; \
        } \
    } while(0)

#define CHECK_AND_RETURN_VAL(expr, error_code, context, return_val) \
    do { \
        if (!(expr)) { \
            REPORT_ERROR(error_code, context); \
            return return_val; \
        } \
    } while(0)

// Global error handler instance
extern ErrorHandlerManager* g_error_handler;

// Initialize error handling framework
bool InitializeErrorHandling();

// Convert a kernel error to a standard integer code
int32_t KernelErrorToInt(KernelError error);

// Create a kernel error from an integer code
KernelError IntToKernelError(int32_t code);

#endif // _Kernel_ErrorHandling_h_