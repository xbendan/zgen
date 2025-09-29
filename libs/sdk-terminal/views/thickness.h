#pragma once

#include <sdk-meta/types.h>

namespace Sdk::Terminal::Views {

struct Thickness {
    u32 left { 0 };
    u32 top { 0 };
    u32 right { 0 };
    u32 bottom { 0 };

    constexpr Thickness() = default;

    constexpr Thickness(u32 all)
        : left(all),
          top(all),
          right(all),
          bottom(all) { }

    constexpr Thickness(u32 horizontal, u32 vertical)
        : left(horizontal),
          top(vertical),
          right(horizontal),
          bottom(vertical) { }

    constexpr Thickness(u32 left, u32 top, u32 right, u32 bottom)
        : left(left),
          top(top),
          right(right),
          bottom(bottom) { }
};

} // namespace Sdk::Terminal::Views
