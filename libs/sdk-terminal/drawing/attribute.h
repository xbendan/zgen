#pragma once

#include <sdk-meta/types.h>
#include <sdk-terminal/drawing/color.h>

namespace Sdk::Terminal::Drawing {

struct Attribute {
    i32   _plat;
    Color _foreground, _background;
};

} // namespace Sdk::Terminal::Drawing
