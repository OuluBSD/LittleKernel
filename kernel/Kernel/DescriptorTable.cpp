#include "Kernel.h"

DescriptorTable::DescriptorTable() {
    // Initialize the GDT and IDT pointers
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (uint32)gdt;
    
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32)idt;
}

void DescriptorTable::Initialize() {
    // Null descriptor
    SetGdtEntry(0, 0, 0, 0, 0);
    
    // Code segment (kernel, 0x08)
    SetGdtEntry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    
    // Data segment (kernel, 0x10)
    SetGdtEntry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    
    // Code segment (user, 0x18)
    SetGdtEntry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    
    // Data segment (user, 0x20)
    SetGdtEntry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    
    // TSS descriptor (0x28)
    // For now, we'll set it to 0, but in a full implementation, 
    // we would set up a proper TSS (Task State Segment)
    
    // Initialize all IDT entries to zero initially
    for (int i = 0; i < 256; i++) {
        SetIdtEntry(i, 0, 0x08, 0x8E); // Code segment selector, present and interrupt gate
    }
    
    // Initialize the interrupt manager
    interrupt_manager.Initialize();
    
    // Load the GDT and IDT
    LoadGdt();
    LoadIdt();
    
    DLOG("Descriptor tables initialized");
}

void DescriptorTable::SetGdtEntry(int index, uint32 base, uint32 limit, uint8 access, uint8 gran) {
    gdt[index].base_low = (base & 0xFFFF);
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;
    
    gdt[index].limit_low = (limit & 0xFFFF);
    gdt[index].granularity = ((limit >> 16) & 0x0F);
    
    gdt[index].granularity |= (gran & 0xF0);
    gdt[index].access = access;
}

void DescriptorTable::SetIdtEntry(int index, uint32 offset, uint16 selector, uint8 type_attr) {
    idt[index].offset_low = offset & 0xFFFF;
    idt[index].offset_high = (offset >> 16) & 0xFFFF;
    
    idt[index].selector = selector;
    idt[index].zero = 0;
    idt[index].type_attr = type_attr;
}

void DescriptorTable::LoadGdt() {
    LoadGdtAsm((uint32)&gdt_ptr);
}

void DescriptorTable::LoadIdt() {
    LoadIdtAsm((uint32)&idt_ptr);
}