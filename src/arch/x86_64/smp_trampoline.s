smp_trampoline_entry equ 0x2000
smp_trampoline_data equ 0x1000
smp_trampoline_data_flags equ smp_trampoline_data + 0x0
smp_trampoline_id equ smp_trampoline_data + 0x2
smp_trampoline_gdt_ptr equ smp_trampoline_data + 0x10
smp_trampoline_pagemap equ smp_trampoline_data + 0x20
smp_trampoline_stack equ smp_trampoline_data + 0x28
smp_trampoline_entrypoint equ smp_trampoline_data + 0x30
smp_magic equ 0xb33f

[Section .text]
[Bits 16]
smp_trampoline_start:
    cli
    cld

    mov ax, smp_magic
    mov word [smp_trampoline_data_flags], ax

    mov eax, cr4
    or eax, 1 << 5        ; enable physical address extension
    mov cr4, eax
    mov eax, dword [smp_trampoline_pagemap]
    mov cr3, eax          ; load pagemap

    mov ecx, 0xc0000080   ; EFER msr
    rdmsr
    or eax, 1 << 8        ; enable long mode
    or eax, 1             ; enable syscall
    wrmsr

    mov eax, cr0
    or eax, 0x80000001    ; enable protected mode and paging
    mov cr0, eax

    lgdt [smp_trampoline_gdt_ptr]
    jmp 08h:(smp_trampoline_long_mode_entry)
    hlt

[Bits 64]
align 0x40
smp_trampoline_long_mode_entry:
    mov ax, 10h
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, [smp_trampoline_stack]
    
    mov rax, cr0
    and ax, 0xfffb        ; clear coprocessor emulation flag
    or ax, 0x2            ; set coprocessor monitoring flags
    mov cr0, rax
    
    mov rax, cr4
    or ax, 3 << 9
    mov cr4, rax

    xor rbp, rbp
    mov rdi, [smp_trampoline_id]

    call [smp_trampoline_entrypoint]
    cli
    hlt
smp_trampoline_end: