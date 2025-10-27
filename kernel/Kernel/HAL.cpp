#include "HAL.h"
#include "Kernel.h"

// Global HAL manager instance
HalManager* g_hal_manager = nullptr;

// Implementation for x86 architecture
#ifdef ARCH_X86

// x86-specific CPU HAL implementation
class X86CpuHal : public CpuHal {
private:
    CpuArchitecture arch;
    char vendor_string[13];  // 12 chars + null terminator

public:
    X86CpuHal() : arch(CpuArchitecture::X86) {
        // Initialize vendor string to empty
        for (int i = 0; i < 13; i++) {
            vendor_string[i] = 0;
        }
        // Detect CPU vendor
        DetectCpuVendor();
    }

    virtual HalResult Initialize() override {
        // Initialize x86-specific features
        // For now, just return success
        return HalResult::SUCCESS;
    }

    virtual void Halt() override {
        asm volatile("hlt");
    }

    virtual bool DisableInterrupts() override {
        bool old_state;
        asm volatile("pushf; pop %0; cli" : "=r"(old_state) :: "memory");
        return old_state & 0x200;  // Check IF flag
    }

    virtual void EnableInterrupts() override {
        asm volatile("sti" ::: "memory");
    }

    virtual void RestoreInterrupts(bool state) override {
        if (state) {
            EnableInterrupts();
        } else {
            asm volatile("cli" ::: "memory");
        }
    }

    virtual CpuArchitecture GetArchitecture() const override {
        return arch;
    }

    virtual const char* GetVendorString() override {
        return vendor_string;
    }

    virtual uint64_t GetFeatures() override {
        // Implement CPU feature detection
        // For now, return a placeholder
        return 0;
    }

    virtual uint8 InByte(uint16_t port) override {
        uint8 result;
        asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
        return result;
    }

    virtual uint16_t InWord(uint16_t port) override {
        uint16_t result;
        asm volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
        return result;
    }

    virtual uint32 InDWord(uint16_t port) override {
        uint32 result;
        asm volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
        return result;
    }

    virtual void OutByte(uint16_t port, uint8 value) override {
        asm volatile("outb %0, %1" :: "a"(value), "Nd"(port));
    }

    virtual void OutWord(uint16_t port, uint16_t value) override {
        asm volatile("outw %0, %1" :: "a"(value), "Nd"(port));
    }

    virtual void OutDWord(uint16_t port, uint32 value) override {
        asm volatile("outl %0, %1" :: "a"(value), "Nd"(port));
    }

    virtual void MemoryBarrier() override {
        asm volatile("mfence" ::: "memory");
    }

    virtual void InvalidateTlb() override {
        // For x86, invalidate TLB by reloading CR3
        uint32 cr3_value;
        asm volatile("mov %%cr3, %0" : "=r"(cr3_value));
        asm volatile("mov %0, %%cr3" :: "r"(cr3_value) : "memory");
    }

private:
    void DetectCpuVendor() {
        uint32 ebx, edx, ecx;
        
        // Execute CPUID with eax=0 to get vendor string
        asm volatile("cpuid" 
                     : "=b"(ebx), "=c"(ecx), "=d"(edx) 
                     : "a"(0)
                     : "memory");
        
        // The vendor string is returned in ebx, edx, ecx
        // Store in vendor_string in the correct order
        *(uint32*)(&vendor_string[0]) = ebx;
        *(uint32*)(&vendor_string[4]) = edx;
        *(uint32*)(&vendor_string[8]) = ecx;
        vendor_string[12] = 0;  // Ensure null termination
    }
};

// x86-specific Memory HAL implementation
class X86MemoryHal : public MemoryHal {
public:
    virtual HalResult Initialize() override {
        return HalResult::SUCCESS;
    }

    virtual uint64_t GetPhysicalMemorySize() override {
        // For now, return a fixed value
        // In a real implementation, this would be determined from multiboot info
        if (g_kernel_config) {
            return g_kernel_config->max_virtual_memory_per_process;
        }
        return 512 * 1024 * 1024;  // 512MB default
    }

    virtual uint64_t GetAvailableMemory() override {
        // For now, return a fixed value
        return GetPhysicalMemorySize();  // Simplified
    }

    virtual void* AllocatePages(uint32 count) override {
        // Use kernel memory manager for page allocation
        // This is a simplified implementation
        if (global && global->memory_manager) {
            return global->memory_manager->AllocatePages(count);
        }
        return nullptr;
    }

    virtual void FreePages(void* addr, uint32 count) override {
        // Use kernel memory manager for page deallocation
        if (global && global->memory_manager) {
            global->memory_manager->FreePages(addr, count);
        }
    }

    virtual void* MapPhysicalMemory(uint32 physical_addr, uint32 size) override {
        // Use paging manager to map physical memory
        if (global && global->paging_manager) {
            return global->paging_manager->MapPhysicalMemory(physical_addr, size);
        }
        return nullptr;
    }

    virtual void UnmapVirtualMemory(void* virtual_addr) override {
        // Use paging manager to unmap virtual memory
        if (global && global->paging_manager) {
            global->paging_manager->UnmapVirtualMemory(virtual_addr);
        }
    }

    virtual uint32 GetPageSize() override {
        return 4096;  // Standard x86 page size
    }
};

// x86-specific Interrupt HAL implementation
class X86InterruptHal : public InterruptHal {
public:
    virtual HalResult Initialize() override {
        return HalResult::SUCCESS;
    }

    virtual HalResult RegisterHandler(uint8 irq, void (*handler)(void*)) override {
        // Use the existing interrupt manager
        if (global && global->descriptor_table) {
            // Note: This is a simplified mapping - in reality, you'd need to convert 
            // the IRQ to the appropriate interrupt number based on your interrupt controller
            global->descriptor_table->interrupt_manager.SetHandler(32 + irq, [handler]() {
                handler(nullptr);  // Call the handler with null context for now
            });
            return HalResult::SUCCESS;
        }
        return HalResult::ERROR_NOT_INITIALIZED;
    }

    virtual HalResult UnregisterHandler(uint8 irq) override {
        // Implementation would go here
        return HalResult::SUCCESS;
    }

    virtual HalResult EnableInterrupt(uint8 irq) override {
        // For x86 with PIC, this would involve enabling the IRQ in the PIC
        return HalResult::SUCCESS;
    }

    virtual HalResult DisableInterrupt(uint8 irq) override {
        // For x86 with PIC, this would involve disabling the IRQ in the PIC
        return HalResult::SUCCESS;
    }

    virtual bool IsInterruptEnabled(uint8 irq) override {
        // Implementation would go here
        return true;
    }

    virtual void EndOfInterrupt(uint8 irq) override {
        // Send EOI to PIC
        if (irq >= 8) {
            OutByte(0xA0, 0x20);  // Send EOI to slave PIC
        }
        OutByte(0x20, 0x20);      // Send EOI to master PIC
    }

    virtual const char* GetControllerType() override {
        return "8259 PIC";
    }

private:
    void OutByte(uint16_t port, uint8 value) {
        asm volatile("outb %0, %1" :: "a"(value), "Nd"(port));
    }
};

// x86-specific Timer HAL implementation
class X86TimerHal : public TimerHal {
private:
    uint32 frequency;
    
public:
    X86TimerHal() : frequency(0) {}

    virtual HalResult Initialize() override {
        // The timer should already be initialized by the kernel
        frequency = g_kernel_config ? g_kernel_config->timer_frequency : 100;
        return HalResult::SUCCESS;
    }

    virtual HalResult SetFrequency(uint32 hz) override {
        if (hz == 0 || hz > 10000) {  // Reasonable upper limit
            return HalResult::ERROR_INVALID_PARAMETER;
        }
        
        frequency = hz;
        // Note: In a real implementation, we would program the hardware timer here
        return HalResult::SUCCESS;
    }

    virtual uint32 GetFrequency() override {
        return frequency;
    }

    virtual uint64_t GetTickCount() override {
        // Use the global timer if available
        if (global_timer) {
            return global_timer->GetTickCount();
        }
        return 0;
    }

    virtual uint64_t GetHighResolutionTime() override {
        // Implementation would go here
        return GetTickCount();
    }

    virtual void Sleep(uint32 milliseconds) override {
        // Implementation would go here
        // For now, this is a busy wait approach
        uint64_t start_tick = GetTickCount();
        uint64_t target_ticks = milliseconds * GetFrequency() / 1000;
        
        while ((GetTickCount() - start_tick) < target_ticks) {
            asm volatile("pause");
        }
    }

    virtual HalResult RegisterHandler(void (*handler)()) override {
        // For x86 timer, the handler is already registered
        // This is a simplified implementation
        return HalResult::SUCCESS;
    }
};

// x86-specific PCI HAL implementation
class X86PciHal : public PciHal {
public:
    virtual HalResult Initialize() override {
        return HalResult::SUCCESS;
    }

    virtual uint32 ReadConfig(uint8 bus, uint8 device, uint8 function, uint8 offset) override {
        uint32 address = 0x80000000 | 
                          ((uint32)bus << 16) | 
                          ((uint32)device << 11) | 
                          ((uint32)function << 8) | 
                          (offset & 0xFC);
        
        OutDWord(0xCF8, address);
        return InDWord(0xCFC);
    }

    virtual void WriteConfig(uint8 bus, uint8 device, uint8 function, uint8 offset, uint32 value) override {
        uint32 address = 0x80000000 | 
                          ((uint32)bus << 16) | 
                          ((uint32)device << 11) | 
                          ((uint32)function << 8) | 
                          (offset & 0xFC);
        
        OutDWord(0xCF8, address);
        OutDWord(0xCFC, value);
    }

    virtual HalResult FindDevice(uint16_t vendor_id, uint16_t device_id, uint8* bus, uint8* device, uint8* function) override {
        for (uint8 b = 0; b < 256; b++) {
            for (uint8 dev = 0; dev < 32; dev++) {
                for (uint8 func = 0; func < 8; func++) {
                    uint32 id = ReadConfig(b, dev, func, 0);
                    if ((id & 0xFFFF) == vendor_id && ((id >> 16) & 0xFFFF) == device_id) {
                        *bus = b;
                        *device = dev;
                        *function = func;
                        return HalResult::SUCCESS;
                    }
                    
                    // If function 0 is not present, don't check other functions
                    if (func == 0 && (id == 0xFFFFFFFF || id == 0xFFFF0000 || id == 0x0000FFFF)) {
                        break;
                    }
                }
            }
        }
        return HalResult::ERROR_RESOURCE_UNAVAILABLE;
    }

    virtual uint32 EnumerateDevices() override {
        uint32 count = 0;
        for (uint8 bus = 0; bus < 256; bus++) {
            for (uint8 device = 0; device < 32; device++) {
                for (uint8 function = 0; function < 8; function++) {
                    uint32 id = ReadConfig(bus, device, function, 0);
                    if (id != 0xFFFFFFFF && id != 0xFFFF0000 && id != 0x0000FFFF) {
                        count++;
                    }
                    
                    // If function 0 is not present, don't check other functions
                    if (function == 0 && (id == 0xFFFFFFFF || id == 0xFFFF0000 || id == 0x0000FFFF)) {
                        break;
                    }
                }
            }
        }
        return count;
    }

private:
    uint32 InDWord(uint16_t port) {
        uint32 result;
        asm volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
        return result;
    }

    void OutDWord(uint16_t port, uint32 value) {
        asm volatile("outl %0, %1" :: "a"(value), "Nd"(port));
    }
};

#endif // ARCH_X86

// HAL Manager implementation
HalManager::HalManager() 
    : cpu_hal(nullptr)
    , memory_hal(nullptr)
    , interrupt_hal(nullptr)
    , timer_hal(nullptr)
    , pci_hal(nullptr)
{
}

HalManager::~HalManager() {
    // Clean up HAL implementations
    if (cpu_hal) delete cpu_hal;
    if (memory_hal) delete memory_hal;
    if (interrupt_hal) delete interrupt_hal;
    if (timer_hal) delete timer_hal;
    if (pci_hal) delete pci_hal;
}

HalResult HalManager::Initialize() {
#ifdef ARCH_X86
    cpu_hal = new X86CpuHal();
    memory_hal = new X86MemoryHal();
    interrupt_hal = new X86InterruptHal();
    timer_hal = new X86TimerHal();
    pci_hal = new X86PciHal();
#endif

    // Initialize each HAL component
    if (cpu_hal && cpu_hal->Initialize() != HalResult::SUCCESS) {
        LOG("Error: Failed to initialize CPU HAL");
        return HalResult::ERROR_NOT_INITIALIZED;
    }

    if (memory_hal && memory_hal->Initialize() != HalResult::SUCCESS) {
        LOG("Error: Failed to initialize Memory HAL");
        return HalResult::ERROR_NOT_INITIALIZED;
    }

    if (interrupt_hal && interrupt_hal->Initialize() != HalResult::SUCCESS) {
        LOG("Error: Failed to initialize Interrupt HAL");
        return HalResult::ERROR_NOT_INITIALIZED;
    }

    if (timer_hal && timer_hal->Initialize() != HalResult::SUCCESS) {
        LOG("Error: Failed to initialize Timer HAL");
        return HalResult::ERROR_NOT_INITIALIZED;
    }

    if (pci_hal && pci_hal->Initialize() != HalResult::SUCCESS) {
        LOG("Error: Failed to initialize PCI HAL");
        return HalResult::ERROR_NOT_INITIALIZED;
    }

    LOG("HAL Manager initialized successfully");
    LOG("CPU Vendor: " << cpu_hal->GetVendorString());
    LOG("Memory Size: " << memory_hal->GetPhysicalMemorySize() / (1024*1024) << " MB");
    LOG("Timer Frequency: " << timer_hal->GetFrequency() << " Hz");
    LOG("PCI Devices Found: " << pci_hal->EnumerateDevices());

    return HalResult::SUCCESS;
}