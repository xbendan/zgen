module;

export module sdk.text:parse;

import sdk;
import :rune;
import :runes;
import :traits;

export namespace Realms::Text {

struct AtoxOptions {
    u8   base     = 10;
    bool allowExp = true;
};

inline Opt<u8> _parseDigit(Rune rune, AtoxOptions options = {}) {
    rune      = toAsciiLower(rune);
    u8 result = 255;

    if (isAsciiAlpha(rune))
        result = rune - 'a' + 10;
    else if (isAsciiDigit(rune))
        result = rune - '0';

    if (result >= options.base)
        return NONE;

    return result;
}

template <StaticEncoding E>
Opt<u8> _nextDigit(_Runes<E>& s, AtoxOptions options = {}) {
    if (s.ended())
        return NONE;

    auto d = _parseDigit(s.peek(), options);
    if (d)
        s.next();
    return d;
}

template <StaticEncoding E>
Opt<usize> atou(_Runes<E>& s, AtoxOptions options = {}) {
    bool  isNum  = false;
    usize result = 0;

    while (not s.ended()) {
        auto maybeDigit = _nextDigit(s, options);
        if (not maybeDigit)
            break;
        isNum  = true;
        result = result * options.base + maybeDigit.unwrap();
    }

    if (not isNum)
        return NONE;

    return result;
}

template <StaticEncoding E>
Opt<isize> atoi(_Runes<E>& s, AtoxOptions options = {}) {
    bool  isNeg  = false;
    bool  isNum  = false;
    isize result = 0;

    auto rollback = s.defer();

    if (not s.skip('+'))
        isNeg = s.skip('-');

    while (not s.ended()) {
        auto maybeDigit = _nextDigit(s, options);
        if (not maybeDigit)
            break;
        isNum  = true;
        result = result * options.base + maybeDigit.unwrap();
    }

    if (not isNum)
        return NONE;

    if (isNeg)
        result = -result;

    rollback.disarm();
    return result;
}

} // namespace Realms::Text
