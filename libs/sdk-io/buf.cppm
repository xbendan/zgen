module;

export module sdk.io:buf;

import sdk;
import sdk.text;
import :text;

export namespace Realms::Io {

using Meta::Buf;
using Meta::InlineBuf;
using Text::Rune;
using Text::StaticEncoding;

template <StaticEncoding E, usize Len>
struct _StringBuf;

template <StaticEncoding E, usize Len>
struct _StringBuf : Io::TextWriter {
    using Unit = typename E::Unit;

    InlineBuf<Unit, Len> _buf {};

    Res<> write([[maybe_unused]] byte b) override {
        return Error::notSupported("StringBuf::write(byte): not supported");
    }

    Res<usize> write([[maybe_unused]] Bytes bytes) override {
        return Error::notSupported("StringBuf::write(Bytes): not supported");
    }

    Res<> writeRune(Rune rune) override {
        append(rune);
        return Ok();
    }

    Res<> writeUnit(Slice<typename E::Unit> units) {
        append(units);
        return Ok();
    }

    String toString() { return String { _buf.buf(), _buf.len() }; }

    Str toStr() { return Str { _buf.buf(), _buf.len() }; }

    [[gnu::always_inline]] void ensure(usize cap) { _buf.ensure(cap + 1); }

    [[gnu::always_inline]] void append(Rune rune) {
        typename E::One one;
        if (not E::encodeUnit(rune, one))
            return;

        for (auto unit : one)
            _buf.insert(_buf.len(), move(unit));
    }

    [[gnu::always_inline]] void append(Sliceable<Rune> auto const& runes) {
        for (auto rune : runes)
            append(rune);
    }

    [[gnu::always_inline]] void append(Sliceable<Unit> auto const& units) {
        _buf.insert(COPY, _buf.len(), units.buf(), units.len());
    }

    [[gnu::always_inline]] void reduce(usize n) {
        if (n > _buf.len()) [[unlikely]]
            panic("StringBuf: cannot reduce more than its length");
        _buf.trunc(_buf.len() - n);
    }

    [[gnu::always_inline]] usize len() const { return _buf.len(); }

    [[gnu::always_inline]] Unit* buf() { return _buf.buf(); }

    [[gnu::always_inline]] Unit const* buf() const { return _buf.buf(); }
};

template <StaticEncoding E>
struct _StringBuf<E, 0> : Io::TextWriter {
    using Unit = typename E::Unit;

    Buf<Unit> _buf {};

    Res<> write([[maybe_unused]] byte b) override {
        return Error::notSupported("StringBuf::write(byte): not supported");
    }

    Res<usize> write([[maybe_unused]] Bytes bytes) override {
        return Error::notSupported("StringBuf::write(Bytes): not supported");
    }

    Res<> writeRune(Rune rune) override {
        append(rune);
        return Ok();
    }

    Res<> writeUnit(Slice<typename E::Unit> units) {
        append(units);
        return Ok();
    }

    String toString() { return String { _buf.buf(), _buf.len() }; }

    Str toStr() { return Str { _buf.buf(), _buf.len() }; }

    [[gnu::always_inline]] void ensure(usize cap) { _buf.ensure(cap + 1); }

    [[gnu::always_inline]] void append(Rune rune) {
        typename E::One one;
        if (not E::encodeUnit(rune, one))
            return;

        for (auto unit : one)
            _buf.emplace(_buf.len(), move(unit));
    }

    [[gnu::always_inline]] void append(Sliceable<Rune> auto const& runes) {
        for (auto rune : runes)
            append(rune);
    }

    [[gnu::always_inline]] void append(Sliceable<Unit> auto const& units) {
        for (usize i = 0; i < units.len(); ++i) {
            _buf.emplace(_buf.len(), units[i]);
        }
    }

    [[gnu::always_inline]] void reduce(usize n) {
        if (n > _buf.len()) [[unlikely]]
            panic("StringBuf: cannot reduce more than its length");
        _buf._len -= n;
    }

    [[gnu::always_inline]] usize len() const { return _buf.len(); }

    [[gnu::always_inline]] Unit* buf() { return _buf.buf(); }

    [[gnu::always_inline]] Unit const* buf() const { return _buf.buf(); }
};

template <usize Len = 0>
using StringBuf = _StringBuf<Text::Encoding, Len>;

} // namespace Realms::Io
