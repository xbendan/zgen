#pragma once

#include <sdk-meta/types.h>

struct Debug {
    char const* file;
    char const* func;
    usize       line;
    usize       column;

    static constexpr Debug breakpoint(char const* file   = __builtin_FILE(),
                                      char const* func   = __builtin_FUNCTION(),
                                      usize       line   = __builtin_LINE(),
                                      usize       column = __builtin_COLUMN()) {
        return Debug { file, func, line, column };
    }
};
