module;

#include <sdk-meta/_macros.h>

export module sdk.text.format:type_numbers;

import sdk;
import sdk.text;
import sdk.io;
import :base;

namespace Realms::Text {

using Meta::InlineVec;

export struct NumberFormatter {
    bool  prefix        = false;
    bool  isChar        = false;
    usize base          = 10;
    usize width         = 0;
    char  fill          = ' ';
    bool  trailingZeros = false;
    usize precision     = 6;

    Str formatPrefix() {
        if (base == 16)
            return "0x";

        if (base == 8)
            return "0o";

        if (base == 2)
            return "0b";

        return "";
    }

    void parse(Str str) {
        Runes rs(str);
        parse(rs);
    }

    void parse(Runes& rs) {
        if (rs.skip('#'))
            prefix = true;

        if (rs.skip('0'))
            fill = '0';

        width = atoi(rs).unwrapOrElse(0);

        if (rs.skip('.')) {
            if (rs.skip('0'))
                trailingZeros = true;
            precision = atoi(rs).unwrapOrDefault(6);
        }

        if (rs.ended())
            return;
        Rune c = rs.next();
        switch (c) {
            case 'b': base = 2; break;
            case 'o': base = 8; break;
            case 'd': base = 10; break;
            case 'x': base = 16; break;
            case 'p': {
                prefix = true;
                base   = 16;
                fill   = '0';
                width  = sizeof(usize) * 2;
                break;
            }

                // case 'c': isChar = true; break;

            default: break;
        }
    }

    Res<> formatUnsigned(Io::TextWriter& writer, usize val) {
        auto digit = [](usize v) {
            if (v < 10) {
                return '0' + v;
            }
            return 'a' + (v - 10);
        };

        InlineVec<char, 128> buf;

        do {
            buf.pushBack(digit(val % base));
            val /= base;
        } while (val != 0 && buf.len() < buf.cap());

        while (width > buf.len()) {
            buf.pushBack(fill);
        }
        reverse(slice(buf));

        if (prefix)
            try$(writer.writeStr(formatPrefix()));
        try$(writer.writeStr(Str { buf.buf(), buf.len() }));

        return Ok();
    }

    Res<> formatSigned(Io::TextWriter& writer, isize val) {
        if (val < 0) {
            try$(writer.writeRune('-'));
            val = -val;
        }
        try$(formatUnsigned(writer, val));
        return Ok();
    }

    Res<> formatRune(Io::TextWriter& writer, Rune val) {
        if (not prefix)
            return writer.writeRune(val);

        if (val == '\'')
            return writer.writeStr("\\'"s);

        if (val == '\"')
            return writer.writeStr("\\\""s);

        if (val == '\?')
            return writer.writeStr("\\?"s);

        if (val == '\\')
            return writer.writeStr("\\\\"s);

        if (val == '\a')
            return writer.writeStr("\\a"s);

        if (val == '\b')
            return writer.writeStr("\\b"s);

        if (val == '\f')
            return writer.writeStr("\\f"s);

        if (val == '\n')
            return writer.writeStr("\\n"s);

        if (val == '\r')
            return writer.writeStr("\\r"s);

        if (val == '\t')
            return writer.writeStr("\\t"s);

        if (val == '\v')
            return writer.writeStr("\\v"s);

        if (not isAsciiPrint(val))
            return format(writer, "\\u{x}", val);

        return writer.writeRune(val);
    }
};

export template <Unsigned T>
struct Formatter<T> : NumberFormatter {
    Res<> format(Io::TextWriter& writer, T const& val) {
        if (isChar)
            return formatRune(writer, val);
        return formatUnsigned(writer, val);
    }
};

export template <Signed T>
struct Formatter<T> : NumberFormatter {
    Res<> format(Io::TextWriter& writer, T const& val) {
        if (isChar)
            return writer.writeRune(val);
        return formatSigned(writer, val);
    }
};

// #if !defined(__sdk_freestanding__)
// template <Float T>
// struct Formatter<T> : NumberFormatter {
//     Res<> format(Io::TextWriter& writer, f64 const& val) {
//         return formatFloat(writer, val);
//     }
// };
// #endif

export template <typename T>
struct Formatter<Be<T>> : Formatter<T> {
    Res<> format(Io::TextWriter& writer, Be<T> const& val) {
        return Formatter<T>::format(writer, val.value());
    }
};

export template <typename T>
struct Formatter<Le<T>> : Formatter<T> {
    Res<> format(Io::TextWriter& writer, Le<T> const& val) {
        return Formatter<T>::format(writer, val.value());
    }
};

export template <>
struct Formatter<bool> {
    Res<> format(Io::TextWriter& writer, bool val) {
        return writer.writeStr(val ? "True"s : "False"s);
    }
};

} // namespace Realms::Text
