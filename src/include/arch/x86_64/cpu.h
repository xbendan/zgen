#pragma once

#include <arch/x86_64/gdt.h>
#include <arch/x86_64/idt.h>
#include <arch/x86_64/regs.h>
#include <sdk-meta/traits.h>

namespace Hal::x86_64 {

struct [[gnu::aligned(0x10)]] CpuLocal : Meta::Pinned {
    CpuLocal* self;
    u32       id;
    Gdt       gdt;
    Gdt::Pack gdtPtr;
    Idt::Pack idtPtr;
    Tss       tss __attribute__((aligned(0x10)));

    CpuLocal() = delete;

    CpuLocal(u32 id, Idt const& idt)
        : self(this),
          id(id),
          gdt(tss),
          gdtPtr(gdt),
          idtPtr(idt),
          tss() { }
};

} // namespace Hal::x86_64
