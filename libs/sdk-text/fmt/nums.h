#pragma once

#include <sdk-io/text.h>
#include <sdk-meta/endian.h>
#include <sdk-meta/traits.h>
#include <sdk-text/fmt/base.h>
#include <sdk-text/runes.h>

namespace Sdk::Text {

struct NumberFormatter {
    bool  prefix        = false;
    bool  isChar        = false;
    usize base          = 10;
    usize width         = 0;
    char  fillChar      = ' ';
    bool  trailingZeros = false;
    usize precision     = 6;

    Str formatPrefix();

    void parse(Str str);

    void parse(Runes& rs);

    Res<> formatUnsigned(Io::TextWriter& writer, usize val);

    Res<> formatSigned(Io::TextWriter& writer, isize val);

#ifndef __ck_freestanding__
    Res<> formatFloat(Io::TextWriter& writer, f64 val);
#endif

    Res<> formatRune(Io::TextWriter& writer, Rune val);
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

#ifndef __sdk_freestanding__
template <Meta::Float T>
struct Formatter<T> : NumberFormatter {
    Res<> format(Io::TextWriter& writer, f64 const& val) {
        return formatFloat(writer, val);
    }
};
#endif

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

} // namespace Sdk::Text
