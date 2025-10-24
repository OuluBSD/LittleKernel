#include "Kernel.h"

InterruptManager::InterruptManager() {
    lock.Initialize();
    for (int i = 0; i < 256; i++) {
        handlers[i] = nullptr;
    }
}

void InterruptManager::Initialize() {
    lock.Acquire();
    for (int i = 0; i < 256; i++) {
        handlers[i] = nullptr;
    }
    lock.Release();
    
    DLOG("Interrupt manager initialized");
}

void InterruptManager::Enable() {
    EnableInterrupts();
}

void InterruptManager::Disable() {
    DisableInterrupts();
}

void InterruptManager::SetHandler(uint8 interrupt, IrqHandler handler) {
    lock.Acquire();
    handlers[interrupt] = handler;
    lock.Release();
}

void InterruptManager::UnsetHandler(uint8 interrupt) {
    lock.Acquire();
    handlers[interrupt] = nullptr;
    lock.Release();
}

IrqHandler InterruptManager::GetHandler(uint8 interrupt) {
    lock.Acquire();
    IrqHandler handler = handlers[interrupt];
    lock.Release();
    return handler;
}

void InterruptManager::HandleException(Registers regs) {
    IrqHandler handler = GetHandler(regs.int_no);
    if (handler) {
        handler(regs);
    } else {
        LOG("Unhandled interrupt: " << regs.int_no);
    }
}

void InterruptManager::HandleIrq(Registers regs) {
    // Send end-of-interrupt signal to PIC
    if (regs.int_no >= 40) {  // IRQs 8-15 come from slave PIC
        outportb(0xA0, 0x20); // Send EOI to slave
    }
    outportb(0x20, 0x20);     // Send EOI to master
    
    // Handle the interrupt
    IrqHandler handler = GetHandler(regs.int_no);
    if (handler) {
        handler(regs);
    }
}

extern "C" void isr_handler(Registers regs) {
    global->descriptor_table->interrupt_manager.HandleException(regs);
}

extern "C" void irq_handler(Registers regs) {
    global->descriptor_table->interrupt_manager.HandleIrq(regs);
}

extern "C" void EnableInterrupts() {
    __asm__ volatile("sti");
}

extern "C" void DisableInterrupts() {
    __asm__ volatile("cli");
}

// Specific interrupt handlers
void TimerIrqHandler(Registers regs) {
    global->timer->Tick();
    
    // Call the scheduler if it exists
    if (process_manager) {
        process_manager->Schedule();
    }
}

void KeyboardIrqHandler(Registers regs) {
    // Handle keyboard input
    uint8 scan_code = inportb(0x60);
    
    // Find the keyboard driver instance and process the scancode
    if (global && global->driver_framework) {
        Device* keyboard_dev = global->driver_framework->FindDeviceByType(DEVICE_TYPE_KEYBOARD);
        if (keyboard_dev && keyboard_dev->private_data) {
            KeyboardDriver* keyboard_driver = (KeyboardDriver*)keyboard_dev->private_data;
            keyboard_driver->ProcessScancode(scan_code);
        }
    }
    
    // Send End of Interrupt (EOI) to PIC
    outportb(0x20, 0x20);
}

void MouseIrqHandler(Registers regs) {
    // Handle mouse input
    uint8 data = inportb(0x60);
    
    // Find the mouse driver instance and process the data
    if (global && global->driver_framework) {
        Device* mouse_dev = global->driver_framework->FindDeviceByType(DEVICE_TYPE_MOUSE);
        if (mouse_dev && mouse_dev->private_data) {
            MouseDriver* mouse_driver = (MouseDriver*)mouse_dev->private_data;
            
            // Build mouse packet byte by byte
            if (mouse_driver->packet_byte_index < 3) {
                mouse_driver->packet_bytes[mouse_driver->packet_byte_index] = data;
                mouse_driver->packet_byte_index++;
                
                // If we have a complete 3-byte packet, process it
                if (mouse_driver->packet_byte_index == 3) {
                    mouse_driver->ProcessPacket();
                }
            }
        }
    }
    
    // Send End of Interrupt (EOI) to PIC
    outportb(0xA0, 0x20); // Slave PIC
    outportb(0x20, 0x20); // Master PIC
}

void PageFaultHandler(Registers regs) {
    // Get the faulting address from CR2 register
    uint32 faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
    
    // Extract error code information
    uint32 present = regs.err_code & 0x1;         // Page not present
    uint32 write = regs.err_code & 0x2;           // Write operation?
    uint32 user = regs.err_code & 0x4;            // User mode?
    uint32 reserved = regs.err_code & 0x8;        // Reserved bit set?
    uint32 id_fetch = regs.err_code & 0x10;       // Instruction fetch?
    
    LOG("Page fault at address: 0x" << faulting_address);
    LOG("Error code details:");
    LOG("  Present bit: " << (present ? "set (protection violation)" : "not set (page not present)"));
    LOG("  Write bit: " << (write ? "write" : "read"));
    LOG("  User bit: " << (user ? "user mode" : "supervisor mode"));
    LOG("  Reserved bit: " << (reserved ? "set (reserved bit violation)" : "not set"));
    LOG("  Instruction fetch: " << (id_fetch ? "yes" : "no"));
    
    if (!present) {
        // Page not present - this is a demand paging opportunity
        if (global && global->paging_manager && global->memory_manager) {
            // Check if this address is within a valid memory mapping for the current process
            ProcessControlBlock* current_process = nullptr;
            if (process_manager) {
                current_process = process_manager->GetCurrentProcess();
            }
            
            if (current_process) {
                // Check if this is a memory-mapped file or a valid part of process memory
                bool handled = false;
                
                // For memory-mapped files, we would check if the address falls in a mapped region
                if (global->memory_mapping_manager) {
                    // Check if the faulting address is in a memory-mapped file region
                    // This is a simplified check - in reality, we'd need to check all mappings for this process
                    MemoryMappedFile* mapping = nullptr; // Find by address range
                    // This is complex to implement without a proper mapping lookup
                    // We'll implement a basic version for now by checking if it's in a reasonable range
                    if (faulting_address >= 0x50000000 && faulting_address < 0xA0000000) {
                        // Likely a memory-mapped file or dynamically allocated area
                        // Allocate a page and map it
                        void* new_page = global->memory_manager->AllocatePage();
                        if (new_page) {
                            uint32 new_page_phys = VirtualToPhysical(new_page);
                            
                            // Zero the page
                            memset(new_page, 0, PAGE_SIZE);
                            
                            // Map the page in the current process's page directory
                            uint32 page_vaddr = faulting_address & PAGE_MASK;
                            uint32 page_flags = PAGE_PRESENT | PAGE_USER;
                            
                            if (write) {
                                page_flags |= PAGE_WRITABLE;
                            }
                            
                            if (global->paging_manager->MapPage(page_vaddr, new_page_phys, page_flags, current_process->page_directory)) {
                                LOG("Demand paging: Allocated and mapped page for address 0x" << page_vaddr);
                                handled = true;
                            } else {
                                LOG("Demand paging: Failed to map page");
                                global->memory_manager->FreePage(new_page);
                            }
                        } else {
                            LOG("Demand paging: Failed to allocate physical page");
                        }
                    }
                }
                
                if (!handled) {
                    LOG("Unhandled page fault for address: 0x" << faulting_address << " in process PID: " << current_process->pid);
                    
                    // In a real system, we might want to kill this process
                    // For now, we'll just log it
                }
            } else {
                LOG("Page fault occurred but no current process is set");
            }
        }
    } else {
        // Protection violation - potentially accessing memory without proper permissions
        LOG("Protection fault at address: 0x" << faulting_address);
        
        // Determine appropriate response - potentially kill the process
        ProcessControlBlock* current_process = nullptr;
        if (process_manager) {
            current_process = process_manager->GetCurrentProcess();
        }
        
        if (current_process) {
            LOG("Process PID " << current_process->pid << " caused protection fault");
        }
    }
    
    LOG("Page fault processed");
}