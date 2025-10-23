#ifndef _Kernel_Interrupts_h_
#define _Kernel_Interrupts_h_

// Don't include other headers in this file - only the package header should include other headers

// Forward declarations
struct Registers;
struct Spinlock;

// Interrupt Request (IRQ) mappings
#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

// Function pointer type for interrupt handlers
typedef void (*IrqHandler)(Registers regs);

class InterruptManager {
private:
    IrqHandler handlers[256];
    Spinlock lock;
    
public:
    InterruptManager();
    void Initialize();
    void Enable();
    void Disable();
    void SetHandler(uint8 interrupt, IrqHandler handler);
    void UnsetHandler(uint8 interrupt);
    IrqHandler GetHandler(uint8 interrupt);
    
    // Called by assembly interrupt handlers
    void HandleException(Registers regs);
    void HandleIrq(Registers regs);
};

// Assembly functions called from C/C++
extern "C" {
    void EnableInterrupts();
    void DisableInterrupts();
    void isr_handler(Registers regs);
    void irq_handler(Registers regs);
}

// Specific interrupt handlers
void TimerIrqHandler(Registers regs);
void KeyboardIrqHandler(Registers regs);
void PageFaultHandler(Registers regs);

#endif