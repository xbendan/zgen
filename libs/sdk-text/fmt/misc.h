#pragma once

#include <sdk-io/text.h>
#include <sdk-text/fmt/base.h>
#include <sdk-text/runes.h>

namespace Sdk::Text {
template <>
struct Formatter<Empty> {
    Res<> format(Io::TextWriter& writer, Empty const&) {
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
        if constexpr (Meta::Same<T, Empty>)
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

// MARK: Format Unions ---------------------------------------------------------

template <typename... Ts>
struct Formatter<Union<Ts...>> {
    Res<> format(Io::TextWriter& writer, Union<Ts...> const& val) {
        return val.visit(
            [&](auto const& v) -> Res<> { return format(writer, "{}", v); });
    }
};

} // namespace Sdk::Text
