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
    
    // Set initialized flag
    initialized = true;
    
    LOG("Global system initialized");
}