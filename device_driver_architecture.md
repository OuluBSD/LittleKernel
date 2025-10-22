# Device Driver Interface Architecture Design

## Overview
This document outlines the design of the device driver interface architecture. The system will follow Ultimate++ and Windows naming conventions while supporting various device types including block devices, character devices, and network interfaces. The architecture will support loadable kernel modules (LKMs) and provide a Windows NT-style driver model with compatibility for Windows 98 VxDs.

## Naming Conventions
- Class names use PascalCase (e.g., DriverManager, DeviceDriver)
- Function names follow Windows/Ultimate++ style (e.g., LoadDriver, UnloadDriver)
- Variable names use lowercase with underscores (e.g., device_type, driver_name)
- Base classes use Base suffix (e.g., DriverBase, DeviceBase)
- Macros use UPPER_CASE (e.g., DRIVER_VERSION, DEVICE_TYPE_DISK)

## Architecture Overview

### Core Components
```
+----------------------+
|   Driver Manager     |
|   (Driver Loading)   |
+----------------------+
|   Device Manager     |
|   (Device Creation)  |
+----------------------+
|   I/O Manager        |
|   (I/O Dispatch)     |
+----------------------+
|   Hardware Abstraction Layer (HAL)
|   (Hardware Access)  |
+----------------------+
```

## 1. Driver Model

### Base Driver Classes
```cpp
class DriverBase {
public:
    char driver_name[256];             // Driver name
    uint32 driver_version;             // Version number
    uint32 flags;                      // Driver flags
    void* driver_extension;            // Driver-specific data
    
    virtual bool Initialize();
    virtual bool Start();
    virtual bool Stop();
    virtual bool Unload();
    virtual ~DriverBase();
    
    // I/O request handlers
    virtual uint32 DispatchCreate(DeviceBase* device, Irp* irp);
    virtual uint32 DispatchClose(DeviceBase* device, Irp* irp);
    virtual uint32 DispatchRead(DeviceBase* device, Irp* irp);
    virtual uint32 DispatchWrite(DeviceBase* device, Irp* irp);
    virtual uint32 DispatchIoControl(DeviceBase* device, Irp* irp);
};

class DeviceBase {
public:
    char device_name[256];             // Device name (e.g., "\\Device\\Keyboard")
    char symbolic_link[256];           // Symbolic link (e.g., "\\DosDevices\\KBD0")
    DeviceType type;                   // Device type
    uint32 flags;                      // Device flags
    DriverBase* driver;                // Associated driver
    DeviceBase* next;                  // Next device in list
    void* device_extension;            // Device-specific data
    spinlock lock;                     // For thread-safe access
    
    virtual bool Create();
    virtual bool Destroy();
    virtual ~DeviceBase();
};
```

### Driver Types
```cpp
enum DriverType {
    DRIVER_FILE_SYSTEM = 0,           // File system driver
    DRIVER_DEVICE = 1,                // Hardware device driver
    DRIVER_NETWORK = 2,               // Network driver
    DRIVER_BUS = 3,                   // Bus enumerator driver
    DRIVER_MINIPORT = 4,              // Miniport driver (hardware-specific)
    DRIVER_FILTER = 5                 // Filter driver
};

enum DeviceType {
    DEVICE_DISK = 0,                  // Disk device
    DEVICE_TAPE = 1,                  // Tape device
    DEVICE_PRINTER = 2,               // Printer device
    DEVICE_KEYBOARD = 3,              // Keyboard device
    DEVICE_MOUSE = 4,                 // Mouse device
    DEVICE_SERIAL = 5,                // Serial port
    DEVICE_PARALLEL = 6,              // Parallel port
    DEVICE_DISPLAY = 7,               // Display device
    DEVICE_NETWORK = 8,               // Network interface
    DEVICE_SOUND = 9,                 // Sound device
    DEVICE_CDROM = 10,                // CD-ROM device
    DEVICE_RAMDISK = 11               // RAM disk
};
```

## 2. I/O Request Packet (IRP) System

### I/O Request Structure
```cpp
class Irp {
public:
    IrpMjFunction* major_function;     // Major function code
    IrpMnFunction* minor_function;     // Minor function code (when applicable)
    uint32 flags;                      // IRP flags
    void* user_buffer;                 // User buffer pointer
    void* system_buffer;               // System buffer pointer
    uint32 buffer_length;              // Length of buffer
    uint32 io_status;                  // I/O status code
    uint32 bytes_transferred;          // Number of bytes transferred
    DeviceBase* device;                // Target device
    Process* requesting_process;       // Process that initiated I/O
    Irp* next_irp;                     // Next IRP in queue
    
    Irp();
    ~Irp();
    
    void Initialize(DeviceBase* dev, IrpMjFunction func, 
                   void* buffer, uint32 length);
    void Complete(uint32 status, uint32 bytes);
};

enum IrpMjFunction {
    IRP_MJ_CREATE = 0,
    IRP_MJ_CLOSE = 1,
    IRP_MJ_READ = 2,
    IRP_MJ_WRITE = 3,
    IRP_MJ_DEVICE_CONTROL = 4,
    IRP_MJ_INTERNAL_DEVICE_CONTROL = 5,
    IRP_MJ_FLUSH_BUFFERS = 6,
    IRP_MJ_SHUTDOWN = 7,
    IRP_MJ_LOCK_CONTROL = 8,
    IRP_MJ_CLEANUP = 9
};

enum IrpMnFunction {
    IRP_MN_QUERY_DEVICE_RELATIONS = 0,
    IRP_MN_QUERY_INTERFACE = 1,
    IRP_MN_QUERY_CAPABILITIES = 2,
    IRP_MN_QUERY_RESOURCES = 3,
    IRP_MN_QUERY_RESOURCE_REQUIREMENTS = 4
};
```

## 3. Driver Manager

### Driver Loading and Management
```cpp
class DriverManager {
private:
    DriverBase* driver_list;           // List of loaded drivers
    spinlock lock;                     // For thread-safe access
    
public:
    DriverBase* LoadDriver(const char* driver_path, DriverType type);
    bool UnloadDriver(DriverBase* driver);
    DriverBase* FindDriverByName(const char* name);
    bool RegisterDriver(DriverBase* driver, DriverType type);
    bool InitializeDriverManager();
    void EnumerateHardware();
    void InitializeAllDevices();
    DriverBase* GetDriverByDevice(DeviceBase* device);
};

class DriverLoader {
public:
    static DriverBase* LoadFromFile(const char* file_path);
    static DriverBase* LoadFromMemory(void* image, uint32 size);
    static bool ValidateDriverImage(void* image);
    static bool ResolveDriverImports(DriverBase* driver);
    static void UnloadDriverImage(DriverBase* driver);
};
```

## 4. Device Manager

### Device Creation and Management
```cpp
class DeviceManager {
private:
    DeviceBase* device_list;           // List of all devices
    spinlock lock;                     // For thread-safe access
    
public:
    DeviceBase* CreateDevice(DeviceType type, const char* name, DriverBase* driver);
    bool DestroyDevice(DeviceBase* device);
    DeviceBase* FindDeviceByName(const char* name);
    DeviceBase* FindDeviceByType(DeviceType type);
    bool RegisterDevice(DeviceBase* device);
    bool CreateSymbolicLink(const char* device_name, const char* symbolic_link);
    bool DeleteSymbolicLink(const char* symbolic_link);
    void EnumerateDevices();
    uint32 GetDeviceCount(DeviceType type);
};

// Device interface for standard operations
class DeviceInterface {
public:
    static uint32 Read(DeviceBase* device, void* buffer, uint32 length, 
                      uint32 offset = 0);
    static uint32 Write(DeviceBase* device, const void* buffer, uint32 length, 
                       uint32 offset = 0);
    static uint32 IoControl(DeviceBase* device, uint32 control_code, 
                           void* input_buffer, uint32 input_length,
                           void* output_buffer, uint32 output_length);
    static bool Open(DeviceBase* device);
    static bool Close(DeviceBase* device);
};
```

## 5. I/O Manager

### I/O Request Processing
```cpp
class IoManager {
private:
    Irp* pending_irps;                 // Pending I/O requests
    spinlock lock;                     // For thread-safe access
    
public:
    Irp* CreateIrp(DeviceBase* device, IrpMjFunction function, 
                  void* buffer = nullptr, uint32 length = 0);
    void DeleteIrp(Irp* irp);
    uint32 CallDriver(DeviceBase* device, Irp* irp);
    uint32 SynchronousCall(DeviceBase* device, IrpMjFunction function,
                          void* buffer = nullptr, uint32 length = 0);
    bool QueueIrp(Irp* irp);
    Irp* GetNextPendingIrp();
    void CompleteIrp(Irp* irp, uint32 status, uint32 bytes);
    void CancelIrp(Irp* irp);
};

enum IoStatus {
    IO_STATUS_SUCCESS = 0,
    IO_STATUS_PENDING = 1,
    IO_STATUS_ERROR = 2,
    IO_STATUS_INVALID_PARAMETER = 3,
    IO_STATUS_NO_SUCH_DEVICE = 4,
    IO_STATUS_NO_SUCH_FILE = 5,
    IO_STATUS_ACCESS_DENIED = 6,
    IO_STATUS_SHARING_VIOLATION = 7
};
```

## 6. Hardware Abstraction Layer (HAL)

### Hardware Access Abstraction
```cpp
class Hal {
public:
    // I/O port operations
    static uint8 ReadPortByte(uint16 port);
    static uint16 ReadPortWord(uint16 port);
    static uint32 ReadPortDword(uint16 port);
    static void WritePortByte(uint16 port, uint8 value);
    static void WritePortWord(uint16 port, uint16 value);
    static void WritePortDword(uint16 port, uint32 value);
    
    // Memory-mapped I/O
    static uint8 ReadRegisterByte(void* reg);
    static uint16 ReadRegisterWord(void* reg);
    static uint32 ReadRegisterDword(void* reg);
    static void WriteRegisterByte(void* reg, uint8 value);
    static void WriteRegisterWord(void* reg, uint16 value);
    static void WriteRegisterDword(void* reg, uint32 value);
    
    // Interrupt management
    static void EnableInterrupt(uint8 irq);
    static void DisableInterrupt(uint8 irq);
    static void AcknowledgeInterrupt(uint8 irq);
    static void SetInterruptHandler(uint8 irq, IrqHandler handler);
    
    // DMA operations
    static bool SetupDma(uint8 channel, void* buffer, uint32 length, bool read);
    static bool CompleteDma(uint8 channel);
    
    // System timer
    static void SetTimerFrequency(uint32 frequency_hz);
    static uint32 GetTimerTicks();
    static void Sleep(uint32 milliseconds);
    
    // System-specific operations
    static void HaltSystem();
    static void ResetSystem();
    static void PowerOff();
};

typedef void (*IrqHandler)(uint32 irq, Irp* irp);
```

## 7. Specific Device Driver Implementations

### Storage Device Driver
```cpp
class StorageDriver : public DriverBase {
private:
    DeviceBase* disk_devices[8];       // Up to 8 disk devices
    DeviceBase* cdrom_devices[4];      // Up to 4 CD-ROM devices
    
public:
    bool Initialize() override;
    uint32 DispatchRead(DeviceBase* device, Irp* irp) override;
    uint32 DispatchWrite(DeviceBase* device, Irp* irp) override;
    uint32 DispatchIoControl(DeviceBase* device, Irp* irp) override;
    
    uint32 ReadSector(DeviceBase* device, uint32 lba, void* buffer, uint32 sectors);
    uint32 WriteSector(DeviceBase* device, uint32 lba, const void* buffer, uint32 sectors);
    bool GetDeviceParameters(DeviceBase* device, DeviceParameters* params);
    
    // ATA/IDE specific functions
    bool IdentifyDevice(uint16 bus, uint8 device, DeviceParameters* params);
    bool ReadAtaSectors(uint16 bus, uint8 device, uint32 lba, 
                       void* buffer, uint32 sectors);
    bool WriteAtaSectors(uint16 bus, uint8 device, uint32 lba, 
                        const void* buffer, uint32 sectors);
};

class StorageDevice : public DeviceBase {
public:
    StorageDeviceType storage_type;    // HDD, CD-ROM, Floppy, etc.
    uint32 sector_size;                // Size of each sector
    uint32 total_sectors;              // Total number of sectors
    uint16 bus_number;                 // Bus number (IDE channel)
    uint8 device_number;               // Device number (master/slave)
    
    bool Create() override;
};
```

### Keyboard Driver
```cpp
class KeyboardDriver : public DriverBase {
public:
    bool Initialize() override;
    uint32 DispatchRead(DeviceBase* device, Irp* irp) override;
    
    void KeyboardIrqHandler(uint32 irq, Irp* irp);
    bool InitializeKeyboard();
    uint32 TranslateScanCode(uint8 scan_code, uint16* key_code);
    bool ProcessKeyPress(uint8 scan_code);
};

class KeyboardDevice : public DeviceBase {
public:
    RingBuffer<KeyboardEvent> event_buffer; // Keyboard event buffer
    KeyboardLayout layout;              // Current keyboard layout
    
    bool Create() override;
};

struct KeyboardEvent {
    uint16 key_code;                   // Translated key code
    uint8 scan_code;                   // Raw scan code
    bool key_pressed;                  // True if key pressed, false if released
    uint32 timestamp;                  // Time of event
};
```

### Network Driver
```cpp
class NetworkDriver : public DriverBase {
public:
    bool Initialize() override;
    uint32 DispatchIoControl(DeviceBase* device, Irp* irp) override;
    
    uint32 SendPacket(DeviceBase* device, const void* packet, uint32 length);
    uint32 ReceivePacket(DeviceBase* device, void* packet, uint32 max_length);
    bool GetMacAddress(DeviceBase* device, uint8* mac_addr);
    bool SetIpAddress(DeviceBase* device, uint32 ip_addr);
};

class NetworkDevice : public DeviceBase {
public:
    NetworkMediaType media_type;       // Ethernet, WiFi, etc.
    uint8 mac_address[6];              // MAC address
    uint32 ip_address;                 // IP address (if assigned)
    uint32 subnet_mask;                // Subnet mask
    bool link_up;                      // Physical link status
    RingBuffer<NetworkPacket> rx_queue; // Received packets
    RingBuffer<NetworkPacket> tx_queue; // Packets to transmit
    
    bool Create() override;
};

struct NetworkPacket {
    void* data;                        // Packet data
    uint32 length;                     // Length of data
    uint32 flags;                      // Packet flags
    uint32 timestamp;                  // Time of reception/transmission
};
```

## 8. Windows 98 VxD Compatibility

### VxD Emulation Layer
```cpp
class VxdManager {
private:
    Vector<VxdModule> loaded_vxds;     // Loaded VxD modules
    spinlock lock;                     // For thread-safe access
    
public:
    VxdModule* LoadVxd(const char* vxd_name);
    bool UnloadVxd(const char* vxd_name);
    uint32 CallVxdService(const char* vxd_name, uint16 service_id, 
                         void* params);
    bool InitializeVxdEnvironment();
    void EnumerateLegacyDevices();
};

class VxdEmulator {
public:
    static uint32 EmulateVxdCall(uint16 vxd_id, uint16 service_id, 
                                void* params);
    static bool InitializeVxdApi();
    static void RegisterVxdApi(uint16 vxd_id, VxdApiHandler handler);
};

typedef uint32 (*VxdApiHandler)(uint16 service_id, void* params);
```

## 9. System Calls Interface

### Device-related System Calls
```cpp
// Windows-style function names for system calls
uint32 SyscallCreateFile(const char* filename, uint32 access, 
                        uint32 share_mode, uint32 creation_disposition,
                        uint32 flags_and_attributes, uint32 template_file);
uint32 SyscallReadFile(uint32 handle, void* buffer, uint32 bytes_to_read,
                      uint32* bytes_read, uint32* overlapped);
uint32 SyscallWriteFile(uint32 handle, const void* buffer, 
                       uint32 bytes_to_write, uint32* bytes_written,
                       uint32* overlapped);
uint32 SyscallDeviceIoControl(uint32 handle, uint32 io_control_code,
                             void* input_buffer, uint32 input_buffer_length,
                             void* output_buffer, uint32 output_buffer_length,
                             uint32* bytes_returned, uint32* overlapped);

// For Linux compatibility layer (internal naming)
int32 SyscallOpen(const char* pathname, int32 flags, uint32 mode);
int32 SyscallClose(uint32 fd);
int32 SyscallRead(uint32 fd, void* buf, uint32 count);
int32 SyscallWrite(uint32 fd, const void* buf, uint32 count);
int32 SyscallIoctl(uint32 fd, uint32 request, void* argp);
```

## 10. Driver Development Framework

### Driver Development Helper Classes
```cpp
// Base class for block device drivers
class BlockDeviceDriverBase : public DriverBase {
protected:
    uint32 block_size;                 // Size of each block
    uint32 total_blocks;               // Total number of blocks
    uint32 bytes_per_sector;           // Bytes per sector
    
    virtual uint32 ReadBlocks(uint32 start_block, uint32 count, void* buffer) = 0;
    virtual uint32 WriteBlocks(uint32 start_block, uint32 count, const void* buffer) = 0;
    
public:
    uint32 DispatchRead(DeviceBase* device, Irp* irp) override;
    uint32 DispatchWrite(DeviceBase* device, Irp* irp) override;
    virtual uint32 GetBlockSize();
    virtual uint32 GetTotalBlocks();
};

// Base class for character device drivers
class CharacterDeviceDriverBase : public DriverBase {
protected:
    RingBuffer<uint8>* input_buffer;   // Input character buffer
    RingBuffer<uint8>* output_buffer;  // Output character buffer
    uint32 bytes_available;            // Number of bytes available
    
    virtual uint32 WriteCharacters(const uint8* chars, uint32 count) = 0;
    virtual uint32 ReadCharacters(uint8* buffer, uint32 max_count) = 0;
    
public:
    uint32 DispatchRead(DeviceBase* device, Irp* irp) override;
    uint32 DispatchWrite(DeviceBase* device, Irp* irp) override;
};

// Driver initialization macros following Windows conventions
#define DRIVER_INITIALIZE(DriverName) \
    extern "C" uint32 DriverName##Initialize() { \
        return DriverName##Driver::InitializeDriver(); \
    }

#define DRIVER_DISPATCH(DeviceObject, Irp) \
    ((Driver*)(DeviceObject)->driver)->DispatchIo(DeviceObject, Irp)
```

## Implementation Strategy

### Phase 1: Core Framework
1. Implement DriverBase and DeviceBase classes
2. Create I/O request packet (IRP) system
3. Implement basic Driver and Device managers
4. Add HAL for basic hardware access
5. Test with simple null driver

### Phase 2: I/O Management
1. Implement IoManager for IRP processing
2. Add synchronous and asynchronous I/O
3. Implement I/O completion routines
4. Test I/O request flow

### Phase 3: Basic Device Drivers
1. Implement keyboard driver
2. Implement simple storage driver
3. Implement serial port driver
4. Test with basic I/O operations

### Phase 4: Advanced Drivers
1. Implement network driver
2. Add USB support (basic)
3. Add display driver
4. Test with real hardware

### Phase 5: Compatibility Layer
1. Implement VxD compatibility layer
2. Add Windows 98 device detection
3. Test with legacy drivers
4. Verify compatibility

### Phase 6: LKM Support
1. Implement loadable kernel module loader
2. Add driver signing/verification
3. Implement dynamic loading/unloading
4. Test with external driver modules

### Phase 7: System Integration
1. Connect with file system layer
2. Integrate with security model
3. Add power management support
4. Performance optimization
5. Stability testing