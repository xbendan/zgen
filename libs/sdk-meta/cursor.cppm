module;

export module sdk:cursor;

import :slice;

export template <typename T>
struct Cursor {
    using E = T;

    T* _begin = nullptr;
    T* _end   = nullptr;

    constexpr Cursor() = default;

    [[gnu::always_inline]] constexpr Cursor(None) : Cursor() { }

    [[gnu::always_inline]] constexpr Cursor(T const* ptr)
        : Cursor(ptr, ptr ? 1 : 0) { }

    [[gnu::always_inline]] constexpr Cursor(T const* ptr, usize len)
        : _begin(const_cast<T*>(ptr)),
          _end(const_cast<T*>(ptr) + len) {
        if (_begin == nullptr and _begin != _end) [[unlikely]]
            panic("null pointer with non-zero length");
    }

    [[gnu::always_inline]] constexpr Cursor(T const* begin, T const* end)
        : Cursor(begin, end - begin) { }

    [[gnu::always_inline]] constexpr Cursor(Sliceable<T> auto& slice)
        : Cursor { begin(slice), end(slice) } { }

    [[gnu::always_inline]] constexpr Cursor(Slice<T> slice)
        : Cursor { begin(slice), end(slice) } { }

    [[gnu::always_inline]] constexpr T const& operator[](usize i) const {
        if (i >= len()) [[unlikely]]
            panic("index out of bounds");
        return _begin[i];
    }

    [[gnu::always_inline]] constexpr T& operator[](usize i) {
        if (i >= len()) [[unlikely]]
            panic("index out of bounds");
        return const_cast<T&>(_begin[i]);
    }

    [[gnu::always_inline]] constexpr operator T*() { return _begin; }

    [[gnu::always_inline]] constexpr operator T const*() const {
        return _begin;
    }

    [[gnu::always_inline]] constexpr bool ended() const {
        return _begin >= _end;
    }

    [[gnu::always_inline]] constexpr usize rem() const { return _end - _begin; }

    [[gnu::always_inline]] constexpr T const& peek(usize i = 0) const {
        if (i >= len()) [[unlikely]]
            panic("index out of bounds");
        return _begin[i];
    }

    [[gnu::always_inline]] constexpr T const& operator*() const {
        return peek();
    }

    [[gnu::always_inline]] constexpr T const* operator->() const {
        return &peek();
    }

    [[gnu::always_inline]] constexpr T const& next() {
        if (ended()) [[unlikely]]
            panic("next() called on ended cursor");

        T const& r = *_begin;
        _begin++;
        return r;
    }

    [[gnu::always_inline]] constexpr Slice<T> next(usize n) {
        if (n > rem()) [[unlikely]]
            panic("next() called on ended cursor");

        auto slice = Slice<T> { _begin, n };
        _begin += n;
        return slice;
    }

    template <Equatable<T> U>
    [[gnu::always_inline]] constexpr bool skip(U const& c) {
        if (ended()) [[unlikely]]
            return false;

        if (peek() == c) {
            _begin++;
            return true;
        }

        return false;
    }

    [[gnu::always_inline]] constexpr bool put(T c) {
        if (_begin == _end) {
            return true;
        }

        *_begin++ = c;
        return false;
    }

    [[gnu::always_inline]] constexpr T* buf() { return _begin; }

    [[gnu::always_inline]] constexpr T const* buf() const { return _begin; }

    [[gnu::always_inline]] constexpr usize len() const { return _end - _begin; }

    [[gnu::always_inline]] constexpr Bytes bytes() const {
        return Bytes { _begin, _end };
    }

    /// Creates a rollback point for the cursor. If not manually disarmed,
    /// the cursor's state will be restored to its position at the time of
    /// this rollback point's creation when it goes out of scope.
    // auto rollbackPoint() {
    //     return ArmedDefer { [&, saved = *this] { *this = saved; } };
    // }
};
