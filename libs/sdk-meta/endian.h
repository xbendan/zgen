#pragma once

#include <sdk-meta/slice.h>
#include <sdk-meta/types.h>

#ifdef __SIZEOF_INT128__
using u128 = __uint128_t;

inline u128 _bswap128(u128 value) {
    u64 high = __builtin_bswap64(static_cast<u64>(value));
    u64 low  = __builtin_bswap64(static_cast<u64>(value >> 64));
    return (static_cast<u128>(high) << 64) | low;
}
#endif

template <typename T>
    requires(sizeof(T) <= 16)
always_inline constexpr T bswap(T value) {
#ifdef __SIZEOF_INT128__
    if (sizeof(T) == 16)
        return _bswap128(value);
#endif
    if (sizeof(T) == 8)
        return __builtin_bswap64(value);
    if (sizeof(T) == 4)
        return __builtin_bswap32(value);
    if (sizeof(T) == 2)
        return __builtin_bswap16(value);
    if (sizeof(T) == 1)
        return value;
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
template <typename T>
always_inline constexpr T toLe(T value) {
    return value;
}
#else
template <typename T>
always_inline constexpr T toLe(T value) {
    return bswap(value);
}
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
template <typename T>
always_inline constexpr T toBe(T value) {
    return bswap(value);
}
#else
template <typename T>
always_inline constexpr T toBe(T value) {
    return value;
}
#endif

static constexpr bool isLittleEndian() {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return true;
#else
    return false;
#endif
}

static constexpr bool isBigEndian() {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return true;
#else
    return false;
#endif
}

template <typename T>
struct [[gnu::packed]] Be {
    T _value;

    always_inline constexpr Be() = default;

    always_inline constexpr Be(T value) : _value(toBe(value)) { }

    always_inline constexpr operator T() const { return value(); }

    always_inline constexpr Bytes bytes() const {
        return Bytes((u8 const*) &_value, sizeof(T));
    }

    always_inline constexpr T value() const { return toBe(_value); }
};

template <typename T>
struct [[gnu::packed]] Le {
    T _value;

    always_inline constexpr Le() = default;

    always_inline constexpr Le(T value) : _value(toLe(value)) { }

    always_inline constexpr operator T() const { return value(); }

    always_inline constexpr Bytes bytes() const {
        return Bytes((u8 const*) &_value, sizeof(T));
    }

    always_inline constexpr T value() const { return toLe(_value); }
};

static_assert(sizeof(Be<u32>) == sizeof(u32));
static_assert(sizeof(Le<u32>) == sizeof(u32));
static_assert(Meta::Pod<Le<u32>>);
static_assert(Meta::Pod<Be<u32>>);

using u8be  = Be<u8>;
using u16be = Be<u16>;
using u32be = Be<u32>;
using u64be = Be<u64>;

using i8be  = Be<i8>;
using i16be = Be<i16>;
using i32be = Be<i32>;
using i64be = Be<i64>;

using u8le  = Le<u8>;
using u16le = Le<u16>;
using u32le = Le<u32>;
using u64le = Le<u64>;

using i8le  = Le<i8>;
using i16le = Le<i16>;
using i32le = Le<i32>;
using i64le = Le<i64>;
