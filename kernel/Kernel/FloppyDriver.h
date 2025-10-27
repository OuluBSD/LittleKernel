/*
 * FloppyDriver.h - Floppy disk driver for LittleKernel OS
 * Implements support for 1.44MB floppy disks through QEMU fda parameter
 */

#ifndef FLOPPYDRIVER_H
#define FLOPPYDRIVER_H

#include "DriverBase.h"
#include "Defs.h"
#include "FloppyConstants.h"

// Floppy disk constants
#define FLOPPY_SECTOR_SIZE 512
#define FLOPPY_TRACKS 80
#define FLOPPY_HEADS 2
#define FLOPPY_SECTORS_PER_TRACK 18
#define FLOPPY_TOTAL_SECTORS (FLOPPY_TRACKS * FLOPPY_HEADS * FLOPPY_SECTORS_PER_TRACK)  // 2880 sectors
#define FLOPPY_DISK_SIZE (FLOPPY_TOTAL_SECTORS * FLOPPY_SECTOR_SIZE)  // 1474560 bytes (1.44MB)

// Floppy controller I/O ports
#define FDC_BASE_PORT 0x3F0
#define FDC_STATUS_REG_A 0x3F0
#define FDC_STATUS_REG_B 0x3F1
#define FDC_DIGITAL_OUTPUT_REG 0x3F2
#define FDC_TAPE_DRIVE_REG 0x3F3
#define FDC_MAIN_STATUS_REG 0x3F4
#define FDC_DATARATE_SELECT_REG 0x3F4
#define FDC_DATA_FIFO 0x3F5
#define FDC_DIGITAL_INPUT_REG 0x3F7
#define FDC_CONFIG_CONTROL_REG 0x3F7

// Floppy controller commands
#define FDC_CMD_READ_TRACK 0x02
#define FDC_CMD_SPECIFY 0x03
#define FDC_CMD_SENSE_DRIVE_STATUS 0x04
#define FDC_CMD_WRITE_DATA 0x05
#define FDC_CMD_READ_DATA 0x06
#define FDC_CMD_RECALIBRATE 0x07
#define FDC_CMD_SENSE_INTERRUPT 0x08
#define FDC_CMD_WRITE_DELETED_DATA 0x09
#define FDC_CMD_READ_ID 0x0A
#define FDC_CMD_READ_DELETED_DATA 0x0C
#define FDC_CMD_FORMAT_TRACK 0x0D
#define FDC_CMD_DUMPREG 0x0E
#define FDC_CMD_SEEK 0x0F
#define FDC_CMD_VERSION 0x10
#define FDC_CMD_SCAN_EQUAL 0x11
#define FDC_CMD_PERPENDICULAR_MODE 0x12
#define FDC_CMD_CONFIGURE 0x13
#define FDC_CMD_UNLOCK 0x14
#define FDC_CMD_LOCK 0x94
#define FDC_CMD_VERIFY 0x16
#define FDC_CMD_SCAN_LOW_OR_EQUAL 0x19
#define FDC_CMD_SCAN_HIGH_OR_EQUAL 0x1D

// Floppy controller status bits
#define FDC_STATUS_BUSY 0x10
#define FDC_STATUS_DMA 0x20
#define FDC_STATUS_DIRECTION 0x40
#define FDC_STATUS_READY 0x80

// Floppy drive types (already defined in FloppyConstants.h)
// enum FloppyDriveType {
//     FLOPPY_DRIVE_NONE = 0,
//     FLOPPY_DRIVE_360KB_525 = 1,
//     FLOPPY_DRIVE_12MB_525 = 2,
//     FLOPPY_DRIVE_720KB_35 = 3,
//     FLOPPY_DRIVE_144MB_35 = 4,
//     FLOPPY_DRIVE_288MB_35 = 5
// };

// Floppy controller state
struct FloppyControllerState {
    uint8 current_drive;
    bool motor_on[4];           // Motor state for each drive (A:..D:)
    uint8 step_rate;          // Step rate for stepping motor
    uint8 head_load_time;     // Head load time
    uint8 head_unload_time;   // Head unload time
    uint8 dma_mode;           // DMA mode (1 = enabled)
    FloppyDriveType drive_types[4];  // Drive types for A:..D:
    uint8 cylinders[4];       // Current cylinder positions
    uint8 heads[4];           // Current head positions
    uint8 sectors[4];          // Current sector positions
    bool recalibrating[4];      // Recalibration status
    bool seeking[4];            // Seeking status
    uint32 disk_image_size;   // Size of disk image file
    void* disk_image_data;      // Pointer to disk image data (for QEMU fda support)
};

// Floppy disk driver class
class FloppyDriver : public BlockDeviceDriver {
private:
    FloppyControllerState controller_state;
    uint16_t base_io_port;      // Base I/O port for floppy controller
    uint8 irq_line;           // IRQ line for floppy controller
    uint8 dma_channel;        // DMA channel for floppy controller
    char disk_image_path[256];  // Path to disk image file (for QEMU support)
    bool qemu_mode;             // Whether we're running in QEMU with fda parameter

public:
    FloppyDriver(const char* driver_name = "FloppyDriver", 
                 const char* driver_version = "1.0.0",
                 uint32 vid = 0, uint32 did = 0, uint32 irq = 6);
    
    virtual ~FloppyDriver();

    // Implement required virtual functions
    virtual DriverInitResult Initialize() override;
    virtual int Shutdown() override;
    virtual int HandleInterrupt() override;
    virtual int ProcessIoRequest(IoRequest* request) override;

    // Override BlockDeviceDriver functions
    virtual uint32 ReadBlocks(uint32 start_block, uint32 num_blocks, void* buffer) override;
    virtual uint32 WriteBlocks(uint32 start_block, uint32 num_blocks, const void* buffer) override;
    
    // Floppy-specific functions
    bool InitializeQemuMode();
    bool InitializeHardwareMode();
    bool DetectFloppyDrives();
    bool CalibrateDrive(uint8 drive);
    bool SeekToSector(uint8 drive, uint8 cylinder, uint8 head, uint8 sector);
    bool ReadSector(uint8 drive, uint8 cylinder, uint8 head, uint8 sector, void* buffer);
    bool WriteSector(uint8 drive, uint8 cylinder, uint8 head, uint8 sector, const void* buffer);
    bool WaitForIrq();
    bool WaitForRdy();
    uint8 ReadFdcStatus();
    void WriteFdcCommand(uint8 command);
    uint8 ReadFdcData();
    void WriteFdcData(uint8 data);
    void SendByteToFdc(uint8 byte);
    uint8 ReceiveByteFromFdc();
    bool ResetController();
    void ConfigureController();
    void TurnMotorOn(uint8 drive);
    void TurnMotorOff(uint8 drive);
    void DelayMs(uint32 milliseconds);
    
    // QEMU-specific functions
    bool LoadDiskImage(const char* image_path);
    bool SaveDiskImage(const char* image_path);
    bool ReadSectorFromImage(uint32 sector, void* buffer);
    bool WriteSectorToImage(uint32 sector, const void* buffer);
    
    // Utility functions
    void LbaToChs(uint32 lba, uint8* cylinder, uint8* head, uint8* sector);
    uint32 ChsToLba(uint8 cylinder, uint8 head, uint8 sector);
    bool IsSectorValid(uint32 sector);
    const char* GetDriveTypeName(FloppyDriveType type);
};

// Global floppy driver instance
extern FloppyDriver* g_floppy_driver;

// Initialize the floppy driver
bool InitializeFloppyDriver();

#endif // FLOPPYDRIVER_H