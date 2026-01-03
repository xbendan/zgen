#include <realms/hal/vmm.h>
#include <realms/mm/pmm.bits.h>
#include <sdk-logs/logger.h>

namespace Realms::Sys {

Res<Hal::PmmRange> PmmBits::alloc(u64 size, Flags<Hal::PmmFlags> flags) {
    LockScoped lock(_lock);

    size = Hal::pageAlignUp(size);
    size /= Hal::PAGE_SIZE;

    auto range = try$(_bits.alloc(size, 0));
    logInfo("PmmBits::alloc: allocated bits range {} - {} ({} pages)\n",
            range.start(),
            range.end(),
            range.size());

    return Ok(asPmmRange(range));
}

Res<> PmmBits::free(Hal::PmmRange range) {
    pre$(_usable.overlaps(range));
    pre$(range.aligned(Hal::PAGE_SIZE));

    LockScoped lock(_lock);
    _bits.setRange(asBitsRange(range), false);

    return Ok();
}

Res<> PmmBits::take(Hal::PmmRange range) {
    pre$(_usable.overlaps(range));
    pre$(range.aligned(Hal::PAGE_SIZE));

    LockScoped lock(_lock);
    _bits.setRange(asBitsRange(range), true);

    return Ok();
}

Res<> PmmBits::mark(Hal::PmmRange range, bool used) {
    pre$(_usable.overlaps(range));
    pre$(range.aligned(Hal::PAGE_SIZE));

    LockScoped lock(_lock);
    auto       bitsRange = asBitsRange(range);
    logInfo("PmmBits::mark: marking range {:#x} - {:#x} as {}\n",
            bitsRange.start(),
            bitsRange.end(),
            used ? "used"s : "free"s);
    _bits.setRange(bitsRange, used);

    return Ok();
}

} // namespace Realms::Sys

