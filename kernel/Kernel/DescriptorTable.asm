[bits 32]

section .text

; Load GDT
global LoadGdtAsm
LoadGdtAsm:
    mov eax, [esp+4]  ; Get the pointer to the GDT pointer structure
    lgdt [eax]        ; Load the GDT
    mov ax, 0x10      ; Data segment selector (2nd entry in GDT)
    mov ds, ax        ; Load data segment
    mov es, ax        ; Load extra segment
    mov fs, ax        ; Load FS
    mov gs, ax        ; Load GS
    mov ss, ax        ; Load stack segment
    jmp 0x08:flush    ; Far jump to code segment (1st entry in GDT)
    
flush:
    ret

; Load IDT
global LoadIdtAsm
LoadIdtAsm:
    mov eax, [esp+4]  ; Get the pointer to the IDT pointer structure
    lidt [eax]        ; Load the IDT
    ret

; Task State Segment (TSS) for context switching
global LoadTss
LoadTss:
    mov ax, [esp+4]   ; Get the TSS selector
    ltr ax            ; Load the TSS
    ret

; Interrupt Service Routine (ISR) Stubs
; We'll define stubs for the first 32 ISRs (exceptions)
extern isr_handler

%macro ISR_NOERRCODE 1
  global isr%1
  isr%1:
    cli
    push 0          ; Push a dummy error code
    push %1         ; Push the interrupt number
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
  global isr%1
  isr%1:
    cli
    push %1         ; Push the interrupt number
    jmp isr_common_stub
%endmacro

; Define the ISR stubs
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

; Common ISR stub
isr_common_stub:
    pusha                    ; Push all registers
    
    mov ax, ds
    push eax                 ; Save data segment

    mov ax, 0x10             ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler         ; Call the C handler

    pop ebx                  ; Restore data segment
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa                     ; Pop all registers
    add esp, 8               ; Clean up error code and ISR number
    sti
    iret                     ; Return from interrupt

; IRQ Stubs
extern irq_handler

%macro IRQ 2
  global irq%1
  irq%1:
    cli
    push 0
    push %2
    jmp irq_common_stub
%endmacro

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; Common IRQ stub
irq_common_stub:
    pusha                    ; Push all registers
    
    mov ax, ds
    push eax                 ; Save data segment

    mov ax, 0x10             ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_handler         ; Call the C handler

    pop ebx                  ; Restore data segment
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa                     ; Pop all registers
    add esp, 8               ; Clean up error code and IRQ number
    sti
    iret                     ; Return from interrupt