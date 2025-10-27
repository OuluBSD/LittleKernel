#ifndef _Kernel_Virtio_h_
#define _Kernel_Virtio_h_

#include "Common.h"
#include "Defs.h"
#include "DriverFramework.h"
#include "Vfs.h"
#include "Logging.h"

// Virtio constants
#define VIRTIO_PCI_VENDOR_ID 0x1AF4
#define VIRTIO_MMIO_MAGIC_VALUE 0x74726976  // "virt" in little-endian

// Virtio device IDs
#define VIRTIO_DEVICE_ID_NET 1
#define VIRTIO_DEVICE_ID_BLOCK 2
#define VIRTIO_DEVICE_ID_CONSOLE 3
#define VIRTIO_DEVICE_ID_ENTROPY 4
#define VIRTIO_DEVICE_ID_BALLOON 5
#define VIRTIO_DEVICE_ID_IOMEMORY 6
#define VIRTIO_DEVICE_ID_RPMSG 7
#define VIRTIO_DEVICE_ID_SCSI 8
#define VIRTIO_DEVICE_ID_9P 9
#define VIRTIO_DEVICE_ID_RPROC_SERIAL 11
#define VIRTIO_DEVICE_ID_CAIF 12
#define VIRTIO_DEVICE_ID_GPU 16
#define VIRTIO_DEVICE_ID_INPUT 18
#define VIRTIO_DEVICE_ID_SOCKET 19
#define VIRTIO_DEVICE_ID_CRYPTO 20
#define VIRTIO_DEVICE_ID_SIGNAL_DIST 21
#define VIRTIO_DEVICE_ID_PSTORE 22
#define VIRTIO_DEVICE_ID_IOMMU 23
#define VIRTIO_DEVICE_ID_MEM 24
#define VIRTIO_DEVICE_ID_SOUND 25
#define VIRTIO_DEVICE_ID_FS 26
#define VIRTIO_DEVICE_ID_PMEM 27
#define VIRTIO_DEVICE_ID_RPMB 28
#define VIRTIO_DEVICE_ID_MAC80211_HWSIM 29
#define VIRTIO_DEVICE_ID_VIDEO_ENCODER 30
#define VIRTIO_DEVICE_ID_VIDEO_DECODER 31
#define VIRTIO_DEVICE_ID_SCMI 32
#define VIRTIO_DEVICE_ID_NITRO_SEC_MOD 33
#define VIRTIO_DEVICE_ID_I2C_ADAPTER 34
#define VIRTIO_DEVICE_ID_WATCHDOG 35
#define VIRTIO_DEVICE_ID_CAN 36
#define VIRTIO_DEVICE_ID_DMABUF 37
#define VIRTIO_DEVICE_ID_PARAM_SERV 38
#define VIRTIO_DEVICE_ID_AUDIO_POLICY 39
#define VIRTIO_DEVICE_ID_BT 40
#define VIRTIO_DEVICE_ID_GPIO 41
#define VIRTIO_DEVICE_ID_RDMA 42

// Virtio feature bits
#define VIRTIO_F_NOTIFY_ON_EMPTY (1 << 24)
#define VIRTIO_F_ANY_LAYOUT (1 << 27)
#define VIRTIO_F_RING_INDIRECT_DESC (1 << 28)
#define VIRTIO_F_RING_EVENT_IDX (1 << 29)
#define VIRTIO_F_VERSION_1 (1 << 32)
#define VIRTIO_F_ACCESS_PLATFORM (1 << 33)
#define VIRTIO_F_RING_PACKED (1 << 34)
#define VIRTIO_F_IN_ORDER (1 << 35)
#define VIRTIO_F_ORDER_PLATFORM (1 << 36)
#define VIRTIO_F_SR_IOV (1 << 37)
#define VIRTIO_F_NOTIFICATION_DATA (1 << 38)

// Virtio status bits
#define VIRTIO_STATUS_ACKNOWLEDGE 1
#define VIRTIO_STATUS_DRIVER 2
#define VIRTIO_STATUS_DRIVER_OK 4
#define VIRTIO_STATUS_FEATURES_OK 8
#define VIRTIO_STATUS_DEVICE_NEEDS_RESET 64
#define VIRTIO_STATUS_FAILED 128

// Virtio queue constants
#define VIRTQ_DESC_SIZE 16
#define VIRTQ_AVAIL_SIZE 6
#define VIRTQ_USED_SIZE 6

// Virtio descriptor flags
#define VRING_DESC_F_NEXT 1
#define VRING_DESC_F_WRITE 2
#define VRING_DESC_F_INDIRECT 4

// Virtio ring descriptor structure
struct VirtqDesc {
    uint64_t addr;     // Buffer address (64-bit for compatibility)
    uint32 len;      // Buffer length
    uint16_t flags;    // VRING_DESC_F_* flags
    uint16_t next;     // Next descriptor index
};

// Virtio available ring structure
struct VirtqAvail {
    uint16_t flags;     // VRING_AVAIL_F_* flags
    uint16_t idx;       // Index of the next descriptor to be added
    uint16_t ring[1];   // Array of available descriptors
    uint16_t used_event; // Used ring event (optional)
};

// Virtio used ring element structure
struct VirtqUsedElem {
    uint32 id;        // Index of the descriptor chain
    uint32 len;       // Length of data written to buffer
};

// Virtio used ring structure
struct VirtqUsed {
    uint16_t flags;          // VRING_USED_F_* flags
    uint16_t idx;            // Index of the next descriptor to be consumed
    struct VirtqUsedElem ring[1]; // Array of used descriptors
    uint16_t avail_event;    // Available ring event (optional)
};

// Virtio ring structure
struct Virtq {
    struct VirtqDesc* desc;  // Descriptor table
    struct VirtqAvail* avail; // Available ring
    struct VirtqUsed* used;   // Used ring
    uint16_t num;             // Number of descriptors in the ring
    uint16_t free_num;        // Number of free descriptors
    uint16_t last_used_idx;   // Last used index seen by driver
    uint16_t* free_desc;      // Free descriptor chain
    Spinlock ring_lock;       // Lock for ring access
};

// Virtio PCI configuration structure
struct VirtioPciConfig {
    uint32 device_features;     // Device features
    uint32 driver_features;     // Driver features
    uint16_t msix_config;         // MSI-X configuration
    uint16_t num_queues;          // Number of queues
    uint8 device_status;        // Device status
    uint8 config_generation;    // Configuration generation
    uint8 queue_select;         // Selected queue
    uint16_t queue_size;          // Queue size
    uint16_t queue_msix_vector;   // Queue MSI-X vector
    uint16_t queue_enable;        // Queue enable
    uint16_t queue_notify_off;    // Queue notify offset
    uint32 queue_desc_lo;       // Queue descriptor address (low)
    uint32 queue_desc_hi;       // Queue descriptor address (high)
    uint32 queue_avail_lo;      // Queue available ring address (low)
    uint32 queue_avail_hi;      // Queue available ring address (high)
    uint32 queue_used_lo;       // Queue used ring address (low)
    uint32 queue_used_hi;       // Queue used ring address (high)
};

// Virtio MMIO configuration structure
struct VirtioMmioConfig {
    uint32 magic;              // Magic value (0x74726976)
    uint32 version;            // Device version
    uint32 device_id;          // Virtio device ID
    uint32 vendor_id;          // Vendor ID
    uint32 device_features;    // Device features
    uint32 device_features_sel; // Device features selector
    uint32 driver_features;    // Driver features
    uint32 driver_features_sel; // Driver features selector
    uint32 guest_page_size;    // Guest page size
    uint32 queue_sel;          // Queue selector
    uint32 queue_num_max;      // Maximum queue size
    uint32 queue_num;          // Queue size
    uint32 queue_align;        // Queue alignment
    uint32 queue_pfn;          // Queue page frame number
    uint32 queue_ready;        // Queue ready
    uint32 queue_notify;       // Queue notify
    uint32 interrupt_status;   // Interrupt status
    uint32 interrupt_ack;      // Interrupt acknowledge
    uint32 status;             // Device status
    uint32 config_generation;  // Configuration generation
    uint8 config[256];         // Configuration space
};

// Virtio device structure
struct VirtioDevice {
    Device base_device;          // Base device structure
    uint32 device_id;          // Virtio device ID
    uint32 vendor_id;          // Vendor ID
    uint64_t features;           // Negotiated features
    uint8 status;              // Device status
    uint32 queue_count;        // Number of queues
    struct Virtq* queues;        // Array of queues
    uint32 config_size;        // Size of configuration space
    void* config_space;          // Device-specific configuration space
    uint32 irq_line;           // IRQ line for the device
    uint32 mmio_base;          // MMIO base address (if MMIO)
    uint32 pci_base;           // PCI base address (if PCI)
    bool is_mmio;                // Whether device is MMIO-based
    bool is_pci;                 // Whether device is PCI-based
    Spinlock device_lock;        // Lock for device access
};

// Virtio driver base class
class VirtioDriver : public DriverBase {
protected:
    VirtioDevice* virtio_device;  // Associated Virtio device
    uint32 negotiated_features; // Features negotiated with device
    
public:
    VirtioDriver(const char* driver_name, const char* driver_version, 
                 uint32 vid = 0, uint32 did = 0, uint32 irq = 0);
    virtual ~VirtioDriver();
    
    // Initialize the Virtio driver
    virtual DriverInitResult Initialize() override;
    
    // Shutdown the Virtio driver
    virtual int Shutdown() override;
    
    // Handle Virtio interrupt
    virtual int HandleInterrupt() override;
    
    // Process I/O request
    virtual int ProcessIoRequest(IoRequest* request) override;
    
    // Virtio-specific functions
    virtual bool NegotiateFeatures(uint64_t device_features);
    virtual bool SetupQueues(uint32 queue_count);
    virtual bool InitializeQueue(uint32 queue_index, uint16_t queue_size);
    virtual bool CleanupQueues();
    virtual bool SendBuffer(uint32 queue_index, void* buffer, uint32 size);
    virtual bool ReceiveBuffer(uint32 queue_index, void** buffer, uint32* size);
    virtual uint32 GetQueueSize(uint32 queue_index);
    virtual bool NotifyQueue(uint32 queue_index);
    virtual bool ResetDevice();
    virtual bool SetStatus(uint8 status);
    virtual uint8 GetStatus();
    virtual uint64_t GetDeviceFeatures();
    virtual bool SetDriverFeatures(uint64_t features);
    virtual uint32 GetConfigGeneration();
    virtual bool ReadConfig(uint32 offset, void* buffer, uint32 size);
    virtual bool WriteConfig(uint32 offset, const void* buffer, uint32 size);
    
    // Get the associated Virtio device
    VirtioDevice* GetVirtioDevice() { return virtio_device; }

protected:
    // Internal helper functions
    virtual bool InitializePciDevice();
    virtual bool InitializeMmioDevice();
    virtual bool SetupRing(uint32 queue_index, uint16_t queue_size);
    virtual bool CleanupRing(uint32 queue_index);
    virtual bool AddBufferToQueue(uint32 queue_index, void* buffer, uint32 size, bool write);
    virtual bool ProcessUsedBuffers(uint32 queue_index);
    virtual bool HandleConfigChange();
    
private:
    // Driver framework callbacks
    static DriverInitResult VirtioInit(Device* device);
    static int VirtioShutdown(Device* device);
    static int VirtioHandleInterrupt(Device* device);
    static int VirtioProcessIoRequest(Device* device, IoRequest* request);
};

// Global Virtio driver manager
extern VirtioDriver* g_virtio_driver;

// Initialize the Virtio driver system
bool InitializeVirtio();

#endif