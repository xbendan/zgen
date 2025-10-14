#include <zgen/hal/vmm.h>
#include <zgen/mm/pmm.bits.h>

namespace Zgen::Core {

Res<Hal::PmmRange> PmmBits::alloc(u64 size, Flags<Hal::PmmFlags> flags) {
    LockScoped lock(_lock);

    size = Hal::pageAlignUp(size);
    size /= Hal::PAGE_SIZE;
    auto range = try$(_bits.alloc(size, flags[Hal::PmmFlags::Kernel] ? -1 : 0));
    return Ok(asPmmRange(range));
}

Res<> PmmBits::free(Hal::PmmRange range) {
    if (not _usable.contains(range)) {
        return Error::invalidArgument("PmmBits::free: pmm range out of bounds");
    }

    if (not range.aligned(Hal::PAGE_SIZE)) {
        return Error::invalidArgument("PmmBits::free: pmm range not aligned");
    }

    LockScoped lock(_lock);

    auto bitsRange = asBitsRange(range);
    _bits.setRange<false>(bitsRange);
    return Ok();
}

Res<> PmmBits::take(Hal::PmmRange range) {
    if (not _usable.overlaps(range)) {
        return Error::invalidArgument("PmmBits::take: pmm range out of bounds");
    }

    if (not range.aligned(Hal::PAGE_SIZE)) {
        return Error::invalidArgument("PmmBits::take: pmm range not aligned");
    }

    LockScoped lock(_lock);
    auto       bitsRange = asBitsRange(range);
    _bits.setRange<true>(bitsRange);
    return Ok();
}

} // namespace Zgen::Core

