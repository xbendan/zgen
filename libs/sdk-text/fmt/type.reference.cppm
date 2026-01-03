module;

#include <sdk-meta/_macros.h>

export module sdk.text.format:type_reference;

import sdk;
import sdk.text;
import sdk.io;
import :base;
import :type_numbers;

namespace Realms::Text {

export template <typename T>
struct Formatter<T*> {
    bool prefix = false;

    void parse(Runes& rs) { prefix = rs.skip('#'); }

    Res<> format(Io::TextWriter& writer, T* val) {
        if (prefix) {
            try$(writer.writeRune('('));
            try$(writer.writeStr(nameOf<T>()));
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

export template <typename T>
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

export template <typename T>
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

export template <typename T>
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

} // namespace Realms::Text
