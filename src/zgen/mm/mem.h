#pragma once

#include <sdk-meta/rc.h>
#include <sdk-meta/ref.h>
#include <zgen/hal/kmm.h>
#include <zgen/hal/pmm.h>
#include <zgen/hal/vmm.h>

namespace Zgen::Core {

Hal::Pmm& pmm();

Hal::Kmm& kmm();

Res<uflat> mmapIo(uflat phys);

Hal::Vmm& globalVmm();

Res<Hal::Vmm&> createKernelVmm();

Res<Rc<Hal::Vmm>> createUserVmm();

} // namespace Zgen::Core
