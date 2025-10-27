#ifndef _Kernel_HAL_h_
#define _Kernel_HAL_h_

#include "Defs.h"

// Hardware Abstraction Layer interface definitions
// This layer provides an abstract interface to hardware components
// allowing the kernel to be more portable across different architectures

// Standard return codes for HAL functions
enum class HalResult {
    SUCCESS = 0,
    ERROR_INVALID_PARAMETER = -1,
    ERROR_NOT_SUPPORTED = -2,
    ERROR_NOT_INITIALIZED = -3,
    ERROR_RESOURCE_UNAVAILABLE = -4,
    ERROR_TIMEOUT = -5
};

// CPU architecture types
enum class CpuArchitecture {
    X86 = 0,
    X86_64 = 1,
    ARM = 2,
    ARM64 = 3,
    MIPS = 4,
    RISCV = 5
};

// HAL interface for CPU-specific operations
class CpuHal {
public:
    virtual ~CpuHal() = default;
    
    // Initialize CPU-specific features
    virtual HalResult Initialize() = 0;
    
    // Halt the CPU until next interrupt
    virtual void Halt() = 0;
    
    // Disable interrupts and return previous interrupt state
    virtual bool DisableInterrupts() = 0;
    
    // Enable interrupts
    virtual void EnableInterrupts() = 0;
    
    // Restore interrupt state
    virtual void RestoreInterrupts(bool state) = 0;
    
    // Get CPU architecture
    virtual CpuArchitecture GetArchitecture() const = 0;
    
    // Get CPU vendor string
    virtual const char* GetVendorString() = 0;
    
    // Get CPU features
    virtual uint64_t GetFeatures() = 0;
    
    // I/O port operations
    virtual uint8 InByte(uint16_t port) = 0;
    virtual uint16_t InWord(uint16_t port) = 0;
    virtual uint32 InDWord(uint16_t port) = 0;
    virtual void OutByte(uint16_t port, uint8 value) = 0;
    virtual void OutWord(uint16_t port, uint16_t value) = 0;
    virtual void OutDWord(uint16_t port, uint32 value) = 0;
    
    // CPU-specific memory operations
    virtual void MemoryBarrier() = 0;
    virtual void InvalidateTlb() = 0;
};

// HAL interface for memory management
class MemoryHal {
public:
    virtual ~MemoryHal() = default;
    
    // Initialize memory management
    virtual HalResult Initialize() = 0;
    
    // Get physical memory size
    virtual uint64_t GetPhysicalMemorySize() = 0;
    
    // Get available physical memory
    virtual uint64_t GetAvailableMemory() = 0;
    
    // Allocate physical pages
    virtual void* AllocatePages(uint32 count) = 0;
    
    // Free physical pages
    virtual void FreePages(void* addr, uint32 count) = 0;
    
    // Map physical memory to virtual
    virtual void* MapPhysicalMemory(uint32 physical_addr, uint32 size) = 0;
    
    // Unmap virtual memory
    virtual void UnmapVirtualMemory(void* virtual_addr) = 0;
    
    // Get page size
    virtual uint32 GetPageSize() = 0;
};

// HAL interface for interrupt management
class InterruptHal {
public:
    virtual ~InterruptHal() = default;
    
    // Initialize interrupt management
    virtual HalResult Initialize() = 0;
    
    // Register an interrupt handler
    virtual HalResult RegisterHandler(uint8 irq, void (*handler)(void*)) = 0;
    
    // Unregister an interrupt handler
    virtual HalResult UnregisterHandler(uint8 irq) = 0;
    
    // Enable an interrupt
    virtual HalResult EnableInterrupt(uint8 irq) = 0;
    
    // Disable an interrupt
    virtual HalResult DisableInterrupt(uint8 irq) = 0;
    
    // Get interrupt status
    virtual bool IsInterruptEnabled(uint8 irq) = 0;
    
    // Send End of Interrupt signal
    virtual void EndOfInterrupt(uint8 irq) = 0;
    
    // Get interrupt controller type
    virtual const char* GetControllerType() = 0;
};

// HAL interface for timer operations
class TimerHal {
public:
    virtual ~TimerHal() = default;
    
    // Initialize timer hardware
    virtual HalResult Initialize() = 0;
    
    // Set timer frequency
    virtual HalResult SetFrequency(uint32 hz) = 0;
    
    // Get current timer frequency
    virtual uint32 GetFrequency() = 0;
    
    // Get timer tick count
    virtual uint64_t GetTickCount() = 0;
    
    // Get high-resolution time
    virtual uint64_t GetHighResolutionTime() = 0;
    
    // Sleep for specified milliseconds
    virtual void Sleep(uint32 milliseconds) = 0;
    
    // Register timer interrupt handler
    virtual HalResult RegisterHandler(void (*handler)()) = 0;
};

// HAL interface for PCI bus operations
class PciHal {
public:
    virtual ~PciHal() = default;
    
    // Initialize PCI bus
    virtual HalResult Initialize() = 0;
    
    // Read PCI configuration space
    virtual uint32 ReadConfig(uint8 bus, uint8 device, uint8 function, uint8 offset) = 0;
    
    // Write PCI configuration space
    virtual void WriteConfig(uint8 bus, uint8 device, uint8 function, uint8 offset, uint32 value) = 0;
    
    // Find PCI device by vendor and device ID
    virtual HalResult FindDevice(uint16_t vendor_id, uint16_t device_id, uint8* bus, uint8* device, uint8* function) = 0;
    
    // Enumerate all PCI devices
    virtual uint32 EnumerateDevices() = 0;
};

// Main HAL manager that provides access to all HAL interfaces
class HalManager {
private:
    CpuHal* cpu_hal;
    MemoryHal* memory_hal;
    InterruptHal* interrupt_hal;
    TimerHal* timer_hal;
    PciHal* pci_hal;
    
public:
    HalManager();
    ~HalManager();
    
    HalResult Initialize();
    
    // Get HAL interfaces
    CpuHal* GetCpuHal() const { return cpu_hal; }
    MemoryHal* GetMemoryHal() const { return memory_hal; }
    InterruptHal* GetInterruptHal() const { return interrupt_hal; }
    TimerHal* GetTimerHal() const { return timer_hal; }
    PciHal* GetPciHal() const { return pci_hal; }
};

// Global HAL manager instance
extern HalManager* g_hal_manager;

// Helper macros for common HAL operations
#define HAL_CPU() (g_hal_manager->GetCpuHal())
#define HAL_MEMORY() (g_hal_manager->GetMemoryHal())
#define HAL_INTERRUPT() (g_hal_manager->GetInterruptHal())
#define HAL_TIMER() (g_hal_manager->GetTimerHal())
#define HAL_PCI() (g_hal_manager->GetPciHal())

#endif // _Kernel_HAL_h_