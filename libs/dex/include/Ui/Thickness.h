#pragma once

#include <sdk-meta/types.h>

namespace Ui {

struct Thickness {
    f64 left, top, right, bottom;

    Thickness() : left(0), top(0), right(0), bottom(0) { }

    Thickness(f64 l, f64 t, f64 r, f64 b)
        : left(l),
          top(t),
          right(r),
          bottom(b) { }

    Thickness(f64 uniform)
        : left(uniform),
          top(uniform),
          right(uniform),
          bottom(uniform) { }
};

} // namespace Ui
