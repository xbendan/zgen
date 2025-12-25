#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/types.h>

namespace Realms::Hal::x86_64 {

extern "C" void _idtLoad(void const* p);

extern "C" Array<usize, 256> _intVec;

struct Idt {
    static constexpr usize LEN = 256;

    enum Flags : u8 {
        INTR = 0b1000'1110,
        TRAP = 0b1110'1111,
        USER = 0b0110'0000,
    };

    struct [[packed]] Entry {
        u16 baseLow;
        u16 sel;
        u8  ist: 3;
        u8  zero: 5;
        u8  flags;
        u16 baseMedium;
        u32 baseHigh;
        u32 __reserved__0;

        constexpr Entry() = default;

        constexpr Entry(u64 base, u16 sel, u8 flags, u8 ist = 0)
            : baseLow(base & 0xFFFF),
              sel(sel),
              ist(ist),
              zero(0),
              flags(flags),
              baseMedium((base >> 16) & 0xFF),
              baseHigh((base >> 32) & 0xFFFF'FFFF),
              __reserved__0(0) { }
    };

    Array<Entry, LEN> entries {};

    struct [[packed]] Pack {
        u16 limit;
        u64 base;

        constexpr Pack(Idt const& idt)
            : limit(sizeof(Idt) - 1),
              base((usize) &idt) { }

        void load() const { _idtLoad(this); }
    };
};

} // namespace Realms::Hal::x86_64
