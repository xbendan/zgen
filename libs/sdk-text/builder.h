#pragma once

#include <sdk-meta/buf.h>
#include <sdk-meta/iter.h>
#include <sdk-meta/types.h>
#include <sdk-text/str.h>

namespace Sdk::Text {

template <StaticEncoding E>
struct _StringBuilder {
    Buf<typename E::Unit> _buf {};

    _StringBuilder(usize cap = 16) : _buf(cap) { }

    _StringBuilder(String&& str)
        : _buf(Move, Meta::exchange(str._buf, nullptr),
               Meta::exchange(str._len, 0)) { }

    void ensure(usize cap) {
        // NOTE: This way client code don't have to take
        //       the null-terminator into account
        _buf.ensure(cap + 1);
    }

    void append(Rune rune) {
        typename E::One one;
        if (not E::encodeUnit(rune, one))
            return;

        for (auto unit : iter(one))
            _buf.insert(_buf.len(), move(unit));
    }

    void append(Sliceable<Rune> auto const& runes) {
        for (auto rune : runes)
            append(rune);
    }

    void append(Sliceable<typename E::Unit> auto const& units) {
        _buf.insert(Copy, _buf.len(), units.buf(), units.len());
    }

    void reduce(usize n) {
        if (n > _buf.len()) [[unlikely]]
            panic("StringBuilder: cannot reduce more than its length");
        _buf.trunc(_buf.len() - n);
    }

    usize len() const { return _buf.size(); }

    _Str<E> str() const { return _buf; }

    Bytes bytes() const { return { (byte*) _buf.buf(), _buf.len() }; }

    void clear() { _buf.trunc(0); }

    _String<E> take() {
        usize len = _buf.len();
        _buf.insert(len, 0);
        return { Move, _buf.take(), len };
    }
};

using StringBuilder = _StringBuilder<typename Sys::Encoding>;

} // namespace Sdk::Text
