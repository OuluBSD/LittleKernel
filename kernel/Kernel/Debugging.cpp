#include "Kernel.h"
#include "Debugging.h"

// Global debugger instance
KernelDebugger* g_kernel_debugger = nullptr;

KernelDebugger::KernelDebugger() {
    breakpoint_count = 0;
    active_debug_flags = DEBUG_FLAG_NONE;
    debugger_enabled = false;
    debugger_lock.Initialize();
    
    // Initialize all breakpoints
    for (int i = 0; i < MAX_BREAKPOINTS; i++) {
        breakpoints[i].address = nullptr;
        breakpoints[i].type = BP_EXECUTION;
        breakpoints[i].length = 1;
        breakpoints[i].enabled = false;
        breakpoints[i].hit_count = 0;
        breakpoints[i].description = nullptr;
    }
}

KernelDebugger::~KernelDebugger() {
    // Debugger cleanup
}

bool KernelDebugger::Initialize() {
    LOG("Initializing kernel debugger");
    
    // Basic initialization
    debugger_enabled = true;
    active_debug_flags = DEBUG_FLAG_NONE;
    
    LOG("Kernel debugger initialized successfully");
    return true;
}

void KernelDebugger::Enable(bool enable) {
    debugger_lock.Acquire();
    debugger_enabled = enable;
    debugger_lock.Release();
}

void KernelDebugger::SetDebugFlags(uint32_t flags) {
    debugger_lock.Acquire();
    active_debug_flags = flags;
    debugger_lock.Release();
}

void KernelDebugger::AddDebugFlag(uint32_t flag) {
    debugger_lock.Acquire();
    active_debug_flags |= flag;
    debugger_lock.Release();
}

void KernelDebugger::RemoveDebugFlag(uint32_t flag) {
    debugger_lock.Acquire();
    active_debug_flags &= ~flag;
    debugger_lock.Release();
}

int KernelDebugger::SetBreakpoint(void* address, BreakpointType type, uint32_t length, const char* description) {
    debugger_lock.Acquire();
    
    int slot = FindFreeBreakpointSlot();
    if (slot == -1) {
        debugger_lock.Release();
        return -1;  // No free slots
    }
    
    breakpoints[slot].address = address;
    breakpoints[slot].type = type;
    breakpoints[slot].length = length;
    breakpoints[slot].enabled = true;
    breakpoints[slot].hit_count = 0;
    breakpoints[slot].description = description;
    
    // Insert actual breakpoint instruction at the address if it's an execution breakpoint
    // For now, we just track it; in a real implementation, we'd modify memory
    if (type == BP_EXECUTION) {
        // In a real implementation, we'd replace the instruction with an INT3 (0xCC)
        LOG("Execution breakpoint set at 0x" << (uint32)address);
    }
    
    debugger_lock.Release();
    return slot;
}

bool KernelDebugger::RemoveBreakpoint(int bp_id) {
    if (bp_id < 0 || bp_id >= MAX_BREAKPOINTS) {
        return false;
    }
    
    debugger_lock.Acquire();
    
    if (!breakpoints[bp_id].enabled) {
        debugger_lock.Release();
        return false;  // Breakpoint not active
    }
    
    // Remove the breakpoint
    breakpoints[bp_id].enabled = false;
    breakpoints[bp_id].address = nullptr;
    
    // Restore original instruction if it was an execution breakpoint
    // In a real implementation, we'd restore the original instruction
    
    debugger_lock.Release();
    return true;
}

bool KernelDebugger::RemoveBreakpointAtAddress(void* address) {
    int bp_id = FindBreakpoint(address);
    if (bp_id == -1) {
        return false;
    }
    
    return RemoveBreakpoint(bp_id);
}

bool KernelDebugger::EnableBreakpoint(int bp_id) {
    if (bp_id < 0 || bp_id >= MAX_BREAKPOINTS) {
        return false;
    }
    
    debugger_lock.Acquire();
    breakpoints[bp_id].enabled = true;
    debugger_lock.Release();
    
    return true;
}

bool KernelDebugger::DisableBreakpoint(int bp_id) {
    if (bp_id < 0 || bp_id >= MAX_BREAKPOINTS) {
        return false;
    }
    
    debugger_lock.Acquire();
    breakpoints[bp_id].enabled = false;
    debugger_lock.Release();
    
    return true;
}

Breakpoint* KernelDebugger::GetBreakpoint(int bp_id) {
    if (bp_id < 0 || bp_id >= MAX_BREAKPOINTS) {
        return nullptr;
    }
    
    return &breakpoints[bp_id];
}

int KernelDebugger::FindBreakpoint(void* address) {
    for (int i = 0; i < MAX_BREAKPOINTS; i++) {
        if (breakpoints[i].enabled && breakpoints[i].address == address) {
            return i;
        }
    }
    
    return -1;  // Not found
}

bool KernelDebugger::HasBreakpoint(void* address) {
    return FindBreakpoint(address) != -1;
}

bool KernelDebugger::ReadMemory(void* address, void* buffer, uint32_t size) {
    if (!address || !buffer || size == 0) {
        return false;
    }
    
    try {
        // For safety, we should validate the address is in kernel space
        // For now, just do a basic copy
        memcpy(buffer, address, size);
        return true;
    }
    catch (...) {
        return false;
    }
}

bool KernelDebugger::WriteMemory(void* address, const void* buffer, uint32_t size) {
    if (!address || !buffer || size == 0) {
        return false;
    }
    
    try {
        // For safety, we should validate the address is in kernel space
        // For now, just do a basic copy
        memcpy(address, buffer, size);
        return true;
    }
    catch (...) {
        return false;
    }
}

void KernelDebugger::DumpMemory(void* address, uint32_t size, MemoryDumpFlags flags) {
    if (!address || size == 0) {
        LOG("Cannot dump memory: invalid address or size");
        return;
    }
    
    LOG("Memory dump at 0x" << (uint32)address << ", size: " << size << " bytes");
    
    uint8_t* addr = (uint8_t*)address;
    char line[100];
    char ascii[17];
    
    for (uint32_t i = 0; i < size; i += 16) {
        uint32_t j;
        snprintf(line, sizeof(line), "%08X: ", (uint32)(addr + i));
        
        // Hex part
        for (j = 0; j < 16 && (i + j) < size; j++) {
            snprintf(line + strlen(line), sizeof(line) - strlen(line), "%02X ", addr[i + j]);
            ascii[j] = (addr[i + j] >= 32 && addr[i + j] < 127) ? addr[i + j] : '.';
        }
        
        // Pad hex part if needed
        for (; j < 16; j++) {
            strcat(line, "   ");
            ascii[j] = ' ';
        }
        
        ascii[16] = '\0';
        
        if (flags & MEM_DUMP_HEX) {
            LOG(line);
        }
        
        if (flags & MEM_DUMP_ASCII) {
            LOG("ASCII: " << ascii);
        }
        
        if (flags == MEM_DUMP_BOTH) {
            LOG(line << "ASCII: " << ascii);
        }
    }
}

void KernelDebugger::DumpRegisters() {
    LOG("Register dump not implemented yet - would show CPU registers");
    // In a real implementation, this would show CPU registers
}

void KernelDebugger::PrintStackTrace() {
    LOG("Stack trace:");
    
    // This is a simplified implementation
    // In a real kernel, we'd walk the stack frames using frame pointers
    
    // For x86, typical frame pointer is stored at EBP
    void** frame_ptr = (void**)get_frame_pointer();  // We'd need to implement this
    
    for (uint32_t i = 0; i < 16; i++) {  // Print up to 16 stack frames
        if (frame_ptr && is_kernel_address(frame_ptr) && is_kernel_address(frame_ptr[1])) {
            LOG("  [" << i << "] 0x" << (uint32)frame_ptr[1]);  // Return address is at [1]
            
            // Move to next frame
            void** next_frame = (void**)frame_ptr[0];
            if (next_frame <= frame_ptr) {  // Prevent infinite loop
                break;
            }
            frame_ptr = next_frame;
        } else {
            break;
        }
    }
}

uint32_t KernelDebugger::GetStackTrace(StackFrame* frames, uint32_t max_frames) {
    if (!frames || max_frames == 0) {
        return 0;
    }
    
    // Simplified implementation - in reality would need to walk stack properly
    LOG("Stack trace implementation would go here");
    return 0;
}

void KernelDebugger::DumpProcessList() {
    if (!process_manager) {
        LOG("Process manager not available for process dump");
        return;
    }
    
    LOG("Process List:");
    process_manager->PrintProcessList();
}

void KernelDebugger::DumpProcessInfo(ProcessControlBlock* pcb) {
    if (!pcb) {
        LOG("Invalid process control block");
        return;
    }
    
    LOG("Process Information:");
    LOG("  PID: " << pcb->pid);
    LOG("  Name: " << pcb->name);
    LOG("  State: " << pcb->state);
    LOG("  Priority: " << pcb->priority);
    LOG("  Stack Pointer: 0x" << (uint32)pcb->stack_pointer);
    LOG("  Base Pointer: 0x" << (uint32)pcb->base_pointer);
    LOG("  Instruction Pointer: 0x" << (uint32)pcb->instruction_pointer);
}

void KernelDebugger::DumpCurrentProcess() {
    if (!g_current_process) {
        LOG("No current process to dump");
        return;
    }
    
    DumpProcessInfo(g_current_process);
}

void KernelDebugger::DumpSystemState() {
    LOG("=== SYSTEM STATE DUMP ===");
    
    // Process information
    DumpProcessList();
    
    // Memory information
    if (global && global->memory_manager) {
        LOG("Memory Manager State:");
        global->memory_manager->PrintStats();
    }
    
    // Paging information
    if (global && global->paging_manager) {
        LOG("Paging Manager State:");
        // Print paging stats if available
    }
    
    // Timer information
    if (global_timer) {
        LOG("Timer State: Current tick count = " << global_timer->GetTickCount());
    }
    
    LOG("=========================");
}

void KernelDebugger::DumpMemoryLayout() {
    LOG("=== MEMORY LAYOUT ===");
    LOG("Kernel memory layout:");
    LOG("  0x00000000 - 0x000FFFFF: Reserved");
    LOG("  0x00100000 - 0x0FFFFFFF: Kernel space");
    LOG("  0x10000000 - 0xBFFFFFFF: User space (per process)");
    LOG("  0xC0000000 - 0xFFFFFFFF: Shared kernel space");
    
    if (global && global->memory_manager) {
        global->memory_manager->PrintMemoryMap();
    }
    
    LOG("==================");
}

void KernelDebugger::DumpFilesystemState() {
    if (!g_vfs) {
        LOG("VFS not initialized");
        return;
    }
    
    LOG("=== FILE SYSTEM STATE ===");
    // In a full implementation, we'd print information about mounted filesystems
    LOG("Virtual filesystem initialized with root node");
    LOG("========================");
}

void KernelDebugger::DumpDriverState() {
    if (!driver_framework) {
        LOG("Driver framework not initialized");
        return;
    }
    
    LOG("=== DRIVER STATE ===");
    uint32_t device_count = driver_framework->GetDeviceCount();
    LOG("Total registered devices: " << device_count);
    
    if (device_count > 0) {
        Device* device = driver_framework->GetFirstDevice();
        while (device) {
            LOG("  Device ID: " << device->id << ", Name: " << device->name 
                 << ", Type: " << device->type << ", Flags: 0x" << device->flags);
            device = device->next;
        }
    }
    
    LOG("==================");
}

bool KernelDebugger::HandleBreakpoint(void* address) {
    int bp_id = FindBreakpoint(address);
    if (bp_id == -1) {
        return false;
    }
    
    breakpoints[bp_id].hit_count++;
    LogBreakpointHit(bp_id, address);
    
    // Execute the breakpoint action
    ExecuteBreakpointAction(bp_id);
    
    return true;
}

void KernelDebugger::BreakpointTrapHandler() {
    // This would be called from an interrupt handler when a breakpoint is hit
    LOG("Breakpoint trap handler called");
    
    // In a real implementation, we would:
    // 1. Get the address where the breakpoint occurred
    // 2. Find and handle the corresponding breakpoint
    // 3. Potentially enter an interactive debugging mode
    
    // For now, just print a message
    void* current_addr = (void*)get_instruction_pointer();  // Hypothetical function
    LOG("Breakpoint hit at address: 0x" << (uint32)current_addr);
    
    // This might be where we'd enter an interactive debugger
    // For now, just return
}

void KernelDebugger::PrintDebugInfo() {
    LOG("=== KERNEL DEBUG INFO ===");
    LOG("Debug flags: 0x" << active_debug_flags);
    LOG("Breakpoints set: " << breakpoint_count);
    LOG("Debugger enabled: " << (debugger_enabled ? "yes" : "no"));
    LOG("=======================");
}

void KernelDebugger::Panic(const char* message, const char* file, uint32_t line) {
    LOG("!!! KERNEL PANIC !!!");
    LOG("Message: " << message);
    
    if (file) {
        LOG("File: " << file << ", Line: " << line);
    }
    
    // Dump system state for debugging
    DumpSystemState();
    CrashDump();
    
    // In a real kernel, we might try to save state to disk
    // and then halt the system
    
    // Disable interrupts and halt
    asm volatile("cli; hlt");
    
    // If we get here, loop forever
    while (true) {
        asm volatile("hlt");
    }
}

void KernelDebugger::CrashDump() {
    LOG("=== CRASH DUMP ===");
    
    // Dump all relevant system information
    DumpSystemState();
    DumpMemoryLayout();
    DumpProcessList();
    DumpDriverState();
    DumpFilesystemState();
    
    LOG("==================");
}

int KernelDebugger::FindFreeBreakpointSlot() {
    for (int i = 0; i < MAX_BREAKPOINTS; i++) {
        if (!breakpoints[i].enabled) {
            return i;
        }
    }
    
    return -1;  // No free slots
}

void KernelDebugger::ExecuteBreakpointAction(int bp_id) {
    // In a complete debugger, this might pause execution, enter a 
    // debugging mode, or trigger other debugging actions
    LOG("Breakpoint " << bp_id << " hit, action executed");
}

void KernelDebugger::LogBreakpointHit(int bp_id, void* address) {
    if (breakpoints[bp_id].description) {
        LOG("Breakpoint hit: " << breakpoints[bp_id].description 
             << " at 0x" << (uint32)address 
             << ", hit count: " << breakpoints[bp_id].hit_count);
    } else {
        LOG("Breakpoint hit at 0x" << (uint32)address 
             << ", hit count: " << breakpoints[bp_id].hit_count);
    }
}

void KernelDebugger::SanitizeString(char* str, uint32_t max_len) {
    for (uint32_t i = 0; i < max_len && str[i] != '\0'; i++) {
        if (str[i] < 32 || str[i] > 126) {
            str[i] = '?';
        }
    }
}

bool InitializeDebugger() {
    if (!g_kernel_debugger) {
        g_kernel_debugger = new KernelDebugger();
        if (!g_kernel_debugger) {
            LOG("Failed to create kernel debugger instance");
            return false;
        }
        
        if (!g_kernel_debugger->Initialize()) {
            LOG("Failed to initialize kernel debugger");
            delete g_kernel_debugger;
            g_kernel_debugger = nullptr;
            return false;
        }
        
        LOG("Kernel debugger initialized successfully");
    }
    
    return true;
}