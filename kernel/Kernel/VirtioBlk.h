#ifndef _Kernel_VirtioBlock_h_
#define _Kernel_VirtioBlock_h_

#include "Common.h"
#include "Defs.h"
#include "Virtio.h"
#include "DriverFramework.h"
#include "DriverBase.h"

// Virtio block device feature bits
#define VIRTIO_BLK_F_SIZE_MAX    (1 << 1)   // Maximum size of any single segment
#define VIRTIO_BLK_F_SEG_MAX     (1 << 2)   // Maximum number of segments
#define VIRTIO_BLK_F_GEOMETRY    (1 << 4)   // Disk geometry valid
#define VIRTIO_BLK_F_RO          (1 << 5)   // Device is read-only
#define VIRTIO_BLK_F_BLK_SIZE    (1 << 6)   // Block size of disk is available
#define VIRTIO_BLK_F_FLUSH       (1 << 9)   // Cache flush command support
#define VIRTIO_BLK_F_TOPOLOGY    (1 << 10)  // Topology information available
#define VIRTIO_BLK_F_CONFIG_WCE  (1 << 11)  // Writeback mode available in config
#define VIRTIO_BLK_F_MQ          (1 << 12)  // Multiqueue support
#define VIRTIO_BLK_F_DISCARD     (1 << 13)  // Discard command support
#define VIRTIO_BLK_F_WRITE_ZEROES (1 << 14) // Write zeroes command support

// Virtio block device configuration structure
struct VirtioBlkConfig {
    uint64_t capacity;        // Number of sectors (512 bytes each)
    uint32 size_max;        // Maximum segment size
    uint32 seg_max;         // Maximum number of segments
    struct {
        uint16_t cylinders;   // Number of cylinders
        uint8 heads;        // Number of heads
        uint8 sectors;      // Number of sectors per track
    } geometry;
    uint32 blk_size;        // Block size of device
    struct {
        uint8 physical_block_exp;    // Exponent for physical block per logical block
        uint8 alignment_offset;      // Alignment offset in logical blocks
        uint16_t min_io_size;         // Minimum I/O size without performance penalty
        uint32 opt_io_size;         // Optimal I/O size
    } topology;
    uint8 writeback;        // Writeback mode (1 = WB, 0 = WT)
    uint8 unused0;          // Padding
    uint16_t num_queues;      // Number of queues for multiqueue support
    uint32 max_discard_sectors;     // Maximum discard sectors
    uint32 max_discard_seg;         // Maximum discard segments
    uint32 discard_sector_alignment; // Discard sector alignment
    uint32 max_write_zeroes_sectors; // Maximum write zeroes sectors
    uint32 max_write_zeroes_seg;     // Maximum write zeroes segments
    uint8 write_zeroes_may_unmap;    // Write zeroes may unmap
    uint8 unused1[3];       // Padding
};

// Virtio block request types
#define VIRTIO_BLK_T_IN          0    // Read operation
#define VIRTIO_BLK_T_OUT         1    // Write operation
#define VIRTIO_BLK_T_FLUSH       4    // Flush operation
#define VIRTIO_BLK_T_DISCARD     11   // Discard operation
#define VIRTIO_BLK_T_WRITE_ZEROES 13  // Write zeroes operation
#define VIRTIO_BLK_T_GET_ID      8    // Get device ID

// Virtio block request status
#define VIRTIO_BLK_S_OK          0    // Request completed successfully
#define VIRTIO_BLK_S_IOERR       1    // Device failure
#define VIRTIO_BLK_S_UNSUPP      2    // Request not supported

// Virtio block request header
struct VirtioBlkReqHdr {
    uint32 type;            // Request type (VIRTIO_BLK_T_*)
    uint32 reserved;        // Reserved (set to 0)
    uint64_t sector;          // Sector number (512-byte sectors)
};

// Virtio block request footer
struct VirtioBlkReqFooter {
    uint8 status;           // Request status (VIRTIO_BLK_S_*)
};

// Virtio block device driver class
class VirtioBlkDriver : public BlockDeviceDriver, public VirtioDriver {
private:
    VirtioBlkConfig config;           // Device configuration
    uint32 queue_size;              // Size of virtqueue
    uint32 block_size;              // Block size in bytes (usually 512)
    uint64_t total_blocks;            // Total number of blocks
    bool read_only;                   // Whether device is read-only
    bool flush_supported;             // Whether flush is supported
    bool discard_supported;           // Whether discard is supported
    bool write_zeroes_supported;      // Whether write zeroes is supported
    uint32 max_segments;            // Maximum number of segments per request
    uint32 max_segment_size;        // Maximum size of any single segment
    uint32 num_queues;              // Number of queues
    char device_id[256];              // Device identifier string

public:
    VirtioBlkDriver(const char* driver_name, const char* driver_version, 
                    uint32 vid = 0, uint32 did = 0, uint32 irq = 0);
    virtual ~VirtioBlkDriver();
    
    // Implement required virtual functions from BlockDeviceDriver
    virtual DriverInitResult Initialize() override;
    virtual int Shutdown() override;
    virtual int HandleInterrupt() override;
    virtual int ProcessIoRequest(IoRequest* request) override;
    virtual uint32 ReadBlocks(uint32 start_block, uint32 num_blocks, void* buffer) override;
    virtual uint32 WriteBlocks(uint32 start_block, uint32 num_blocks, const void* buffer) override;
    virtual uint32 GetBlockSize() const override { return block_size; }
    virtual uint32 GetTotalBlocks() const override { return total_blocks; }
    virtual bool IsReadOnly() const override { return read_only; }
    
    // Implement required virtual functions from VirtioDriver
    virtual bool NegotiateFeatures(uint64_t device_features) override;
    virtual bool SetupQueues(uint32 queue_count) override;
    virtual bool InitializeQueue(uint32 queue_index, uint16_t queue_size) override;
    virtual bool CleanupQueues() override;
    virtual bool SendBuffer(uint32 queue_index, void* buffer, uint32 size) override;
    virtual bool ReceiveBuffer(uint32 queue_index, void** buffer, uint32* size) override;
    virtual uint32 GetQueueSize(uint32 queue_index) override;
    virtual bool NotifyQueue(uint32 queue_index) override;
    virtual bool ResetDevice() override;
    virtual bool SetStatus(uint8 status) override;
    virtual uint8 GetStatus() override;
    virtual bool SetDriverFeatures(uint64_t features) override;
    virtual uint64_t GetDeviceFeatures() override;
    virtual uint32 GetConfigGeneration() override;
    virtual bool ReadConfig(uint32 offset, void* buffer, uint32 size) override;
    virtual bool WriteConfig(uint32 offset, const void* buffer, uint32 size) override;
    
    // Virtio block-specific functions
    bool GetDeviceConfig();
    bool SetWritebackMode(bool writeback);
    bool FlushDevice();
    bool DiscardBlocks(uint32 start_block, uint32 num_blocks);
    bool WriteZeroesBlocks(uint32 start_block, uint32 num_blocks);
    bool GetDeviceId(char* id_buffer, uint32 buffer_size);
    
    // Block device-specific functions
    uint32 ReadSectors(uint64_t start_sector, uint32 num_sectors, void* buffer);
    uint32 WriteSectors(uint64_t start_sector, uint32 num_sectors, const void* buffer);

private:
    // Internal helper functions
    bool InitializePciDevice();
    bool InitializeMmioDevice();
    bool SetupRing(uint32 queue_index, uint16_t queue_size);
    bool CleanupRing(uint32 queue_index);
    bool AddBufferToQueue(uint32 queue_index, void* buffer, uint32 size, bool write);
    bool ProcessUsedBuffers(uint32 queue_index);
    bool HandleConfigChange();
    bool SendBlockRequest(uint32 type, uint64_t sector, void* data, uint32 size);
    bool ReceiveBlockResponse(void* data, uint32 size, uint8* status);
};

// Global Virtio block driver instance
extern VirtioBlkDriver* g_virtio_blk_driver;

// Initialize the Virtio block driver
bool InitializeVirtioBlk();

#endif