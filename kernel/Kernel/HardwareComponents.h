#ifndef _Kernel_HardwareComponents_h_
#define _Kernel_HardwareComponents_h_

#include "Defs.h"
#include "HAL.h"

// Forward declarations
class HardwareComponent;
class PCIDevice;
class PCIBridge;
class PCIDeviceManager;

// Hardware component types
enum class HardwareComponentType {
    UNKNOWN = 0,
    PCI_DEVICE = 1,
    USB_DEVICE = 2,
    ATA_DEVICE = 3,
    NETWORK_CARD = 4,
    GRAPHICS_CARD = 5,
    SOUND_CARD = 6,
    INPUT_DEVICE = 7,
    MEMORY_CONTROLLER = 8,
    PROCESSOR = 9,
    INTERRUPT_CONTROLLER = 10,
    TIMER = 11
};

// Hardware component interface - base class for all hardware components
class HardwareComponent {
protected:
    char name[64];
    char description[128];
    HardwareComponentType type;
    bool initialized;
    bool enabled;
    uint32_t vendor_id;
    uint32_t device_id;
    void* private_data;
    
public:
    HardwareComponent(const char* component_name, HardwareComponentType comp_type, 
                      uint32_t vendor = 0, uint32_t device = 0);
    virtual ~HardwareComponent();
    
    // Pure virtual functions that must be implemented by derived classes
    virtual HalResult Initialize() = 0;
    virtual HalResult Shutdown() = 0;
    virtual HalResult Enable() = 0;
    virtual HalResult Disable() = 0;
    virtual HalResult Reset() = 0;
    virtual HalResult HandleInterrupt() = 0;
    
    // Virtual functions with default implementations that can be overridden
    virtual HalResult Configure();
    virtual HalResult GetStatus(void* status_buffer, uint32_t buffer_size);
    virtual HalResult SetPowerState(uint32_t state);
    virtual uint32_t GetPowerState();
    
    // Getters
    const char* GetName() const { return name; }
    const char* GetDescription() const { return description; }
    HardwareComponentType GetType() const { return type; }
    bool IsInitialized() const { return initialized; }
    bool IsEnabled() const { return enabled; }
    uint32_t GetVendorId() const { return vendor_id; }
    uint32_t GetDeviceId() const { return device_id; }
    
    // Setters
    void SetDescription(const char* desc) { strncpy(description, desc, sizeof(description) - 1); }
    void SetPrivateData(void* data) { private_data = data; }
    void* GetPrivateData() const { return private_data; }
    
    // Utility functions
    virtual const char* GetTypeString() const;
    virtual void PrintInfo();
};

// PCI device component - specialized for PCI hardware
class PCIDevice : public HardwareComponent {
protected:
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    uint16_t class_code;
    uint16_t subclass;
    uint16_t prog_if;
    uint16_t revision_id;
    uint8_t header_type;
    
    // PCI BARs (Base Address Registers)
    uint32_t bar[6];
    
public:
    PCIDevice(const char* name, uint8_t b, uint8_t d, uint8_t f, 
              uint32_t vendor = 0, uint32_t device_id = 0);
    virtual ~PCIDevice();
    
    // Implement pure virtual functions
    virtual HalResult Initialize() override;
    virtual HalResult Shutdown() override;
    virtual HalResult Enable() override;
    virtual HalResult Disable() override;
    virtual HalResult Reset() override;
    virtual HalResult HandleInterrupt() override;
    
    // PCI-specific functions
    virtual uint32_t ReadConfig(uint8_t offset);
    virtual void WriteConfig(uint8_t offset, uint32_t value);
    virtual HalResult MapBAR(uint8_t bar_num, uint32_t* address);
    virtual HalResult UnmapBAR(uint8_t bar_num);
    
    // Getters for PCI-specific properties
    uint8_t GetBus() const { return bus; }
    uint8_t GetDevice() const { return device; }
    uint8_t GetFunction() const { return function; }
    uint16_t GetClassCode() const { return class_code; }
    uint16_t GetSubclass() const { return subclass; }
    uint16_t GetProgIF() const { return prog_if; }
    uint32_t GetBAR(uint8_t num) const { return num < 6 ? bar[num] : 0; }
    
    // Identify device type based on class code
    virtual HardwareComponentType IdentifyDeviceType();
    
    // Override base functions with PCI-specific behavior
    virtual void PrintInfo() override;
};

// PCI Bridge component (specialized PCI device)
class PCIBridge : public PCIDevice {
protected:
    uint8_t secondary_bus;
    uint8_t subordinate_bus;
    uint32_t memory_base;
    uint32_t memory_limit;
    
public:
    PCIBridge(const char* name, uint8_t b, uint8_t d, uint8_t f, 
              uint32_t vendor = 0, uint32_t device_id = 0);
    virtual ~PCIBridge();
    
    // Implement pure virtual functions
    virtual HalResult Initialize() override;
    virtual HalResult Shutdown() override;
    virtual HalResult Enable() override;
    virtual HalResult Disable() override;
    virtual HalResult Reset() override;
    virtual HalResult HandleInterrupt() override;
    
    // Bridge-specific functions
    virtual uint8_t GetSecondaryBus() const { return secondary_bus; }
    virtual uint8_t GetSubordinateBus() const { return subordinate_bus; }
    virtual void SetBusNumbers(uint8_t sec_bus, uint8_t sub_bus);
    
    virtual void PrintInfo() override;
};

// PCI Device Manager to handle enumeration and management
class PCIDeviceManager {
private:
    static const uint32_t MAX_DEVICES = 256;
    PCIDevice* devices[MAX_DEVICES];
    uint32_t device_count;
    
public:
    PCIDeviceManager();
    ~PCIDeviceManager();
    
    // Initialize the PCI device manager
    HalResult Initialize();
    
    // Enumerate all PCI devices
    HalResult EnumerateDevices();
    
    // Add a PCI device to the manager
    HalResult AddDevice(PCIDevice* device);
    
    // Remove a PCI device from the manager
    HalResult RemoveDevice(uint8_t bus, uint8_t device, uint8_t function);
    
    // Find a PCI device by its location
    PCIDevice* FindDevice(uint8_t bus, uint8_t device, uint8_t function);
    
    // Find a PCI device by vendor and device ID
    PCIDevice* FindDevice(uint16_t vendor_id, uint16_t device_id);
    
    // Find PCI devices by class code
    uint32_t FindDevicesByClass(uint16_t class_code, uint16_t subclass, PCIDevice** found_devices, uint32_t max_count);
    
    // Get all devices
    PCIDevice** GetDevices(uint32_t* count);
    
    // Initialize all managed devices
    HalResult InitializeAllDevices();
    
    // Shutdown all managed devices
    HalResult ShutdownAllDevices();
    
    // Print device list
    void PrintDeviceList();
    
    // Handle interrupts for managed devices
    HalResult HandleInterrupts();
};

// Hardware component factory for creating specialized hardware components
class HardwareComponentFactory {
public:
    static HardwareComponent* CreateComponent(HardwareComponentType type, 
                                             const char* name,
                                             uint32_t vendor_id = 0,
                                             uint32_t device_id = 0);
    
    static PCIDevice* CreatePCIDevice(uint8_t bus, uint8_t device, uint8_t function);
    
    static void DestroyComponent(HardwareComponent* component);
};

// Common hardware components

// Timer hardware component
class TimerComponent : public HardwareComponent {
private:
    uint32_t frequency;
    uint64_t tick_count;
    
public:
    TimerComponent(const char* name, uint32_t vendor = 0, uint32_t device = 0);
    virtual ~TimerComponent();
    
    virtual HalResult Initialize() override;
    virtual HalResult Shutdown() override;
    virtual HalResult Enable() override;
    virtual HalResult Disable() override;
    virtual HalResult Reset() override;
    virtual HalResult HandleInterrupt() override;
    
    virtual void SetFrequency(uint32_t hz);
    virtual uint32_t GetFrequency() const { return frequency; }
    virtual uint64_t GetTickCount() const { return tick_count; }
    
    virtual void PrintInfo() override;
};

// Memory controller component
class MemoryController : public HardwareComponent {
private:
    uint64_t total_memory;
    uint64_t available_memory;
    uint32_t memory_slots;
    uint32_t slot_count;
    
public:
    MemoryController(const char* name, uint32_t vendor = 0, uint32_t device = 0);
    virtual ~MemoryController();
    
    virtual HalResult Initialize() override;
    virtual HalResult Shutdown() override;
    virtual HalResult Enable() override;
    virtual HalResult Disable() override;
    virtual HalResult Reset() override;
    virtual HalResult HandleInterrupt() override;
    
    virtual uint64_t GetTotalMemory() const { return total_memory; }
    virtual uint64_t GetAvailableMemory() const { return available_memory; }
    virtual uint32_t GetMemorySlots() const { return memory_slots; }
    virtual uint32_t GetSlotCount() const { return slot_count; }
    
    virtual void PrintInfo() override;
};

// Global PCI device manager instance
extern PCIDeviceManager* g_pci_device_manager;

#endif // _Kernel_HardwareComponents_h_