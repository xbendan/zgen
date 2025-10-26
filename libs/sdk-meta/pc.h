#pragma once

#include <sdk-meta/_embed.h>
#include <sdk-meta/vec.h>

namespace Meta {

template <typename T>
struct Pc {
    Vec<T> _buf;

    T& operator*() {
        // get current cpu id
    }

    usize id() const { return 0; /* TODO: implement CPU id retrieval */ }

    T& operator()() { return _buf[id()]; }
};

} // namespace Meta

using Meta::Pc;
