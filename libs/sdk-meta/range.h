#pragma once

#include <sdk-meta/decl.h>
#include <sdk-meta/math.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/types.h>
#include <sdk-meta/vec.h>

template <typename T, typename = struct _RangeTag>
struct Range {
    using Size = decltype(Meta::declval<T>() - Meta::declval<T>());

    T    _start {};
    Size _size;

    static constexpr Range from(T a, T b) {
        return (a < b) ? Range { a, b - a } : Range { b, a - b };
    }

    always_inline constexpr Range() = default;

    always_inline constexpr explicit Range(T start)
        : _start(start),
          _size(0) { }

    always_inline constexpr Range(T start, Size size)
        : _start(start),
          _size(size) { }

    always_inline constexpr T end() const { return _start + _size; }

    always_inline constexpr T endsAt(T val) {
        if (val >= _start) {
            _size = val - _start;
        }
        return end();
    }

    always_inline constexpr bool isEmpty() const { return _size == 0; }

    always_inline constexpr bool contains(T val) const {
        return val >= _start && val < end();
    }

    always_inline constexpr bool contains(Range other) const {
        return other._start >= _start && other.end() <= end();
    }

    always_inline constexpr bool contigousWith(Range other) const {
        return other._start == end() || _start == other.end();
    }

    always_inline constexpr bool overlaps(Range other) const {
        return other._start < end() && _start < other.end();
    }

    always_inline constexpr Range merge(Range other) const {
        return from( //
            min(_start, other._start),
            max(end(), other.end()));
    }

    always_inline constexpr bool aligned(T align) const {
        return isAlign(_start, align) && isAlign(_size, align);
    }

    always_inline constexpr Range slice(Size offset, Size length) const {
        return { _start + offset, length };
    }

    always_inline constexpr Range slice(Size offset) const {
        return { _start + offset, _size - offset };
    }

    template <typename U>
    always_inline constexpr Range<U> cast() const {
        return { static_cast<U>(_start),
                 static_cast<typename Range<U>::Size>(_size) };
    }

    template <typename U>
    always_inline constexpr U into() const {
        return U { _start, _size };
    }

    Opt<bool> ensureAligned(T align) const {
        if (not isAlign(_start, align) or not isAlign(_size, align))
            return Empty {};

        return true;
    }

    constexpr bool operator==(Range const&) const = default;

    constexpr explicit operator bool() const noexcept { return not isEmpty(); }
};

