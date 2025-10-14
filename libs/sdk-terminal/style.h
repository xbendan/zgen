#pragma once

#include <sdk-graph/format.h>
#include <sdk-io/text.h>
#include <sdk-meta/res.h>
#include <sdk-text/format.h>
#include <sdk-text/runes.h>
#include <sdk-text/style.h>

namespace Sdk::Cli {

using Sdk::Graph::iRgb;
using Sdk::Text::TextDecoration;

struct [[gnu::packed]] Style {
    u8             _fg { 0x10 };
    u8             _bg { 0x10 };
    TextDecoration decos { TextDecoration::None };

    constexpr Style() = default;

    constexpr Style(iRgb fg)
        : _fg(fg),
          _bg(iRgb::Black),
          decos(TextDecoration::None) { }

    constexpr Style(iRgb fg, iRgb bg)
        : _fg(fg),
          _bg(bg),
          decos(TextDecoration::None) { }

    constexpr Style(iRgb fg, iRgb bg, TextDecoration decos)
        : _fg(fg),
          _bg(bg),
          decos(decos) { }

    static Style const Default;

    constexpr Style fg(iRgb color) {
        _fg = color;
        return *this;
    };

    constexpr Style bg(iRgb color) {
        _bg = color;
        return *this;
    };

    constexpr Style deco(TextDecoration decos) {
        this->decos = decos;
        return *this;
    };

    constexpr Style bold() {
        decos |= TextDecoration::Bold;
        return *this;
    };

    constexpr Style italic() {
        decos |= TextDecoration::Italic;
        return *this;
    };

    constexpr Style underline() {
        decos |= TextDecoration::Underline;
        return *this;
    };

    constexpr Style blink() {
        decos |= TextDecoration::Blink;
        return *this;
    };

    constexpr Style strikethrough() {
        decos |= TextDecoration::Strikethrough;
        return *this;
    };
};

inline Style const Style::Default = { iRgb::White, iRgb::Black };

constexpr Style style(auto... args) {
    return Style { args... };
}

template <typename T>
struct Styled {
    T     _inner;
    Style _color;
};

} // namespace Sdk::Cli

namespace Sdk::Text {

template <>
struct Formatter<Sdk::Cli::Style> {

    Formatter<Str> _colorFmt {};

    Res<> format(Io::TextWriter& writer, Sdk::Cli::Style const& val) {
        if (val.decos == TextDecoration::None) {
            try$(writer.writeStr("\x1b[0m"s));
            return Ok();
        }

        if (val._fg < 0x10) {
            try$(Text::format(writer,
                              (val._fg & 0x8) ? "\x1b[{}m"s : "\x1b[{};1m"s,
                              30 + (val._fg & 0x7)));
        }

        if (val._bg < 0x10) {
            try$(Text::format(writer,
                              (val._bg & 0x8) ? "\x1b[{}m"s : "\x1b[{};1m"s,
                              40 + (val._bg & 0x7)));
        }

        if (val.decos & TextDecoration::Bold) {
            try$(Text::format(writer, "\x1b[1m"s));
        }

        if (val.decos & TextDecoration::Dim) {
            try$(Text::format(writer, "\x1b[2m"s));
        }

        if (val.decos & TextDecoration::Underline) {
            try$(Text::format(writer, "\x1b[4m"s));
        }

        if (val.decos & TextDecoration::Blink) {
            try$(Text::format(writer, "\x1b[5m"s));
        }

        if (val.decos & TextDecoration::Reverse) {
            try$(Text::format(writer, "\x1b[7m"s));
        }

        if (val.decos & TextDecoration::Hidden) {
            try$(Text::format(writer, "\x1b[8m"s));
        }

        return Ok();
    }
};

template <typename T>
struct Formatter<Sdk::Cli::Styled<T>> {
    Formatter<Sdk::Cli::Style> _styleFmt {};
    Formatter<T>               _innerFmt {};

    void parse(Runes& rs) {
        if constexpr (requires() { _innerFmt.parse(rs); }) {
            _innerFmt.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Sdk::Cli::Styled<T> const& val) {

        try$(_styleFmt.format(writer, val._color));
        try$(_innerFmt.format(writer, val._inner));
        try$(writer.writeStr("\x1b[0m"s));

        return Ok();
    }
};

} // namespace Sdk::Text
