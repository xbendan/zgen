#include <sdk-logs/logger.h>
#include <sdk-meta/literals.h>
#include <zgen/hal/vmm.h>
#include <zgen/mm/kmm.inc.h>
#include <zgen/mm/mem.h>

namespace Zgen::Core {

Res<usize> KmmInc::expand(usize size) {
    if (not isAlign(size, Hal::PAGE_SIZE)) {
        return Error::invalidArgument("size is not page aligned");
    }

    auto& vmm = Zgen::Core::globalVmm();
    try$(vmm.map({ _range->end(), size },
                 try$(_pmm->alloc(size, Hal::PmmFlags::Kernel)),
                 Hal::VmmFlags::WRITE));

    _range->_size += size;
    logInfo("KmmInc: expanded by {:#x}, new range: {:#x} - {:#x} ({} KiB)",
            size,
            _range->_start,
            _range->end(),
            _range->_size / 1_KiB);

    return Ok(size);
}

Res<Hal::KmmRange> KmmInc::alloc(usize size, Flags<Hal::KmmAllocFlags> flags) {
    while (uflat(size) > _range->_size) {
        try$(expand(Hal::pageAlignUp(size - _range->_size)));
    }

    return Ok(_range->take(size));
}

Res<> KmmInc::free(uflat addr) {
    return Error::notImplemented();
}

Res<> KmmInc::free(Hal::KmmRange range) {
    return Error::notImplemented();
}

} // namespace Zgen::Core
