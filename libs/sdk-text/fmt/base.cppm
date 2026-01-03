module;

#include <sdk-meta/_macros.h>

export module sdk.text.format:base;

import sdk;
import sdk.text;
import sdk.io;

export namespace Realms::Text {

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
                using U = RemoveCvRef<decltype(t)>;
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
    Io::StringBuf buf {};

    Args<Ts...> args { ::forward<Ts>(ts)... };
    _format(buf, format, args).unwrap("formating string");
    return buf.toString();
}

template <typename T>
inline String toString(T const& t, Str format = "") {
    Io::StringBuf buf {};
    Formatter<T>  formatter;
    if constexpr (requires(Runes& rs) { formatter.parse(rs); }) {
        Runes rs { format };
        formatter.parse(rs);
    }
    formatter.format(buf, t).unwrap("formating string");
    return buf.toString();
}

enum struct Align {
    LEFT,
    RIGHT,
    CENTER,
};

// MARK: - [Aligned]

// MARK: - [Case]

// MARK: - [Number]

// MARK: - [String]

// MARK: - [Pointer & Reference]

// MARK: - [Miscellaneous]

template <Enum T>
struct Formatter<T> {
    Res<> format(Io::TextWriter& writer, T val) {
        return format(writer, "({} {})", nameOf<T>(), toUnderlyingType(val));
    }
};

// MARK: - [Res & Opt]

// MARK: - [Sliceable]

} // namespace Realms::Text
