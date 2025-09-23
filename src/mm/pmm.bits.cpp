#include <neodymium/hal/vmm.h>
#include <neodymium/mm/pmm.bits.h>
#include <sdk-meta/try.h>

namespace Sys {

Res<Hal::PmmRange> PmmBits::alloc(u64 size, Flags<Hal::PmmFlags> flags) {
    LockScoped lock(_lock);

    size = Hal::pageAlignUp(size);
    size /= Hal::PAGE_SIZE;
    auto range = try$(_bits.alloc(size, flags[Hal::PmmFlags::KERNEL] ? -1 : 0));
    return Ok(asPmmRange(range));
}

Res<> PmmBits::free(Hal::PmmRange range) {
    if (not _usable.contains(range)) {
        return Error::invalidArgument("pmm range out of bounds");
    }

    if (not range.aligned(Hal::PAGE_SIZE)) {
        return Error::invalidArgument("pmm range not aligned");
    }

    LockScoped lock(_lock);

    auto bitsRange = asBitsRange(range);
    _bits.setRange(bitsRange, false);
    return Ok();
}

Res<> PmmBits::take(Hal::PmmRange range) {
    if (not _usable.overlaps(range)) {
        return Error::invalidArgument("pmm range out of bounds");
    }

    if (not range.aligned(Hal::PAGE_SIZE)) {
        return Error::invalidArgument("pmm range not aligned");
    }

    LockScoped lock(_lock);
    auto       bitsRange = asBitsRange(range);
    _bits.setRange(bitsRange, true);
    return Ok();
}

} // namespace Sys

