#pragma once

#include <realms/hal/kmm.h>
#include <realms/hal/pmm.h>
#include <realms/hal/vmm.h>
#include <sdk-meta/range.h>
#include <sdk-meta/rc.h>

namespace Realms::Sys {

struct MemoryRange : Range<uflat, struct _MemoryRangeTag> {
    using Range<uflat, struct _MemoryRangeTag>::Range;

    enum Type {
        Usable,
        Reserved,
        Reclaimable,
        Corrupted,
        Nvs,
    } type;

    [[gnu::always_inline]] bool usable() const { return type == Type::Usable; }
};

Sys::Pmm& pmm();

Sys::Kmm& kmm();

Opt<uflat> mmapVirtIo(uflat phys);

template <typename R>
Opt<R> mmapVirtIoRange(R phys) {
    return mmapVirtIo(phys.start()).mapTo$(R(it, phys.size()));
}

Opt<uflat> mmapPhys(uflat virt);

Sys::Vmm& globalVmm();

Res<Sys::Vmm&> createKernelVmm();

Res<Rc<Sys::Vmm>> createUserVmm();

} // namespace Realms::Sys
