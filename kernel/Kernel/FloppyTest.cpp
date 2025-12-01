/*
 * FloppyTest.cpp - Simple test application for floppy disk driver
 */

#include "Kernel.h"
#include "FloppyDriver.h"
#include "Logging.h"
#include "Vfs.h"

// Test function to verify floppy driver functionality
int TestFloppyDriver() {
    LOG("Starting floppy driver test");
    
    // Check if floppy driver is initialized
    if (!g_floppy_driver) {
        LOG("Floppy driver not initialized");
        return -1;
    }
    
    LOG("Floppy driver is available");
    
    // Test basic functionality
    LOG("Testing basic floppy driver operations");
    
    // Try to read the boot sector (sector 0)
    uint8 boot_sector[FLOPPY_SECTOR_SIZE];
    memset(boot_sector, 0, FLOPPY_SECTOR_SIZE);
    
    LOG("Reading boot sector (LBA 0)");
    uint32 sectors_read = g_floppy_driver->ReadBlocks(0, 1, boot_sector);
    
    if (sectors_read == 1) {
        LOG("Successfully read boot sector");
        
        // Display first 32 bytes of boot sector for verification
        LOG("Boot sector signature:");
        for (int i = 0; i < 32; i++) {
            if (i % 16 == 0) {
                LOG("");
            }
            LOG("0x" << (uint32)boot_sector[i] << " ");
        }
        LOG("");
        
        // Check for common boot signatures
        if (boot_sector[510] == 0x55 && boot_sector[511] == 0xAA) {
            LOG("Boot sector has valid signature (0x55AA)");
        } else {
            LOG("Boot sector signature not found or invalid");
        }
    } else {
        LOG("Failed to read boot sector, read " << sectors_read << " sectors");
        return -1;
    }
    
    // Test writing (if not read-only)
    if (!g_floppy_driver->IsReadOnly()) {
        LOG("Testing write operations");
        
        // Create a test buffer with a pattern
        uint8 test_buffer[FLOPPY_SECTOR_SIZE];
        for (int i = 0; i < FLOPPY_SECTOR_SIZE; i++) {
            test_buffer[i] = (uint8)(i % 256);
        }
        
        // Try to write to a safe location (e.g., sector 100)
        LOG("Writing test pattern to sector 100");
        uint32 sectors_written = g_floppy_driver->WriteBlocks(100, 1, test_buffer);
        
        if (sectors_written == 1) {
            LOG("Successfully wrote test pattern to sector 100");
            
            // Read it back to verify
            uint8 verify_buffer[FLOPPY_SECTOR_SIZE];
            memset(verify_buffer, 0, FLOPPY_SECTOR_SIZE);
            
            LOG("Reading back sector 100 to verify write");
            uint32 sectors_verified = g_floppy_driver->ReadBlocks(100, 1, verify_buffer);
            
            if (sectors_verified == 1) {
                // Verify the data
                bool data_matches = true;
                for (int i = 0; i < FLOPPY_SECTOR_SIZE; i++) {
                    if (test_buffer[i] != verify_buffer[i]) {
                        data_matches = false;
                        break;
                    }
                }
                
                if (data_matches) {
                    LOG("Write verification successful - data matches");
                } else {
                    LOG("Write verification failed - data mismatch");
                }
            } else {
                LOG("Failed to read back sector for verification");
            }
        } else {
            LOG("Failed to write test pattern, wrote " << sectors_written << " sectors");
        }
    } else {
        LOG("Floppy is read-only, skipping write tests");
    }
    
    // Test multiple sector read
    LOG("Testing multi-sector read (10 sectors starting at sector 10)");
    uint8 multi_sector_buffer[FLOPPY_SECTOR_SIZE * 10];
    memset(multi_sector_buffer, 0, FLOPPY_SECTOR_SIZE * 10);
    
    uint32 multi_sectors_read = g_floppy_driver->ReadBlocks(10, 10, multi_sector_buffer);
    LOG("Read " << multi_sectors_read << " sectors in multi-sector read");
    
    if (multi_sectors_read > 0) {
        LOG("Multi-sector read successful");
    } else {
        LOG("Multi-sector read failed");
    }
    
    // Test sector bounds checking
    LOG("Testing sector bounds checking");
    
    // Try to read beyond the end of the disk
    uint8 bounds_test_buffer[FLOPPY_SECTOR_SIZE];
    uint32 bounds_sectors_read = g_floppy_driver->ReadBlocks(FLOPPY_TOTAL_SECTORS, 1, bounds_test_buffer);
    
    if (bounds_sectors_read == 0) {
        LOG("Bounds checking working correctly - rejected read beyond disk");
    } else {
        LOG("Bounds checking failed - allowed read beyond disk");
    }
    
    LOG("Floppy driver test completed");
    return 0;
}

// Main test function called by kernel
extern "C" int run_floppy_tests() {
    return TestFloppyDriver();
}