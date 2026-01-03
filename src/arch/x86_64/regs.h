#pragma once

#include <realms/hal/io.h>
#include <sdk-meta/types.h>

namespace Realms::Hal::x86_64 {

struct [[gnu::packed]] Regs {
    u64 r15, r14, r13, r12, r11, r10, r9, r8;
    u64 rbp, rdi, rsi, rdx, rcx, rbx, rax;
    u64 err;
    u64 rip, cs, rflags, rsp, ss;
};

using MsrApic = Hal::Reg<u64, 0x1b>;

struct _Msr : public Hal::Io {
    inline Res<usize> in(usize offset, usize size) override {
        if (size != 8) {
            return Error::invalidArgument(
                "Msr::in: only 8-byte accesses are supported");
        }

        u32 low;
        u32 high;
        asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"((u32) offset));
        return Ok(((u64) high << 32) | low);
    }

    inline Res<> out(usize offset, usize size, usize val) override {
        if (size != 8) {
            return Error::invalidArgument(
                "Msr::out: only 8-byte accesses are supported");
        }

        u32 low  = (u32) val;
        u32 high = (u32) (val >> 32);
        asm volatile("wrmsr" : : "a"(low), "d"(high), "c"((u32) offset));
        return Ok();
    }
};

enum class Msr : u32 {
    Apic         = (0x1b),
    Efer         = (0xc000'0080),
    Star         = (0xc000'0081),
    Lstar        = (0xc000'0082),
    Cstar        = (0xc000'0083),
    Syscall      = (0xc000'0084),
    FsBase       = (0xc000'0100),
    GsBase       = (0xc000'0101),
    KernelGsBase = (0xc000'0102)
};

static inline u64 rdmsr(Msr msr) {
    u32 low;
    u32 high;
    asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"((u32) msr));
    return ((u64) high << 32) | low;
}

static inline void wrmsr(Msr msr, u64 val) {
    u32 low  = (u32) val;
    u32 high = (u32) (val >> 32);
    asm volatile("wrmsr" : : "a"(low), "d"(high), "c"((u32) msr));
}

} // namespace Realms::Hal::x86_64
