#pragma once

#include <sdk-io/text.h>
#include <sdk-meta/res.h>
#include <sdk-meta/tuple.h>
#include <sdk-meta/types.h>
#include <sdk-text/runes.h>

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

Res<> _format(Io::TextWriter& writer, Str format, _Args& args);

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

} // namespace Sdk::Text
