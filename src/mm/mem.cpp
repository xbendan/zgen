#include <neodymium/hal/vmm.h>
#include <neodymium/init/boot.h>
#include <neodymium/mm/mem.h>
#include <neodymium/mm/pmm.bits.h>
#include <sdk-logs/logger.h>
#include <sdk-meta/size.h>

namespace Sys {

Opt<PmmBits> _pmm;

Res<> setupMemory(PrekernelInfo* info) {
    logInfo("Scanning usable memory regions...");
    auto usable = iter(info->memmap)
                      .filter$(x.type == 0)
                      .map$(Pair(x.base, x.base + x.size))
                      .reduce$(Pair(min(x->v0, y->v0), max(x->v1, y->v1)))
                      .mapTo$(Hal::PmmRange(x.v0, x.v1 - x.v0));
    if (usable->_size < 16_MiB) {
        logError("Not enough usable memory found");
        return Error::outOfMemory("not enough memory");
    }
    logInfo("Done! Usable memory: {:#x} - {:#x} ({} MiB)",
            usable->_start,
            usable->end(),
            usable->_size / 1_MiB);
    usize bitsSize = Hal::pageAlignUp(usable->end() / Hal::PAGE_SIZE / 8);

    auto bitsRange = iter(info->memmap)
                         .filter$(x.type == 0 and x.base != 0)
                         .first$(x.size >= bitsSize)
                         .mapTo$(Hal::PmmRange(x.base, bitsSize));
    if (not bitsRange) {
        return Error::outOfMemory("no suitable range for pmm bits");
    }
    logInfo("Using memory range {:#x} - {:#x} for pmm bits",
            bitsRange->_start,
            bitsRange->end());
    _pmm.emplace(*usable,
                 Slice { (u8*) bitsRange->_start + Hal::KERNEL_VIRTUAL_BASE,
                         bitsRange->_size });
}

} // namespace Sys
