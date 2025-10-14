#pragma once

#include <sdk-meta/hash.h>
#include <sdk-meta/panic.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>
#include <sdk-meta/utility.h>

template <typename T, typename U = typename T::Inner>
concept Sliceable = requires(T const& t) {
    typename T::Inner;
    { t.len() } -> Meta::Same<usize>;
    { t.buf() } -> Meta::Same<U const*>;
    { t[0uz] } -> Meta::Same<U const&>;
};

template <Sliceable T, Sliceable U>
    requires Meta::Comparable<typename T::Inner, typename U::Inner>
constexpr auto operator<=>(T const& lhs, U const& rhs)
    -> decltype(lhs[0uz] <=> rhs[0uz]) {
    for (usize i = 0; i < lhs.len() && i < rhs.len(); i++) {
        if (auto cmp = lhs[i] <=> rhs[i]; cmp != 0) {
            return cmp;
        }
    }
    return lhs.len() <=> rhs.len();
}

template <Sliceable T, Sliceable U>
    requires Meta::Equatable<typename T::Inner, typename U::Inner>
constexpr auto operator==(T const& lhs, U const& rhs) -> bool {
    if (lhs.len() != rhs.len()) {
        return false;
    }

    for (usize i = 0; i < lhs.len(); i++) {
        if (!(lhs[i] == rhs[i])) {
            return false;
        }
    }
    return true;
}

template <Sliceable T>
static constexpr u64 hash(T const& slice)
    requires(not requires(T const t) {
        { t.hash() } -> Meta::Same<u64>;
    })
{
    u64 res = ::hash();
    for (usize i = 0; i < slice.len(); i++) {
        res = hash(res, slice.buf()[i]);
    }
    return res;
}

template <typename T>
struct Slice {
    using Inner = T;

    T const* _buf {};
    usize    _len {};

    static constexpr Slice fromNullterminated(T const* buf) {
        usize len = 0;
        while (buf[len])
            len++;
        return { buf, len };
    }

    static constexpr Slice fromNullterminated(T const* buf, usize maxLen) {
        usize len = 0;
        while (buf[len] and len < maxLen)
            len++;
        return { buf, len };
    }

    constexpr Slice() = default;

    constexpr Slice(T const* buf, usize len) : _buf(buf), _len(len) { }

    constexpr Slice(T const* begin, T const* end)
        : _buf(begin),
          _len(end - begin) { }

    constexpr Slice(Sliceable<T> auto const& other)
        : _buf(other.buf()),
          _len(other.len()) { }

    constexpr T const& operator[](usize i) const {
        if (i >= _len) [[unlikely]] {
            panic("Slice::operator[]: index out of bounds: {} >= {}");
        }
        return _buf[i];
    }

    constexpr T& operator[](usize i) {
        if (i >= _len) [[unlikely]] {
            panic("Slice::operator[]: index out of bounds: {} >= {}");
        }
        return const_cast<T&>(_buf[i]);
    }

    constexpr T const* begin() const { return _buf; }

    constexpr T const* end() const { return _buf + _len; }

    constexpr T const* buf() const { return _buf; }

    constexpr T* buf() { return const_cast<T*>(_buf); }

    constexpr usize len() const { return _len; }

    template <typename U>
    constexpr Slice<U> cast() const {
        return { (U const*) _buf, _len };
    }

    constexpr explicit operator bool() const noexcept { return _len > 0; }
};

using Bytes        = Slice<byte>;
using CharSequence = Slice<char>;

static_assert(Sliceable<Bytes>);

constexpr auto begin(Sliceable auto const& slice) {
    return slice.buf();
}

constexpr auto end(Sliceable auto const& slice) {
    return slice.buf() + slice.len();
}

constexpr auto slice(Sliceable auto const& s, usize start, usize end)
    -> Slice<Meta::RemoveCvRef<decltype(s[0uz])>> {
    if (start > end || end > s.len()) [[unlikely]] {
        panic("Slice::slice: invalid range: {}..{} for length {}",
              start,
              end,
              s.len());
    }
    return { s.buf() + start, end - start };
}

constexpr bool isEmpty(Sliceable auto const& s) {
    return s.len() == 0;
}

constexpr auto first(Sliceable auto const& s) -> decltype(s[0uz]) {
    if (s.len() == 0) [[unlikely]] {
        panic("Slice::first: slice is empty");
    }
    return s[0uz];
}

constexpr auto last(Sliceable auto const& s) -> decltype(s[0uz]) {
    if (s.len() == 0) [[unlikely]] {
        panic("Slice::last: slice is empty");
    }
    return s[s.len() - 1];
}

constexpr usize len(Sliceable auto const& s) {
    return s.len();
}

template <Sliceable S>
Bytes bytes(S const& s) {
    return { reinterpret_cast<byte const*>(s.buf()),
             s.len() * sizeof(typename S::Inner) };
};

template <Sliceable S, typename T = typename S::Inner>
constexpr Slice<T> slice(S const& s) {
    return { s.buf(), s.len() };
}

template <typename T>
constexpr void reverse(Slice<T> slice) {
    for (usize i = 0; i < slice.len() / 2; i++) {
        ::swap(slice[i], slice[slice.len() - 1 - i]);
    }
}

template <typename T>
constexpr void fill(Slice<T> slice, T value) {
    for (usize i = 0; i < slice.len(); i++) {
        slice[i] = value;
    }
}
