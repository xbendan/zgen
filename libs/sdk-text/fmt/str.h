#pragma once

#include <sdk-io/text.h>
#include <sdk-text/fmt/base.h>
#include <sdk-text/fmt/nums.h>
#include <sdk-text/traits.h>

namespace Sdk::Text {

template <StaticEncoding E>
struct StringFormatter {
    bool prefix = false;

    void parse(Runes& rs) {
        if (rs.skip('#'))
            prefix = true;
    }

    Res<> format(Io::TextWriter& writer, _Str<E> text) {
        if (not prefix)
            return writer.writeStr(text);

        try$(writer.writeRune('"'));
        for (Rune c : iter(text)) {
            if (c == '"')
                try$(writer.writeStr("\\\""s));
            else if (c == '\\')
                try$(writer.writeStr("\\\\"s));
            else if (c == '\a')
                try$(writer.writeStr("\\a"s));
            else if (c == '\b')
                try$(writer.writeStr("\\b"s));
            else if (c == '\f')
                try$(writer.writeStr("\\f"s));
            else if (c == '\n')
                try$(writer.writeStr("\\n"s));
            else if (c == '\r')
                try$(writer.writeStr("\\r"s));
            else if (c == '\t')
                try$(writer.writeStr("\\t"s));
            else if (c == '\v')
                try$(writer.writeStr("\\v"s));
            else if (not isAsciiPrint(c))
                try$(Text::format(writer, "\\u{x}", c));
            else
                try$(writer.writeRune(c));
        }
        try$(writer.writeRune('"'));
        return Ok();
    }
};

template <StaticEncoding E>
struct Formatter<_Str<E>> : StringFormatter<E> { };

template <StaticEncoding E>
struct Formatter<_String<E>> : StringFormatter<E> {
    Res<> format(Io::TextWriter& writer, _String<E> const& text) {
        return StringFormatter<E>::format(writer, text.str());
    }
};

template <>
struct Formatter<char const*> : StringFormatter<Utf8> {
    Res<> format(Io::TextWriter& writer, char const* text) {
        _Str<Utf8> str = Str::fromNullterminated(text);
        return StringFormatter::format(writer, str);
    }
};

} // namespace Sdk::Text
