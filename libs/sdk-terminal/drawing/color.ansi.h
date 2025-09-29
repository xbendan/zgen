#pragma once

#include <sdk-meta/types.h>

namespace Sdk::Terminal::Drawing {

enum ColorAnsiCode : u8 {
    Black         = 0x0,
    Red           = 0x1,
    Green         = 0x2,
    Yellow        = 0x3,
    Blue          = 0x4,
    Magenta       = 0x5,
    Cyan          = 0x6,
    White         = 0x7,
    BrightBlack   = 0x8,
    BrightRed     = 0x9,
    BrightGreen   = 0xA,
    BrightYellow  = 0xB,
    BrightBlue    = 0xC,
    BrightMagenta = 0xD,
    BrightCyan    = 0xE,
    BrightWhite   = 0xF,
};

} // namespace Sdk::Terminal::Drawing
