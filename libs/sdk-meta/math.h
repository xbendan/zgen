#pragma once

#include <sdk-meta/limits.h>
#include <sdk-meta/types.h>

always_inline constexpr auto max(auto v) {
    return v;
}

always_inline constexpr auto max(auto v, auto... vs) {
    auto rhs = max(vs...);
    return (v > rhs) ? v : rhs;
}

always_inline constexpr auto min(auto v) {
    return v;
}

always_inline constexpr auto min(auto v, auto... vs) {
    auto rhs = min(vs...);
    return (v < rhs) ? v : rhs;
}

always_inline constexpr auto clamp(auto v, auto min_v, auto max_v) {
    return (v < min_v) ? min_v : (v > max_v) ? max_v : v;
}

always_inline constexpr auto abs(auto v) {
    return (v < 0) ? -v : v;
}

// Align the nearest _lower_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 8
// ex: 7 with align = 8 -> 0
always_inline constexpr usize alignDown(usize addr, usize align) {
    return addr & ~(align - 1);
}

// Align the nearest _upper_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 8
always_inline constexpr usize alignUp(usize addr, usize align) {
    return (addr + align - 1) & ~(align - 1);
}

always_inline constexpr bool isAlign(usize addr, usize align) {
    return alignDown(addr, align) == addr;
}
