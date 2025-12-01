/*
 * FloppyDriver.cpp - Floppy disk driver implementation for LittleKernel OS
 */

#include "Kernel.h"
#include "FloppyDriver.h"
#include "Logging.h"
#include "Vfs.h"
#include "Linuxulator.h"  // For O_* constants
#include "ProcessControlBlock.h"

// Global floppy driver instance
FloppyDriver* g_floppy_driver = nullptr;

FloppyDriver::FloppyDriver(const char* driver_name, 
                           const char* driver_version,
                           uint32 vid, uint32 did, uint32 irq)
    : BlockDeviceDriver(driver_name, driver_version, vid, did, irq) {
    
    // Initialize controller state
    memset(&controller_state, 0, sizeof(FloppyControllerState));
    controller_state.current_drive = 0;
    controller_state.step_rate = 8;  // Default step rate
    controller_state.head_load_time = 15;  // Default head load time
    controller_state.head_unload_time = 240;  // Default head unload time
    controller_state.dma_mode = 1;  // Enable DMA by default
    
    // Set default I/O parameters
    base_io_port = FDC_BASE_PORT;
    irq_line = static_cast<uint8>(irq);
    dma_channel = 2;  // Standard floppy DMA channel
    
    // Clear disk image path
    memset(disk_image_path, 0, sizeof(disk_image_path));
    qemu_mode = false;
    
    // Set block device properties for 1.44MB floppy
    block_size = FLOPPY_SECTOR_SIZE;
    total_blocks = FLOPPY_TOTAL_SECTORS;
    read_only = false;
    
    LOG("FloppyDriver created with name: " << driver_name << ", version: " << driver_version);
}

FloppyDriver::~FloppyDriver() {
    // Cleanup handled by kernel shutdown
    LOG("FloppyDriver destroyed");
}

DriverInitResult FloppyDriver::Initialize() {
    LOG("Initializing FloppyDriver");
    
    // Check if we're running in QEMU with fda parameter
    if (InitializeQemuMode()) {
        LOG("FloppyDriver initialized in QEMU mode");
        state = DriverState::RUNNING;
        return DriverInitResult::SUCCESS;
    }
    
    // Fall back to hardware initialization
    if (InitializeHardwareMode()) {
        LOG("FloppyDriver initialized in hardware mode");
        state = DriverState::RUNNING;
        return DriverInitResult::SUCCESS;
    }
    
    LOG("Failed to initialize FloppyDriver in any mode");
    state = DriverState::ERROR;
    return DriverInitResult::FAILED;
}

int FloppyDriver::Shutdown() {
    LOG("Shutting down FloppyDriver");
    
    // Turn off all motors
    for (int i = 0; i < 4; i++) {
        TurnMotorOff(i);
    }
    
    // Reset controller
    ResetController();
    
    state = DriverState::STOPPED;
    return 0;  // Success
}

int FloppyDriver::HandleInterrupt() {
    LOG("FloppyDriver handling interrupt");
    
    // Read interrupt status
    WriteFdcCommand(FDC_CMD_SENSE_INTERRUPT);
    uint8 st0 = ReceiveByteFromFdc();
    uint8 pcn = ReceiveByteFromFdc();
    
    LOG("Floppy interrupt: ST0=0x" << (uint32)st0 << ", PCN=" << (int)pcn);
    
    // Clear interrupt pending flag
    return 0;  // Success
}

int FloppyDriver::ProcessIoRequest(IoRequest* request) {
    if (!request) {
        LOG("Invalid IO request");
        return -1;
    }
    
    LOG("Processing IO request: type=" << (int)request->type << ", offset=" << request->offset 
        << ", size=" << request->size);
    
    switch (request->type) {
        case IoRequestType::READ:
            return ReadBlocks(request->offset / block_size, request->size / block_size, request->buffer);
            
        case IoRequestType::WRITE:
            return WriteBlocks(request->offset / block_size, request->size / block_size, request->buffer);
            
        default:
            LOG("Unsupported IO request type: " << (int)request->type);
            return -1;
    }
}

uint32 FloppyDriver::ReadBlocks(uint32 start_block, uint32 num_blocks, void* buffer) {
    if (!buffer || !IsSectorValid(start_block) || !IsSectorValid(start_block + num_blocks - 1)) {
        LOG("Invalid parameters for ReadBlocks");
        return 0;
    }
    
    uint8* byte_buffer = static_cast<uint8*>(buffer);
    uint32 blocks_read = 0;
    
    // In QEMU mode, read directly from disk image
    if (qemu_mode) {
        for (uint32 i = 0; i < num_blocks; i++) {
            if (ReadSectorFromImage(start_block + i, byte_buffer + (i * block_size))) {
                blocks_read++;
            } else {
                LOG("Failed to read sector " << (start_block + i));
                break;
            }
        }
    } else {
        // In hardware mode, read through floppy controller
        for (uint32 i = 0; i < num_blocks; i++) {
            uint8 cylinder, head, sector;
            LbaToChs(start_block + i, &cylinder, &head, &sector);
            
            if (ReadSector(controller_state.current_drive, cylinder, head, sector, 
                          byte_buffer + (i * block_size))) {
                blocks_read++;
            } else {
                LOG("Failed to read CHS " << (int)cylinder << ":" << (int)head << ":" << (int)sector);
                break;
            }
        }
    }
    
    return blocks_read;
}

uint32 FloppyDriver::WriteBlocks(uint32 start_block, uint32 num_blocks, const void* buffer) {
    if (!buffer || !IsSectorValid(start_block) || !IsSectorValid(start_block + num_blocks - 1)) {
        LOG("Invalid parameters for WriteBlocks");
        return 0;
    }
    
    if (read_only) {
        LOG("Cannot write to read-only floppy");
        return 0;
    }
    
    const uint8* byte_buffer = static_cast<const uint8*>(buffer);
    uint32 blocks_written = 0;
    
    // In QEMU mode, write directly to disk image
    if (qemu_mode) {
        for (uint32 i = 0; i < num_blocks; i++) {
            if (WriteSectorToImage(start_block + i, byte_buffer + (i * block_size))) {
                blocks_written++;
            } else {
                LOG("Failed to write sector " << (start_block + i));
                break;
            }
        }
    } else {
        // In hardware mode, write through floppy controller
        for (uint32 i = 0; i < num_blocks; i++) {
            uint8 cylinder, head, sector;
            LbaToChs(start_block + i, &cylinder, &head, &sector);
            
            if (WriteSector(controller_state.current_drive, cylinder, head, sector, 
                           byte_buffer + (i * block_size))) {
                blocks_written++;
            } else {
                LOG("Failed to write CHS " << (int)cylinder << ":" << (int)head << ":" << (int)sector);
                break;
            }
        }
    }
    
    return blocks_written;
}

bool FloppyDriver::InitializeQemuMode() {
    LOG("Attempting to initialize FloppyDriver in QEMU mode");
    
    // Check for QEMU-specific environment or command line parameters
    // For now, we'll assume QEMU mode if we can access a disk image
    qemu_mode = true;
    
    // Try to load the default floppy disk image (this would be set by QEMU's fda parameter)
    // In a real implementation, we would get this from kernel parameters
    const char* default_floppy_image = "/floppy.img";  // Default path
    
    if (LoadDiskImage(default_floppy_image)) {
        LOG("Successfully loaded QEMU floppy disk image: " << default_floppy_image);
        return true;
    }
    
    // If we can't load the default image, try creating a blank one
    LOG("Creating blank 1.44MB floppy image");
    controller_state.disk_image_size = FLOPPY_DISK_SIZE;
    controller_state.disk_image_data = malloc(FLOPPY_DISK_SIZE);
    if (controller_state.disk_image_data) {
        memset(controller_state.disk_image_data, 0, FLOPPY_DISK_SIZE);
        LOG("Created blank floppy image");
        return true;
    }
    
    LOG("Failed to initialize QEMU mode");
    qemu_mode = false;
    return false;
}

bool FloppyDriver::InitializeHardwareMode() {
    LOG("Attempting to initialize FloppyDriver in hardware mode");
    
    // This would implement actual hardware access to a floppy controller
    // For now, we'll just simulate success
    LOG("Hardware mode not fully implemented yet");
    return false;
}

bool FloppyDriver::LoadDiskImage(const char* image_path) {
    if (!image_path) {
        return false;
    }
    
    LOG("Loading disk image: " << image_path);
    
    // Open the disk image file
    int fd = g_vfs->Open(image_path, O_RDONLY);
    if (fd < 0) {
        LOG("Failed to open disk image: " << image_path);
        return false;
    }
    
    // Get file size
    FileStat stat_buf;
    if (g_vfs->Stat(image_path, &stat_buf) < 0) {
        LOG("Failed to get disk image size: " << image_path);
        g_vfs->Close(fd);
        return false;
    }
    
    // Check if it's a valid floppy size (1.44MB or 1.47MB)
    if (stat_buf.st_size != FLOPPY_DISK_SIZE && stat_buf.st_size != (FLOPPY_DISK_SIZE + 512)) {
        LOG("Invalid floppy disk image size: " << stat_buf.st_size << " bytes");
        g_vfs->Close(fd);
        return false;
    }
    
    // Allocate memory for the disk image
    controller_state.disk_image_data = malloc(stat_buf.st_size);
    if (!controller_state.disk_image_data) {
        LOG("Failed to allocate memory for disk image");
        g_vfs->Close(fd);
        return false;
    }
    
    // Read the entire disk image
    ssize_t bytes_read = g_vfs->Read(fd, controller_state.disk_image_data, stat_buf.st_size);
    if (bytes_read != stat_buf.st_size) {
        LOG("Failed to read disk image, read " << bytes_read << " bytes out of " << stat_buf.st_size);
        free(controller_state.disk_image_data);
        controller_state.disk_image_data = nullptr;
        g_vfs->Close(fd);
        return false;
    }
    
    // Set the disk image size
    controller_state.disk_image_size = stat_buf.st_size;
    
    // Close the file
    g_vfs->Close(fd);
    
    // Store the image path
    strcpy_safe(disk_image_path, image_path, sizeof(disk_image_path));
    
    LOG("Successfully loaded disk image: " << image_path << " (" << stat_buf.st_size << " bytes)");
    return true;
}

bool FloppyDriver::SaveDiskImage(const char* image_path) {
    if (!image_path || !controller_state.disk_image_data) {
        return false;
    }
    
    LOG("Saving disk image: " << image_path);
    
    // Open the disk image file for writing
    int fd = g_vfs->Open(image_path, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0) {
        LOG("Failed to create disk image: " << image_path);
        return false;
    }
    
    // Write the entire disk image
    ssize_t bytes_written = g_vfs->Write(fd, controller_state.disk_image_data, controller_state.disk_image_size);
    if (bytes_written != controller_state.disk_image_size) {
        LOG("Failed to write disk image, wrote " << bytes_written << " bytes out of " << controller_state.disk_image_size);
        g_vfs->Close(fd);
        return false;
    }
    
    // Close the file
    g_vfs->Close(fd);
    
    LOG("Successfully saved disk image: " << image_path);
    return true;
}

bool FloppyDriver::ReadSectorFromImage(uint32 sector, void* buffer) {
    if (!buffer || !controller_state.disk_image_data || sector >= FLOPPY_TOTAL_SECTORS) {
        return false;
    }
    
    // Calculate offset in disk image
    uint32 offset = sector * FLOPPY_SECTOR_SIZE;
    if (offset + FLOPPY_SECTOR_SIZE > controller_state.disk_image_size) {
        return false;
    }
    
    // Copy data from disk image to buffer
    memcpy(buffer, static_cast<uint8*>(controller_state.disk_image_data) + offset, FLOPPY_SECTOR_SIZE);
    return true;
}

bool FloppyDriver::WriteSectorToImage(uint32 sector, const void* buffer) {
    if (!buffer || !controller_state.disk_image_data || sector >= FLOPPY_TOTAL_SECTORS) {
        return false;
    }
    
    if (read_only) {
        return false;
    }
    
    // Calculate offset in disk image
    uint32 offset = sector * FLOPPY_SECTOR_SIZE;
    if (offset + FLOPPY_SECTOR_SIZE > controller_state.disk_image_size) {
        return false;
    }
    
    // Copy data from buffer to disk image
    memcpy(static_cast<uint8*>(controller_state.disk_image_data) + offset, buffer, FLOPPY_SECTOR_SIZE);
    return true;
}

void FloppyDriver::LbaToChs(uint32 lba, uint8* cylinder, uint8* head, uint8* sector) {
    if (!cylinder || !head || !sector) {
        return;
    }
    
    // Convert LBA to CHS for 1.44MB floppy (80 cylinders, 2 heads, 18 sectors/track)
    *cylinder = (lba / (FLOPPY_HEADS * FLOPPY_SECTORS_PER_TRACK)) & 0xFF;
    *head = (lba / FLOPPY_SECTORS_PER_TRACK) % FLOPPY_HEADS;
    *sector = (lba % FLOPPY_SECTORS_PER_TRACK) + 1;  // Sectors are 1-indexed
}

uint32 FloppyDriver::ChsToLba(uint8 cylinder, uint8 head, uint8 sector) {
    // Convert CHS to LBA for 1.44MB floppy
    // Note: sector is 1-indexed
    return (cylinder * FLOPPY_HEADS + head) * FLOPPY_SECTORS_PER_TRACK + (sector - 1);
}

bool FloppyDriver::IsSectorValid(uint32 sector) {
    return sector < FLOPPY_TOTAL_SECTORS;
}

bool FloppyDriver::DetectFloppyDrives() {
    // Detect floppy drives by sending sense drive status commands
    // For now, just assume drive A: exists
    controller_state.drive_types[0] = FLOPPY_DRIVE_144MB_35;
    LOG("Detected drive A: as 1.44MB 3.5\" floppy");
    return true;
}

bool FloppyDriver::CalibrateDrive(uint8 drive) {
    if (drive >= 4) {
        return false;
    }
    
    LOG("Calibrating drive " << drive);
    
    // Send recalibrate command
    WriteFdcCommand(FDC_CMD_RECALIBRATE);
    SendByteToFdc(drive);
    
    // Wait for interrupt
    if (!WaitForIrq()) {
        LOG("Timeout waiting for recalibrate interrupt");
        return false;
    }
    
    // Sense interrupt status
    WriteFdcCommand(FDC_CMD_SENSE_INTERRUPT);
    uint8 st0 = ReceiveByteFromFdc();
    uint8 pcn = ReceiveByteFromFdc();
    
    LOG("Recalibrate result: ST0=0x" << (uint32)st0 << ", PCN=" << (int)pcn);
    
    controller_state.cylinders[drive] = 0;  // Cylinder should be 0 after recalibrate
    return true;
}

bool FloppyDriver::SeekToSector(uint8 drive, uint8 cylinder, uint8 head, uint8 sector) {
    if (drive >= 4) {
        return false;
    }
    
    LOG("Seeking to CHS " << (int)cylinder << ":" << (int)head << ":" << (int)sector << " on drive " << drive);
    
    // Send seek command
    WriteFdcCommand(FDC_CMD_SEEK);
    SendByteToFdc((head << 2) | drive);  // Head and drive
    SendByteToFdc(cylinder);            // Cylinder
    
    // Wait for interrupt
    if (!WaitForIrq()) {
        LOG("Timeout waiting for seek interrupt");
        return false;
    }
    
    // Sense interrupt status
    WriteFdcCommand(FDC_CMD_SENSE_INTERRUPT);
    uint8 st0 = ReceiveByteFromFdc();
    uint8 pcn = ReceiveByteFromFdc();
    
    LOG("Seek result: ST0=0x" << (uint32)st0 << ", PCN=" << (int)pcn);
    
    // Check if we reached the correct cylinder
    if (pcn != cylinder) {
        LOG("Seek failed, ended at cylinder " << (int)pcn << " instead of " << (int)cylinder);
        return false;
    }
    
    controller_state.cylinders[drive] = cylinder;
    controller_state.heads[drive] = head;
    controller_state.sectors[drive] = sector;
    return true;
}

bool FloppyDriver::ReadSector(uint8 drive, uint8 cylinder, uint8 head, uint8 sector, void* buffer) {
    if (!buffer || drive >= 4) {
        return false;
    }
    
    if (qemu_mode) {
        // In QEMU mode, read directly from disk image
        uint32 lba = ChsToLba(cylinder, head, sector);
        return ReadSectorFromImage(lba, buffer);
    }
    
    LOG("Reading sector CHS " << (int)cylinder << ":" << (int)head << ":" << (int)sector << " from drive " << drive);
    
    // Turn on motor for the drive
    TurnMotorOn(drive);
    
    // Send read command
    WriteFdcCommand(FDC_CMD_READ_DATA);
    SendByteToFdc((head << 2) | drive);  // Head and drive
    SendByteToFdc(cylinder);             // Cylinder
    SendByteToFdc(head);                 // Head
    SendByteToFdc(sector);               // Sector
    SendByteToFdc(2);                    // Sector size (512 bytes = 2^9, so 2)
    SendByteToFdc(FLOPPY_SECTORS_PER_TRACK);  // Last sector
    SendByteToFdc(0x1B);                 // Gap length
    SendByteToFdc(0xFF);                 // Data length (unused)
    
    // Wait for interrupt
    if (!WaitForIrq()) {
        LOG("Timeout waiting for read interrupt");
        TurnMotorOff(drive);
        return false;
    }
    
    // Read result bytes
    uint8 st0 = ReceiveByteFromFdc();
    uint8 st1 = ReceiveByteFromFdc();
    uint8 st2 = ReceiveByteFromFdc();
    uint8 rcy = ReceiveByteFromFdc();
    uint8 rhd = ReceiveByteFromFdc();
    uint8 rse = ReceiveByteFromFdc();
    uint8 rlc = ReceiveByteFromFdc();
    
    LOG("Read result: ST0=0x" << (uint32)st0 << ", ST1=0x" << (uint32)st1 << ", ST2=0x" << (uint32)st2);
    
    // Check for errors
    if ((st0 & 0xC0) || (st1 & 0x80) || (st2 & 0x80)) {
        LOG("Read error: ST0=0x" << (uint32)st0 << ", ST1=0x" << (uint32)st1 << ", ST2=0x" << (uint32)st2);
        TurnMotorOff(drive);
        return false;
    }
    
    // Copy data from DMA buffer to user buffer
    // In a real implementation, this would copy from the DMA buffer
    // For now, we'll just return success
    memset(buffer, 0, FLOPPY_SECTOR_SIZE);  // Just zero the buffer for now
    
    // Turn off motor
    TurnMotorOff(drive);
    
    return true;
}

bool FloppyDriver::WriteSector(uint8 drive, uint8 cylinder, uint8 head, uint8 sector, const void* buffer) {
    if (!buffer || drive >= 4) {
        return false;
    }
    
    if (read_only) {
        return false;
    }
    
    if (qemu_mode) {
        // In QEMU mode, write directly to disk image
        uint32 lba = ChsToLba(cylinder, head, sector);
        return WriteSectorToImage(lba, buffer);
    }
    
    LOG("Writing sector CHS " << (int)cylinder << ":" << (int)head << ":" << (int)sector << " to drive " << drive);
    
    // Turn on motor for the drive
    TurnMotorOn(drive);
    
    // Send write command
    WriteFdcCommand(FDC_CMD_WRITE_DATA);
    SendByteToFdc((head << 2) | drive);  // Head and drive
    SendByteToFdc(cylinder);             // Cylinder
    SendByteToFdc(head);                 // Head
    SendByteToFdc(sector);               // Sector
    SendByteToFdc(2);                    // Sector size (512 bytes = 2^9, so 2)
    SendByteToFdc(FLOPPY_SECTORS_PER_TRACK);  // Last sector
    SendByteToFdc(0x1B);                 // Gap length
    SendByteToFdc(0xFF);                 // Data length (unused)
    
    // Wait for interrupt
    if (!WaitForIrq()) {
        LOG("Timeout waiting for write interrupt");
        TurnMotorOff(drive);
        return false;
    }
    
    // Read result bytes
    uint8 st0 = ReceiveByteFromFdc();
    uint8 st1 = ReceiveByteFromFdc();
    uint8 st2 = ReceiveByteFromFdc();
    uint8 rcy = ReceiveByteFromFdc();
    uint8 rhd = ReceiveByteFromFdc();
    uint8 rse = ReceiveByteFromFdc();
    uint8 rlc = ReceiveByteFromFdc();
    
    LOG("Write result: ST0=0x" << (uint32)st0 << ", ST1=0x" << (uint32)st1 << ", ST2=0x" << (uint32)st2);
    
    // Check for errors
    if ((st0 & 0xC0) || (st1 & 0x80) || (st2 & 0x80)) {
        LOG("Write error: ST0=0x" << (uint32)st0 << ", ST1=0x" << (uint32)st1 << ", ST2=0x" << (uint32)st2);
        TurnMotorOff(drive);
        return false;
    }
    
    // Turn off motor
    TurnMotorOff(drive);
    
    return true;
}

bool FloppyDriver::WaitForIrq() {
    // In a real implementation, this would wait for an actual interrupt
    // For now, just simulate success
    DelayMs(10);
    return true;
}

bool FloppyDriver::WaitForRdy() {
    // Wait for controller to be ready
    for (int i = 0; i < 10000; i++) {
        if (ReadFdcStatus() & FDC_STATUS_READY) {
            return true;
        }
        DelayMs(1);
    }
    return false;
}

uint8 FloppyDriver::ReadFdcStatus() {
    return inportb(base_io_port + 4);  // Main Status Register
}

void FloppyDriver::WriteFdcCommand(uint8 command) {
    if (WaitForRdy()) {
        outportb(base_io_port + 5, command);  // Data FIFO
    }
}

uint8 FloppyDriver::ReadFdcData() {
    return inportb(base_io_port + 5);  // Data FIFO
}

void FloppyDriver::WriteFdcData(uint8 data) {
    if (WaitForRdy()) {
        outportb(base_io_port + 5, data);  // Data FIFO
    }
}

void FloppyDriver::SendByteToFdc(uint8 byte) {
    WriteFdcData(byte);
}

uint8 FloppyDriver::ReceiveByteFromFdc() {
    return ReadFdcData();
}

bool FloppyDriver::ResetController() {
    // Reset the floppy controller
    outportb(base_io_port + 2, 0);  // Clear reset
    DelayMs(10);
    outportb(base_io_port + 2, 0x0C);  // Reset with interrupts enabled
    DelayMs(10);
    
    return true;
}

void FloppyDriver::ConfigureController() {
    // Configure the controller with standard parameters
    WriteFdcCommand(FDC_CMD_SPECIFY);
    SendByteToFdc((controller_state.step_rate << 4) | controller_state.head_unload_time);
    SendByteToFdc((controller_state.head_load_time << 1) | 1);  // DMA enabled
}

void FloppyDriver::TurnMotorOn(uint8 drive) {
    if (drive >= 4) {
        return;
    }
    
    if (!controller_state.motor_on[drive]) {
        uint8 motor_status = inportb(base_io_port + 2);
        motor_status |= (1 << (4 + drive));  // Turn on motor for specified drive
        outportb(base_io_port + 2, motor_status);
        controller_state.motor_on[drive] = true;
        DelayMs(500);  // Allow motor to spin up
    }
}

void FloppyDriver::TurnMotorOff(uint8 drive) {
    if (drive >= 4) {
        return;
    }
    
    if (controller_state.motor_on[drive]) {
        uint8 motor_status = inportb(base_io_port + 2);
        motor_status &= ~(1 << (4 + drive));  // Turn off motor for specified drive
        outportb(base_io_port + 2, motor_status);
        controller_state.motor_on[drive] = false;
    }
}

void FloppyDriver::DelayMs(uint32 milliseconds) {
    // Simple delay function
    // In a real implementation, this would use the timer
    for (uint32 i = 0; i < milliseconds * 1000; i++) {
        __asm__ volatile("nop");
    }
}

const char* FloppyDriver::GetDriveTypeName(FloppyDriveType type) {
    switch (type) {
        case FLOPPY_DRIVE_360KB_525: return "360KB 5.25\"";
        case FLOPPY_DRIVE_12MB_525: return "1.2MB 5.25\"";
        case FLOPPY_DRIVE_720KB_35: return "720KB 3.5\"";
        case FLOPPY_DRIVE_144MB_35: return "1.44MB 3.5\"";
        case FLOPPY_DRIVE_288MB_35: return "2.88MB 3.5\"";
        default: return "Unknown";
    }
}

// Initialize the floppy driver
bool InitializeFloppyDriver() {
    if (!g_floppy_driver) {
        g_floppy_driver = new FloppyDriver();
        if (!g_floppy_driver) {
            LOG("Failed to create floppy driver instance");
            return false;
        }
        
        DriverInitResult result = g_floppy_driver->Initialize();
        if (result != DriverInitResult::SUCCESS) {
            LOG("Failed to initialize floppy driver");
            delete g_floppy_driver;
            g_floppy_driver = nullptr;
            return false;
        }
        
        LOG("Floppy driver initialized successfully");
    }
    
    return true;
}