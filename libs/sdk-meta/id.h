#pragma once

#include <sdk-meta/types.h>

namespace Meta {

using Id = usize;

template <typename T>
static constexpr Id idOf() {
    char const* cstr = __PRETTY_FUNCTION__;
    usize       len  = sizeof(__PRETTY_FUNCTION__);
    usize       hash = 0uz;
    for (char const* b = cstr; b < cstr + len; b++)
        hash = (100'0003 * hash) ^ *b;
    hash ^= len;

    if (hash == 0uz) [[unlikely]] {
        panic("Meta::id: hash is zero");
    }

    return hash;
}

template <typename T>
static constexpr char const* nameOf() {
    char const* cstr  = __PRETTY_FUNCTION__;
    usize       len   = sizeof(__PRETTY_FUNCTION__);
    usize       start = 0uz;

    // Skip the return type and template parameters
    for (usize i = 0; i < len; i++) {
        if (cstr[i] == '=' || cstr[i] == '<') {
            start = i + 1;
            break;
        }
    }

    // Find the end of the type name
    for (usize i = start; i < len; i++) {
        if (cstr[i] == ' ' || cstr[i] == ';') {
            return cstr + start;
        }
    }

    return cstr + start;
}

} // namespace Meta
