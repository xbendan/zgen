#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/types.h>

namespace Hal::x86_64 {

extern "C" void _gdtLoad(void const* p);

struct [[gnu::packed]] Tss {
    u32           __reserved__0 __attribute__((aligned(16)));
    Array<u64, 3> rsp;
    u64           __reserved__1;
    Array<u64, 7> ist;
    u64           __reserved__2;
    u16           __reserved__3;
    u32           iopb;
};

struct Gdt {
    enum class Sel {
        NONE      = 0,
        KERN_CODE = 1,
        KERN_DATA = 2,
        USER_CODE = 3,
        USER_DATA = 4,
        TSS       = 5
    };

    enum Flags : u32 {
        SEGMENT    = 0b0001'0000,
        PRESENT    = 0b1000'0000,
        USER       = 0b0110'0000,
        EXEC       = 0b0000'1000,
        READ_WRITE = 0b0000'0010
    };

    enum Granularity : u32 {
        BYTE      = 0b0000'0000,
        PAGE      = 0b0000'1000,
        LONG_MODE = 0b0010'0000
    };

    struct [[gnu::packed]] Entry {
        u16 limitLow;
        u16 baseLow;
        u8  baseMedium;
        u8  flags;
        u8  limitHigh: 4;
        u8  granularity: 4;
        u8  baseHigh;

        constexpr Entry() = default;

        constexpr Entry(u8 flags, u8 granularity)
            : flags(flags),
              granularity(granularity) { };

        constexpr Entry(u32 base, u32 limit, u8 flags, u8 granularity)
            : limitLow(limit & 0xFFFF),
              baseLow(base & 0xFFFF),
              baseMedium((base >> 16) & 0xFF),
              flags(flags),
              limitHigh((limit >> 16) & 0x0F),
              granularity(granularity),
              baseHigh((base >> 24) & 0xFF) { }
    };

    struct [[gnu::packed]] TssEntry {
        u16 len;
        u16 baseLow;
        u8  baseMedium;
        u8  flags0;
        u8  flags1;
        u8  baseHigh;
        u32 baseHighest;
        u32 __reserved__0;

        constexpr TssEntry() = default;

        constexpr TssEntry(Tss const& tss)
            : len(sizeof(Tss)),
              baseLow((usize) &tss & 0xFFFF),
              baseMedium(((usize) &tss >> 16) & 0xFF),
              flags0(Flags::PRESENT
                     | Flags::EXEC
                     | Flags::READ_WRITE
                     | Flags::USER),
              flags1(0),
              baseHigh(((usize) &tss >> 24) & 0xFF),
              baseHighest(((usize) &tss >> 32) & 0xFFFF'FFFF),
              __reserved__0(0) { }
    };

    // clang-format off
    Array<Entry, 5> entries = {
        /* Null */  Entry {},
        /* Kernel Code */ { Flags::PRESENT | Flags::SEGMENT | Flags::READ_WRITE | Flags::EXEC,               Granularity::LONG_MODE },
        /* Kernel Data */ { Flags::PRESENT | Flags::SEGMENT | Flags::READ_WRITE,                             0                      },
        /* User Code */   { Flags::PRESENT | Flags::SEGMENT | Flags::READ_WRITE | Flags::USER | Flags::EXEC, Granularity::LONG_MODE },
        /* User Data */   { Flags::PRESENT | Flags::SEGMENT | Flags::READ_WRITE | Flags::USER,               0                      }
    };
    TssEntry tssEntry;
    // clang-format on

    constexpr Gdt(Tss const& tss) : tssEntry(tss) { }

    struct [[gnu::packed]] Pack {
        u16 limit;
        u64 base;

        constexpr Pack(Gdt const& gdt)
            : limit(sizeof(Gdt) - 1),
              base((usize) &gdt) { }

        void load() const { _gdtLoad(this); }
    };
};

} // namespace Hal::x86_64
