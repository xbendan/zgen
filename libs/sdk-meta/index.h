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

    [[gnu::always_inline]] constexpr auto operator<=>(
        Index const& other) const {
        return val <=> other.val;
    }

    [[gnu::always_inline]] constexpr auto operator<=>(usize other) const {
        return val <=> other;
    }

    [[gnu::always_inline]] constexpr bool operator==(Index const& other) const {
        return val == other.val;
    }
};

// clang-format off

constexpr auto operator""i(unsigned long long int v) {
    return Index { static_cast<usize>(v), false };
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
