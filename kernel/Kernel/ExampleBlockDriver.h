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
    uint8_t* simulated_disk;   // Simulated disk storage
    uint32_t disk_size;        // Size of simulated disk in bytes

public:
    ExampleBlockDriver(const char* driver_name, const char* driver_version, 
                       uint32_t vid = 0, uint32_t did = 0, uint32_t irq = 0);

    virtual ~ExampleBlockDriver();

    // Implement required virtual functions
    virtual DriverInitResult Initialize() override;
    virtual int Shutdown() override;
    virtual int HandleInterrupt() override;
    virtual int ProcessIoRequest(IoRequest* request) override;

    // Override BlockDeviceDriver functions as needed
    virtual uint32_t ReadBlocks(uint32_t start_block, uint32_t num_blocks, void* buffer) override;
    virtual uint32_t WriteBlocks(uint32_t start_block, uint32_t num_blocks, const void* buffer) override;
};

#endif // EXAMPLEBLOCKDRIVER_H