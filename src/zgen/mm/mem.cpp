#include <sdk-logs/logger.h>
#include <sdk-meta/iter.h>
#include <sdk-meta/literals.h>
#include <zgen/hal/vma.h>
#include <zgen/hal/vmm.h>
#include <zgen/init/boot.h>
#include <zgen/mm/kmm.inc.h>
#include <zgen/mm/mem.h>
#include <zgen/mm/pmm.bits.h>

namespace Zgen::Core {

using Sdk::Text::Align;
using Sdk::Text::aligned;

Opt<PmmBits> _pmm;
Opt<KmmInc>  _kmm;

Res<> setupMemory(PrekernelInfo* info) {
    logInfo("Scanning usable memory regions...\n");
    iter(info->memmap)
        .forEach$(logInfo(" > {:#x} - {:#x} ({} MiB) {}\n",
                          aligned(it.range.start(), Align::RIGHT, 16),
                          aligned(it.range.end(), Align::RIGHT, 16),
                          aligned(it.range.size() / 1_MiB, Align::RIGHT, 6),
                          it.type == 0 ? "-> Usable"s : ""s));
    auto usable = iter(info->memmap)
                      .filter$(it.type == 0)
                      .select$(it.range)
                      .reduce$(x.merge(y))
                      .mapTo$(it.template into<Hal::PmmRange>());
    if (usable->_size < 16_MiB) {
        logError("Not enough usable memory found\n");
        return Error::outOfMemory("not enough memory");
    }
    logInfo("Done! Usable memory: {} KiB\n", usable->size() / 1_KiB);

    usize bitsSize = Hal::pageAlignUp(usable->end() / Hal::PAGE_SIZE / 8);
    auto  bitsRange
        = iter(info->memmap)
              .filter$(it.type == 0 and it.range.start() != 0)
              .first$(it.range.size() >= bitsSize)
              .mapTo$(it.range.template into<Hal::PmmRange>().take(bitsSize));
    if (not bitsRange) {
        return Error::outOfMemory("no suitable range for pmm bits");
    }
    logInfo("Using memory range {:#x} - {:#x} for pmm bits\n",
            bitsRange->start(),
            bitsRange->end());
    _pmm.emplace(
        *usable,
        Bits((u8*) mmapIo(bitsRange->start()).take(), bitsRange->size()));

    try$(Core::globalVmm().load());
    logInfo("Created and loaded global vmm.\n");

    auto kmmRange = Hal::KmmRange {
        Hal::KERNEL_VMA + 1_GiB,
        16_MiB,
    };

    // try$(vmm.map(kmmRange.into<Hal::VmmRange>(),
    //              try$(_pmm->alloc(kmmRange._size, Hal::PmmFlags::Kernel)),
    //              Hal::VmmFlags::WRITE));
    // logInfo("Kmm: initialized at {:#x} - {:#x} ({} MiB)\n",
    //         kmmRange._start,
    //         kmmRange.end(),
    //         kmmRange._size / 1_MiB);
    // _kmm.emplace(Zgen::Core::pmm(), kmmRange);

    return Ok();
}

Hal::Pmm& pmm() {
    if (not _pmm) {
        panic("pmm: not initialized");
    }
    return *_pmm;
}

Hal::Kmm& kmm() {
    if (not _kmm) {
        panic("kmm: not initialized");
    }
    return *_kmm;
}

Res<uflat> mmapIo(uflat phys) {
    if (phys > 0xffff'ffff) {
        return Error::invalidInput(
            "Zgen::Core::mmapIo: physical address out of range");
    }
    return Ok(phys + Hal::KernelDirectRegion.start());
}

} // namespace Zgen::Core
