#pragma once

#include <realms/hal/kmm.h>
#include <realms/hal/pmm.h>
#include <sdk-meta/opt.h>

namespace Realms::Sys {

struct KmmInc : public Hal::Kmm {
    Hal::Pmm&          _pmm;
    Opt<Hal::KmmRange> _range;

    KmmInc(Hal::Pmm& pmm, Hal::KmmRange range) : _pmm(pmm), _range(range) { }
    ~KmmInc() = default;

    Res<usize> expand(usize size);

    Res<Hal::KmmRange> alloc(usize size, Flags<Hal::KmmFlags> flags) override;

    Res<> free(uflat addr) override;

    Res<> free(Hal::KmmRange range) override;
};

} // namespace Realms::Sys
