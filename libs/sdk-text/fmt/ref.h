#pragma once

#include <sdk-io/text.h>
#include <sdk-meta/rc.h>
#include <sdk-text/fmt/base.h>
#include <sdk-text/fmt/nums.h>
#include <sdk-text/str.h>

namespace Sdk::Text {

template <typename T>
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
            fmt.base     = 16;
            fmt.fillChar = '0';
            fmt.width    = sizeof(T*) * 2;
            fmt.prefix   = true;
            try$(fmt.formatUnsigned(writer, (usize) val));
        } else {
            try$(writer.writeStr("nullptr"s));
        }

        return Ok();
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

} // namespace Sdk::Text
