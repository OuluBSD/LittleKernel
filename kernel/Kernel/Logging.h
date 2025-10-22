#ifndef _Kernel_Logging_h_
#define _Kernel_Logging_h_

// Don't include other headers in this file - only the package header should include other headers

// Forward declarations
class Monitor;

// Logging macros
#define LOG(message) _log_message(__FILE__, __LINE__, message)
#define DLOG(message) _debug_log_message(__FILE__, __LINE__, message)

// Function prototypes
void _log_message(const char* file, int line, const char* message);
void _debug_log_message(const char* file, int line, const char* message);
void InitializeSerial();

// For stream-like syntax support
class LogStream {
private:
    char buffer[512];
    uint32 pos;
    
    void AppendString(const char* str);
    void AppendInt(int32 val);
    void AppendUint32(uint32 val);
    void AppendHex(uint32 val);
    void AppendChar(char c);
    
public:
    LogStream();
    LogStream& operator<<(const char* str);
    LogStream& operator<<(int32 val);
    LogStream& operator<<(uint32 val);
    LogStream& operator<<(char c);
    void Flush(const char* prefix);
};

#define LOG_STREAM(message) do { \
    LogStream ls; \
    ls << message; \
    ls.Flush("[LOG]"); \
} while(0)

#endif