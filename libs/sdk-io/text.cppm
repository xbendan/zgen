module;

#include <sdk-meta/_macros.h>

export module sdk.io:text;

import sdk.text;
import :traits;

export namespace Realms::Io {

using Text::_Str;
using Text::Rune;
using Text::StaticEncoding;

struct TextWriter : Writer, Flusher {
    using Writer::write;

    template <StaticEncoding E>
    Res<> writeStr(_Str<E> str) {
        for (auto rune : str)
            try$(writeRune(rune));

        return Ok();
    }

    Res<> writeStr(char const* cstr) {
        return writeStr(_Str<Text::Encoding>(cstr));
    }

    virtual Res<> writeRune(Rune rune) = 0;

    Res<> flush() override { return Ok(); }
};

template <StaticEncoding E = Text::Encoding>
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

template <StaticEncoding E = Text::Encoding>
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

template <StaticEncoding E = Text::Encoding>
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

} // namespace Realms::Io
