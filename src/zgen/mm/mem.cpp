#include <sdk-logs/logger.h>
#include <sdk-math/funcs.h>
#include <sdk-meta/iter.h>
#include <sdk-meta/literals.h>
#include <zgen/hal/vma.h>
#include <zgen/hal/vmm.h>
#include <zgen/init/boot.h>
#include <zgen/mm/kmm.inc.h>
#include <zgen/mm/mem.h>
#include <zgen/mm/pmm.bits.h>

extern "C" u64 __kernel_start;
extern "C" u64 __kernel_end;
extern "C" u64 __kernel_load_end;
extern "C" u64 __bss_start;
extern "C" u64 __bss_end;
extern "C" u64 _stackTop;
extern "C" u64 _stackBottom;
extern "C" u64 _pml4;

namespace Zgen::Core {

using Sdk::Text::Align;
using Sdk::Text::aligned;

Opt<PmmBits> _pmm;
Opt<KmmInc>  _kmm;

Res<> setupMemory(PrekernelInfo* info) {
    usize kImageSize
        = Hal::pageAlignUp((uflat) &__kernel_end - 0xffff'ffff'8010'0000);

    logInfo("Scanning usable memory regions...\n");
    for (int i = 0; i < info->memmap.len(); i++) {
        if (info->memmap[i].range.start() == 0x10'0000) {
            info->memmap[i].range.take(kImageSize);
        }
    }
    iter(info->memmap)
        .forEach$(logInfo(" > {:#x} - {:#x} ({} MiB) {}\n",
                          aligned(it.range.start(), Align::RIGHT, 16),
                          aligned(it.range.end(), Align::RIGHT, 16),
                          aligned(it.range.size() / 1_MiB, Align::RIGHT, 6),
                          (u32) it.type));

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

    // Reserve memory for pmm bits
    usize bitsSize = Hal::pageAlignUp(
        Math::div(usable->end(), Hal::PAGE_SIZE * 8, Math::ROUND_UP));
    Opt<Hal::PmmRange> bitsRange = NONE;

    for (auto& entry : iter(info->memmap)) {
        if ((entry.type != 0)
            or (entry.range.start() == 0)
            or (entry.range.size() < bitsSize)) {
            continue;
        }

        logInfo("Reserving {:#x} - {:#x} for pmm bits\n",
                entry.range.start(),
                entry.range.start() + bitsSize);
        bitsRange.emplace(entry.range.start(), bitsSize);
        break;
    }
    // auto bitsRange = iter(info->memmap)
    //                      .first$((it.type == 0)
    //                              and (it.range.start() != 0)
    //                              and (it.range.size() >= bitsSize))
    //                      .mapTo$(it.range.take(bitsSize));
    if (not bitsRange) {
        return Error::outOfMemory("no suitable range for pmm bits");
    }
    _pmm.emplace(
        *usable,
        Bits((u8*) mmapVirtIo(bitsRange->start()).take(), bitsRange->size()));

    auto& vmm = Core::createKernelVmm(
                    { 0x0, (uflat) &__kernel_end - 0xffff'ffff'8000'0000 })
                    .unwrap();
    logInfo("Created and loaded global vmm.\n");

    auto kmmRange = Hal::KmmRange {
        Hal::KERNEL_VMA + 1_GiB,
        16_MiB,
    };

    __asm__ __volatile__("cli; hlt");

    try$(vmm.map(kmmRange.into<Hal::VmmRange>(),
                 try$(_pmm->alloc(kmmRange._size, Hal::PmmFlags::Kernel)),
                 Hal::VmmFlags::WRITE));
    logInfo("Kmm: initialized at {:#x} - {:#x} ({} MiB)\n",
            kmmRange._start,
            kmmRange.end(),
            kmmRange._size / 1_MiB);
    _kmm.emplace(Zgen::Core::pmm(), kmmRange);

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

Opt<uflat> mmapVirtIo(uflat phys) {
    if (phys > 0xffff'ffff) {
        return NONE;
    }
    return phys + Hal::DIRECT_IO_REGION.start();
}

Opt<uflat> mmapPhys(uflat virt) {
    return virt - Hal::DIRECT_IO_REGION.start();
}

} // namespace Zgen::Core
