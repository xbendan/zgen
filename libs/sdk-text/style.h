#pragma once

#include <sdk-meta/flags.h>
#include <sdk-meta/types.h>

namespace Sdk::Text {

enum struct TextWrapping {
    Disabled,
    Wrap,
    Truncate,
    Ellipsis,
    Overflow
};

enum struct TextAlignment {
    Left,
    Center,
    Right
};

enum TextDecoration : u8 {
    None          = 0,
    Bold          = 1 << 0,
    Dim           = 1 << 1,
    Italic        = 1 << 2,
    Underline     = 1 << 3,
    Blink         = 1 << 4,
    Reverse       = 1 << 5,
    Hidden        = 1 << 6,
    Strikethrough = 1 << 7
};
MakeFlags$(TextDecoration);

} // namespace Sdk::Text
