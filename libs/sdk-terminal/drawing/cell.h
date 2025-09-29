#pragma once

#include <sdk-terminal/drawing/attribute.h>
#include <sdk-text/rune.h>

namespace Sdk::Terminal::Drawing {

using Text::Rune;

struct Cell {
    bool      _dirty;
    Rune      _rune;
    Attribute _attr;
};

} // namespace Sdk::Terminal::Drawing
