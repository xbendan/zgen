#pragma once

#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

// https://www.ietf.org/archive/id/draft-eastlake-fnv-21.html
static constexpr u64 fnv64(byte const* buf, usize len) {
    u64 hash = 0xcbf2'9ce4'8422'2325;
    for (usize i = 0; i < len; i++) {
        hash ^= buf[i];
        hash *= 0x100'0000'01b3;
    }
    return hash;
}

static constexpr u64 hash(Meta::Boolean auto const& v) {
    return hash(v ? 0x1 : 0x0);
}

static_assert(Meta::Integral<char>);

static constexpr u64 hash(Meta::Integral auto const& v) {
    return fnv64(reinterpret_cast<byte const*>(&v), sizeof(v));
}

static constexpr u64 hash(Meta::Float auto const& v) {
    return fnv64(reinterpret_cast<byte const*>(&v), sizeof(v));
}

template <typename T>
static constexpr u64 hash(T const& t)
    requires requires(T const t) {
        { t.hash() } -> Meta::Same<u64>;
    }
{
    return t.hash();
}

static constexpr u64 hash(Meta::Enum auto const& v) {
    return hash(toUnderlyingType(v));
}

static constexpr u64 hash() {
    return 0xcbf2'9ce4'8422'2325;
}

template <typename T>
static constexpr u64 hash(u64 a, T const& v) {
    return a ^ (hash(v) + 0x9e37'79b9'7f4a'7c15);
}
