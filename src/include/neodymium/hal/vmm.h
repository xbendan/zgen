#pragma once

#include <neodymium/hal/pmm.h>
#include <sdk-meta/flags.h>
#include <sdk-meta/range.h>
#include <sdk-meta/res.h>

namespace Hal {

static constexpr usize PAGE_SIZE           = 0x1000;
static constexpr uptr  KERNEL_VIRTUAL_BASE = 0xffff'8000'0000'0000;

inline usize pageAlignUp(usize addr) {
    return alignUp(addr, PAGE_SIZE);
}

inline usize pageAlignDown(usize addr) {
    return alignDown(addr, PAGE_SIZE);
}

enum struct VmmFlags : u64 {
    NONE          = 0,
    READ          = (1 << 0),
    WRITE         = (1 << 1),
    EXECUTE       = (1 << 2),
    USER          = (1 << 3),
    PRESENT       = (1 << 4),
    UNCACHED      = (1 << 5),
    GLOBAL        = (1 << 6),
    NO_EXECUTE    = (1 << 8),
    WRITE_THROUGH = (1 << 9),
    DIRTY         = (1 << 10)
};

struct VmmPage {
    Flags<VmmFlags> flags;
    usize           phys;
};

using VmmRange = Range<uptr, struct _VmmRangeTag>;

struct Vmm {
    virtual ~Vmm() = default;

    virtual Res<VmmRange> alloc(usize size) = 0;

    virtual Res<> free(VmmRange range) = 0;

    virtual Res<> map(VmmRange virt, PmmRange phys, Flags<VmmFlags> flags) = 0;

    virtual Res<> unmap(VmmRange range) = 0;

    virtual Res<VmmPage> at(usize address) = 0;

    virtual usize count() const = 0;
};

} // namespace Hal
