#include "Kernel.h"
#include "GenericOutput.h"


extern "C" {


int multiboot_main(struct multiboot *mboot_ptr) {
	
    ResetInterruptHandlers();
    
    // Descriptor table
    global->dt.Init();
    
    
    // All our initialisation calls will go in here.
    MON.Init();
	MON.Clear();
	
	// Initialize serial port
	init_serial();
	
	// Find the location of our initial ramdisk.
	uint32 initrd_location = 0;
	uint32 initrd_end = 0;
	if (mboot_ptr->mods_count > 0) {
		initrd_location = *((uint32*)mboot_ptr->mods_addr);
		initrd_end = *(uint32*)(mboot_ptr->mods_addr+4);
	}
	
	
	InitLinkerVariables(initrd_end);
	
	
	GenericWrite("Enabling interrupts\n");
	EnableInterrupts();
	
	GenericWrite("Enabling paging\n");
	InitialisePaging();
	
	GenericWrite("Initialising tasking\n");
	InitialiseTasking();
	
	GenericWrite("Initialising initrd\n");
	fs_root = InitialiseInitrd(initrd_location);
	
	GenericWrite("Initialising syscalls\n");
	InitialiseSyscalls();

	SwitchToUserMode();
	
	syscall_MonitorWrite("Hello, user world!\n");
	
	#if 0
	uint32 a = KMemoryAllocate(4);
    uint32 b = KMemoryAllocate(8);
    uint32 c = KMemoryAllocate(8);
    GenericWrite("a: ");
    GenericWriteHex(a);
    GenericWrite(", b: ");
    GenericWriteHex(b);
    GenericWrite("\nc: ");
    GenericWriteHex(c);

    KFree((void*)c);
    KFree((void*)b);
    uint32 d = KMemoryAllocate(12);
    GenericWrite(", d: ");
    GenericWriteHex(d).NewLine();
    
    
    a = KMemoryAllocate(1024*1024);
    GenericWrite("\nhuge a: ");
    GenericWriteHex(a);
    GenericWrite("\n");
    
    global->timer.Init(1);
    #endif
    
	return 0xDEADABBA;
}

}