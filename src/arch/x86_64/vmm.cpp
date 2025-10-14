#include <arch/x86_64/vmm.h>
#include <sdk-meta/literals.h>
#include <sdk-meta/opt.h>
#include <zgen/hal/vma.h>
#include <zgen/mm/mem.h>

namespace Zgen::Hal::x86_64 {

Opt<x86_64::Vmm> _vmm = NONE;
Pml<4>           _kpml4;
Pml<3>           _kDirectRegion;
Pml<3>           _kCoreRegionL3;
Pml<2>           _kCoreRegionL2; // 512 * 2MiB pages, lower half maps to kernel
                                 // higher half can be used for modules etc.

template <usize L>
Res<> Pml<L>::map(Index index, uflat addr, Flags<Hal::VmmFlags> flags) {
    prerequisite$(index.val < Len,
                  "Pml::map: index {} out of range [0, {})",
                  index.val,
                  Len);
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
Res<> Pml<L>::map(VmmRange virt, PmmRange phys, Flags<Hal::VmmFlags> flags) {
    prerequisite$(virt.aligned(granularity()),
                  "Pml::map: virt {:#x} is not aligned to granularity.\n",
                  virt.start());
    prerequisite$(phys.aligned(granularity()),
                  "Pml::map: phys {:#x} is not aligned to granularity.\n",
                  phys.start());
    prerequisite$(virt.size() == phys.size(),
                  "Pml::map: size of virt ({}) and phys ({}) do not "
                  "match",
                  virt.size(),
                  phys.size());

    IndexRange range = indexRange(indexOf(virt.start()),
                                  min((virt.size() / granularity()), 0x200));

    iter(*this)
        .skip(range.start())
        .limit(range.size())
        .filter$(not it.present())
        .index()
        .forEach$(it.v1.with(flags).with(phys.start() + it.v0 * granularity()));
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
    if (vrange and not vrange->aligned(Hal::PAGE_SIZE)) {
        logError("Vmm::alloc: given range {:#x} is not aligned to {:#x}",
                 vrange->_start,
                 Hal::PAGE_SIZE);
        return Error::invalidArgument("Vmm::alloc: range is not page aligned");
    }

    if (amount == 0) {
        return Error::invalidArgument("Vmm::alloc: amount is zero");
    }

    if (auto bitsRange = _bits.alloc(
            amount,
            vrange.unwrapOrElse({ 0, 0 }).downscale(Hal::PAGE_SIZE).start())) {
        return Ok(bitsRange->into<VmmRange>().downscale(Hal::PAGE_SIZE));
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
    return Error::notImplemented("Vmm::map: not implemented");
}

Res<> Vmm::unmap(VmmRange range) {
    return Error::notImplemented("Vmm::unmap: not implemented");
}

Res<VmmPage> Vmm::at(usize address) {
    return Error::notImplemented("Vmm::at: not implemented");
}

Res<> Vmm::load() {
    asm volatile("mov %0, %%cr3" ::"r"((u64) &_pml4 - KernelCoreRegion.start())
                 : "memory");
    return Ok();
}

Res<> createKernelVmm() {
    if (_vmm) {
        return Error::alreadyExists(
            "x86_64::createKernelVmm: kernel vmm already exists");
    }

    try$(_kpml4.map(Hal::KernelDirectRegion.start(),
                    (uflat) &_kDirectRegion - Hal::KernelCoreRegion.start()));
    try$(_kDirectRegion.map({ Hal::KernelDirectRegion.start(), 512_GiB },
                            { 0x0, 512_GiB }));

    try$(_kpml4.map(Hal::KernelCoreRegion.start(),
                    (uflat) &_kCoreRegionL3 - Hal::KernelCoreRegion.start()));
    try$(_kCoreRegionL3.map(Hal::KernelCoreRegion.start(),
                            (uflat) &_kCoreRegionL2
                                - Hal::KernelCoreRegion.start()));
    try$(_kCoreRegionL2.map({ Hal::KernelCoreRegion.start(), 512_MiB },
                            { 0x0, 512_MiB }));

    _vmm.emplace(Core::pmm(), &_kpml4);

    return Ok();
}

} // namespace Zgen::Hal::x86_64

namespace Zgen::Core {

using Hal::x86_64::_vmm;

Hal::Vmm& globalVmm() {
    if (not _vmm) {
        logWarn("Core::globalVmm: no global vmm exists, creating one...\n");

        Hal::x86_64::createKernelVmm().unwrap(
            "Core::globalVmm: failed to create kernel vmm");
        logInfo("Core::globalVmm: created global vmm\n");

        __asm__ __volatile__("cli; hlt;");
    }
    return *_vmm;
}

} // namespace Zgen::Core
