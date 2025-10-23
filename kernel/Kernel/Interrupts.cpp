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
    
    // In a real implementation, we would process the keyboard input
    // For now, just log it
    LOG("Keyboard scan code: " << (uint32)scan_code);
}