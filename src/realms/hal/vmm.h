#pragma once

#include <realms/hal/pmm.h>
#include <sdk-meta/flags.h>
#include <sdk-meta/literals.h>
#include <sdk-meta/range.h>
#include <sdk-meta/res.h>

namespace Realms::Sys {

// ========================================================================================================================
//     Start addr         |   Offset   |     End addr          |  Size   | VM area description
// ========================================================================================================================
//                        |            |                       |         |
//  0x0000'0000'0000'0000 |            | 0x0000'7fff'ffff'ffff | 128 TiB | User space
//                        |            |                       |         |
// -----------------------+------------+-----------------------+---------+-------------------------------------------------
//                        |            |                       |         |
//  0xffff'8000'0000'0000 |            | 0xffff'ffff'ffff'ffff | 128 TiB | Kernel space
//                        |            |                       |         |
// -----------------------+------------+-----------------------+---------+-------------------------------------------------
//                        |            |                       |         |
//  0xffff'8000'0000'0000 |            | 0xffff'bfff'ffff'ffff | 64 TiB  | Direct mapped region (1:1 phys to virt mapping)
//  0xffff'ffc0'0000'0000 |            | 0xffff'ffc0'ffff'ffff | 4 GiB   | Kernel heap region (for kmalloc, vmalloc etc.)
//  0xffff'ffff'8000'0000 |            | 0xffff'ffff'9fff'ffff | 512 MiB | Kernel core region (for kernel code, data, modules etc.)
//  0xffff'ffff'a000'0000 |            | 0xffff'ffff'ffff'ffff | 1536 MiB| Kernel modules region (for loadable kernel modules)
//                        |            |                       |         |
// ========================================================================================================================

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

#define VMA_RANGE(NAME, START, SIZE)                                           \
    static VmmRange const NAME = { START, SIZE }

VMA_RANGE(USER_REGION, 0x0000'0000'0000'0000ull, 128_TiB);
VMA_RANGE(KERNEL_REGION, 0xffff'8000'0000'0000ull, 128_TiB);
VMA_RANGE(DIRECT_IO_REGION, 0xffff'8000'0000'0000ull, 64_TiB);
VMA_RANGE(HEAP_REGION, 0xffff'ffc0'0000'0000ull, 4_GiB);
VMA_RANGE(CORE_REGION, 0xffff'ffff'8000'0000ull, 512_MiB);
VMA_RANGE(MODULES_REGION, 0xffff'ffff'a000'0000ull, 1536_MiB);

struct Vmm {
    virtual ~Vmm() = default;

    virtual Res<VmmRange> alloc(Opt<VmmRange>   vrange,
                                usize           amount,
                                Flags<VmmFlags> flags)
        = 0;

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

} // namespace Realms::Sys
