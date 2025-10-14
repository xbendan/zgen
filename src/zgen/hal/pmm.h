#pragma once

#include <sdk-meta/flags.h>
#include <sdk-meta/ptr.h>
#include <sdk-meta/range.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>

namespace Zgen::Hal {

enum struct PmmFlags : u64 {
    Atomic       = (1 << 0),
    Kernel       = (1 << 1),
    Swapped      = (1 << 2),
    SharedMemory = (1 << 3),
    Highmem      = (1 << 4),
    Dma          = (1 << 5),
    Cached       = (1 << 6),
};

using PmmRange = Range<uflat, struct _PmmRangeTag>;

struct Pmm {
    virtual ~Pmm() = default;

    virtual Res<PmmRange> alloc(u64 size, Flags<Hal::PmmFlags> flags = {}) = 0;

    virtual Res<> free(PmmRange range) = 0;

    virtual Res<> take(PmmRange range) = 0;
};

} // namespace Zgen::Hal
