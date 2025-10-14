#pragma once

#include <sdk-io/traits.h>
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

    Res<> writeStr(char const* cstr) {
        return writeStr(_Str<Zgen::Core::Encoding>(cstr));
    }

    virtual Res<> writeRune(Rune rune) = 0;

    Res<> flush() override { return Ok(); }
};

template <StaticEncoding E = typename Zgen::Core::Encoding>
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

template <StaticEncoding E = typename Zgen::Core::Encoding>
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

template <StaticEncoding E = typename Zgen::Core::Encoding>
struct TextDecoderBase : TextReader {
    using Reader::read;
};

struct Null : TextWriter, TextReader {
    Res<> write(byte) override { return Ok(); }

    Res<usize> write(Bytes bytes) override { return Ok(bytes.len()); }

    Res<> writeRune(Rune) override { return Ok(); }

    Res<byte> read() override { return Ok(0); }

    Res<usize> read(Bytes) override { return Ok(0); }

    Res<Rune> readRune() override { return Ok(0); }

    Res<> flush() override { return Ok(); }
};

template <StaticEncoding E, usize N>
struct _StringWriter : TextWriter, _StringBuilder<E, N> {

    _StringWriter() : _StringBuilder<E, N>() { }

    Res<> write(byte b) override { panic("can't write raw byte to a string"); }

    Res<usize> write(Bytes) override {
        panic("can't write raw bytes to a string");
    }

    Res<> writeRune(Rune rune) override {
        _StringBuilder<E, N>::append(rune);
        return Ok();
    }

    Res<> writeUnit(Slice<typename E::Unit> unit) {
        _StringBuilder<E, N>::append(unit);
        return Ok();
    }
};

template <usize N = 64>
using StringWriter = _StringWriter<Zgen::Core::Encoding, N>;

} // namespace Sdk::Io
