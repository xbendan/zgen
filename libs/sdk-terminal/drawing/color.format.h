#pragma once

#include <sdk-terminal/drawing/color.ansi.h>
#include <sdk-terminal/drawing/color.h>
#include <sdk-text/fmt/base.h>

namespace Sdk::Text {

template <>
struct Formatter<Terminal::Drawing::Color> { };

template <>
struct Formatter<Terminal::Drawing::ColorAnsiCode> { };

} // namespace Sdk::Text
