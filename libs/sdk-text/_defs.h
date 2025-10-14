#pragma once

#include <sdk-text/rune.h>

namespace Zgen::Core {
#if defined(__sdk_encoding_utf8) or not defined(__sdk_encoding)
using Encoding = Sdk::Text::Utf8;
#elif defined(__sdk_encoding_utf16)
using Encoding = Sdk::Text::Utf16;
#elif defined(__sdk_encoding_ascii)
using Encoding = Sdk::Text::Ascii;
#else
#    error "Unknown system encoding"
#endif

} // namespace Zgen::Core
