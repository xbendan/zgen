module;

#include <sdk-meta/_macros.h>

export module sdk.text.format:type_meta;

import sdk;
import sdk.io;
import sdk.text;
import :base;

namespace Realms::Text {

export template <>
struct Formatter<::None> {
    Res<> format(Io::TextWriter& writer, ::None const&) {
        return writer.writeStr("Empty"s);
    }
};

export template <typename T>
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

export template <typename T>
struct Formatter<Ok<T>> {
    Formatter<T> formatter;

    void parse(Runes& rs) {
        if constexpr (requires() { formatter.parse(rs); }) {
            formatter.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Ok<T> const& val) {
        if constexpr (Same<T, None>)
            return writer.writeStr("Ok"s);
        else
            return formatter.format(writer, val.inner);
    }
};

export template <>
struct Formatter<Error> {
    Res<> format(Io::TextWriter& writer, Error const& val) {
        Str msg = Str::nullterminated(val.msg());
        try$(writer.writeStr(msg));
        return Ok();
    }
};

export template <typename T, typename E>
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

export template <typename... Ts>
struct Formatter<Union<Ts...>> {
    Res<> format(Io::TextWriter& writer, Union<Ts...> const& val) {
        return val.visit(
            [&](auto const& v) -> Res<> { return format(writer, "{}", v); });
    }
};

export template <Sliceable T>
struct Formatter<T> {
    Formatter<typename T::E> inner;

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

export template <typename T, typename Tag>
struct Formatter<Meta::Range<T, Tag>> {

    Formatter<T> inner;

    void parse(Runes& rs) {
        if constexpr (requires() { inner.parse(rs); }) {
            inner.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Meta::Range<T, Tag> const& val) {
        try$(writer.writeStr("["s));
        try$(inner.format(writer, val.start));
        try$(writer.writeStr("-"s));
        try$(inner.format(writer, val.end()));
        try$(writer.writeStr("]"s));
        return Ok();
    }
};

} // namespace Realms::Text
