module;

export module sdk.text:runes;

import sdk;
import :rune;
import :string;

export namespace Realms::Text {

template <StaticEncoding En>
struct _Runes {
    using Encoding = En;
    using Unit     = typename En::Unit;
    using E        = Rune;

    Cursor<Unit> _cur;
    Cursor<Unit> _begin;

    _Runes(_Str<En> str) : _cur(str) { }

    bool ended() const { return _cur.ended(); }

    Rune next() {
        if (_cur.ended())
            return '\0';

        Rune r {};
        return En::decodeUnit(r, _cur) ? r : Text::Unknown;
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
            if (En::decodeUnit(r, curr))
                result++;
        }
        return result;
    }

    Rune peek() const {
        if (_cur.ended())
            return '\0';

        Rune r {};
        auto curr = _cur;
        return En::decodeUnit(r, curr) ? r : Text::Unknown;
    }

    _Str<En> toStr() const { return { _begin, _cur }; }

    _Str<En> toStr(usize n) const {
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

    bool skip(_Str<En> str) {
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
    bool eat(_Str<En> str) {
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

    _Str<En> end() { return { _begin, _cur }; }

    auto defer() {
        return ArmedDefer([&, saved = *this] { *this = saved; });
    }
};

using Runes = _Runes<Utf8>;

} // namespace Realms::Text

// template <Sliceable S>
//     requires(Realms::Text::StaticEncoding<typename S::Encoding>)
// auto iter(S const& slice) {
//     using E = typename S::Encoding;
//     using U = typename E::Unit;

//     Cursor<U> cursor(slice);
//     return Iter([cursor] mutable -> Opt<Sdk::Text::Rune> {
//         if (cursor.ended()) {
//             return None {};
//         }

//         Sdk::Text::Rune r;
//         return E::decodeUnit(r, cursor) ? Opt<Sdk::Text::Rune>(r)
//                                         : Opt<Sdk::Text::Rune>();
//     });
// }
