module;

#include <sdk-meta/_macros.h>

export module sdk.text.format:type_string;

import sdk;
import sdk.text;
import sdk.io;
import :base;
import :type_numbers;

namespace Realms::Text {

export template <StaticEncoding E>
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
        for (Rune c : text) {
            if (not isAsciiPrint(c))
                try$(Text::format(writer, "\\u{x}", c));
            else {
                Str s = match(c,
                              ""s,
                              _case('"', "\\\""s),
                              _case('\\', "\\\\"s),
                              _case('\a', "\\a"s),
                              _case('\b', "\\b"s),
                              _case('\f', "\\f"s),
                              _case('\n', "\\n"s),
                              _case('\r', "\\r"s),
                              _case('\t', "\\t"s),
                              _case('\v', "\\v"s));
                if (s != ""s) {
                    try$(writer.writeStr(s));
                    continue;
                }
                try$(writer.writeRune(c));
            }
            // if (c == '"')
            //     try$(writer.writeStr("\\\""s));
            // else if (c == '\\')
            //     try$(writer.writeStr("\\\\"s));
            // else if (c == '\a')
            //     try$(writer.writeStr("\\a"s));
            // else if (c == '\b')
            //     try$(writer.writeStr("\\b"s));
            // else if (c == '\f')
            //     try$(writer.writeStr("\\f"s));
            // else if (c == '\n')
            //     try$(writer.writeStr("\\n"s));
            // else if (c == '\r')
            //     try$(writer.writeStr("\\r"s));
            // else if (c == '\t')
            //     try$(writer.writeStr("\\t"s));
            // else if (c == '\v')
            //     try$(writer.writeStr("\\v"s));
            // else if (not isAsciiPrint(c))
            //     try$(Text::format(writer, "\\u{x}", c));
            // else
            //     try$(writer.writeRune(c));
        }
        try$(writer.writeRune('"'));
        return Ok();
    }
};

export template <StaticEncoding E>
struct Formatter<_Str<E>> : StringFormatter<E> { };

export template <StaticEncoding E>
struct Formatter<_String<E>> : StringFormatter<E> {
    Res<> format(Io::TextWriter& writer, _String<E> const& text) {
        return StringFormatter<E>::format(writer, text.str());
    }
};

export template <>
struct Formatter<char const*> : StringFormatter<Utf8> {
    Res<> format(Io::TextWriter& writer, char const* text) {
        Str str = Str::nullterminated(text);
        return StringFormatter<Utf8>::format(writer, str);
    }
};

} // namespace Realms::Text
