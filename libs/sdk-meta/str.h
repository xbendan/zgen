#pragma once

#include <sdk-meta/types.h>

constexpr inline bool cstrEq(char const* a, char const* b) {
    if (a == nullptr || b == nullptr) {
        return a == b;
    }
    while (*a && *b) {
        if (*a != *b) {
            return false;
        }
        a++;
        b++;
    }
    return *a == *b; // both should be '\0' at this point
}
