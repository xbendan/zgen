OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

ENTRY(kinit_multiboot2_entry32)

SECTIONS {
    KERNEL_VMA = 0xffffffff80000000;

    . = 0x100000; /* 1 MiB */

    .multiboot2.text : {
        KEEP(*(.multiboot2.text))
    }

    .multiboot2.data : {
        KEEP(*(.multiboot2.data))
    }

    . += KERNEL_VMA;

    .requests : {
        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    }

    __kernel_start = .;
	
    /* Move to the next memory page for .text */

    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VMA) {
        KEEP(*(.text*))
    }

    /* Move to the next memory page for .rodata */

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VMA) {
        __ctors_start = .;
        *(SORT(.ctors*))
        __ctors_end = .;

        __dtors_start = .;
        *(SORT(.dtors*))
        __dtors_end = .;

        KEEP(*(.rodata*))
    }

    /* Move to the next memory page for .data */

    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VMA) {
        KEEP(*(.data*))
    }

    __kernel_load_end = .;

    /* NOTE: .bss needs to be the last thing mapped to :data, otherwise lots of */
    /* unnecessary zeros will be written to the binary. */
    /* If you need, for example, .init_array and .fini_array, those should be placed */
    /* above this. */
    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VMA) {
        __bss_start = .;
        KEEP(*(COMMON))
        KEEP(*(.bss*))
        . = ALIGN(4K);
        __bss_end = .;
    }

    __kernel_end = .;

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note*)
        *(.note.*)
        *(.note_*)
        *(.rel*)
        *(.rela*)
        *(.symtab)
    }
}