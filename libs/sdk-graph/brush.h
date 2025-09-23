#pragma once

#include <sdk-graph/format.h>
#include <sdk-meta/types.h>

namespace Sdk::Graph {

struct Color {
    u8 r, g, b, a;
};

struct Brush { };

struct SolidBrush : Brush {
    Color raw;
};

struct LinearBrush : Brush {
    Color  begin;
    Color  end;
    double angle;
};

struct RadialBrush : Brush { };

struct ImageBrush : Brush { };

} // namespace Sdk::Graph
