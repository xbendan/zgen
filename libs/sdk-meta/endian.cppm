module;

export module sdk:endian;

import :slice;
import :types;

#ifdef __SIZEOF_INT128__
export using u128 = __uint128_t;

export inline u128 _bswap128(u128 value) {
    u64 high = __builtin_bswap64(static_cast<u64>(value));
    u64 low  = __builtin_bswap64(static_cast<u64>(value >> 64));
    return (static_cast<u128>(high) << 64) | low;
}
#endif

export template <typename T>
    requires(sizeof(T) <= 16)
[[gnu::always_inline]] constexpr T bswap(T value) {
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
export template <typename T>
[[gnu::always_inline]] constexpr T toLe(T value) {
    return value;
}
#else
export template <typename T>
[[gnu::always_inline]] constexpr T toLe(T value) {
    return bswap(value);
}
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
export template <typename T>
[[gnu::always_inline]] constexpr T toBe(T value) {
    return bswap(value);
}
#else
export template <typename T>
[[gnu::always_inline]] constexpr T toBe(T value) {
    return value;
}
#endif

export constexpr bool isLittleEndian() {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return true;
#else
    return false;
#endif
}

export constexpr bool isBigEndian() {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return true;
#else
    return false;
#endif
}

export template <typename T>
struct [[gnu::packed]] Be {
    T _value;

    [[gnu::always_inline]] constexpr Be() = default;

    [[gnu::always_inline]] constexpr Be(T value) : _value(toBe(value)) { }

    [[gnu::always_inline]] constexpr operator T() const { return value(); }

    [[gnu::always_inline]] constexpr Bytes bytes() const {
        return Bytes((u8 const*) &_value, sizeof(T));
    }

    [[gnu::always_inline]] constexpr T value() const { return toBe(_value); }
};

export template <typename T>
struct [[gnu::packed]] Le {
    T _value;

    [[gnu::always_inline]] constexpr Le() = default;

    [[gnu::always_inline]] constexpr Le(T value) : _value(toLe(value)) { }

    [[gnu::always_inline]] constexpr operator T() const { return value(); }

    [[gnu::always_inline]] constexpr Bytes bytes() const {
        return Bytes((u8 const*) &_value, sizeof(T));
    }

    [[gnu::always_inline]] constexpr T value() const { return toLe(_value); }
};

static_assert(sizeof(Be<u32>) == sizeof(u32));
static_assert(sizeof(Le<u32>) == sizeof(u32));
static_assert(Pod<Le<u32>>);
static_assert(Pod<Be<u32>>);

export using u8be  = Be<u8>;
export using u16be = Be<u16>;
export using u32be = Be<u32>;
export using u64be = Be<u64>;

export using i8be  = Be<i8>;
export using i16be = Be<i16>;
export using i32be = Be<i32>;
export using i64be = Be<i64>;

export using u8le  = Le<u8>;
export using u16le = Le<u16>;
export using u32le = Le<u32>;
export using u64le = Le<u64>;

export using i8le  = Le<i8>;
export using i16le = Le<i16>;
export using i32le = Le<i32>;
export using i64le = Le<i64>;
