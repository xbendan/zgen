#pragma once

#include <sdk-meta/range.h>
#include <sdk-meta/types.h>

namespace Meta {

struct Index {
    usize val;
    bool  inverse;

    constexpr Index() = delete;

    [[gnu::always_inline]] constexpr Index(usize v, bool inv = false)
        : val(v),
          inverse(inv) { }

    [[gnu::always_inline]] constexpr operator usize() const { return val; }
};

// clang-format off

constexpr auto operator""i(unsigned long long int v) {
    return Index { static_cast<usize>(v), false };
}

constexpr auto operator""ie(unsigned long long int v) {
    return Index { static_cast<usize>(v), true };
}

using IndexRange = Range<Index, struct _IndexRangeTag>;

inline IndexRange indexRange(usize start, usize size = 1) {
    return { Index { start, false }, size };
}

// clang-format on

} // namespace Meta

using Meta::Index;
using Meta::IndexRange;
using Meta::indexRange;
