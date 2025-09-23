#pragma once

#include <neodymium/hal/kmm.h>
#include <neodymium/hal/pmm.h>
#include <neodymium/hal/vmm.h>
#include <neodymium/init/prekernel.h>

namespace Sys {

struct PmmBuddy : public Hal::Pmm {
    struct _Page { };

    Res<Hal::PmmRange> alloc(u64                  size,
                             Flags<Hal::PmmFlags> flags = {}) override;

    Res<> free(Hal::PmmRange range) override;

    Res<> take(Hal::PmmRange range) override;
};

} // namespace Sys
