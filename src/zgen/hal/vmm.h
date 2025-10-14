#pragma once

#include <sdk-meta/flags.h>
#include <sdk-meta/range.h>
#include <sdk-meta/res.h>
#include <zgen/hal/pmm.h>

namespace Zgen::Hal {

static constexpr usize PAGE_SIZE  = 0x1000;
static constexpr uflat KERNEL_VMA = 0xffff'ffff'8000'0000ull;

inline usize pageAlignUp(usize addr) {
    return alignUp(addr, PAGE_SIZE);
}

inline usize pageAlignDown(usize addr) {
    return alignDown(addr, PAGE_SIZE);
}

inline bool pageAligned(usize addr) {
    return isAlign(addr, PAGE_SIZE);
}

enum struct VmmFlags : u64 {
    NONE          = 0,
    READ          = (1 << 0),
    WRITE         = (1 << 1),
    EXECUTE       = (1 << 2),
    USER          = (1 << 3),
    PRESENT       = (1 << 4),
    UNCACHED      = (1 << 5),
    PAGE_SIZE     = (1 << 6),
    NO_EXECUTE    = (1 << 8),
    WRITE_THROUGH = (1 << 9),
    DIRTY         = (1 << 10)
};
MakeFlags$(VmmFlags);

enum Granularity : usize {
    PAGE_4K   = (1ull << 12),
    PAGE_16K  = (1ull << 14),
    PAGE_2M   = (1ull << 21),
    PAGE_1G   = (1ull << 30),
    PAGE_512G = (1ull << 39)
};

struct VmmPage {
    Flags<VmmFlags> flags;
    usize           phys;
};

using VmmRange = Range<uflat, struct _VmmRangeTag>;

struct Vmm {
    virtual ~Vmm() = default;

    virtual Res<VmmRange> alloc(Opt<VmmRange>   vrange,
                                usize           amount,
                                Flags<VmmFlags> flags) = 0;

    virtual Res<> free(VmmRange range) = 0;

    virtual Res<> map(VmmRange virt, PmmRange phys, Flags<VmmFlags> flags) = 0;

    virtual Res<> unmap(VmmRange range) = 0;

    virtual Res<VmmPage> at(usize address) = 0;

    virtual usize count() const = 0;

    virtual Res<> load() = 0;
};

struct DirectMapper {
    template <typename T>
    T map(T phys) const {
        return phys;
    }

    template <typename T>
    T unmap(T virt) const {
        return virt;
    }
};

struct UpperHalfMapper {
    template <typename T>
    T map(T phys) const {
        return phys + KERNEL_VMA;
    }

    template <typename T>
    T unmap(T virt) const {
        return virt - KERNEL_VMA;
    }
};

} // namespace Zgen::Hal
