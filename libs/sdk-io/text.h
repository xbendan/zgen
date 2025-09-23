#pragma once

#include <sdk-io/traits.h>
#include <sdk-meta/try.h>
#include <sdk-text/_defs.h>
#include <sdk-text/builder.h>
#include <sdk-text/rune.h>
#include <sdk-text/str.h>

namespace Sdk::Io {

using Sdk::Text::_Str;
using Sdk::Text::_StringBuilder;
using Sdk::Text::Rune;
using Sdk::Text::StaticEncoding;

struct TextWriter : Writer, Flusher {
    using Writer::write;

    template <StaticEncoding E>
    Res<> writeStr(_Str<E> str) {
        for (auto rune : iter(str))
            try$(writeRune(rune));
        return Ok();
    }

    virtual Res<> writeRune(Rune rune) = 0;

    Res<> flush() override { return Ok(); }
};

template <StaticEncoding E = typename Sys::Encoding>
struct TextEncoderBase : TextWriter {
    using Writer::write;

    Res<> writeRune(Rune rune) override {
        typename E::One one;
        if (not E::encodeUnit(rune, one))
            return Error::invalidInput("encoding error");
        try$(write(bytes(one)));
        return Ok();
    }
};

template <StaticEncoding E = typename Sys::Encoding>
struct TextEncoder : TextEncoderBase<E> {
    Io::Writer& _writer;

    TextEncoder(Io::Writer& writer) : _writer(writer) { }

    Res<usize> write(Bytes bytes) override { return _writer.write(bytes); }
};

struct TextReader : Reader, Flusher {
    using Reader::read;

    template <StaticEncoding E>
    Res<_Str<E>> readStr(typename E::Unit delimiter) { }

    virtual Res<Rune> readRune() = 0;
};

template <StaticEncoding E = typename Sys::Encoding>
struct TextDecoderBase : TextReader {
    using Reader::read;
};

template <StaticEncoding E>
struct _StringWriter : TextWriter, _StringBuilder<E> {

    _StringWriter(usize cap = 16) : _StringBuilder<E>(cap) { }

    Res<> write(byte b) override { panic("can't write raw byte to a string"); }

    Res<usize> write(Bytes) override {
        panic("can't write raw bytes to a string");
    }

    Res<> writeRune(Rune rune) override {
        _StringBuilder<E>::append(rune);
        return Ok();
    }

    Res<> writeUnit(Slice<typename E::Unit> unit) {
        _StringBuilder<E>::append(unit);
        return Ok();
    }
};

using StringWriter = _StringWriter<Sys::Encoding>;

} // namespace Sdk::Io
