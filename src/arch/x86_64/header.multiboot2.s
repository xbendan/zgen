[Bits 32]
Section .multiboot2.text

kernel_vma equ 0xffffffff80000000

magic        equ 0xe85250d6
architecture equ 0
length       equ (multiboot2_request_end - multiboot2_request_start)
checksum     equ -(magic + architecture + length)

[Extern __kernel_start]
[Extern __kernel_load_end]
[Extern __kernel_end]
[Extern __bss_start]
[Extern __bss_end]

align 0x08
multiboot2_request_start:
    dd magic
    dd architecture
    dd length
    dd checksum

    .addr_start:
        align 0x08
        dw 2
        dw 0
        dd .addr_end - .addr_start
        dd 0x00100000
        dd 0x00100000
        dd __kernel_load_end - 0xffffffff80000000
        dd __kernel_end - 0xffffffff80000000
    .addr_end:

    .entry_start:
        align 0x08
        dw 3
        dw 0
        dd .entry_end - .entry_start
        dd kinit_multiboot2_entry32
    .entry_end:

    .framebuffer_start:
        align 0x08
        dw 5
        dw 0
        dd .framebuffer_end - .framebuffer_start
        dd 1024
        dd 768
        dd 32
    .framebuffer_end:

    .end:
        align 0x08
        dw 0
        dw 0
        dd 8
multiboot2_request_end:

[Bits 32]
[Section .multiboot2.data]
kvma_pml4_index equ (kernel_vma >> 39) & 0x1ff
kvma_pdpt_index equ (kernel_vma >> 30) & 0x1ff

align 0x10
_Gdt64:
    .Null: equ $ - _Gdt64
        dq 0x0
    .Code: equ $ - _Gdt64
        dw 0                         ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 10011010b                 ; Access (exec/read).
        db 00100000b                 ; Granularity, 64 bits flag, limit19:16.
        db 0                         ; Base (high).
    .Data: equ $ - _Gdt64
        dw 0                         ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 10010010b                 ; Access (read/write).
        db 00000000b                 ; Granularity.
        db 0                         ; Base (high).
    .UserCode: equ $ - _Gdt64
        dw 0                         ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 11111010b                 ; Access (read/write).
        db 00100000b                 ; Granularity.
        db 0                         ; Base (high).
    .UserData: equ $ - _Gdt64
        dw 0                         ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 11110010b                 ; Access (exec/read).
        db 00000000b                 ; Granularity, 64 bits flag, limit19:16.
        db 0                         ; Base (high).
    .Tss: equ $ - _Gdt64
        dw 108                       ; TSS Length - the x86_64 TSS is 108 bytes long
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 10001001b                 ; Flags
        db 00000000b                 ; Flags 2
        db 0
        dd 0                         ; High 32 bits
        dd 0
    .Pack32:
        dw $ - _Gdt64 - 1            ; Limit
        dq _Gdt64                    ; Base

[Global _pml4]

align 0x1000
_pml4:
    times 512 dq 0
_pde:
    times 512 dq 0
_pdpt:
    dq 0
    times 511 dq 0
_pdpt2:
    times kvma_pdpt_index dq 0
    dq 0

[Bits 32]
[Section .multiboot2.text]
[Extern kinit_multiboot2]

unsupported_msg db "Long mode is not supported.", 0
unsupported_msg_end:

[Global kinit_multiboot2_entry32]
kinit_multiboot2_entry32:
    mov dword [multiboot2_tags], ebx
    
    ; check long mode support
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .unsupported

    ; enable physical address extension
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 512
    mov eax, _pde
    mov ebx, 0x83
.fill_pde:
    mov dword [eax], ebx
    add ebx, 0x200000
    add eax, 8
    loop .fill_pde

    mov eax, _pdpt
    or eax, 0x03
    mov dword [_pml4], eax

    mov eax, _pdpt2
    or eax, 0x03
    mov dword [_pml4 + 8 * kvma_pml4_index], eax

    mov eax, _pde
    or eax, 0x03
    mov dword [_pdpt], eax
    mov dword [_pdpt2 + 8 * kvma_pdpt_index], eax

    mov eax, _pml4
    mov cr3, eax

    mov ecx, 0xc0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    or eax, 1 << 0
    mov cr0, eax


    lgdt [_Gdt64.Pack32]
    jmp 0x8:kinit_multiboot2_entry64 - kernel_vma

.unsupported:
    mov eax, 0xb8000
    mov edi, eax
    mov ecx, 80 * 25
    xor eax, eax
    mov ah, 0x0F
    rep stosw
    mov edi, eax
    mov esi, unsupported_msg
    mov ecx, unsupported_msg_end - unsupported_msg
    rep movsb
    cli
    hlt ; Generate by Copilot, I don't know why it's here

[Bits 64]
    cli
    hlt
    
multiboot2_tags:
    dd 0
    dd 0

[Section .data]
[Global GdtPack64]
GdtPack64:
    dw _Gdt64.Pack32 - _Gdt64 - 1
    dq _Gdt64 + kernel_vma

[Section .bss]
[Global _stackTop]
[Global _stackBottom]
align 0x40
_stackBottom:
    resb 0x10000
_stackTop:

[Section .text]
kinit_multiboot2_entry64:
    lgdt [GdtPack64]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rdi, __bss_start
    mov rcx, __bss_end
    sub rcx, __bss_start
    xor rax, rax
    rep stosb

    mov rsp, _stackTop
    mov rax, cr0
    and ax, 0xfffb
    or ax, 0x2
    mov cr0, rax
    
    mov rcx, 0x277
    rdmsr
    mov rbx, 0xffffffffffffff
    and rax, rbx
    mov rbx, 0x100000000000000
    or rax, rbx ; Set PA7 to Write-combining (0x1, WC)
    wrmsr

    xor rbp, rbp
    mov rdi, qword [multiboot2_tags]
    call kinit_multiboot2

    cli
    hlt
