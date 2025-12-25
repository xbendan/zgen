#pragma once

#include <realms/hal/pmm.h>
#include <sdk-meta/bits.h>
#include <sdk-meta/lock.h>

namespace Realms::Core {

struct PmmBits : public Hal::Pmm {
    Hal::PmmRange _usable;
    Bits          _bits;
    Lock          _lock;

    PmmBits(Hal::PmmRange usable, Bits bits) : _usable(usable), _bits(bits) {
        fill(_bits.bytes(), (u8) 0xff);
    }

    ~PmmBits() override = default;

    Res<Hal::PmmRange> alloc(u64                  size,
                             Flags<Hal::PmmFlags> flags = {}) override;

    Res<> free(Hal::PmmRange range) override;

    Res<> take(Hal::PmmRange range) override;

    Res<> mark(Hal::PmmRange range, bool used) override;

    BitsRange asBitsRange(Hal::PmmRange range) {
        range._start -= _usable._start;
        range._start /= Hal::PAGE_SIZE;
        range._size /= Hal::PAGE_SIZE;

        return range.into<BitsRange>();
    }

    Hal::PmmRange asPmmRange(BitsRange range) {
        range._start *= Hal::PAGE_SIZE;
        range._start += _usable._start;
        range._size *= Hal::PAGE_SIZE;

        return range.into<Hal::PmmRange>();
    }
};

} // namespace Realms::Core
