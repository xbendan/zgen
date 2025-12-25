#pragma once

#include <sdk-meta/types.h>

namespace Sdk::Graph {

struct Rgb {
    u8 r, g, b, a;
};

union [[packed]] iRgb {
    u8 bits;
    struct {
        u8 r: 1;
        u8 g: 1;
        u8 b: 1;
        u8 a: 1;
        u8 __reserved__: 4;
    };

    static iRgb const Black;
    static iRgb const Blue;
    static iRgb const Green;
    static iRgb const Cyan;
    static iRgb const Red;
    static iRgb const Magenta;
    static iRgb const Brown;
    static iRgb const LightGray;
    static iRgb const DarkGray;
    static iRgb const LightBlue;
    static iRgb const LightGreen;
    static iRgb const LightCyan;
    static iRgb const LightRed;
    static iRgb const LightMagenta;
    static iRgb const Yellow;
    static iRgb const White;

    constexpr iRgb(u8 bits) : bits(bits) { }

    constexpr iRgb(bool r, bool g, bool b, bool a) : r(r), g(g), b(b), a(a) { }

    constexpr iRgb(Rgb color)
        : r(color.r > 127),
          g(color.g > 127),
          b(color.b > 127),
          a(color.a > 127) { }

    constexpr operator u8() const { return bits & 0x0f; }
};

inline iRgb const iRgb::Black        = 0;
inline iRgb const iRgb::Blue         = 1;
inline iRgb const iRgb::Green        = 2;
inline iRgb const iRgb::Cyan         = 3;
inline iRgb const iRgb::Red          = 4;
inline iRgb const iRgb::Magenta      = 5;
inline iRgb const iRgb::Brown        = 6;
inline iRgb const iRgb::LightGray    = 7;
inline iRgb const iRgb::DarkGray     = 8;
inline iRgb const iRgb::LightBlue    = 9;
inline iRgb const iRgb::LightGreen   = 10;
inline iRgb const iRgb::LightCyan    = 11;
inline iRgb const iRgb::LightRed     = 12;
inline iRgb const iRgb::LightMagenta = 13;
inline iRgb const iRgb::Yellow       = 14;
inline iRgb const iRgb::White        = 15;

} // namespace Sdk::Graph
