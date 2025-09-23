#pragma once

#include <sdk-meta/types.h>

namespace Ui {

struct Size {
    usize height {};
    usize width {};
};

enum struct HorizontalAlignment {
    Left,
    Center,
    Right,
    Stretch
};

enum struct VerticalAlignment {
    Top,
    Center,
    Bottom,
    Stretch
};

struct CornerRadius {
    usize tl, tr, bl, br;
};

} // namespace Ui
