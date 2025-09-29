#pragma once

#include <sdk-terminal/views/thickness.h>
#include <sdk-text/str.h>

namespace Sdk::Terminal::Views {

struct View {
    Str       _id;
    Thickness _margin, _padding, _border;
};

} // namespace Sdk::Terminal::Views
