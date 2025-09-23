#pragma once

#include <sdk-io/text.h>
#include <sdk-text/fmt/base.h>

namespace Sdk::Text {

template <Meta::Enum T>
struct Formatter<T> {
    Res<> format(Io::TextWriter& writer, T val) {
        return format(writer, "({} {})", nameOf<T>(), toUnderlyingType(val));
    }
};

} // namespace Sdk::Text
