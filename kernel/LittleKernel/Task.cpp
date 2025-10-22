#include "Kernel.h"
#include "GenericOutput.h"
#include "LogStream.h"

// Define DLOG macro for debugging tasking system using new LOG macro
#define DLOG(x) LOG("[TASKING] " << x)

void InitialiseTasking() {
	// Rather important stuff happening, no interrupts please!
	asm volatile("cli");
	
	DLOG("InitialiseTasking: Starting tasking initialization");
	
	// Relocate the stack so we know where it is.
	DLOG("InitialiseTasking: Moving stack to 0xE0000000");
	MoveStack((void*)0xE0000000, 0x2000);
	DLOG("InitialiseTasking: Stack move completed");
	
	// Initialise the first task (kernel task)
	DLOG("InitialiseTasking: Initializing first task...");
	auto& ready_queue = global->ready_queue;
	auto& current_task = global->current_task;
	current_task = ready_queue = (Task*)KMemoryAllocate(sizeof(Task));
	
	if (!current_task) {
		DLOG("InitialiseTasking: ERROR - Failed to allocate memory for first task!");
		return;
	}
	DLOG("InitialiseTasking: Successfully allocated first task");
	
	current_task->id = global->next_pid++;
	DLOG("InitialiseTasking: Assigned PID to current task");
	current_task->esp = current_task->ebp = 0;
	current_task->eip = 0;
	current_task->page_directory = global->current_directory;
	current_task->next = 0;
	DLOG("InitialiseTasking: Setting up kernel stack...");
	current_task->kernel_stack = KMemoryAllocateAligned(KERNEL_STACK_SIZE);
	
	if (!current_task->kernel_stack) {
		DLOG("InitialiseTasking: ERROR - Failed to allocate kernel stack!");
		return;
	}
	DLOG("InitialiseTasking: Successfully allocated kernel stack");
	
	DLOG("InitialiseTasking: Tasking initialization completed successfully");
	// Reenable interrupts.
	asm volatile("sti");
}

#if 0  // DEPRECATED - problematic implementation causing page faults - to be re-implemented
void MoveStack(void *new_stack_start, uint32 size) {
	DLOG("MoveStack: Starting stack relocation to 0xE0000000");
	uint32 i;
	uint32 new_stack_start_addr = (uint32)new_stack_start;
	uint32 stack_end = new_stack_start_addr - size;
	
	DLOG("MoveStack: new_stack_start: ");
	GenericWriteHex(new_stack_start_addr);
	DLOG("");
	DLOG("MoveStack: stack_end: ");
	GenericWriteHex(stack_end);
	DLOG("");
	
	DLOG("MoveStack: Calculating pages to allocate...");
	uint32 pages_to_allocate = size / 0x1000;  // Number of full pages
	if (size % 0x1000) pages_to_allocate++;    // Add one more if there's a remainder
	
	DLOG("MoveStack: Total pages to allocate: ");
	GenericWriteDec(pages_to_allocate);
	DLOG("");
	
	// Loop through each page in the range, from high address to low address
	for (i = 0; i < pages_to_allocate; i++) {
		uint32 page_addr = new_stack_start_addr - (i * 0x1000);
		
		// Safety check to make sure we don't go too low
		if (page_addr < stack_end) {
			DLOG("MoveStack: Stopping allocation - reached end of range at: ");
			GenericWriteHex(page_addr);
			DLOG("");
			break;
		}
		
		DLOG("MoveStack: About to process page for address: ");
		GenericWriteHex(page_addr);
		DLOG("");
		
		// Get or create page, with detailed debugging
		Page *page = GetPage(page_addr, 1, global->current_directory); // 1 means create if doesn't exist
		if (!page) {
			DLOG("MoveStack: ERROR - GetPage returned NULL for address: ");
			GenericWriteHex(page_addr);
			DLOG("");
			break;
		}
		
		DLOG("MoveStack: Got page for address: ");
		GenericWriteHex(page_addr);
		DLOG("");
		
		// General-purpose stack is in user-mode.
		AllocFrame(page, 0 /* User mode */, 1 /* Is writable */);
		DLOG("MoveStack: AllocFrame completed for address: ");
		GenericWriteHex(page_addr);
		DLOG("");
	}
	
	DLOG("MoveStack: Flushing TLB...");
	// Flush the TLB by reading and writing the page directory address again.
	uint32 pd_addr;
	asm volatile("mov %%cr3, %0" : "=r"(pd_addr));
	asm volatile("mov %0, %%cr3" : : "r"(pd_addr));

	// Old ESP and EBP, read from registers.
	uint32 old_stack_pointer;
	asm volatile("mov %%esp, %0" : "=r"(old_stack_pointer));
	DLOG("MoveStack: Old stack pointer: ");
	GenericWriteHex(old_stack_pointer);
	DLOG("");
	
	uint32 old_base_pointer;
	asm volatile("mov %%ebp, %0" : "=r"(old_base_pointer));
	DLOG("MoveStack: Old base pointer: ");
	GenericWriteHex(old_base_pointer);

	// Offset to add to old stack addresses to get a new stack address.
	uint32 offset            = (uint32)new_stack_start - global->initial_esp;
	DLOG("MoveStack: Offset: ");
	GenericWriteHex(offset);
	
	// New ESP and EBP.
	uint32 new_stack_pointer = old_stack_pointer + offset;
	uint32 new_base_pointer  = old_base_pointer  + offset;
	
	DLOG("MoveStack: Copying stack...");
	// Copy the stack.
	memcpy((void*)new_stack_pointer, (void*)old_stack_pointer, global->initial_esp - old_stack_pointer);
	
	// Backtrace through the original stack, copying new values into
	// the new stack.
	for (i = (uint32)new_stack_start; i > (uint32)new_stack_start - size; i -= 4) {
		uint32 tmp = * (uint32*)i;
		// If the value of tmp is inside the range of the old stack, assume it is a base pointer
		// and remap it. This will unfortunately remap ANY value in this range, whether they are
		// base pointers or not.
		if ((old_stack_pointer < tmp) && (tmp < global->initial_esp)) {
			tmp = tmp + offset;
			uint32 *tmp2 = (uint32*)i;
			*tmp2 = tmp;
		}
	}
	
	DLOG("MoveStack: Changing stacks...");
	// Change stacks.
	asm volatile("mov %0, %%esp" : : "r"(new_stack_pointer));
	asm volatile("mov %0, %%ebp" : : "r"(new_base_pointer));
	DLOG("MoveStack: Stack relocation completed successfully");
}
#endif

// Placeholder for new MoveStack implementation - currently stubbed
void MoveStack(void *new_stack_start, uint32 size) {
    // TODO: Implement safe stack relocation function
    // This is temporarily disabled due to page fault issues
    // Will be re-implemented during kernel rewrite
    GenericWrite("[TASKING] MoveStack: Function disabled - page fault issues\n");
}

void switch_task() {
	DLOG("switch_task: Entering task switch");
	auto& current_task = global->current_task;
	auto& ready_queue = global->ready_queue;
	auto& current_directory = global->current_directory;
	
	// If we haven't initialised tasking yet, just return.
	if (!current_task) {
		DLOG("switch_task: Tasking not initialized, returning");
		return;
	}
		
	// Read esp, ebp now for saving later on.
	uint32 esp, ebp, eip;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));

	// Read the instruction pointer. We do some cunning logic here:
	// One of two things could have happened when this function exits -
	//   (a) We called the function and it returned the EIP as requested.
	//   (b) We have just switched tasks, and because the saved EIP is essentially
	//       the instruction after read_eip(), it will seem as if read_eip has just
	//       returned.
	// In the second case we need to return immediately. To detect it we put a dummy
	// value in EAX further down at the end of this function. As C returns values in EAX,
	// it will look like the return value is this dummy value! (0x12345).
	eip = read_eip();
	
	// Have we just switched tasks?
	if (eip == 0x12345) {
		DLOG("switch_task: Task switch completed, returning");
		return;
	}
		
	// No, we didn't switch tasks. Let's save some register values and switch.
	current_task->eip = eip;
	current_task->esp = esp;
	current_task->ebp = ebp;
	
	// Get the next task to run.
	current_task = current_task->next;
	// If we fell off the end of the linked list start again at the beginning.
	if (!current_task)
		current_task = ready_queue;
		
	eip = current_task->eip;
	esp = current_task->esp;
	ebp = current_task->ebp;
	
	// Make sure the memory manager knows we've changed page directory.
	current_directory = current_task->page_directory;
	
	// Change our kernel stack over.
	global->dt.SetKernelStack(current_task->kernel_stack + KERNEL_STACK_SIZE);
	
	DLOG("switch_task: About to perform low-level context switch");
	// Here we:
	// * Stop interrupts so we don't get interrupted.
	// * Temporarily put the new EIP location in ECX.
	// * Load the stack and base pointers from the new task struct.
	// * Change page directory to the physical address (physicalAddr) of the new directory.
	// * Put a dummy value (0x12345) in EAX so that above we can recognise that we've just
	//   switched task.
	// * Restart interrupts. The STI instruction has a delay - it doesn't take effect until after
	//   the next instruction.
	// * Jump to the location in ECX (remember we put the new EIP in there).
	asm volatile("         \
			cli;                 \
			mov %0, %%ecx;       \
			mov %1, %%esp;       \
			mov %2, %%ebp;       \
			mov %3, %%cr3;       \
			mov $0x12345, %%eax; \
			sti;                 \
			jmp *%%ecx           "
		 : : "r"(eip), "r"(esp), "r"(ebp), "r"(current_directory->physical_addr));
}

int Fork() {
	DLOG("Fork: Starting process fork operation");
	auto& current_task = global->current_task;
	auto& current_directory = global->current_directory;
	auto& next_pid = global->next_pid;
	auto& ready_queue = global->ready_queue;
	
	// We are modifying kernel structures, and so cannot be interrupted.
	asm volatile("cli");
	
	// Take a pointer to this process' task struct for later reference.
	Task *parent_task = (Task*)current_task;
	
	// Clone the address space.
	PageDirectory *directory = CloneDirectory(current_directory);
	
	// Create a new process.
	Task *new_task = (Task*)KMemoryAllocate(sizeof(Task));
	new_task->id = next_pid++;
	new_task->esp = new_task->ebp = 0;
	new_task->eip = 0;
	new_task->page_directory = directory;
	current_task->kernel_stack = KMemoryAllocateAligned(KERNEL_STACK_SIZE);
	new_task->next = 0;
	
	// Add it to the end of the ready queue.
	// Find the end of the ready queue...
	Task *tmp_task = (Task*)ready_queue;
	while (tmp_task->next)
		tmp_task = tmp_task->next;
	// ...And extend it.
	tmp_task->next = new_task;
	
	// This will be the entry point for the new process.
	uint32 eip = read_eip();
	
	// We could be the parent or the child here - check.
	if (current_task == parent_task) {
		// We are the parent, so set up the esp/ebp/eip for our child.
		uint32 esp;
	asm volatile("mov %%esp, %0" : "=r"(esp));
		uint32 ebp;
	asm volatile("mov %%ebp, %0" : "=r"(ebp));
		new_task->esp = esp;
		new_task->ebp = ebp;
		new_task->eip = eip;
		// All finished: Reenable interrupts.
		DLOG("Fork: Parent process setup completed");
		asm volatile("sti");
		
		// And by convention return the PID of the child.
		return new_task->id;
	}
	else {
		// We are the child - by convention return 0.
		DLOG("Fork: Child process returning 0");
		return 0;
	}
	
}

int GetPid() {
	return global->current_task->id;
}

void SwitchToUserMode() {
	// Set up our kernel stack.
	global->dt.SetKernelStack(global->current_task->kernel_stack + KERNEL_STACK_SIZE);
	
	// Set up a stack structure for switching to user mode.
	asm volatile("  \
			cli; \
			mov $0x23, %ax; \
			mov %ax, %ds; \
			mov %ax, %es; \
			mov %ax, %fs; \
			mov %ax, %gs; \
			\
			\
			mov %esp, %eax; \
			pushl $0x23; \
			pushl %esp; \
			pushf; \
			pushl $0x1B; \
			push $1f; \
			iret; \
			1: \
			");
            
}