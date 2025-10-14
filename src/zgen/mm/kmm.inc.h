#pragma once

#include <sdk-meta/opt.h>
#include <sdk-meta/ref.h>
#include <zgen/hal/kmm.h>
#include <zgen/hal/pmm.h>

namespace Zgen::Core {

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

} // namespace Zgen::Core
