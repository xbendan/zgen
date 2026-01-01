#pragma once

#include <sdk-meta/types.h>

namespace Boot {

enum struct Tag : u32 {
    Empty = 0,

    Module,
    CommandLine,
    Config,
    Memory,
    Hhdm,
    Kernel,

    // Firmware & Hardware Description
    DeviceTreeBlob,
    EfiSystemTable,
    AcpiTable,

    // Graphics / Framebuffer
    Framebuf,
    Edid,

    // SMP & CPU Specific
    SymmetricProcTable,
    BootHartId,

    // Security & Randomness
    RandomSeed,
    SecureBoot,

    // Auxiliary
    Epoch,
    Stack,
};

template <Tag>
struct _Record;

struct Record {
    Tag tag;
    u32 flags;
    u32 size;

    template <Tag T>
    _Record<T>& as() {
        // TODO: static_assert to check tag
        return *(static_cast<_Record<T>*>(this));
    }
};

template <>
struct _Record<Tag::Memory> : Record {
    u64 start, length;
    u32 type;
};

} // namespace Boot
