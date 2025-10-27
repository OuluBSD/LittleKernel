#ifndef _Kernel_NvidiaSupport_h_
#define _Kernel_NvidiaSupport_h_

#include "Common.h"
#include "Defs.h"
#include "ProcessControlBlock.h"
#include "AbiMultiplexer.h"

// NVIDIA driver support context
struct NvidiaDriverContext {
    uint32 driver_version;          // NVIDIA driver version
    uint32 abi_flags;               // ABI-specific flags
    void* driver_module;              // Loaded NVIDIA driver module
    uint32 module_size;             // Size of driver module
    void* device_context;             // Device-specific context
    uint32 device_count;            // Number of NVIDIA devices
    void** device_handles;            // Array of device handles
};

// NVIDIA driver support interface class
class NvidiaDriverSupport {
private:
    NvidiaDriverContext global_context;  // Global context for NVIDIA driver support
    Spinlock nvidia_lock;                 // Lock for thread safety

public:
    NvidiaDriverSupport();
    ~NvidiaDriverSupport();
    
    // Initialize the NVIDIA driver support
    bool Initialize();
    
    // Load and initialize NVIDIA driver
    bool LoadNvidiaDriver(const char* driver_path);
    
    // Unload NVIDIA driver
    bool UnloadNvidiaDriver();
    
    // Detect NVIDIA hardware
    bool DetectNvidiaHardware();
    
    // Initialize NVIDIA hardware
    bool InitializeNvidiaHardware();
    
    // NVIDIA-specific syscalls
    int NvidiaSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, 
                      uint32 arg4, uint32 arg5, uint32 arg6);
    
    // NVIDIA driver initialization
    int NvidiaInit();
    
    // NVIDIA driver cleanup
    int NvidiaCleanup();
    
    // NVIDIA device enumeration
    int NvidiaEnumerateDevices();
    
    // NVIDIA device initialization
    int NvidiaInitializeDevice(int device_id);
    
    // NVIDIA device cleanup
    int NvidiaCleanupDevice(int device_id);
    
    // NVIDIA memory management
    int NvidiaAllocateMemory(size_t size, void** ptr);
    int NvidiaFreeMemory(void* ptr);
    int NvidiaMapMemory(void* host_ptr, size_t size, void** device_ptr);
    int NvidiaUnmapMemory(void* device_ptr);
    int NvidiaCopyHostToDevice(void* dst, const void* src, size_t size);
    int NvidiaCopyDeviceToHost(void* dst, const void* src, size_t size);
    int NvidiaCopyDeviceToDevice(void* dst, const void* src, size_t size);
    
    // NVIDIA graphics operations
    int NvidiaCreateContext(int device_id, void** context);
    int NvidiaDestroyContext(void* context);
    int NvidiaMakeContextCurrent(void* context);
    int NvidiaGetCurrentContext(void** context);
    int NvidiaSynchronizeContext();
    
    // NVIDIA compute operations
    int NvidiaCreateStream(void** stream);
    int NvidiaDestroyStream(void* stream);
    int NvidiaSynchronizeStream(void* stream);
    int NvidiaQueryStream(void* stream);
    int NvidiaRecordEvent(void* event, void* stream);
    int NvidiaSynchronizeEvent(void* event);
    int NvidiaQueryEvent(void* event);
    int NvidiaElapsedTime(float* ms, void* start, void* end);
    
    // NVIDIA kernel launch
    int NvidiaLaunchKernel(const void* func, unsigned int gridDimX, unsigned int gridDimY, 
                          unsigned int gridDimZ, unsigned int blockDimX, unsigned int blockDimY, 
                          unsigned int blockDimZ, unsigned int sharedMemBytes, void* stream, 
                          void** kernelParams, void** extra);
    
private:
    // Internal helper functions
    int DispatchNvidiaSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, 
                             uint32 arg4, uint32 arg5, uint32 arg6);
    bool ValidateNvidiaDevice(int device_id);
    bool ValidateNvidiaContext(void* context);
    bool ValidateNvidiaStream(void* stream);
    bool ValidateNvidiaEvent(void* event);
    bool ValidateNvidiaMemory(void* ptr);
};

// Global NVIDIA driver support instance
extern NvidiaDriverSupport* g_nvidia_driver_support;

// Initialize the NVIDIA driver support
bool InitializeNvidiaDriverSupport();

// Handle NVIDIA driver syscalls
extern "C" int HandleNvidiaSyscall(uint32 syscall_num, 
                                  uint32 arg1, uint32 arg2, uint32 arg3, 
                                  uint32 arg4, uint32 arg5, uint32 arg6);

// Setup NVIDIA driver syscall table for the ABI multiplexer
bool SetupNvidiaDriverSyscallTable();

#endif