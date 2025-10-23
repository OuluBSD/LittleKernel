/*
 * DriverBase.h - Base driver class for kernel device drivers
 * Defines the common interface and functionality that all drivers should implement
 */

#ifndef DRIVERBASE_H
#define DRIVERBASE_H

#include "Kernel.h"  // Include main kernel header
#include "Defs.h"    // Include definitions header

// Forward declarations
struct DeviceBase;
struct IoRequest;

// Driver states enum
enum class DriverState {
    STOPPED,
    STARTING,
    RUNNING,
    STOPPING,
    ERROR
};

// Driver initialization result codes
enum class DriverInitResult {
    SUCCESS = 0,
    FAILED = -1,
    NOT_SUPPORTED = -2,
    INSUFFICIENT_RESOURCES = -3,
    DEVICE_NOT_FOUND = -4
};

// I/O request types
enum class IoRequestType {
    READ,
    WRITE,
    IOCTL,
    OPEN,
    CLOSE,
    FLUSH
};

// Base driver class - abstract class that all drivers should inherit from
class DriverBase {
protected:
    char name[64];                  // Driver name
    char version[16];               // Driver version string
    DriverState state;              // Current driver state
    uint32_t vendor_id;             // Hardware vendor ID
    uint32_t device_id;             // Hardware device ID
    void* device_handle;            // Handle to the physical device
    uint32_t interrupt_number;      // Associated interrupt number

public:
    // Constructor
    DriverBase(const char* driver_name, const char* driver_version, 
               uint32_t vid = 0, uint32_t did = 0, uint32_t irq = 0);

    // Destructor
    virtual ~DriverBase();

    // Pure virtual functions that must be implemented by derived classes
    virtual DriverInitResult Initialize() = 0;
    virtual int Shutdown() = 0;
    virtual int HandleInterrupt() = 0;
    virtual int ProcessIoRequest(IoRequest* request) = 0;

    // Virtual functions that can be overridden by derived classes
    virtual int RegisterDevice(DeviceBase* device);
    virtual int UnregisterDevice(DeviceBase* device);
    virtual DriverInitResult StartDevice(DeviceBase* device);
    virtual int StopDevice(DeviceBase* device);
    
    // Common utility functions
    DriverState GetState() const { return state; }
    const char* GetName() const { return name; }
    const char* GetVersion() const { return version; }
    uint32_t GetVendorId() const { return vendor_id; }
    uint32_t GetDeviceId() const { return device_id; }
    uint32_t GetInterruptNumber() const { return interrupt_number; }
    void SetState(DriverState new_state) { state = new_state; }
    
    // Logging convenience functions
    void LogInfo(const char* message);
    void LogError(const char* message);
    void LogDebug(const char* message);
};

// Base class for block device drivers (disks, etc.)
class BlockDeviceDriver : public DriverBase {
protected:
    uint32_t block_size;            // Size of each block in bytes
    uint32_t total_blocks;          // Total number of blocks
    bool read_only;                 // Whether the device is read-only

public:
    BlockDeviceDriver(const char* driver_name, const char* driver_version, 
                      uint32_t vid = 0, uint32_t did = 0, uint32_t irq = 0);

    // Implement pure virtual functions
    virtual DriverInitResult Initialize() override;
    virtual int Shutdown() override;
    virtual int HandleInterrupt() override;
    virtual int ProcessIoRequest(IoRequest* request) override;

    // Block device specific functions
    virtual uint32_t ReadBlocks(uint32_t start_block, uint32_t num_blocks, void* buffer);
    virtual uint32_t WriteBlocks(uint32_t start_block, uint32_t num_blocks, const void* buffer);
    virtual uint32_t GetBlockSize() const { return block_size; }
    virtual uint32_t GetTotalBlocks() const { return total_blocks; }
    virtual bool IsReadOnly() const { return read_only; }
};

// Base class for character device drivers (serial, keyboard, etc.)
class CharacterDeviceDriver : public DriverBase {
protected:
    bool buffered;                  // Whether I/O is buffered

public:
    CharacterDeviceDriver(const char* driver_name, const char* driver_version, 
                          uint32_t vid = 0, uint32_t did = 0, uint32_t irq = 0);

    // Implement pure virtual functions
    virtual DriverInitResult Initialize() override;
    virtual int Shutdown() override;
    virtual int HandleInterrupt() override;
    virtual int ProcessIoRequest(IoRequest* request) override;

    // Character device specific functions
    virtual int Read(void* buffer, uint32_t size);
    virtual int Write(const void* buffer, uint32_t size);
    virtual int WaitForInput();     // Block until input is available
    virtual int WaitForOutput();    // Block until output is possible
};

// Base class for network device drivers
class NetworkDriver : public DriverBase {
protected:
    uint8_t mac_address[6];         // MAC address of the device
    uint32_t mtu;                   // Maximum transmission unit
    bool link_up;                   // Whether the physical link is up

public:
    NetworkDriver(const char* driver_name, const char* driver_version, 
                  uint32_t vid = 0, uint32_t did = 0, uint32_t irq = 0);

    // Implement pure virtual functions
    virtual DriverInitResult Initialize() override;
    virtual int Shutdown() override;
    virtual int HandleInterrupt() override;
    virtual int ProcessIoRequest(IoRequest* request) override;

    // Network device specific functions
    virtual int SendPacket(const void* packet, uint32_t size);
    virtual int ReceivePacket(void* packet, uint32_t max_size);
    virtual const uint8_t* GetMacAddress() const { return mac_address; }
    virtual uint32_t GetMTU() const { return mtu; }
    virtual bool IsLinkUp() const { return link_up; }
    virtual void SetLinkState(bool up) { link_up = up; }
};

// Base class for USB device drivers
class UsbDriver : public DriverBase {
protected:
    uint8_t usb_address;            // USB address (1-127)
    uint8_t usb_port;               // Which USB port the device is connected to
    uint16_t usb_vendor_id;         // USB vendor ID
    uint16_t usb_product_id;        // USB product ID

public:
    UsbDriver(const char* driver_name, const char* driver_version, 
              uint32_t vid = 0, uint32_t did = 0, uint32_t irq = 0);

    // Implement pure virtual functions
    virtual DriverInitResult Initialize() override;
    virtual int Shutdown() override;
    virtual int HandleInterrupt() override;
    virtual int ProcessIoRequest(IoRequest* request) override;

    // USB specific functions
    virtual int UsbControlTransfer(uint8_t request_type, uint8_t request, 
                                   uint16_t value, uint16_t index, 
                                   void* data, uint16_t length);
    virtual int UsbBulkTransfer(uint8_t endpoint, void* data, uint32_t length, bool in);
    virtual int UsbInterruptTransfer(uint8_t endpoint, void* data, uint32_t length, bool in);
    virtual uint8_t GetUsbAddress() const { return usb_address; }
    virtual uint16_t GetUsbVendorId() const { return usb_vendor_id; }
    virtual uint16_t GetUsbProductId() const { return usb_product_id; }
};

// Base class for system modules (not hardware drivers but kernel modules)
class SystemModuleBase {
protected:
    char module_name[64];           // Module name
    char module_version[16];        // Module version
    bool loaded;                    // Whether module is currently loaded
    uint32_t load_address;          // Address where module is loaded

public:
    SystemModuleBase(const char* name, const char* version);
    virtual ~SystemModuleBase();

    // Pure virtual functions
    virtual DriverInitResult Load() = 0;
    virtual int Unload() = 0;
    virtual int InitializeModule() = 0;

    // Common functions
    const char* GetModuleName() const { return module_name; }
    const char* GetModuleVersion() const { return module_version; }
    bool IsLoaded() const { return loaded; }
    uint32_t GetLoadAddress() const { return load_address; }
    void SetLoadAddress(uint32_t addr) { load_address = addr; }
};

#endif // DRIVERBASE_H