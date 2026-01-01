#pragma once

#include <sdk-meta/decl.h>
#include <sdk-meta/math.h>
#include <sdk-meta/slice.h>
#include <sdk-meta/types.h>

template <typename T, typename = struct _RangeTag>
struct Range {
    using Size = decltype(::declval<T>() - ::declval<T>());

    T    _start {};
    Size _size;

    static constexpr Range from(T a, T b) {
        return (a < b) ? Range { a, b - a } : Range { b, a - b };
    }

    [[gnu::always_inline]] constexpr Range() = default;

    [[gnu::always_inline]] constexpr Range(T start)
        : _start(start),
          _size(0) { }

    [[gnu::always_inline]] constexpr Range(T start, Size size)
        : _start(start),
          _size(size) { }

    [[gnu::always_inline]] constexpr T start() const { return _start; }

    [[gnu::always_inline]] constexpr T start(T val) {
        _start = val;
        return _start;
    }

    [[gnu::always_inline]] constexpr Size size() const { return _size; }

    [[gnu::always_inline]] constexpr T end() const { return _start + _size; }

    [[gnu::always_inline]] constexpr T end(T val) {
        if (val >= _start) {
            _size = val - _start;
        }
        return end();
    }

    [[gnu::always_inline]] constexpr bool isEmpty() const { return _size == 0; }

    [[gnu::always_inline]] constexpr bool contains(T val) const {
        return val >= _start && val < end();
    }

    [[gnu::always_inline]] constexpr bool contains(Range other) const {
        return other._start >= _start && other.end() <= end();
    }

    [[gnu::always_inline]] constexpr bool contigousWith(Range other) const {
        return other._start == end() || _start == other.end();
    }

    [[gnu::always_inline]] constexpr bool overlaps(Range other) const {
        return other._start < end() && _start < other.end();
    }

    [[gnu::always_inline]] constexpr Range merge(Range other) const {
        return from( //
            min(_start, other._start),
            max(end(), other.end()));
    }

    [[gnu::always_inline]] constexpr Range take(Size size) {
        if (size > _size)
            size = _size;
        Range r { _start, size };
        _start += size;
        _size -= size;
        return r;
    }

    [[gnu::always_inline]] constexpr bool aligned(T align) const {
        return isAlign(_start, align) && isAlign(_size, align);
    }

    [[gnu::always_inline]] constexpr Range outer(T align) {
        T    newStart = alignDown(_start, align);
        Size newSize  = alignUp(end() - newStart, align);
        return { newStart, newSize };
    }

    [[gnu::always_inline]] constexpr Range inner(T align) {
        T    newStart = alignUp(_start, align);
        Size newSize  = alignDown(end() - newStart, align);
        return { newStart, newSize };
    }

    [[gnu::always_inline]] constexpr Range slice(Size offset,
                                                 Size length) const {
        return { _start + offset, length };
    }

    [[gnu::always_inline]] constexpr Range slice(Size offset) const {
        return { _start + offset, _size - offset };
    }

    template <typename U>
    [[gnu::always_inline]] constexpr Range<U> cast() const {
        return { static_cast<U>(_start),
                 static_cast<typename Range<U>::Size>(_size) };
    }

    template <typename U>
    [[gnu::always_inline]] constexpr U into() const {
        return U { start(), size() };
    }

    bool ensureAligned(T align) const {
        if (not isAlign(_start, align) or not isAlign(_size, align))
            return false;

        return true;
    }

    [[gnu::always_inline]] constexpr Range downscale(usize factor) const {
        return { _start / factor, _size / factor };
    }

    [[gnu::always_inline]] constexpr Range upscale(usize factor) const {
        return { _start * factor, _size * factor };
    }

    [[gnu::always_inline]] constexpr Range offset(T offset) const {
        return { _start + offset, _size };
    }

    [[gnu::always_inline]] constexpr Bytes bytes() const {
        return { reinterpret_cast<byte const*>(&_start), sizeof(T) * size() };
    }

    constexpr bool operator==(Range const&) const = default;

    constexpr explicit operator bool() const noexcept { return not isEmpty(); }

    constexpr explicit operator T() const noexcept { return _start; }
};

