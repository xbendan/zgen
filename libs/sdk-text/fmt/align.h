#pragma once

#include <sdk-io/text.h>
#include <sdk-meta/res.h>
#include <sdk-meta/try.h>
#include <sdk-meta/types.h>
#include <sdk-text/fmt/base.h>
#include <sdk-text/runes.h>

namespace Sdk::Text {

enum struct Align {
    LEFT,
    RIGHT,
    CENTER,
};

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

    void parse(Runes& scan) {
        if constexpr (requires() { _innerFmt.parse(scan); }) {
            _innerFmt.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& writer, Aligned<T> val) {
        Io::StringWriter buf;
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

} // namespace Sdk::Text
