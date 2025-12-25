#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/manual.h>
#include <sdk-meta/math.h>
#include <sdk-meta/slice.h>

template <typename T>
struct Buf {
    using Inner = T;

    Manual<T>* _buf { nullptr };
    usize      _cap { 0 };
    usize      _len { 0 };

    Buf(usize cap = 0) { ensure(cap); }

    Buf(Move, T* buf, usize len)
        : _buf(reinterpret_cast<Manual<T>*>(buf)),
          _cap(len),
          _len(len) { }

    Buf(Sliceable<T> auto const& other) {
        ensure(other.len());

        _len = other.len();
        for (usize i = 0; i < _len; ++i) {
            _buf[i].ctor(other[i]);
        }
    }

    Buf(Buf const& other) {
        ensure(other._cap);
        _len = other._len;

        for (usize i = 0; i < _len; ++i) {
            _buf[i].ctor(other._buf[i].unwrap());
        }
    }

    Buf(Buf&& other) noexcept
        : _buf(other._buf),
          _cap(other._cap),
          _len(other._len) {
        other._buf = nullptr;
        other._cap = 0;
        other._len = 0;
    }

    ~Buf() {
        if (not _buf) {
            return;
        }

        for (usize i = 0; i < _len; ++i) {
            _buf[i].dtor();
        }
        delete[] _buf;
    }

    Buf& operator=(Buf const& other) {
        *this = Buf(other);
        return *this;
    }

    Buf& operator=(Buf&& other) noexcept {
        if (this != (void*) &other) {
            if (_buf) {
                for (usize i = 0; i < _len; ++i) {
                    _buf[i].dtor();
                }
                delete[] reinterpret_cast<byte*>(_buf);
            }
            _buf = other._buf;
            _cap = other._cap;
            _len = other._len;

            other._buf = nullptr;
            other._cap = 0;
            other._len = 0;
        }
        return *this;
    }

    constexpr T& operator[](usize i) { return _buf[i].unwrap(); }

    constexpr T const& operator[](usize i) const { return _buf[i].unwrap(); }

    void ensure(usize desired) {
        if (desired <= _cap) {
            return;
        }

        if (not _buf) {
            _buf = new Manual<T>[desired];
            _cap = desired;
            return;
        }

        usize newCap = max(_cap * 2, desired);
        auto* newBuf = new Manual<T>[newCap];

        for (usize i = 0; i < _len; ++i) {
            newBuf[i].ctor(_buf[i].take());
        }

        delete[] _buf;
        _buf = newBuf;
        _cap = newCap;
    }

    void fit() {
        if (not _buf or _len == _cap) {
            return;
        }

        Manual<T>* newBuf;

        if (_len) {
            newBuf = new Manual<T>[_len];
            for (usize i = 0; i < _len; ++i) {
                newBuf[i].ctor(_buf[i].take());
            }
        }

        delete[] _buf;
        _buf = newBuf;
        _cap = _len;
    }

    template <typename... Args>
    auto& emplace(usize index, Args&&... args) {
        ensure(_len + 1);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(forward<Args>(args)...);
        _len++;
        return _buf[index].unwrap();
    }

    void insert(usize index, T&& value) {
        ensure(_len + 1);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(move(value));
        _len++;
    }

    void replace(usize index, T&& value) {
        if (index >= _len) {
            insert(index, move(value));
            return;
        }

        _buf[index].dtor();
        _buf[index].ctor(move(value));
    }

    void insert(Copy, usize index, T const* first, usize count) {
        ensure(_len + count);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - count].take());
        }

        for (usize i = 0; i < count; i++) {
            _buf[index + i].ctor(first[i]);
        }

        _len += count;
    }

    void insert(Move, usize index, T* first, usize count) {
        ensure(_len + count);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - count].take());
        }

        for (usize i = 0; i < count; i++) {
            _buf[index + i].ctor(move(first[i]));
        }

        _len += count;
    }

    T removeAt(usize index) {
        if (index >= _len) [[unlikely]]
            panic("Buf<T>::removeAt(usize): index out of bounds");

        T ret = _buf[index].take();
        for (usize i = index; i < _len - 1; i++) {
            _buf[i].ctor(_buf[i + 1].take());
        }
        _len--;
        return ret;
    }

    void removeRange(usize index, usize count) {
        if (index > _len) [[unlikely]]
            panic(
                "Buf<T>::removeRange(usize, usize): "
                "index out of bounds");

        if (index + count > _len) [[unlikely]]
            panic(
                "Buf<T>::removeRange(usize, usize): "
                "index + count out of bounds");

        for (usize i = index; i < _len - count; i++)
            _buf[i].ctor(_buf[i + count].take());

        _len -= count;
    }

    void resize(usize newLen, T fill = {}) {
        if (newLen > _len) {
            ensure(newLen);
            for (usize i = _len; i < newLen; i++) {
                _buf[i].ctor(fill);
            }
        } else if (newLen < _len) {
            for (usize i = newLen; i < _len; i++) {
                _buf[i].dtor();
            }
        }
        _len = newLen;
    }

    void trunc(usize newLen) {
        if (newLen >= _len)
            return;

        for (usize i = newLen; i < _len; i++) {
            _buf[i].dtor();
        }

        _len = newLen;
    }

    T* take() {
        T* ret = buf();
        _buf   = nullptr;
        _cap   = 0;
        _len   = 0;

        return ret;
    }

    T* buf() {
        if (_buf == nullptr)
            return nullptr;
        return &_buf->unwrap();
    }

    T const* buf() const {
        if (_buf == nullptr)
            return nullptr;

        return &_buf->unwrap();
    }

    usize len() const { return _len; }

    usize cap() const { return _cap; }

    usize size() const { return _len * sizeof(T); }

    T* begin() { return buf(); }

    T* end() { return buf() + _len; }

    void leak() {
        _buf = nullptr;
        _cap = 0;
        _len = 0;
    }
};
static_assert(sizeof(Buf<u8>) == 24);

template <typename T, usize N>
struct InlineBuf {
    static_assert(N > 0, "InlineBuf<T, N>: N must be greater than 0");

    using Inner = T;

    Array<Manual<T>, N> _buf = {};
    usize               _len = {};

    constexpr InlineBuf() = default;

    InlineBuf(usize cap) {
        if (cap > N) [[unlikely]]
            panic("InlineBuf<T, N>::<init>(usize): cap too large");
    }

    InlineBuf(T const* buf, usize len) {
        if (len > N) [[unlikely]]
            panic("InlineBuf<T, N>::<init>(T const*, usize): len too large");

        _len = len;
        for (usize i = 0; i < _len; i++)
            _buf[i].ctor(buf[i]);
    }

    InlineBuf(InitializerList<T> other)
        : InlineBuf(other.begin(), other.size()) { }

    InlineBuf(Sliceable<T> auto const& other)
        : InlineBuf(other.buf(), other.len()) { }

    InlineBuf(InlineBuf const& other) : InlineBuf(other.buf(), other.len()) { }

    InlineBuf(InlineBuf&& other) {
        for (usize i = 0; i < N; i++) {
            _buf[i].ctor(move(other._buf[i].take()));
        }
        _len = other._len;
    }

    ~InlineBuf() {
        for (usize i = 0; i < _len; i++) {
            _buf[i].dtor();
        }
        _len = 0;
    }

    InlineBuf& operator=(InlineBuf const& other) {
        *this = InlineBuf(other);
        return *this;
    }

    InlineBuf& operator=(InlineBuf&& other) {
        for (usize i = 0; i < min(_len, other._len); i++)
            buf()[i] = move(other.buf()[i]);

        for (usize i = _len; i < other._len; i++)
            _buf[i].ctor(move(other._buf[i].take()));

        for (usize i = other._len; i < _len; i++)
            _buf[i].dtor();

        _len = other._len;

        return *this;
    }

    constexpr T& operator[](usize i) { return _buf[i].unwrap(); }

    constexpr T const& operator[](usize i) const { return _buf[i].unwrap(); }

    void ensure(usize len) {
        if (len > N) [[unlikely]]
            panic("InlineBuf<T, N>::ensure(usize): cap too large");
    }

    void fit() {
        // no-op
    }

    template <typename... Args>
    void emplace(usize index, Args&&... args) {
        if (_len == N) [[unlikely]]
            panic("InlineBuf<T, N>::emplace(usize, ...): cap too large");

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(forward<Args>(args)...);
        _len++;
    }

    void insert(usize index, T&& value) {
        if (_len == N) [[unlikely]]
            panic("InlineBuf<T, N>::insert(usize, T&&): cap too large");

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(move(value));
        _len++;
    }

    void insert(Copy, usize index, T* first, usize count) {
        if (_len + count > N) [[unlikely]]
            panic(
                "InlineBuf<T, N>::insert(_Copy, usize, T*, usize): "
                "cap too large");

        for (usize i = _len; i > index; i--) {
            _buf[i] = _buf[i - count];
        }

        for (usize i = 0; i < count; i++) {
            _buf[index + i] = first[i];
        }

        _len += count;
    }

    void insert(Move, usize index, T* first, usize count) {
        if (_len + count > N) [[unlikely]]
            panic(
                "InlineBuf<T, N>::insert(_Move, usize, T*, usize): "
                "cap too large");

        for (usize i = _len; i > index; i--) {
            _buf[i] = move<T>(_buf[i - count]);
        }

        for (usize i = 0; i < count; i++) {
            _buf[index + i] = move(first[i]);
        }

        _len += count;
    }

    T removeAt(usize index) {
        T tmp = _buf[index].take();
        for (usize i = index; i < _len - 1; i++) {
            _buf[i].ctor(_buf[i + 1].take());
        }
        _len--;
        return tmp;
    }

    void resize(usize newLen, T fill = {}) {
        if (newLen > _len) {
            ensure(newLen);
            for (usize i = _len; i < newLen; i++) {
                _buf[i].ctor(fill);
            }
        } else if (newLen < _len) {
            for (usize i = newLen; i < _len; i++) {
                _buf[i].dtor();
            }
        }
        _len = newLen;
    }

    void trunc(usize newLen) {
        if (newLen >= _len)
            return;

        for (usize i = newLen; i < _len; i++) {
            _buf[i].dtor();
        }

        _len = newLen;
    }

    T* buf() { return &_buf[0].unwrap(); }

    T const* buf() const { return &_buf[0].unwrap(); }

    usize len() const { return _len; }

    usize cap() const { return N; }

    T* begin() { return buf(); }

    T* end() { return buf() + _len; }
};
