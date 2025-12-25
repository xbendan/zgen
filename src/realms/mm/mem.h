#pragma once

#include <realms/hal/kmm.h>
#include <realms/hal/pmm.h>
#include <realms/hal/vmm.h>
#include <sdk-meta/range.h>
#include <sdk-meta/rc.h>
#include <sdk-meta/ref.h>

namespace Realms::Core {

Hal::Pmm& pmm();

Hal::Kmm& kmm();

Opt<uflat> mmapVirtIo(uflat phys);

template <typename R>
Opt<R> mmapVirtIoRange(R phys) {
    return mmapVirtIo(phys.start()).mapTo$(R(it, phys.size()));
}

Opt<uflat> mmapPhys(uflat virt);

Hal::Vmm& globalVmm();

Res<Hal::Vmm&> createKernelVmm();

Res<Rc<Hal::Vmm>> createUserVmm();

} // namespace Realms::Core
