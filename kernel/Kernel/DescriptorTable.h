#ifndef _Kernel_DescriptorTable_h_
#define _Kernel_DescriptorTable_h_

// Don't include other headers in this file - only the package header should include other headers

// Forward declarations
struct Registers;
class InterruptManager;

struct GdtEntry {
    uint16 limit_low;     // Limit bits 0-15
    uint16 base_low;      // Base bits 0-15
    uint8 base_middle;    // Base bits 16-23
    uint8 access;         // Access byte
    uint8 granularity;    // Granularity byte
    uint8 base_high;      // Base bits 24-31
} __attribute__((packed));

struct GdtPtr {
    uint16 limit;         // Limit of the GDT
    uint32 base;          // Base address of the GDT
} __attribute__((packed));

struct IdtEntry {
    uint16 offset_low;    // Offset bits 0-15
    uint16 selector;      // Code segment selector
    uint8 zero;           // Always zero
    uint8 type_attr;      // Type and attributes
    uint16 offset_high;   // Offset bits 16-31
} __attribute__((packed));

struct IdtPtr {
    uint16 limit;         // Limit of the IDT
    uint32 base;          // Base address of the IDT
} __attribute__((packed));

class DescriptorTable {
public:
    GdtEntry gdt[6];      // GDT with 6 entries
    IdtEntry idt[256];    // IDT with 256 entries
    GdtPtr gdt_ptr;
    IdtPtr idt_ptr;
    
    InterruptManager interrupt_manager; // Our interrupt manager
    
    DescriptorTable();
    void Initialize();
    void SetGdtEntry(int index, uint32 base, uint32 limit, uint8 access, uint8 gran);
    void SetIdtEntry(int index, uint32 offset, uint16 selector, uint8 type_attr);
    
    // Assembly functions to load the tables
    void LoadGdt();
    void LoadIdt();
};

// Assembly functions
extern "C" void LoadGdtAsm(uint32 gdt_ptr);
extern "C" void LoadIdtAsm(uint32 idt_ptr);
extern "C" void LoadTss(int32 tss_selector);

#endif