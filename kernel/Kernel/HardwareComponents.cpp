#include "HardwareComponents.h"
#include "Kernel.h"

// Global PCI device manager instance
PCIDeviceManager* g_pci_device_manager = nullptr;

// HardwareComponent implementation
HardwareComponent::HardwareComponent(const char* component_name, HardwareComponentType comp_type, 
                                     uint32_t vendor, uint32_t device) 
    : type(comp_type), initialized(false), enabled(false), 
      vendor_id(vendor), device_id(device), private_data(nullptr) {
    // Initialize name
    strncpy(name, component_name, sizeof(name) - 1);
    name[sizeof(name) - 1] = 0;
    
    // Initialize description
    strncpy(description, "Generic Hardware Component", sizeof(description) - 1);
    description[sizeof(description) - 1] = 0;
}

HardwareComponent::~HardwareComponent() {
    // Cleanup if the component was initialized
    if (initialized) {
        Shutdown();
    }
}

HalResult HardwareComponent::Configure() {
    LOG("Configuring hardware component: " << name);
    return HalResult::SUCCESS;
}

HalResult HardwareComponent::GetStatus(void* status_buffer, uint32_t buffer_size) {
    if (!status_buffer || buffer_size < sizeof(HalResult)) {
        return HalResult::ERROR_INVALID_PARAMETER;
    }
    
    *(HalResult*)status_buffer = initialized ? HalResult::SUCCESS : HalResult::ERROR_NOT_INITIALIZED;
    return HalResult::SUCCESS;
}

HalResult HardwareComponent::SetPowerState(uint32_t state) {
    LOG("Setting power state " << state << " for component: " << name);
    return HalResult::SUCCESS;
}

uint32_t HardwareComponent::GetPowerState() {
    // For simplicity, return 0 (full power) for all components
    return 0;
}

const char* HardwareComponent::GetTypeString() const {
    switch (type) {
        case HardwareComponentType::UNKNOWN: return "Unknown";
        case HardwareComponentType::PCI_DEVICE: return "PCI Device";
        case HardwareComponentType::USB_DEVICE: return "USB Device";
        case HardwareComponentType::ATA_DEVICE: return "ATA Device";
        case HardwareComponentType::NETWORK_CARD: return "Network Card";
        case HardwareComponentType::GRAPHICS_CARD: return "Graphics Card";
        case HardwareComponentType::SOUND_CARD: return "Sound Card";
        case HardwareComponentType::INPUT_DEVICE: return "Input Device";
        case HardwareComponentType::MEMORY_CONTROLLER: return "Memory Controller";
        case HardwareComponentType::PROCESSOR: return "Processor";
        case HardwareComponentType::INTERRUPT_CONTROLLER: return "Interrupt Controller";
        case HardwareComponentType::TIMER: return "Timer";
        default: return "Unknown Type";
    }
}

void HardwareComponent::PrintInfo() {
    LOG("Hardware Component: " << name);
    LOG("  Type: " << GetTypeString());
    LOG("  Vendor ID: 0x" << vendor_id);
    LOG("  Device ID: 0x" << device_id);
    LOG("  Initialized: " << (initialized ? "Yes" : "No"));
    LOG("  Enabled: " << (enabled ? "Yes" : "No"));
    LOG("  Description: " << description);
}

// PCIDevice implementation
PCIDevice::PCIDevice(const char* name, uint8_t b, uint8_t d, uint8_t f, 
                     uint32_t vendor, uint32_t device_id)
    : HardwareComponent(name, HardwareComponentType::PCI_DEVICE, vendor, device_id),
      bus(b), device(d), function(f), class_code(0), subclass(0), 
      prog_if(0), revision_id(0), header_type(0) {
    // Initialize BARs to 0
    for (int i = 0; i < 6; i++) {
        bar[i] = 0;
    }
}

PCIDevice::~PCIDevice() {
    // Device should be shut down before destruction
    if (initialized) {
        Shutdown();
    }
}

HalResult PCIDevice::Initialize() {
    LOG("Initializing PCI device: " << name << " at " << bus << ":" << device << ":" << function);
    
    // Read device header to get class code, etc.
    uint32_t header_dword = ReadConfig(0);
    if (header_dword == 0xFFFFFFFF) {
        LOG("PCI device not found at " << bus << ":" << device << ":" << function);
        return HalResult::ERROR_INVALID_DEVICE;
    }
    
    // Get class information
    class_code = (ReadConfig(0x08) >> 24) & 0xFF;
    subclass = (ReadConfig(0x08) >> 16) & 0xFF;
    prog_if = (ReadConfig(0x08) >> 8) & 0xFF;
    revision_id = ReadConfig(0x08) & 0xFF;
    
    // Get header type
    header_type = (ReadConfig(0x0C) >> 16) & 0xFF;
    
    // Read BARs
    for (int i = 0; i < 6; i++) {
        bar[i] = ReadConfig(0x10 + i * 4);
    }
    
    // Identify the specific device type
    type = IdentifyDeviceType();
    
    initialized = true;
    LOG("PCI device initialized: " << name << " (" << GetTypeString() << ")");
    return HalResult::SUCCESS;
}

HalResult PCIDevice::Shutdown() {
    LOG("Shutting down PCI device: " << name);
    initialized = false;
    enabled = false;
    return HalResult::SUCCESS;
}

HalResult PCIDevice::Enable() {
    if (!initialized) {
        return HalResult::ERROR_NOT_INITIALIZED;
    }
    
    LOG("Enabling PCI device: " << name);
    
    // Enable memory space and bus mastering if supported
    uint32_t cmd = ReadConfig(0x04);
    cmd |= 0x07; // Enable memory space, I/O space, and bus mastering
    WriteConfig(0x04, cmd);
    
    enabled = true;
    return HalResult::SUCCESS;
}

HalResult PCIDevice::Disable() {
    if (!initialized) {
        return HalResult::ERROR_NOT_INITIALIZED;
    }
    
    LOG("Disabling PCI device: " << name);
    
    // Disable memory space and bus mastering
    uint32_t cmd = ReadConfig(0x04);
    cmd &= ~0x07; // Disable memory space, I/O space, and bus mastering
    WriteConfig(0x04, cmd);
    
    enabled = false;
    return HalResult::SUCCESS;
}

HalResult PCIDevice::Reset() {
    LOG("Resetting PCI device: " << name);
    // PCI devices don't have a standard reset mechanism
    // This would be device-specific
    return HalResult::SUCCESS;
}

HalResult PCIDevice::HandleInterrupt() {
    LOG("Handling interrupt for PCI device: " << name);
    // The actual interrupt handling would be device-specific
    // For now, just acknowledge the interrupt
    return HalResult::SUCCESS;
}

uint32_t PCIDevice::ReadConfig(uint8_t offset) {
    if (HAL_PCI()) {
        return HAL_PCI()->ReadConfig(bus, device, function, offset);
    }
    return 0xFFFFFFFF; // Error value
}

void PCIDevice::WriteConfig(uint8_t offset, uint32_t value) {
    if (HAL_PCI()) {
        HAL_PCI()->WriteConfig(bus, device, function, offset, value);
    }
}

HalResult PCIDevice::MapBAR(uint8_t bar_num, uint32_t* address) {
    if (bar_num >= 6 || !address) {
        return HalResult::ERROR_INVALID_PARAMETER;
    }
    
    uint32_t bar_value = bar[bar_num];
    
    if (bar_value == 0) {
        return HalResult::ERROR_INVALID_PARAMETER; // BAR not valid
    }
    
    // Check if it's a memory BAR (bit 0 = 0)
    if ((bar_value & 0x1) == 0) {
        uint32_t base_addr = bar_value & 0xFFFFFFF0; // Mask out flags
        *address = (uint32_t)HAL_MEMORY()->MapPhysicalMemory(base_addr, 0x1000); // Map 4KB
        if (*address) {
            LOG("Mapped BAR" << bar_num << " for device " << name << " to virtual address 0x" << (void*)*address);
            return HalResult::SUCCESS;
        }
    }
    
    return HalResult::ERROR_INVALID_PARAMETER;
}

HalResult PCIDevice::UnmapBAR(uint8_t bar_num) {
    if (bar_num >= 6) {
        return HalResult::ERROR_INVALID_PARAMETER;
    }
    
    // This would unmap the virtual address, but we'd need to store the mapping
    // For this implementation, we'll just return success
    return HalResult::SUCCESS;
}

HardwareComponentType PCIDevice::IdentifyDeviceType() {
    switch (class_code) {
        case 0x01: // Mass storage controller
            if (subclass == 0x01) return HardwareComponentType::ATA_DEVICE;
            break;
        case 0x02: // Network controller
            return HardwareComponentType::NETWORK_CARD;
        case 0x03: // Display controller
            return HardwareComponentType::GRAPHICS_CARD;
        case 0x04: // Multimedia controller
            return HardwareComponentType::SOUND_CARD;
        case 0x06: // Bridge device
            if (subclass == 0x04) return HardwareComponentType::PCI_DEVICE; // PCI-to-PCI bridge
            break;
        case 0x0C: // Serial bus controller
            if (subclass == 0x03) return HardwareComponentType::USB_DEVICE;
            break;
    }
    
    return HardwareComponentType::PCI_DEVICE; // Default to generic PCI device
}

void PCIDevice::PrintInfo() {
    LOG("PCI Device: " << name << " at " << bus << ":" << device << ":" << function);
    LOG("  Vendor ID: 0x" << vendor_id);
    LOG("  Device ID: 0x" << device_id);
    LOG("  Class: 0x" << class_code << ", Subclass: 0x" << subclass << ", Prog IF: 0x" << prog_if);
    LOG("  Revision: 0x" << revision_id);
    LOG("  Header Type: 0x" << header_type);
    for (int i = 0; i < 6; i++) {
        LOG("  BAR" << i << ": 0x" << bar[i]);
    }
    LOG("  Initialized: " << (initialized ? "Yes" : "No"));
    LOG("  Enabled: " << (enabled ? "Yes" : "No"));
}

// PCIBridge implementation
PCIBridge::PCIBridge(const char* name, uint8_t b, uint8_t d, uint8_t f, 
                     uint32_t vendor, uint32_t device_id)
    : PCIDevice(name, b, d, f, vendor, device_id),
      secondary_bus(0), subordinate_bus(0), memory_base(0), memory_limit(0) {
    type = HardwareComponentType::PCI_DEVICE; // Bridges are still PCI devices but with special handling
}

PCIBridge::~PCIBridge() {
    Shutdown();
}

HalResult PCIBridge::Initialize() {
    LOG("Initializing PCI Bridge: " << name << " at " << bus << ":" << device << ":" << function);
    
    // Initialize as a PCI device first
    HalResult result = PCIDevice::Initialize();
    if (result != HalResult::SUCCESS) {
        return result;
    }
    
    // Read bridge-specific information
    secondary_bus = (ReadConfig(0x18) >> 8) & 0xFF;
    subordinate_bus = (ReadConfig(0x18) >> 16) & 0xFF;
    memory_base = (ReadConfig(0x20) & 0xFFF0) << 16; // Memory base (bits 31-16)
    memory_limit = ((ReadConfig(0x20) & 0xFFFF0000) | 0xFFFF) << 16; // Memory limit
    
    LOG("PCI Bridge initialized: " << name << " secondary bus: " << secondary_bus 
        << ", subordinate bus: " << subordinate_bus);
    
    return HalResult::SUCCESS;
}

HalResult PCIBridge::Shutdown() {
    LOG("Shutting down PCI Bridge: " << name);
    return PCIDevice::Shutdown();
}

HalResult PCIBridge::Enable() {
    if (!initialized) {
        return HalResult::ERROR_NOT_INITIALIZED;
    }
    
    LOG("Enabling PCI Bridge: " << name);
    return PCIDevice::Enable();
}

HalResult PCIBridge::Disable() {
    if (!initialized) {
        return HalResult::ERROR_NOT_INITIALIZED;
    }
    
    LOG("Disabling PCI Bridge: " << name);
    return PCIDevice::Disable();
}

HalResult PCIBridge::Reset() {
    LOG("Resetting PCI Bridge: " << name);
    return HalResult::SUCCESS;
}

HalResult PCIBridge::HandleInterrupt() {
    LOG("Handling interrupt for PCI Bridge: " << name);
    return HalResult::SUCCESS;
}

void PCIBridge::SetBusNumbers(uint8_t sec_bus, uint8_t sub_bus) {
    secondary_bus = sec_bus;
    subordinate_bus = sub_bus;
    
    // Update the PCI config space
    uint32_t bus_reg = ReadConfig(0x18);
    bus_reg = (bus_reg & 0xFF000000) | (sub_bus << 16) | (sec_bus << 8) | (bus_reg & 0xFF);
    WriteConfig(0x18, bus_reg);
}

void PCIBridge::PrintInfo() {
    PCIDevice::PrintInfo();
    LOG("  Bridge Info: Secondary bus: " << secondary_bus << ", Subordinate bus: " << subordinate_bus);
    LOG("  Memory: 0x" << memory_base << " - 0x" << memory_limit);
}

// PCIDeviceManager implementation
PCIDeviceManager::PCIDeviceManager() : device_count(0) {
    memset(devices, 0, sizeof(devices));
}

PCIDeviceManager::~PCIDeviceManager() {
    // Shutdown and destroy all devices
    for (uint32_t i = 0; i < device_count; i++) {
        if (devices[i]) {
            devices[i]->Shutdown();
            delete devices[i];
        }
    }
}

HalResult PCIDeviceManager::Initialize() {
    LOG("PCI Device Manager initializing...");
    
    // Enumerate all PCI devices
    HalResult result = EnumerateDevices();
    if (result != HalResult::SUCCESS) {
        LOG("Failed to enumerate PCI devices");
        return result;
    }
    
    LOG("PCI Device Manager initialized with " << device_count << " devices");
    return HalResult::SUCCESS;
}

HalResult PCIDeviceManager::EnumerateDevices() {
    LOG("Enumerating PCI devices...");
    
    if (!HAL_PCI()) {
        LOG("PCI HAL not available");
        return HalResult::ERROR_NOT_INITIALIZED;
    }
    
    for (uint8_t bus = 0; bus < 255; bus++) {
        for (uint8_t device = 0; device < 32; device++) {
            for (uint8_t function = 0; function < 8; function++) {
                // Read vendor ID to check if device exists
                uint32_t id = HAL_PCI()->ReadConfig(bus, device, function, 0);
                
                if (id == 0xFFFFFFFF || id == 0xFFFF0000 || id == 0x0000FFFF) {
                    // Device doesn't exist, skip to next
                    if (function == 0) {
                        break; // If function 0 doesn't exist, neither do other functions
                    }
                    continue;
                }
                
                // Create a PCI device object for this hardware
                char device_name[32];
                snprintf(device_name, sizeof(device_name), "PCI_%d_%d_%d", bus, device, function);
                
                PCIDevice* pci_dev = new PCIDevice(device_name, bus, device, function,
                                                  id & 0xFFFF, (id >> 16) & 0xFFFF);
                
                if (AddDevice(pci_dev) != HalResult::SUCCESS) {
                    delete pci_dev; // Clean up on failure
                    continue;
                }
                
                // Initialize the device
                if (pci_dev->Initialize() != HalResult::SUCCESS) {
                    LOG("Failed to initialize device: " << device_name);
                    RemoveDevice(bus, device, function);
                    continue;
                }
                
                LOG("Found PCI device: " << device_name << " (0x" << 
                    (id & 0xFFFF) << ":0x" << ((id >> 16) & 0xFFFF) << ")");
                
                // If function 0 doesn't exist, don't check other functions
                if (function == 0 && !(id & 0x800000)) { // Header type bit
                    break;
                }
            }
        }
    }
    
    LOG("Enumeration complete, found " << device_count << " devices");
    return HalResult::SUCCESS;
}

HalResult PCIDeviceManager::AddDevice(PCIDevice* device) {
    if (!device || device_count >= MAX_DEVICES) {
        return HalResult::ERROR_INVALID_PARAMETER;
    }
    
    devices[device_count] = device;
    device_count++;
    
    LOG("Added PCI device: " << device->GetName());
    return HalResult::SUCCESS;
}

HalResult PCIDeviceManager::RemoveDevice(uint8_t bus, uint8_t device, uint8_t function) {
    for (uint32_t i = 0; i < device_count; i++) {
        PCIDevice* dev = devices[i];
        if (dev && dev->GetBus() == bus && dev->GetDevice() == device && dev->GetFunction() == function) {
            dev->Shutdown();
            delete dev;
            
            // Shift remaining devices
            for (uint32_t j = i; j < device_count - 1; j++) {
                devices[j] = devices[j + 1];
            }
            devices[device_count - 1] = nullptr;
            device_count--;
            
            LOG("Removed PCI device at " << bus << ":" << device << ":" << function);
            return HalResult::SUCCESS;
        }
    }
    
    return HalResult::ERROR_INVALID_DEVICE;
}

PCIDevice* PCIDeviceManager::FindDevice(uint8_t bus, uint8_t device, uint8_t function) {
    for (uint32_t i = 0; i < device_count; i++) {
        PCIDevice* dev = devices[i];
        if (dev && dev->GetBus() == bus && dev->GetDevice() == device && dev->GetFunction() == function) {
            return dev;
        }
    }
    return nullptr;
}

PCIDevice* PCIDeviceManager::FindDevice(uint16_t vendor_id, uint16_t device_id) {
    for (uint32_t i = 0; i < device_count; i++) {
        PCIDevice* dev = devices[i];
        if (dev && dev->GetVendorId() == vendor_id && dev->GetDeviceId() == device_id) {
            return dev;
        }
    }
    return nullptr;
}

uint32_t PCIDeviceManager::FindDevicesByClass(uint16_t class_code, uint16_t subclass, PCIDevice** found_devices, uint32_t max_count) {
    uint32_t count = 0;
    
    for (uint32_t i = 0; i < device_count && count < max_count; i++) {
        PCIDevice* dev = devices[i];
        if (dev && dev->GetClassCode() == class_code && dev->GetSubclass() == subclass) {
            found_devices[count] = dev;
            count++;
        }
    }
    
    return count;
}

PCIDevice** PCIDeviceManager::GetDevices(uint32_t* count) {
    *count = device_count;
    return devices;
}

HalResult PCIDeviceManager::InitializeAllDevices() {
    for (uint32_t i = 0; i < device_count; i++) {
        if (devices[i] && !devices[i]->IsInitialized()) {
            if (devices[i]->Initialize() != HalResult::SUCCESS) {
                LOG("Failed to initialize device: " << devices[i]->GetName());
            }
        }
    }
    return HalResult::SUCCESS;
}

HalResult PCIDeviceManager::ShutdownAllDevices() {
    for (uint32_t i = 0; i < device_count; i++) {
        if (devices[i] && devices[i]->IsInitialized()) {
            devices[i]->Shutdown();
        }
    }
    return HalResult::SUCCESS;
}

void PCIDeviceManager::PrintDeviceList() {
    LOG("=== PCI Device List ===");
    for (uint32_t i = 0; i < device_count; i++) {
        if (devices[i]) {
            devices[i]->PrintInfo();
            LOG("---");
        }
    }
    LOG("Total devices: " << device_count);
    LOG("=====================");
}

HalResult PCIDeviceManager::HandleInterrupts() {
    // Handle interrupts for managed devices
    // This would typically be called from the IRQ handler
    for (uint32_t i = 0; i < device_count; i++) {
        if (devices[i]) {
            // In a real implementation, we'd check if this device generated the interrupt
            // For now, just assume a general PCI interrupt and try to handle it
            devices[i]->HandleInterrupt();
        }
    }
    return HalResult::SUCCESS;
}

// HardwareComponentFactory implementation
HardwareComponent* HardwareComponentFactory::CreateComponent(HardwareComponentType type, 
                                                           const char* name,
                                                           uint32_t vendor_id,
                                                           uint32_t device_id) {
    switch (type) {
        case HardwareComponentType::TIMER:
            return new TimerComponent(name, vendor_id, device_id);
        case HardwareComponentType::MEMORY_CONTROLLER:
            return new MemoryController(name, vendor_id, device_id);
        case HardwareComponentType::PCI_DEVICE:
            LOG("Error: Use CreatePCIDevice for PCI devices");
            return nullptr;
        default:
            LOG("Warning: Creating generic hardware component for type: " << (int)type);
            // For now, just return a generic component
            return new HardwareComponent(name, type, vendor_id, device_id);
    }
}

PCIDevice* HardwareComponentFactory::CreatePCIDevice(uint8_t bus, uint8_t device, uint8_t function) {
    // Check if device exists
    uint32_t id = HAL_PCI() ? HAL_PCI()->ReadConfig(bus, device, function, 0) : 0xFFFFFFFF;
    if (id == 0xFFFFFFFF) {
        return nullptr;
    }
    
    char name[32];
    snprintf(name, sizeof(name), "PCI_%d_%d_%d", bus, device, function);
    
    return new PCIDevice(name, bus, device, function, id & 0xFFFF, (id >> 16) & 0xFFFF);
}

void HardwareComponentFactory::DestroyComponent(HardwareComponent* component) {
    if (component) {
        delete component;
    }
}

// TimerComponent implementation
TimerComponent::TimerComponent(const char* name, uint32_t vendor, uint32_t device)
    : HardwareComponent(name, HardwareComponentType::TIMER, vendor, device), 
      frequency(0), tick_count(0) {
}

TimerComponent::~TimerComponent() {
    Shutdown();
}

HalResult TimerComponent::Initialize() {
    if (HAL_TIMER()) {
        frequency = HAL_TIMER()->GetFrequency();
        LOG("Timer component initialized with frequency: " << frequency << " Hz");
        initialized = true;
        return HalResult::SUCCESS;
    }
    return HalResult::ERROR_NOT_INITIALIZED;
}

HalResult TimerComponent::Shutdown() {
    initialized = false;
    enabled = false;
    return HalResult::SUCCESS;
}

HalResult TimerComponent::Enable() {
    if (!initialized) {
        return HalResult::ERROR_NOT_INITIALIZED;
    }
    
    enabled = true;
    return HalResult::SUCCESS;
}

HalResult TimerComponent::Disable() {
    if (!initialized) {
        return HalResult::ERROR_NOT_INITIALIZED;
    }
    
    enabled = false;
    return HalResult::SUCCESS;
}

HalResult TimerComponent::Reset() {
    tick_count = 0;
    return HalResult::SUCCESS;
}

HalResult TimerComponent::HandleInterrupt() {
    tick_count++;
    return HalResult::SUCCESS;
}

void TimerComponent::SetFrequency(uint32_t hz) {
    if (HAL_TIMER() && HAL_TIMER()->SetFrequency(hz) == HalResult::SUCCESS) {
        frequency = hz;
    }
}

void TimerComponent::PrintInfo() {
    LOG("Timer Component: " << name);
    LOG("  Frequency: " << frequency << " Hz");
    LOG("  Tick Count: " << tick_count);
    LOG("  Initialized: " << (initialized ? "Yes" : "No"));
    LOG("  Enabled: " << (enabled ? "Yes" : "No"));
}

// MemoryController implementation
MemoryController::MemoryController(const char* name, uint32_t vendor, uint32_t device)
    : HardwareComponent(name, HardwareComponentType::MEMORY_CONTROLLER, vendor, device),
      total_memory(0), available_memory(0), memory_slots(0), slot_count(0) {
}

MemoryController::~MemoryController() {
    Shutdown();
}

HalResult MemoryController::Initialize() {
    if (HAL_MEMORY()) {
        total_memory = HAL_MEMORY()->GetPhysicalMemorySize();
        available_memory = HAL_MEMORY()->GetAvailableMemory();
        
        LOG("Memory controller initialized with " << total_memory / (1024*1024) << " MB total memory");
        initialized = true;
        return HalResult::SUCCESS;
    }
    return HalResult::ERROR_NOT_INITIALIZED;
}

HalResult MemoryController::Shutdown() {
    initialized = false;
    enabled = false;
    return HalResult::SUCCESS;
}

HalResult MemoryController::Enable() {
    if (!initialized) {
        return HalResult::ERROR_NOT_INITIALIZED;
    }
    
    enabled = true;
    return HalResult::SUCCESS;
}

HalResult MemoryController::Disable() {
    if (!initialized) {
        return HalResult::ERROR_NOT_INITIALIZED;
    }
    
    enabled = false;
    return HalResult::SUCCESS;
}

HalResult MemoryController::Reset() {
    // Memory controller reset is not typically needed
    return HalResult::SUCCESS;
}

HalResult MemoryController::HandleInterrupt() {
    // Memory controllers don't typically generate interrupts in this implementation
    return HalResult::SUCCESS;
}

void MemoryController::PrintInfo() {
    LOG("Memory Controller: " << name);
    LOG("  Total Memory: " << total_memory / (1024*1024) << " MB");
    LOG("  Available Memory: " << available_memory / (1024*1024) << " MB");
    LOG("  Initialized: " << (initialized ? "Yes" : "No"));
    LOG("  Enabled: " << (enabled ? "Yes" : "No"));
}