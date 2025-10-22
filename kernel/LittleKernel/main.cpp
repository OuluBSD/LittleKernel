#include "Kernel.h"
#include "GenericOutput.h"
#include "LogStream.h"


extern "C" {

// DEPRECATED - to be removed during kernel rewrite
int multiboot_main_old(struct multiboot *mboot_ptr) {
	
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
	

	LOG("Enabling interrupts");
	EnableInterrupts();
	
	LOG("Enabling paging");
	InitialisePaging();
	
	LOG("Initialising tasking");
	InitialiseTasking();
	
	LOG("Initialising initrd");
	fs_root = InitialiseInitrd(initrd_location);
	
	LOG("Initialising syscalls");
	InitialiseSyscalls();

	SwitchToUserMode();
	
	syscall_MonitorWrite("Hello, user world!\n");
	
	#if 0
	uint32 a = KMemoryAllocate(4);
    uint32 b = KMemoryAllocate(8);
    uint32 c = KMemoryAllocate(8);
    LOG("a: " << a);
    LOG(", b: " << b);
    LOG("\nc: " << c);

    KFree((void*)c);
    KFree((void*)b);
    uint32 d = KMemoryAllocate(12);
    LOG(", d: " << d);
    
    
    a = KMemoryAllocate(1024*1024);
    LOG("\nhuge a: " << a);
    
    global->timer.Init(1);
    #endif
    
	return 0xDEADABBA;
}

// New main function placeholder (to be implemented)
int multiboot_main(struct multiboot *mboot_ptr) {
    LOG("New kernel main function - to be implemented");
    // TODO: Implement new kernel initialization here
    while(1); // For now, just hang until new implementation is ready
}

}
