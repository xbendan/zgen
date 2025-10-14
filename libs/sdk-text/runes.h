#pragma once

#include <sdk-meta/callable.h>
#include <sdk-meta/cursor.h>
#include <sdk-meta/defer.h>
#include <sdk-meta/types.h>
#include <sdk-text/rune.h>
#include <sdk-text/str.h>

namespace Sdk::Text {

template <StaticEncoding E>
struct _Runes {
    using Encoding = E;
    using Unit     = typename E::Unit;
    using Inner    = Rune;

    Cursor<Unit> _cur;
    Cursor<Unit> _begin;

    _Runes(_Str<E> str) : _cur(str) { }

    bool ended() const { return _cur.ended(); }

    Rune next() {
        if (_cur.ended())
            return '\0';

        Rune r {};
        return E::decodeUnit(r, _cur) ? r : Text::Unknown;
    }

    Rune next(usize n) {
        Rune r = '\0';
        for (usize i = 0; i < n; ++i) {
            r = next();
            if (r == Text::Unknown)
                break;
        }
        return r;
    }

    usize rem() const {
        auto curr = _cur;

        usize result = 0;
        while (curr.rem()) {
            Rune r;
            if (E::decodeUnit(r, curr))
                result++;
        }
        return result;
    }

    Rune peek() const {
        if (_cur.ended())
            return '\0';

        Rune r {};
        auto curr = _cur;
        return E::decodeUnit(r, curr) ? r : Text::Unknown;
    }

    _Str<E> toStr() const { return { _begin, _cur }; }

    _Str<E> toStr(usize n) const {
        auto begin = _cur;
        next(n);
        return { begin, _cur };
    }

    bool skip(Rune c) {
        if (peek() == c) {
            next();
            return true;
        }
        return false;
    }

    bool skip(_Str<E> str) {
        auto def = defer();
        for (auto r : str.it())
            if (next() != r)
                return false;
        def.disarm();
        return true;
    }

    /// Keep advancing the cursor while the current rune is `r`.
    bool eat(Rune r) {
        bool result = false;
        if (skip(r)) {
            result = true;
            while (skip(r) and not ended())
                ;
        }
        return result;
    }

    /// Keep advancing the cursor while the current rune is in `str`.
    bool eat(_Str<E> str) {
        bool result = false;
        if (skip(str)) {
            result = true;
            while (skip(str) and not ended())
                ;
        }
        return result;
    }

    auto& begin() {
        _begin = _cur;
        return *this;
    }

    _Str<E> end() { return { _begin, _cur }; }

    auto defer() {
        return ArmedDefer([&, saved = *this] { *this = saved; });
    }
};

using Runes = _Runes<Utf8>;

} // namespace Sdk::Text

template <Sliceable S>
    requires(Sdk::Text::StaticEncoding<typename S::Encoding>)
auto iter(S const& slice) {
    using E = typename S::Encoding;
    using U = typename E::Unit;

    Cursor<U> cursor(slice);
    return Iter([cursor] mutable -> Opt<Sdk::Text::Rune> {
        if (cursor.ended()) {
            return None {};
        }

        Sdk::Text::Rune r;
        return E::decodeUnit(r, cursor) ? Opt<Sdk::Text::Rune>(r)
                                        : Opt<Sdk::Text::Rune>();
    });
}
