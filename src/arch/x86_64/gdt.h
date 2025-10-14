#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/types.h>

namespace Zgen::Hal::x86_64 {

extern "C" void _gdtLoad(void const* p);

// IA-32e (x86_64) TSS layout. Do NOT force "packed" here; natural
// alignment is important for the CPU's interpretation of the TSS and
// pointer arithmetic. The hardware expects a 104-byte TSS structure
// (without an I/O map). Keep natural alignment so sizeof(Tss) == 104.
struct Tss {
    u32           __reserved__0;
    u64           rsp0;
    u64           rsp1;
    u64           rsp2;
    u64           __reserved__1;
    Array<u64, 7> ist;
    u32           __reserved__2;
    u16           __reserved__3;
    u16           iopb;
};
static_assert(sizeof(Tss) == 104, "Tss must be 104 bytes for IA-32e");

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
        // u8  limitHigh: 4;
        // u8  granularity: 4;
        u8  granularity;
        u8  baseHigh;

        constexpr Entry() = default;

        constexpr Entry(u8 flags, u8 granularity)
            : Entry(0, 0xffff'ffff, flags, granularity) { }

        constexpr Entry(u32 base, u32 limit, u8 flags, u8 granularity)
            : limitLow(limit & 0xFFFF),
              baseLow(base & 0xFFFF),
              baseMedium((base >> 16) & 0xFF),
              flags(flags),
              //   limitHigh((limit >> 16) & 0x0F),
              granularity(granularity),
              baseHigh((base >> 24) & 0xFF) { }

        constexpr Entry(u16 limitLow,
                        u16 baseLow,
                        u8  baseMedium,
                        u8  flags,
                        u8  granularity,
                        u8  baseHigh)
            : limitLow(limitLow),
              baseLow(baseLow),
              baseMedium(baseMedium),
              flags(flags),
              //   limitHigh((limitLow >> 16) & 0x0F),
              granularity(granularity),
              baseHigh(baseHigh) { }
    };
    static_assert(sizeof(Entry) == 8);

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
            : len(sizeof(Tss) - 1),
              baseLow(((usize) &tss) & 0xFFFF),
              baseMedium((((usize) &tss) >> 16) & 0xFF),
              // For a 64-bit TSS the descriptor type field should be 0x9
              // (available 64-bit TSS). Combine with PRESENT (0x80) to
              // form 0x89.
              flags0(static_cast<u8>(Flags::PRESENT) | 0x9),
              flags1(0),
              baseHigh((((usize) &tss) >> 24) & 0xFF),
              baseHighest((((usize) &tss) >> 32) & 0xFFFF'FFFF),
              __reserved__0(0) { }
    };

    Entry    entries[5];
    TssEntry tssEntry;

    // constexpr Gdt(Tss const& tss)
    //     : entries({
    //           // clang-format off
    //             /* Null */        Entry {},
    //             /* Kernel Code */ { Flags::PRESENT | Flags::SEGMENT | Flags::READ_WRITE | Flags::EXEC,               Granularity::LONG_MODE },
    //             /* Kernel Data */ { Flags::PRESENT | Flags::SEGMENT | Flags::READ_WRITE,                             0                      },
    //             /* User Code */   { Flags::PRESENT | Flags::SEGMENT | Flags::READ_WRITE | Flags::USER | Flags::EXEC, Granularity::LONG_MODE },
    //             /* User Data */   { Flags::PRESENT | Flags::SEGMENT | Flags::READ_WRITE | Flags::USER,               0                      }}),
    //       // clang-format on
    //       tssEntry(tss) { }

    constexpr Gdt(Tss const& tss)
        : entries({ 0xFFFF, 0x0000, 0x00, 0b0000'0000, 0b0000'0000, 0x00 },
                  { 0x0000, 0x0000, 0x00, 0b1001'1010, 0b0010'0000, 0x00 },
                  { 0x0000, 0x0000, 0x00, 0b1001'0010, 0b0000'0000, 0x00 },
                  { 0x0000, 0x0000, 0x00, 0b1111'1010, 0b0010'0000, 0x00 },
                  { 0x0000, 0x0000, 0x00, 0b1111'0010, 0b0000'0000, 0x00 }),
          tssEntry(tss) { }

    struct [[gnu::packed]] Pack {
        u16 limit;
        u64 base;

        constexpr Pack(Gdt const& gdt)
            : limit(sizeof(Gdt) - 1),
              base((usize) &gdt) { }

        void load() const { _gdtLoad(this); }
    };
};

} // namespace Zgen::Hal::x86_64
