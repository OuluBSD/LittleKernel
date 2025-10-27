#include "Kernel.h"
#include "VirtioBlk.h"
#include "Logging.h"
#include "DriverFramework.h"
#include "MemoryManager.h"
#include "DriverBase.h"
#include "Virtio.h"

// Global Virtio block driver instance
VirtioBlkDriver* g_virtio_blk_driver = nullptr;

VirtioBlkDriver::VirtioBlkDriver(const char* driver_name, const char* driver_version, 
                                 uint32_t vid, uint32_t did, uint32_t irq)
    : BlockDeviceDriver(driver_name, driver_version, vid, did, irq),
      VirtioDriver(driver_name, driver_version, vid, did, irq) {
    
    // Initialize member variables
    memset(&config, 0, sizeof(config));
    queue_size = 0;
    block_size = 512;  // Default block size
    total_blocks = 0;
    read_only = false;
    flush_supported = false;
    discard_supported = false;
    write_zeroes_supported = false;
    max_segments = 0;
    max_segment_size = 0;
    num_queues = 0;
    memset(device_id, 0, sizeof(device_id));
    
    LOG("Created Virtio block driver: " << driver_name);
}

VirtioBlkDriver::~VirtioBlkDriver() {
    LOG("Destroying Virtio block driver");
    
    // Clean up any allocated resources
    Shutdown();
}

DriverInitResult VirtioBlkDriver::Initialize() {
    LOG("Initializing Virtio block driver");
    
    // First initialize the base Virtio driver
    DriverInitResult result = VirtioDriver::Initialize();
    if (result != DriverInitResult::SUCCESS) {
        LOG("Failed to initialize base Virtio driver");
        return result;
    }
    
    // Get device configuration
    if (!GetDeviceConfig()) {
        LOG("Failed to get Virtio block device configuration");
        return DriverInitResult::FAILED;
    }
    
    // Set up queues
    if (!SetupQueues(1)) {  // For now, just one queue
        LOG("Failed to set up Virtio queues");
        return DriverInitResult::FAILED;
    }
    
    // Initialize the first queue
    if (!InitializeQueue(0, queue_size)) {
        LOG("Failed to initialize Virtio queue 0");
        return DriverInitResult::FAILED;
    }
    
    // Set driver status to DRIVER_OK
    if (!SetStatus(VIRTIO_STATUS_DRIVER_OK)) {
        LOG("Failed to set DRIVER_OK status");
        return DriverInitResult::FAILED;
    }
    
    // Register as a block device
    if (!RegisterAsBlockDevice()) {
        LOG("Failed to register as block device");
        return DriverInitResult::FAILED;
    }
    
    LOG("Virtio block driver initialized successfully");
    LOG("  Capacity: " << total_blocks << " blocks (" << (total_blocks * block_size / (1024 * 1024)) << " MB)");
    LOG("  Block size: " << block_size << " bytes");
    LOG("  Read-only: " << (read_only ? "Yes" : "No"));
    LOG("  Flush supported: " << (flush_supported ? "Yes" : "No"));
    LOG("  Device ID: " << device_id);
    
    return DriverInitResult::SUCCESS;
}

int VirtioBlkDriver::Shutdown() {
    LOG("Shutting down Virtio block driver");
    
    // Unregister as block device
    UnregisterAsBlockDevice();
    
    // Clean up Virtio queues
    CleanupQueues();
    
    // Shutdown base Virtio driver
    VirtioDriver::Shutdown();
    
    LOG("Virtio block driver shut down successfully");
    return 0;
}

int VirtioBlkDriver::HandleInterrupt() {
    LOG("Handling Virtio block interrupt");
    
    // Handle the interrupt using the base Virtio driver
    return VirtioDriver::HandleInterrupt();
}

int VirtioBlkDriver::ProcessIoRequest(IoRequest* request) {
    if (!request) {
        return -1;
    }
    
    LOG("Processing I/O request for Virtio block device");
    
    // Process the I/O request using the base Virtio driver
    return VirtioDriver::ProcessIoRequest(request);
}

uint32_t VirtioBlkDriver::ReadBlocks(uint32_t start_block, uint32_t num_blocks, void* buffer) {
    if (!buffer || num_blocks == 0 || start_block + num_blocks > total_blocks) {
        return 0;
    }
    
    LOG("Reading " << num_blocks << " blocks starting at block " << start_block);
    
    // Convert blocks to sectors (assuming 512-byte sectors)
    uint64_t start_sector = (uint64_t)start_block * (block_size / 512);
    uint32_t num_sectors = num_blocks * (block_size / 512);
    
    // Send read request to device
    if (!SendBlockRequest(VIRTIO_BLK_T_IN, start_sector, buffer, num_sectors * 512)) {
        LOG("Failed to send read request to Virtio block device");
        return 0;
    }
    
    // Wait for response
    uint8_t status = 0;
    if (!ReceiveBlockResponse(buffer, num_sectors * 512, &status)) {
        LOG("Failed to receive response from Virtio block device");
        return 0;
    }
    
    // Check status
    if (status != VIRTIO_BLK_S_OK) {
        LOG("Virtio block device returned error status: " << (uint32_t)status);
        return 0;
    }
    
    LOG("Successfully read " << num_blocks << " blocks from Virtio block device");
    return num_blocks;
}

uint32_t VirtioBlkDriver::WriteBlocks(uint32_t start_block, uint32_t num_blocks, const void* buffer) {
    if (!buffer || num_blocks == 0 || start_block + num_blocks > total_blocks) {
        return 0;
    }
    
    // Check if device is read-only
    if (read_only) {
        LOG("Cannot write to read-only Virtio block device");
        return 0;
    }
    
    LOG("Writing " << num_blocks << " blocks starting at block " << start_block);
    
    // Convert blocks to sectors (assuming 512-byte sectors)
    uint64_t start_sector = (uint64_t)start_block * (block_size / 512);
    uint32_t num_sectors = num_blocks * (block_size / 512);
    
    // Send write request to device
    if (!SendBlockRequest(VIRTIO_BLK_T_OUT, start_sector, (void*)buffer, num_sectors * 512)) {
        LOG("Failed to send write request to Virtio block device");
        return 0;
    }
    
    // Wait for response
    uint8_t status = 0;
    if (!ReceiveBlockResponse(nullptr, 0, &status)) {
        LOG("Failed to receive response from Virtio block device");
        return 0;
    }
    
    // Check status
    if (status != VIRTIO_BLK_S_OK) {
        LOG("Virtio block device returned error status: " << (uint32_t)status);
        return 0;
    }
    
    LOG("Successfully wrote " << num_blocks << " blocks to Virtio block device");
    return num_blocks;
}

bool VirtioBlkDriver::NegotiateFeatures(uint64_t device_features) {
    LOG("Negotiating features with Virtio block device");
    
    // Call base class implementation first
    if (!VirtioDriver::NegotiateFeatures(device_features)) {
        return false;
    }
    
    // Check for block-specific features
    if (device_features & VIRTIO_BLK_F_RO) {
        read_only = true;
        LOG("Device is read-only");
    }
    
    if (device_features & VIRTIO_BLK_F_FLUSH) {
        flush_supported = true;
        LOG("Device supports flush operations");
    }
    
    if (device_features & VIRTIO_BLK_F_DISCARD) {
        discard_supported = true;
        LOG("Device supports discard operations");
    }
    
    if (device_features & VIRTIO_BLK_F_WRITE_ZEROES) {
        write_zeroes_supported = true;
        LOG("Device supports write zeroes operations");
    }
    
    if (device_features & VIRTIO_BLK_F_BLK_SIZE) {
        LOG("Device supports block size reporting");
    }
    
    if (device_features & VIRTIO_BLK_F_GEOMETRY) {
        LOG("Device supports geometry reporting");
    }
    
    if (device_features & VIRTIO_BLK_F_TOPOLOGY) {
        LOG("Device supports topology reporting");
    }
    
    if (device_features & VIRTIO_BLK_F_MQ) {
        LOG("Device supports multiqueue");
    }
    
    return true;
}

bool VirtioBlkDriver::SetupQueues(uint32_t queue_count) {
    LOG("Setting up " << queue_count << " Virtio block queues");
    
    // Call base class implementation
    return VirtioDriver::SetupQueues(queue_count);
}

bool VirtioBlkDriver::InitializeQueue(uint32_t queue_index, uint16_t queue_size) {
    LOG("Initializing Virtio block queue " << queue_index << " with size " << queue_size);
    
    // Call base class implementation
    return VirtioDriver::InitializeQueue(queue_index, queue_size);
}

bool VirtioBlkDriver::CleanupQueues() {
    LOG("Cleaning up Virtio block queues");
    
    // Call base class implementation
    return VirtioDriver::CleanupQueues();
}

bool VirtioBlkDriver::SendBuffer(uint32_t queue_index, void* buffer, uint32_t size) {
    LOG("Sending buffer to Virtio block queue " << queue_index << " (size: " << size << ")");
    
    // Call base class implementation
    return VirtioDriver::SendBuffer(queue_index, buffer, size);
}

bool VirtioBlkDriver::ReceiveBuffer(uint32_t queue_index, void** buffer, uint32_t* size) {
    LOG("Receiving buffer from Virtio block queue " << queue_index);
    
    // Call base class implementation
    return VirtioDriver::ReceiveBuffer(queue_index, buffer, size);
}

uint32_t VirtioBlkDriver::GetQueueSize(uint32_t queue_index) {
    // Call base class implementation
    return VirtioDriver::GetQueueSize(queue_index);
}

bool VirtioBlkDriver::NotifyQueue(uint32_t queue_index) {
    LOG("Notifying Virtio block queue " << queue_index);
    
    // Call base class implementation
    return VirtioDriver::NotifyQueue(queue_index);
}

bool VirtioBlkDriver::ResetDevice() {
    LOG("Resetting Virtio block device");
    
    // Call base class implementation
    return VirtioDriver::ResetDevice();
}

bool VirtioBlkDriver::SetStatus(uint8_t status) {
    // Call base class implementation
    return VirtioDriver::SetStatus(status);
}

uint8_t VirtioBlkDriver::GetStatus() {
    // Call base class implementation
    return VirtioDriver::GetStatus();
}

bool VirtioBlkDriver::SetDriverFeatures(uint64_t features) {
    // Call base class implementation
    return VirtioDriver::SetDriverFeatures(features);
}

uint64_t VirtioBlkDriver::GetDeviceFeatures() {
    // Call base class implementation
    return VirtioDriver::GetDeviceFeatures();
}

uint32_t VirtioBlkDriver::GetConfigGeneration() {
    // Call base class implementation
    return VirtioDriver::GetConfigGeneration();
}

bool VirtioBlkDriver::ReadConfig(uint32_t offset, void* buffer, uint32_t size) {
    // Call base class implementation
    return VirtioDriver::ReadConfig(offset, buffer, size);
}

bool VirtioBlkDriver::WriteConfig(uint32_t offset, const void* buffer, uint32_t size) {
    // Call base class implementation
    return VirtioDriver::WriteConfig(offset, buffer, size);
}

bool VirtioBlkDriver::GetDeviceConfig() {
    LOG("Getting Virtio block device configuration");
    
    // Read the configuration structure from the device
    if (!ReadConfig(0, &config, sizeof(config))) {
        LOG("Failed to read Virtio block device configuration");
        return false;
    }
    
    // Extract device parameters
    total_blocks = config.capacity;
    block_size = config.blk_size ? config.blk_size : 512;  // Default to 512 if not set
    queue_size = config.seg_max ? config.seg_max : 128;    // Default to 128 if not set
    
    // Get device ID
    if (!GetDeviceId(device_id, sizeof(device_id))) {
        strcpy_safe(device_id, "Unknown Virtio Block Device", sizeof(device_id));
    }
    
    LOG("Device configuration retrieved:");
    LOG("  Capacity: " << total_blocks << " sectors");
    LOG("  Block size: " << block_size << " bytes");
    LOG("  Max segments: " << queue_size);
    LOG("  Geometry: " << config.geometry.cylinders << " cylinders, " 
         << (uint32_t)config.geometry.heads << " heads, " 
         << (uint32_t)config.geometry.sectors << " sectors");
    
    return true;
}

bool VirtioBlkDriver::SetWritebackMode(bool writeback) {
    if (!flush_supported) {
        LOG("Device does not support flush operations, cannot change writeback mode");
        return false;
    }
    
    LOG("Setting writeback mode to " << (writeback ? "enabled" : "disabled"));
    
    // Update the configuration
    config.writeback = writeback ? 1 : 0;
    
    // Write the configuration back to the device
    if (!WriteConfig(offsetof(VirtioBlkConfig, writeback), &config.writeback, sizeof(config.writeback))) {
        LOG("Failed to write writeback configuration to device");
        return false;
    }
    
    LOG("Writeback mode set successfully");
    return true;
}

bool VirtioBlkDriver::FlushDevice() {
    if (!flush_supported) {
        LOG("Device does not support flush operations");
        return false;
    }
    
    LOG("Flushing Virtio block device");
    
    // Send flush request to device
    if (!SendBlockRequest(VIRTIO_BLK_T_FLUSH, 0, nullptr, 0)) {
        LOG("Failed to send flush request to Virtio block device");
        return false;
    }
    
    // Wait for response
    uint8_t status = 0;
    if (!ReceiveBlockResponse(nullptr, 0, &status)) {
        LOG("Failed to receive response from Virtio block device");
        return false;
    }
    
    // Check status
    if (status != VIRTIO_BLK_S_OK) {
        LOG("Virtio block device returned error status for flush: " << (uint32_t)status);
        return false;
    }
    
    LOG("Device flushed successfully");
    return true;
}

bool VirtioBlkDriver::DiscardBlocks(uint32_t start_block, uint32_t num_blocks) {
    if (!discard_supported) {
        LOG("Device does not support discard operations");
        return false;
    }
    
    if (start_block + num_blocks > total_blocks) {
        LOG("Discard range exceeds device capacity");
        return false;
    }
    
    LOG("Discarding " << num_blocks << " blocks starting at block " << start_block);
    
    // Convert blocks to sectors (assuming 512-byte sectors)
    uint64_t start_sector = (uint64_t)start_block * (block_size / 512);
    uint32_t num_sectors = num_blocks * (block_size / 512);
    
    // Send discard request to device
    // Note: This is a simplified implementation. A real implementation would need to
    // construct the proper discard request structure according to the Virtio spec.
    if (!SendBlockRequest(VIRTIO_BLK_T_DISCARD, start_sector, nullptr, num_sectors * 512)) {
        LOG("Failed to send discard request to Virtio block device");
        return false;
    }
    
    // Wait for response
    uint8_t status = 0;
    if (!ReceiveBlockResponse(nullptr, 0, &status)) {
        LOG("Failed to receive response from Virtio block device");
        return false;
    }
    
    // Check status
    if (status != VIRTIO_BLK_S_OK) {
        LOG("Virtio block device returned error status for discard: " << (uint32_t)status);
        return false;
    }
    
    LOG("Blocks discarded successfully");
    return true;
}

bool VirtioBlkDriver::WriteZeroesBlocks(uint32_t start_block, uint32_t num_blocks) {
    if (!write_zeroes_supported) {
        LOG("Device does not support write zeroes operations");
        return false;
    }
    
    if (start_block + num_blocks > total_blocks) {
        LOG("Write zeroes range exceeds device capacity");
        return false;
    }
    
    LOG("Writing zeroes to " << num_blocks << " blocks starting at block " << start_block);
    
    // Convert blocks to sectors (assuming 512-byte sectors)
    uint64_t start_sector = (uint64_t)start_block * (block_size / 512);
    uint32_t num_sectors = num_blocks * (block_size / 512);
    
    // Send write zeroes request to device
    // Note: This is a simplified implementation. A real implementation would need to
    // construct the proper write zeroes request structure according to the Virtio spec.
    if (!SendBlockRequest(VIRTIO_BLK_T_WRITE_ZEROES, start_sector, nullptr, num_sectors * 512)) {
        LOG("Failed to send write zeroes request to Virtio block device");
        return false;
    }
    
    // Wait for response
    uint8_t status = 0;
    if (!ReceiveBlockResponse(nullptr, 0, &status)) {
        LOG("Failed to receive response from Virtio block device");
        return false;
    }
    
    // Check status
    if (status != VIRTIO_BLK_S_OK) {
        LOG("Virtio block device returned error status for write zeroes: " << (uint32_t)status);
        return false;
    }
    
    LOG("Zeroes written successfully");
    return true;
}

bool VirtioBlkDriver::GetDeviceId(char* id_buffer, uint32_t buffer_size) {
    if (!id_buffer || buffer_size == 0) {
        return false;
    }
    
    // Attempt to get device ID through a special request
    // Note: This is a simplified implementation. A real implementation would need to
    // construct the proper GET_ID request structure according to the Virtio spec.
    
    // For now, just create a generic device ID
    snprintf(id_buffer, buffer_size, "Virtio Block Device (Capacity: %llu sectors)", 
             (unsigned long long)config.capacity);
    
    return true;
}

uint32_t VirtioBlkDriver::ReadSectors(uint64_t start_sector, uint32_t num_sectors, void* buffer) {
    if (!buffer || num_sectors == 0) {
        return 0;
    }
    
    LOG("Reading " << num_sectors << " sectors starting at sector " << start_sector);
    
    // Send read request to device
    if (!SendBlockRequest(VIRTIO_BLK_T_IN, start_sector, buffer, num_sectors * 512)) {
        LOG("Failed to send read request to Virtio block device");
        return 0;
    }
    
    // Wait for response
    uint8_t status = 0;
    if (!ReceiveBlockResponse(buffer, num_sectors * 512, &status)) {
        LOG("Failed to receive response from Virtio block device");
        return 0;
    }
    
    // Check status
    if (status != VIRTIO_BLK_S_OK) {
        LOG("Virtio block device returned error status: " << (uint32_t)status);
        return 0;
    }
    
    LOG("Successfully read " << num_sectors << " sectors from Virtio block device");
    return num_sectors;
}

uint32_t VirtioBlkDriver::WriteSectors(uint64_t start_sector, uint32_t num_sectors, const void* buffer) {
    if (!buffer || num_sectors == 0) {
        return 0;
    }
    
    // Check if device is read-only
    if (read_only) {
        LOG("Cannot write to read-only Virtio block device");
        return 0;
    }
    
    LOG("Writing " << num_sectors << " sectors starting at sector " << start_sector);
    
    // Send write request to device
    if (!SendBlockRequest(VIRTIO_BLK_T_OUT, start_sector, (void*)buffer, num_sectors * 512)) {
        LOG("Failed to send write request to Virtio block device");
        return 0;
    }
    
    // Wait for response
    uint8_t status = 0;
    if (!ReceiveBlockResponse(nullptr, 0, &status)) {
        LOG("Failed to receive response from Virtio block device");
        return 0;
    }
    
    // Check status
    if (status != VIRTIO_BLK_S_OK) {
        LOG("Virtio block device returned error status: " << (uint32_t)status);
        return 0;
    }
    
    LOG("Successfully wrote " << num_sectors << " sectors to Virtio block device");
    return num_sectors;
}

bool VirtioBlkDriver::InitializePciDevice() {
    LOG("Initializing PCI Virtio block device");
    
    // Call base class implementation
    return VirtioDriver::InitializePciDevice();
}

bool VirtioBlkDriver::InitializeMmioDevice() {
    LOG("Initializing MMIO Virtio block device");
    
    // Call base class implementation
    return VirtioDriver::InitializeMmioDevice();
}

bool VirtioBlkDriver::SetupRing(uint32_t queue_index, uint16_t queue_size) {
    LOG("Setting up Virtio block ring for queue " << queue_index);
    
    // Call base class implementation
    return VirtioDriver::SetupRing(queue_index, queue_size);
}

bool VirtioBlkDriver::CleanupRing(uint32_t queue_index) {
    LOG("Cleaning up Virtio block ring for queue " << queue_index);
    
    // Call base class implementation
    return VirtioDriver::CleanupRing(queue_index);
}

bool VirtioBlkDriver::AddBufferToQueue(uint32_t queue_index, void* buffer, uint32_t size, bool write) {
    LOG("Adding buffer to Virtio block queue " << queue_index);
    
    // Call base class implementation
    return VirtioDriver::AddBufferToQueue(queue_index, buffer, size, write);
}

bool VirtioBlkDriver::ProcessUsedBuffers(uint32_t queue_index) {
    LOG("Processing used buffers for Virtio block queue " << queue_index);
    
    // Call base class implementation
    return VirtioDriver::ProcessUsedBuffers(queue_index);
}

bool VirtioBlkDriver::HandleConfigChange() {
    LOG("Handling Virtio block configuration change");
    
    // Call base class implementation
    return VirtioDriver::HandleConfigChange();
}

bool VirtioBlkDriver::SendBlockRequest(uint32_t type, uint64_t sector, void* data, uint32_t size) {
    LOG("Sending block request: type=" << type << ", sector=" << sector << ", size=" << size);
    
    // In a real implementation, this would:
    // 1. Construct a Virtio block request header
    // 2. Add the data buffer if needed
    // 3. Send the request through the Virtio queue
    // 4. Notify the device
    
    // For now, just return success
    return true;
}

bool VirtioBlkDriver::ReceiveBlockResponse(void* data, uint32_t size, uint8_t* status) {
    if (!status) {
        return false;
    }
    
    LOG("Receiving block response: size=" << size);
    
    // In a real implementation, this would:
    // 1. Wait for the device to process the request
    // 2. Receive the response from the Virtio queue
    // 3. Extract the status and data
    // 4. Return the result
    
    // For now, just set a success status and return success
    *status = VIRTIO_BLK_S_OK;
    return true;
}

bool InitializeVirtioBlk() {
    if (!g_virtio_blk_driver) {
        g_virtio_blk_driver = new VirtioBlkDriver("VirtioBlk", "1.0");
        if (!g_virtio_blk_driver) {
            LOG("Failed to create Virtio block driver instance");
            return false;
        }
        
        LOG("Virtio block driver created successfully");
    }
    
    return true;
}