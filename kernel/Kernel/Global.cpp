#include "Kernel.h"

// Global variable definition
Global* global = nullptr;

void Global::Initialize() {
    // Allocate the global structure itself
    // In a real kernel, this would be done carefully to ensure it's in the right memory space
    if (global == nullptr) {
        // For now, we'll assume it's already allocated, which would happen during kernel boot
        // This is a simplified implementation
        global = (Global*)malloc(sizeof(Global));
    }
    
    // Initialize all subsystems
    monitor = new Monitor();
    monitor->Initialize();
    
    timer = new Timer();
    timer->Initialize();
    
    descriptor_table = new DescriptorTable();
    descriptor_table->Initialize();
    
    memory_manager = new MemoryManager();
    memory_manager->Initialize();
    
    // Initialize SerialDriver
    serial_driver = new SerialDriver();
    serial_driver->Initialize();
    
    // Initialize PagingManager
    paging_manager = new PagingManager();
    
    // Initialize SharedMemoryManager
    shared_memory_manager = new SharedMemoryManager();
    
    // Initialize MemoryMappingManager
    memory_mapping_manager = new MemoryMappingManager();
    
    // Set initialized flag
    initialized = true;
    
    LOG("Global system initialized");
}