#pragma once

#include <sdk-meta/traits.h>

namespace Meta {

#define MakeFlags$(T)                                                          \
    constexpr inline T operator~(T rhs) {                                      \
        using U = Meta::UnderlyingType<T>;                                     \
        return (T) ~(U) rhs;                                                   \
    }                                                                          \
    constexpr inline T operator|(T lhs, T rhs) {                               \
        using U = Meta::UnderlyingType<T>;                                     \
        return (T) ((U) lhs | (U) rhs);                                        \
    }                                                                          \
    constexpr inline T operator&(T lhs, T rhs) {                               \
        using U = Meta::UnderlyingType<T>;                                     \
        return (T) ((U) lhs & (U) rhs);                                        \
    }                                                                          \
    constexpr inline T operator^(T lhs, T rhs) {                               \
        using U = Meta::UnderlyingType<T>;                                     \
        return (T) ((U) lhs ^ (U) rhs);                                        \
    }                                                                          \
    constexpr inline T& operator|=(T& lhs, T rhs) {                            \
        return lhs = lhs | rhs;                                                \
    }                                                                          \
    constexpr inline T& operator&=(T& lhs, T rhs) {                            \
        return lhs = lhs & rhs;                                                \
    }                                                                          \
    constexpr inline T& operator^=(T& lhs, T rhs) {                            \
        return lhs = lhs ^ rhs;                                                \
    }

template <Meta::Enum E>
struct Flags {
    using U = Meta::UnderlyingType<E>;

public:
    constexpr Flags() noexcept = default;

    // constexpr Flags(U value) noexcept : _value(value) { }

    constexpr Flags(E value) noexcept : _value(static_cast<U>(value)) { }

    constexpr Flags(Flags const& other) noexcept = default;

    constexpr Flags(Flags&& other) noexcept = default;

    constexpr Flags& operator=(Flags const& other) noexcept = default;

    constexpr Flags& operator=(Flags&& other) noexcept = default;

    constexpr Flags operator|(Flags other) const noexcept {
        return Flags(_value | other._value);
    }

    constexpr Flags operator&(Flags other) const noexcept {
        return Flags(_value & other._value);
    }

    constexpr Flags operator^(Flags other) const noexcept {
        return Flags(_value ^ other._value);
    }

    constexpr Flags operator~() const noexcept { return Flags(~_value); }

    constexpr Flags& operator|=(Flags other) noexcept {
        _value |= other._value;
        return *this;
    }

    constexpr Flags& operator&=(Flags other) noexcept {
        _value &= other._value;
        return *this;
    }

    constexpr Flags& operator^=(Flags other) noexcept {
        _value ^= other._value;
        return *this;
    }

    constexpr bool operator==(Flags other) const noexcept {
        return _value == other._value;
    }

    constexpr bool operator!=(Flags other) const noexcept {
        return _value != other._value;
    }

    constexpr explicit operator U() const noexcept { return _value; }

    constexpr explicit operator bool() const noexcept { return _value != 0; }

    // constexpr bool operator!() const noexcept { return _value == 0; }

    constexpr U operator*() const noexcept { return _value; }

    constexpr bool operator[](E flag) const noexcept {
        return (_value & static_cast<U>(flag)) != 0;
    }

    constexpr void operator+=(E flag) noexcept {
        _value |= static_cast<U>(flag);
    }

    constexpr void operator-=(E flag) noexcept {
        _value &= ~static_cast<U>(flag);
    }

    constexpr void operator+=(Flags other) noexcept { _value |= other._value; }

    constexpr void operator-=(Flags other) noexcept { _value &= ~other._value; }

protected:
    U _value = 0;
};

} // namespace Meta

using Meta::Flags;
