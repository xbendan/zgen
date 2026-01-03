module;

#include <sdk-meta/_macros.h>

export module sdk.text.format:type_align;

import sdk;
import sdk.text;
import sdk.io;
import :base;

namespace Realms::Text {

template <typename T>
struct Aligned {
    T     _inner;
    Align _align;
    usize _width;
};

export inline auto aligned(auto inner, Align align, usize width) {
    return Aligned<decltype(inner)> { inner, align, width };
}

export template <typename T>
struct Formatter<Aligned<T>> {
    Formatter<T> _innerFmt {};

    void parse(Runes& rs) {
        if constexpr (requires() { _innerFmt.parse(rs); }) {
            _innerFmt.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Aligned<T> val) {
        Io::StringBuf buf;
        try$(_innerFmt.format(buf, val._inner));
        usize width = buf.len();

        if (width < val._width) {
            usize pad = val._width - width;
            switch (val._align) {
                case Align::LEFT:
                    try$(writer.writeStr(buf.toStr()));
                    for (usize i = 0; i < pad; i++)
                        try$(writer.writeRune(' '));
                    break;
                case Align::RIGHT:
                    for (usize i = 0; i < pad; i++)
                        try$(writer.writeRune(' '));
                    try$(writer.writeStr(buf.toStr()));
                    break;
                case Align::CENTER:
                    for (usize i = 0; i < pad / 2; i++)
                        try$(writer.writeRune(' '));
                    try$(writer.writeStr(buf.toStr()));
                    for (usize i = 0; i < pad / 2; i++)
                        try$(writer.writeRune(' '));
                    break;
            }
        } else {
            try$(writer.writeStr(buf.toStr()));
        }

        return Ok();
    }
};

} // namespace Realms::Text
