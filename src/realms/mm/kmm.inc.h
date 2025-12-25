#pragma once

#include <realms/hal/kmm.h>
#include <realms/hal/pmm.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/ref.h>

namespace Realms::Core {

struct KmmInc : public Hal::Kmm {
    Nonnull<Hal::Pmm>  _pmm;
    Opt<Hal::KmmRange> _range;

    KmmInc(Hal::Pmm& pmm, Hal::KmmRange range) : _pmm(&pmm), _range(range) { }
    ~KmmInc() = default;

    Res<usize> expand(usize size);

    Res<Hal::KmmRange> alloc(usize                     size,
                             Flags<Hal::KmmAllocFlags> flags) override;

    Res<> free(uflat addr) override;

    Res<> free(Hal::KmmRange range) override;
};

} // namespace Realms::Core
