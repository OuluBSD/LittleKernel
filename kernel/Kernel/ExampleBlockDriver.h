/*
 * ExampleBlockDriver.h - Example implementation of a block device driver
 * Demonstrates the use of the DriverBase inheritance hierarchy
 */

#ifndef EXAMPLEBLOCKDRIVER_H
#define EXAMPLEBLOCKDRIVER_H

#include "DriverBase.h"
#include "Defs.h"

class ExampleBlockDriver : public BlockDeviceDriver {
private:
    uint8* simulated_disk;   // Simulated disk storage
    uint32 disk_size;        // Size of simulated disk in bytes

public:
    ExampleBlockDriver(const char* driver_name, const char* driver_version, 
                       uint32 vid = 0, uint32 did = 0, uint32 irq = 0);

    virtual ~ExampleBlockDriver();

    // Implement required virtual functions
    virtual DriverInitResult Initialize() override;
    virtual int Shutdown() override;
    virtual int HandleInterrupt() override;
    virtual int ProcessIoRequest(IoRequest* request) override;

    // Override BlockDeviceDriver functions as needed
    virtual uint32 ReadBlocks(uint32 start_block, uint32 num_blocks, void* buffer) override;
    virtual uint32 WriteBlocks(uint32 start_block, uint32 num_blocks, const void* buffer) override;
};

#endif // EXAMPLEBLOCKDRIVER_H