#pragma once

#include <sdk-meta/types.h>
#include <sdk-terminal/drawing/color.ansi.h>

namespace Sdk::Terminal::Drawing {

union Color {
    u32 _hex;
    struct {
        u8 _r;
        u8 _g;
        u8 _b;
        u8 _a;
    };

    [[gnu::always_inline]] constexpr Color() : _hex(0) { }

    [[gnu::always_inline]] constexpr Color(u8 r, u8 g, u8 b, u8 a = 255)
        : _r(r),
          _g(g),
          _b(b),
          _a(a) { }

    [[gnu::always_inline]] constexpr Color(u32 hex) : _hex(hex) { }

    [[gnu::always_inline]] constexpr bool operator==(Color const&) const
        = default;

    [[gnu::always_inline]] constexpr bool operator!=(Color const&) const
        = default;

    [[gnu::always_inline]] constexpr ColorAnsiCode ansi() const {
        // TODO: implement
    }
};

} // namespace Sdk::Terminal::Drawing
