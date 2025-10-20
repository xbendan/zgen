#include <arch/x86_64/cpu.h>
#include <arch/x86_64/vmm.h>
#include <sdk-meta/literals.h>
#include <sdk-meta/opt.h>
#include <zgen/mm/mem.h>

namespace Zgen::Hal::x86_64 {

Opt<x86_64::Vmm> _vmm = NONE;

Pml<4>  _kpml4;
Pml<3>  _kPhysPdpt;
Pml<2>  _kImagePde;
Pml<3>  _kpdpt, _kpdptLo;
Pml<2>* _kHeapDir;

template <usize L>
Res<> Pml<L>::map(Index index, uflat addr, Flags<Hal::VmmFlags> flags) {
    pre$(index.val < Len);
    auto& e = entries[index.val];

    if (not e.present()) {
        e.with(flags).with(addr);
        logInfo(
            "Pml<{}>::map: mapped index {} -> {:#x}\n", Level, index.val, addr);
        return Ok();
    }

    return Error::alreadyExists("Pml::map: entry already exists");
}

template <usize L>
Res<> Pml<L>::mapRange(VmmRange             virt,
                       PmmRange             phys,
                       Flags<Hal::VmmFlags> flags) {
    pre$(virt.aligned(granularity()) and phys.aligned(granularity()));
    pre$(virt.size() == phys.size());

    IndexRange range
        = indexRange(indexOf(virt.start()), (virt.size() / granularity()));
    range.end(0x200);

    for (usize i = range.start(); i < range.end(); i++) {
        entries[i].with(flags).with(phys.start() + i * granularity());
    }
    logInfo("Pml<{}>::map: mapped index {}-{} ({:#d}) -> {:#x}\n",
            Level,
            range.start(),
            range.end() - 1,
            range.size(),
            phys.start());

    return Ok();
}

Res<VmmRange> Vmm::alloc(Opt<VmmRange>   vrange,
                         usize           amount,
                         Flags<VmmFlags> flags) {
    pre$(not vrange or vrange->aligned(Hal::PAGE_SIZE));
    pre$(amount > 0);

    if (auto bitsRange = _bits.alloc(
            amount,
            vrange.unwrapOrElse({ 0, 0 }).downscale(Hal::PAGE_SIZE).start())) {
        return Ok(bitsRange->into<VmmRange>()
                      .upscale(Hal::PAGE_SIZE)
                      .offset(Hal::HEAP_REGION.start()));
    }

    return Error::outOfMemory("Vmm::alloc: failed to allocate bits");
}

Res<> Vmm::free(VmmRange range) {
    // if (not range.aligned(Hal::PAGE_SIZE)) {
    //     logError("Vmm::free: given range {:#x} is not aligned to {:#x}",
    //              range._start,
    //              Hal::PAGE_SIZE);
    //     return Error::invalidArgument("Vmm::free: range is not page aligned");
    // }

    // auto bitsRange = range.downscale(Hal::PAGE_SIZE);

    // if (not _bits.free(bitsRange)) {
    //     logError("Vmm::free: failed to free range {:#x}", range._start);
    //     return Error::invalidArgument("Vmm::free: failed to free range");
    // }

    // return Ok();

    return Error::notImplemented("Vmm::free: not implemented");
}

Res<> Vmm::map(VmmRange virt, PmmRange phys, Flags<VmmFlags> flags) {
    pre$(virt.aligned(Hal::PAGE_SIZE) and phys.aligned(Hal::PAGE_SIZE));
    pre$((virt.size() == phys.size()) and (phys.size() % Hal::PAGE_SIZE == 0));
    pre$(Hal::HEAP_REGION.contains(virt));

    return Error::notImplemented("Vmm::map: not implemented");
}

Res<> Vmm::unmap(VmmRange range) {
    return Error::notImplemented("Vmm::unmap: not implemented");
}

Res<VmmPage> Vmm::at(usize address) {
    return Error::notImplemented("Vmm::at: not implemented");
}

Res<> Vmm::load() {
    asm volatile("mov %0, %%cr3" ::"r"((u64) _pml4 - (u64) CORE_REGION.start())
                 : "memory");

    return Ok();
}

Res<> createKernelVmm() {
    if (_vmm) {
        return Error::alreadyExists(
            "x86_64::createKernelVmm: kernel vmm already exists");
    }

    // map kernel physical memory region
    try$(_kpml4.map(DIRECT_IO_REGION.start(),
                    &_kPhysPdpt - CORE_REGION.start()));
    try$(_kPhysPdpt.mapRange({ KERNEL_REGION.start(), 512_GiB },
                             { 0x0, 512_GiB }));

    // map kernel image region
    try$(_kpml4.map(CORE_REGION.start(), &_kpdpt - CORE_REGION.start()));
    try$(_kpml4.map(USER_REGION.start(), &_kpdptLo - CORE_REGION.start()));
    try$(_kpdpt.map(CORE_REGION.start(), &_kImagePde - CORE_REGION.start()));
    try$(_kpdptLo.map(USER_REGION.start(), &_kImagePde - CORE_REGION.start()));
    try$(_kImagePde.mapRange({ 0xffff'ffff'8000'0000, 512_MiB },
                             { 0x0, 512_MiB }));

    _vmm.emplace(Core::pmm(), &_kpml4);

    return Ok();
}

} // namespace Zgen::Hal::x86_64

namespace Zgen::Core {

using Hal::x86_64::_vmm;

Res<Hal::Vmm&> createKernelVmm(Hal::PmmRange kernRange) {
    try$(Hal::x86_64::createKernelVmm());
    try$(_vmm->load());
    try$(Core::pmm().take({ 0x0, 0x10'0000 }));
    try$(Core::pmm().take(kernRange));

    auto heapBits = Core::pmm()
                        .alloc(0x2'0000)
                        .unwrap(
                            "Core::createKernelVmm: failed to allocate "
                            "kernel heap bits")
                        .offset(Hal::DIRECT_IO_REGION.start());
    logInfo("Kernel heap bits initialized at {:#x} - {:#x}\n",
            heapBits.start(),
            heapBits.end());
    _vmm.emplace(Core::pmm(), &Hal::x86_64::_kpml4, heapBits.bytes());

    auto heapRange = Core::pmm()
                         .alloc(16_KiB)
                         .unwrap(
                             "Core::createKernelVmm: failed to allocate kernel "
                             "heap memory")
                         .offset(Hal::DIRECT_IO_REGION.start());

    Hal::x86_64::_kHeapDir = heapRange.start().as<Hal::x86_64::Pml<2>>();
    logInfo("Kernel heap initialized at {:#x} - {:#x}\n",
            heapRange.start(),
            heapRange.end());

    return Ok(*_vmm);
}

Hal::Vmm& globalVmm() {
    return *_vmm;
}

} // namespace Zgen::Core
