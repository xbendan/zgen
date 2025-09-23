#pragma once

#include <sdk-meta/panic.h>
#include <sdk-meta/slice.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

template <typename T, usize N>
struct Array {
    using Inner = T;

    T _buf[N];
    static_assert(N > 0, "Array::N must be greater than 0");

    always_inline constexpr T& operator[](usize i) {
        if (i >= N) [[unlikely]] {
            panic("Array::operator[]: index out of bounds: {} >= {}");
        }
        return _buf[i];
    }

    always_inline constexpr T const& operator[](usize i) const {
        if (i >= N) [[unlikely]] {
            panic("Array::operator[]: index out of bounds: {} >= {}");
        }
        return _buf[i];
    }

    always_inline constexpr T* begin() { return _buf; }

    always_inline constexpr T* end() { return _buf + N; }

    always_inline constexpr T const* begin() const { return _buf; }

    always_inline constexpr T const* end() const { return _buf + N; }

    always_inline constexpr T* buf() { return _buf; }

    always_inline constexpr T const* buf() const { return _buf; }

    always_inline constexpr usize len() const { return N; }
};
