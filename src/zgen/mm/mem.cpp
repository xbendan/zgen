#include <sdk-logs/logger.h>
#include <sdk-math/funcs.h>
#include <sdk-meta/iter.h>
#include <sdk-meta/literals.h>
#include <zgen/hal/vmm.h>
#include <zgen/init/boot.h>
#include <zgen/mm/kmm.slub.h>
#include <zgen/mm/mem.h>
#include <zgen/mm/pmm.bits.h>

extern "C" u64 __kernel_start;
extern "C" u64 __kernel_end;
extern "C" u64 __kernel_load_end;
extern "C" u64 __bss_start;
extern "C" u64 __bss_end;

namespace Zgen::Core {

using Sdk::Text::Align;
using Sdk::Text::aligned;

Opt<PmmBits> _pmm;
Opt<KmmSlub> _kmm;

Res<> setupMemory(PrekernelInfo* info) {
    // MARK: - vmm
    Core::createKernelVmm().unwrap("Failed to create global vmm");
    logInfo("Created and loaded global vmm.\n");

    usize kImageSize
        = Hal::pageAlignUp((uflat) &__kernel_end - 0xffff'ffff'8010'0000);

    // MARK: - scan memory layout
    logInfo("Scanning usable memory regions...\n");
    for (int i = 0; i < info->memmap.len(); i++)
        if (info->memmap[i].range.start() == 0x10'0000) {
            info->memmap[i].range.take(kImageSize);
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
    auto usablePages = usable->size() / Hal::PAGE_SIZE;

    if (usable->size() < 16_MiB) {
        logError("Not enough usable memory found\n");
        return Error::outOfMemory("not enough memory");
    }
    logInfo("Done! Usable memory: {} KiB\n", usable->size() / 1_KiB);

    // MARK: - pmm
    usize bitsSize = Hal::pageAlignUp(
        Math::div(usable->end(), Hal::PAGE_SIZE * 8, Math::ROUND_UP));
    Opt<Hal::PmmRange> bitsRange = NONE;

    for (auto& entry : iter(info->memmap)) {
        if ((entry.type != 0)
            or (entry.range.start() == 0)
            or (entry.range.size() < bitsSize)) {
            continue;
        }

        auto range = entry.range.take(bitsSize);
        logInfo("Reserving {:#x} - {:#x} for pmm bits\n",
                range.start(),
                range.start() + bitsSize);
        bitsRange.emplace(range.start(), bitsSize);
        break;
    }
    if (not bitsRange) {
        return Error::outOfMemory("no suitable range for pmm bits");
    }
    _pmm.emplace(
        *usable,
        Bits((u8*) mmapVirtIo(bitsRange->start()).take(), bitsRange->size()));

    iter(info->memmap)
        .filter$((it.type == 0) and (it.range.start() != 0))
        .forEach$(_pmm->mark(it.range.template into<Hal::PmmRange>().inner(
                                 Hal::PAGE_SIZE),
                             false)
                      .unwrap());

    struct _Kmm : Hal::Kmm {
        Res<Hal::KmmRange> alloc(usize                     size,
                                 Flags<Hal::KmmAllocFlags> flags
                                 = {}) override {
            auto pmm = Core::pmm()
                           .alloc(Hal::pageAlignUp(size))
                           .unwrap("Kmm::alloc: pmm alloc failed")
                           .into<Hal::KmmRange>();
            return Ok(Core::mmapVirtIoRange(pmm).take());
        }

        Res<> free(uflat addr) override { return Error::notImplemented(); }

        Res<> free(Hal::KmmRange range) override {
            return Error::notImplemented();
        }
    } kmm;

    // MARK: - kmm

    // Slice<KmmSlub::Block> blocks {
    //     try$(kmm.alloc((usablePages * sizeof(KmmSlub::Block))))
    //         .start()
    //         .as<KmmSlub::Block>(),
    //     usablePages
    // };

    _kmm.emplace(try$(kmm.alloc(Hal::PAGE_SIZE)),
                 try$(kmm.alloc(usablePages * sizeof(KmmSlub::Block))));

    __asm__ __volatile__("cli; hlt");

    return Ok();
}

Hal::Pmm& pmm() {
    if (not _pmm) {
        panic("Core::pmm: not initialized");
    }
    return *_pmm;
}

Hal::Kmm& kmm() {
    if (not _kmm) {
        panic("Core::kmm: not initialized");
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
