#pragma once

#include <sdk-meta/flags.h>
#include <sdk-meta/range.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>

namespace Hal {

enum struct PmmFlags : u64 {
    ATOMIC  = (1 << 0),
    KERNEL  = (1 << 1),
    SWAPPED = (1 << 2),
    SHARED  = (1 << 3),
    HIGHMEM = (1 << 4),
    DMA     = (1 << 5),
    CACHED  = (1 << 6),
};

using PmmRange = Range<uptr, struct _PmmRangeTag>;

struct Pmm {
    virtual ~Pmm() = default;

    virtual Res<PmmRange> alloc(u64 size, Flags<Hal::PmmFlags> flags = {}) = 0;

    virtual Res<> free(PmmRange range) = 0;

    virtual Res<> take(PmmRange range) = 0;
};

} // namespace Hal
