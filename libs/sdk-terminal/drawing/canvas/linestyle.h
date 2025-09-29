#pragma once

#include <sdk-meta/types.h>

namespace Sdk::Terminal::Drawing {

enum struct LineStyle : u8 {
    None,
    Single,
    Double,
    Dashed,
    Dotted,
    Heavy,
    HeavyDashed,
    HeavyDotted,
    Rounded,
    RoundedDashed,
    RoundedDotted,
};

} // namespace Sdk::Terminal::Drawing
