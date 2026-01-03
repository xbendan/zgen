module;

export module sdk:hash;

import :traits;
import :types;

// https://www.ietf.org/archive/id/draft-eastlake-fnv-21.html
export constexpr u64 fnv64(byte const* buf, usize len) {
    u64 hash = 0xcbf2'9ce4'8422'2325;
    for (usize i = 0; i < len; i++) {
        hash ^= buf[i];
        hash *= 0x100'0000'01b3;
    }
    return hash;
}

export constexpr u64 hash(Boolean auto const& v) {
    return hash(v ? 0x1 : 0x0);
}

static_assert(Integral<char>);

export constexpr u64 hash(Integral auto const& v) {
    return fnv64(reinterpret_cast<byte const*>(&v), sizeof(v));
}

export constexpr u64 hash(Float auto const& v) {
    return fnv64(reinterpret_cast<byte const*>(&v), sizeof(v));
}

export template <typename T>
constexpr u64 hash(T const& t)
    requires requires(T const t) {
        { t.hash() } -> Same<u64>;
    }
{
    return t.hash();
}

export constexpr u64 hash(Enum auto const& v) {
    return hash(toUnderlyingType(v));
}

export constexpr u64 hash() {
    return 0xcbf2'9ce4'8422'2325;
}

export template <typename T>
constexpr u64 hash(u64 a, T const& v) {
    return a ^ (hash(v) + 0x9e37'79b9'7f4a'7c15);
}
