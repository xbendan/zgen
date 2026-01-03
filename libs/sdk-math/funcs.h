#pragma once

#include <sdk-math/const.h>
#include <sdk-meta/limits.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

namespace Math {

enum DivisionPolicy {
    ROUND_UP,
    ROUND_DOWN,
    ROUND_NEAREST
};

[[gnu::always_inline]] constexpr auto div(auto           n,
                                          auto           d,
                                          DivisionPolicy policy = ROUND_DOWN) {
    switch (policy) {
        case ROUND_UP:      return (n + d - 1) / d;
        case ROUND_DOWN:    return n / d;
        case ROUND_NEAREST: return (n + d / 2) / d;
        default:            panic("div: unknown division policy");
    }
}

} // namespace Math
