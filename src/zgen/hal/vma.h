#pragma once

#include <sdk-meta/literals.h>
#include <zgen/hal/vmm.h>

namespace Zgen::Hal {

#define VMA_RANGE(NAME, START, SIZE)                                           \
    static VmmRange const NAME = { START, SIZE }

VMA_RANGE(UserRegion, 0x0000'0000'0000'0000ull, 128_TiB);
VMA_RANGE(KernelRegion, 0xffff'8000'0000'0000ull, 128_TiB);
VMA_RANGE(KernelDirectRegion, 0xffff'8000'0000'0000ull, 64_TiB);
VMA_RANGE(KernelHeapRegion, 0xffff'c000'0000'0000ull, 4_GiB);
VMA_RANGE(KernelCoreRegion, 0xffff'ffff'8000'0000ull, 512_MiB);
VMA_RANGE(KernelModulesRegion, 0xffff'ffff'a000'0000ull, 1536_MiB);

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
//  0xffff'c000'0000'0000 |            | 0xffff'c000'ffff'ffff | 4 GiB   | Kernel heap region (for kmalloc, vmalloc etc.)
//  0xffff'ffff'8000'0000 |            | 0xffff'ffff'9fff'ffff | 512 MiB | Kernel core region (for kernel code, data, modules etc.)
//  0xffff'ffff'a000'0000 |            | 0xffff'ffff'ffff'ffff | 1536 MiB| Kernel modules region (for loadable kernel modules)
//                        |            |                       |         |
// ========================================================================================================================

} // namespace Zgen::Hal
