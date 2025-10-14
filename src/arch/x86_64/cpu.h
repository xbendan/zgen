#pragma once

#include <arch/x86_64/gdt.h>
#include <arch/x86_64/idt.h>
#include <arch/x86_64/regs.h>
#include <sdk-meta/traits.h>

namespace Zgen::Hal::x86_64 {

struct [[gnu::aligned(0x10)]] CpuLocal : Meta::Pinned {
    CpuLocal* self;
    u32       id;
    // Place TSS before GDT so it is constructed first. The Gdt constructor
    // expects a reference to a valid Tss, so tss must be initialized
    // before gdt to avoid using uninitialized memory.
    Tss       tss __attribute__((aligned(0x10)));
    Gdt       gdt;
    Gdt::Pack gdtPtr;
    Idt::Pack idtPtr;

    CpuLocal() = delete;

        CpuLocal(u32 id, Idt const& idt)
                : self(this),
                    id(id),
                    tss(),
                    gdt(tss),
                    gdtPtr(gdt),
                    idtPtr(idt) { }
};

[[noreturn, maybe_unused]] static inline void halt() {
    while (true) {
        __asm__ __volatile__("cli; hlt;");
    }
}

} // namespace Zgen::Hal::x86_64
