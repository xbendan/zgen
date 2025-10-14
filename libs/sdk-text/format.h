#pragma once

#include <sdk-io/text.h>
#include <sdk-meta/box.h>
#include <sdk-meta/endian.h>
#include <sdk-meta/ptr.h>
#include <sdk-meta/range.h>
#include <sdk-meta/rc.h>
#include <sdk-meta/res.h>
#include <sdk-meta/slice.h>
#include <sdk-meta/tuple.h>
#include <sdk-meta/types.h>
#include <sdk-text/parse.h>
#include <sdk-text/runes.h>
#include <sdk-text/traits.h>

namespace Sdk::Text {

template <typename T>
struct Formatter;

struct _Args {
    virtual ~_Args()    = default;
    virtual usize len() = 0;
    virtual Res<> format(Runes& rs, Io::TextWriter& writer, usize index) = 0;
};

template <typename... Ts>
struct Args : _Args {
    Tuple<Ts...> _tuple {};

    Args(Ts&&... ts) : _tuple(::forward<Ts>(ts)...) { }

    usize len() override { return _tuple.len(); }

    Res<> format(Runes& rs, Io::TextWriter& writer, usize index) override {
        Res<> result = Error::invalidData("format index out of range");
        usize i      = 0;
        _tuple.visit([&](auto const& t) {
            if (index == i) {
                using U = Meta::RemoveCvRef<decltype(t)>;
                Formatter<U> formatter;
                if constexpr (requires() { formatter.parse(rs); }) {
                    formatter.parse(rs);
                }
                result = formatter.format(writer, t);
            }
            i++;

            return true;
        });
        return result;
    }
};

inline Res<> _format(Io::TextWriter& writer, Str format, _Args& args) {
    Runes rs { format };
    usize index = 0;

    while (not rs.ended()) {
        Rune c = rs.next();

        if (c == '{') {
            rs.skip(':');
            rs.begin();
            while (rs.peek() != '}') {
                rs.next();
            }
            rs.next();
            Runes inner { rs.end() };
            try$(args.format(inner, writer, index));
            index++;
        } else {
            try$(writer.writeRune(c));
        }
    }

    return Ok();
}

inline Res<> format(Io::TextWriter& writer, Str format) {
    return writer.writeStr(format);
}

template <typename... Ts>
inline Res<> format(Io::TextWriter& writer, Str format, Ts&&... ts) {
    Args<Ts...> args { ::forward<Ts>(ts)... };
    return _format(writer, format, args);
}

inline String format(Str format) {
    return format;
}

template <typename... Ts>
inline String format(Str format, Ts&&... ts) {
    Io::StringWriter writer {};
    Args<Ts...>      args { ::forward<Ts>(ts)... };
    _format(writer, format, args).unwrap("formating string");
    return writer.take();
}

template <typename T>
inline String toString(T const& t, Str format = "") {
    Io::StringWriter writer {};
    Formatter<T>     formatter;
    if constexpr (requires(Runes& rs) { formatter.parse(rs); }) {
        Runes rs { format };
        formatter.parse(rs);
    }
    formatter.format(writer, t).unwrap("formating string");
    return writer.take();
}

enum struct Align {
    LEFT,
    RIGHT,
    CENTER,
};

// MARK: - [Aligned]

template <typename T>
struct Aligned {
    T     _inner;
    Align _align;
    usize _width;
};

inline auto aligned(auto inner, Align align, usize width) {
    return Aligned<decltype(inner)> { inner, align, width };
}

template <typename T>
struct Formatter<Aligned<T>> {
    Formatter<T> _innerFmt {};

    void parse(Runes& rs) {
        if constexpr (requires() { _innerFmt.parse(rs); }) {
            _innerFmt.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Aligned<T> val) {
        Io::StringWriter<80> buf;
        try$(_innerFmt.format(buf, val._inner));
        usize width = buf.len();

        if (width < val._width) {
            usize pad = val._width - width;
            switch (val._align) {
                case Align::LEFT:
                    try$(writer.writeStr(buf.str()));
                    for (usize i = 0; i < pad; i++)
                        try$(writer.writeRune(' '));
                    break;
                case Align::RIGHT:
                    for (usize i = 0; i < pad; i++)
                        try$(writer.writeRune(' '));
                    try$(writer.writeStr(buf.str()));
                    break;
                case Align::CENTER:
                    for (usize i = 0; i < pad / 2; i++)
                        try$(writer.writeRune(' '));
                    try$(writer.writeStr(buf.str()));
                    for (usize i = 0; i < pad / 2; i++)
                        try$(writer.writeRune(' '));
                    break;
            }
        } else {
            try$(writer.writeStr(buf.str()));
        }

        return Ok();
    }
};

// MARK: - [Case]

enum struct Case {
    DEFAULT,
    CAMEL,
    CAPITAL,
    CONSTANT,
    DOT,
    HEADER,
    NO,
    PARAM,
    PASCAL,
    PATH,
    SENTENCE,
    SNAKE,
    TITLE,
    SWAP,
    LOWER,
    LOWER_FIRST,
    UPPER,
    UPPER_FIRST,
    SPONGE,
};

Res<String> toDefaultCase(Str str);

Res<String> toCamelCase(Str str);

Res<String> toCapitalCase(Str str);

Res<String> toConstantCase(Str str);

Res<String> toDotCase(Str str);

Res<String> toHeaderCase(Str str);

Res<String> toNoCase(Str str);

Res<String> toParamCase(Str str);

Res<String> toPascalCase(Str str);

Res<String> toPathCase(Str str);

Res<String> toSentenceCase(Str str);

Res<String> toSnakeCase(Str str);

Res<String> toTitleCase(Str str);

Res<String> toSwapCase(Str str);

Res<String> toLowerCase(Str str);

Res<String> toLowerFirstCase(Str str);

Res<String> toUpperCase(Str str);

Res<String> toUpperFirstCase(Str str);

Res<String> toSpongeCase(Str str);

Res<String> changeCase(Str str, Case toCase);

template <typename T>
struct Cased {
    T    _inner;
    Case _case;
};

inline auto cased(auto inner, Case cased) {
    return Cased<decltype(inner)> { inner, cased };
}

template <typename T>
struct Formatter<Cased<T>> {
    Formatter<T> _innerFmt {};

    void parse(Runes& rs) {
        if constexpr (requires() { _innerFmt.parse(rs); }) {
            _innerFmt.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Cased<T> val) {
        Io::StringWriter sw;
        try$(_innerFmt.format(sw, val._inner));
        String result = try$(changeCase(sw.str(), val._case));
        try$(writer.writeStr(result.str()));
        return Ok();
    }
};

// MARK: - [Number]

struct NumberFormatter {
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
        auto digit = [this](usize v) {
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

template <Meta::Unsigned T>
struct Formatter<T> : NumberFormatter {
    Res<> format(Io::TextWriter& writer, T const& val) {
        if (isChar)
            return formatRune(writer, val);
        return formatUnsigned(writer, val);
    }
};

template <Meta::Signed T>
struct Formatter<T> : NumberFormatter {
    Res<> format(Io::TextWriter& writer, T const& val) {
        if (isChar)
            return writer.writeRune(val);
        return formatSigned(writer, val);
    }
};

// #if !defined(__sdk_freestanding__)
// template <Meta::Float T>
// struct Formatter<T> : NumberFormatter {
//     Res<> format(Io::TextWriter& writer, f64 const& val) {
//         return formatFloat(writer, val);
//     }
// };
// #endif

template <typename T>
struct Formatter<Be<T>> : Formatter<T> {
    Res<> format(Io::TextWriter& writer, Be<T> const& val) {
        return Formatter<T>::format(writer, val.value());
    }
};

template <typename T>
struct Formatter<Le<T>> : Formatter<T> {
    Res<> format(Io::TextWriter& writer, Le<T> const& val) {
        return Formatter<T>::format(writer, val.value());
    }
};

// MARK: - [String]

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
        Str str = Str::fromNullterminated(text);
        return StringFormatter<Utf8>::format(writer, str);
    }
};

// MARK: - [Pointer & Reference]

template <typename T>
struct Formatter<T*> {
    bool prefix = false;

    void parse(Runes& rs) { prefix = rs.skip('#'); }

    Res<> format(Io::TextWriter& writer, T* val) {
        if (prefix) {
            try$(writer.writeRune('('));
            try$(writer.writeStr(Meta::nameOf<T>()));
            try$(writer.writeStr(" *)"s));
        }

        if (val) {
            NumberFormatter fmt;
            fmt.base   = 16;
            fmt.fill   = '0';
            fmt.width  = sizeof(T*) * 2;
            fmt.prefix = true;
            try$(fmt.formatUnsigned(writer, (usize) val));
        } else {
            try$(writer.writeStr("nullptr"s));
        }

        return Ok();
    }
};

template <>
struct Formatter<uflat> : Formatter<u64> {
    Res<> format(Io::TextWriter& writer, uflat const& val) {
        return Formatter<u64>::format(writer, (u64) val);
    }
};

template <>
struct Formatter<std::nullptr_t> {
    Res<> format(Io::TextWriter& writer, std::nullptr_t) {
        return writer.writeStr("nullptr"s);
    }
};

template <typename T>
struct Formatter<Rc<T>> {
    Formatter<T> formatter;

    void parse(Runes& rs) {
        if constexpr (requires() { formatter.parse(rs); }) {
            formatter.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Rc<T> const& val) {
        return formatter.format(writer, val.unwrap());
    }
};

template <typename T>
struct Formatter<WeakRc<T>> {
    Formatter<T> formatter;

    void parse(Runes& rs) {
        if constexpr (requires() { formatter.parse(rs); }) {
            formatter.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, WeakRc<T> const& val) {
        auto inner = val.upgrade();
        if (not inner)
            return writer.writeStr("None"s);
        return formatter.format(writer, inner.unwrap().unwrap());
    }
};

template <typename T>
struct Formatter<Box<T>> {
    Formatter<T> formatter;

    void parse(Runes& rs) {
        if constexpr (requires() { formatter.parse(rs); }) {
            formatter.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Box<T> const& val) {
        return formatter.format(writer, *val);
    }
};

// MARK: - [Miscellaneous]

template <Meta::Enum T>
struct Formatter<T> {
    Res<> format(Io::TextWriter& writer, T val) {
        return format(
            writer, "({} {})", Meta::nameOf<T>(), toUnderlyingType(val));
    }
};

template <>
struct Formatter<::None> {
    Res<> format(Io::TextWriter& writer, ::None const&) {
        return writer.writeStr("Empty"s);
    }
};

template <>
struct Formatter<bool> {
    Res<> format(Io::TextWriter& writer, bool val) {
        return writer.writeStr(val ? "True"s : "False"s);
    }
};

template <typename T>
struct Formatter<Opt<T>> {
    Formatter<T> formatter;

    void parse(Runes& rs) {
        if constexpr (requires() { formatter.parse(rs); }) {
            formatter.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Opt<T> const& val) {
        if (val)
            return formatter.format(writer, *val);
        return writer.writeStr("Empty"s);
    }
};

template <typename T>
struct Formatter<Ok<T>> {
    Formatter<T> formatter;

    void parse(Runes& rs) {
        if constexpr (requires() { formatter.parse(rs); }) {
            formatter.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Ok<T> const& val) {
        if constexpr (Meta::Same<T, None>)
            return writer.writeStr("Ok"s);
        else
            return formatter.format(writer, val.inner);
    }
};

template <>
struct Formatter<Error> {
    Res<> format(Io::TextWriter& writer, Error const& val) {
        Str msg = Str::fromNullterminated(val.msg());
        try$(writer.writeStr(msg));
        return Ok();
    }
};

template <typename T, typename E>
struct Formatter<Res<T, E>> {
    Formatter<T> _fmtOk;
    Formatter<E> _fmtErr;

    void parse(Runes& rs) {
        if constexpr (requires() { _fmtOk.parse(rs); }) {
            _fmtOk.parse(rs);
        }

        if constexpr (requires() { _fmtErr.parse(rs); }) {
            _fmtErr.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Res<T, E> const& val) {
        if (val)
            return _fmtOk.format(writer, val.unwrap());
        return _fmtErr.format(writer, val.none());
    }
};

template <typename... Ts>
struct Formatter<Union<Ts...>> {
    Res<> format(Io::TextWriter& writer, Union<Ts...> const& val) {
        return val.visit(
            [&](auto const& v) -> Res<> { return format(writer, "{}", v); });
    }
};

// MARK: - [Res & Opt]

// MARK: - [Sliceable]

template <Sliceable T>
struct Formatter<T> {
    Formatter<typename T::Inner> inner;

    void parse(Runes& rs) {
        if constexpr (requires() { inner.parse(rs); }) {
            inner.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, T const& val) {
        try$(writer.writeStr("["s));
        for (usize i = 0; i < val.len(); i++) {
            if (i != 0)
                try$(writer.writeStr(", "s));
            try$(inner.format(writer, val[i]));
        }
        return Ok(try$(writer.writeStr("]"s)));
    }
};

template <typename T, typename Tag>
struct Formatter<Range<T, Tag>> {

    Formatter<T> inner;

    void parse(Runes& rs) {
        if constexpr (requires() { inner.parse(rs); }) {
            inner.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Range<T, Tag> const& val) {
        try$(writer.writeStr("["s));
        try$(inner.format(writer, val.start));
        try$(writer.writeStr("-"s));
        try$(inner.format(writer, val.end()));
        try$(writer.writeStr("]"s));
        return Ok();
    }
};

} // namespace Sdk::Text
