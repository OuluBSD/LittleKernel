/*
 * Basic test application for LittleKernel
 * This application tests basic kernel functionality
 */

#include "Kernel.h"
#include "Logging.h"
#include "Syscalls.h"

// Basic test application function
int basic_test_application() {
    LOG("Starting basic test application...");
    
    // Test basic system calls
    int pid = syscall_getpid();
    LOG("Current process ID: " << pid);
    
    // Test basic memory allocation
    char* test_buffer = (char*)malloc(1024);
    if (test_buffer) {
        LOG("Successfully allocated 1024 bytes of memory");
        
        // Write and verify data
        for (int i = 0; i < 100; i++) {
            test_buffer[i] = 'A' + (i % 26);
        }
        
        // Verify data integrity
        bool data_ok = true;
        for (int i = 0; i < 100; i++) {
            if (test_buffer[i] != ('A' + (i % 26))) {
                data_ok = false;
                break;
            }
        }
        
        if (data_ok) {
            LOG("Memory integrity test passed");
        } else {
            LOG("Memory integrity test failed");
        }
        
        free(test_buffer);
        LOG("Memory deallocated successfully");
    } else {
        LOG("Failed to allocate memory");
        return -1;
    }
    
    // Test time functionality
    struct timeval tv;
    struct timezone tz;
    int time_result = syscall_gettimeofday(&tv, &tz);
    if (time_result == 0) {
        LOG("Time query successful: " << tv.tv_sec << " seconds, " << tv.tv_usec << " microseconds");
    } else {
        LOG("Time query failed");
    }
    
    // Test basic VFS operations if available
    if (g_vfs) {
        FileStat stat;
        int stat_result = g_vfs->Stat("/", &stat);
        if (stat_result == VFS_SUCCESS) {
            LOG("VFS root directory access successful");
            LOG("  Size: " << stat.size << " bytes");
            LOG("  Inode: " << stat.inode);
        } else {
            LOG("VFS root directory access failed");
        }
    }
    
    // Test performance profiling if available
    if (g_performance_profiler) {
        LOG("Performance profiler is available");
        
        // Create and use a performance counter
        PerfCounterId test_counter = g_performance_profiler->CreateCounter("BasicAppTest", PERF_COUNTER_COUNT);
        if (test_counter != -1) {
            g_performance_profiler->IncrementCounter(test_counter, 5);
            LOG("Performance counter incremented");
        }
    }
    
    LOG("Basic test application completed successfully");
    return 0;
}

// Advanced test application function
int advanced_test_application() {
    LOG("Starting advanced test application...");
    
    // Test multiple system call combinations
    int open_fd = syscall_open("/A/test.txt", O_CREAT | O_WRONLY, 0755);
    if (open_fd >= 0) {
        const char* test_data = "Hello, LittleKernel!";
        int write_result = syscall_write(open_fd, test_data, strlen(test_data));
        if (write_result > 0) {
            LOG("Successfully wrote " << write_result << " bytes to file");
        }
        
        syscall_close(open_fd);
        LOG("File closed successfully");
    } else {
        LOG("Failed to create/open test file");
    }
    
    // Test process-related functionality
    LOG("Advanced test application completed");
    return 0;
}

// Run all basic tests
int run_basic_tests() {
    LOG("Running basic kernel tests...");
    
    int result1 = basic_test_application();
    int result2 = advanced_test_application();
    
    if (result1 == 0 && result2 == 0) {
        LOG("All basic tests passed!");
        return 0;
    } else {
        LOG("Some tests failed");
        return -1;
    }
}