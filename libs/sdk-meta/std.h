#pragma once

#include <compare>
#include <coroutine>
#include <cstring>
#include <initializer_list>
#include <sdk-meta/types.h>

template <typename T>
using InitializerList = std::initializer_list<T>;

[[gnu::always_inline]] inline void* operator new(usize size, void* p) noexcept {
    return p;
}

[[gnu::always_inline]] inline void operator delete(void*, void*) noexcept {
    // do nothing
}
