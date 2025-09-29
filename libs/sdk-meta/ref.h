#pragma once

#include <sdk-meta/panic.h>
#include <sdk-meta/types.h>

template <typename T>
struct Nonnull {
    T* ptr;

    [[gnu::always_inline]] constexpr Nonnull() = delete;

    [[gnu::always_inline]] constexpr Nonnull(T const* p)
        : ptr(const_cast<T*>(p)) {
        if (ptr == nullptr)
            panic("Nonnull: init with null pointer value.");
    }

    [[gnu::always_inline]] constexpr Nonnull(T& p) : ptr(&p) { }

    [[gnu::always_inline]] constexpr Nonnull(Nonnull const&) = default;

    [[gnu::always_inline]] constexpr Nonnull& operator=(Nonnull const&)
        = default;

    [[gnu::always_inline]] constexpr Nonnull& operator=(T* p) {
        if (p == nullptr)
            panic("Nonnull: assign with null pointer value.");
        ptr = p;
        return *this;
    }

    [[gnu::always_inline]] constexpr Nonnull& operator=(T& p) {
        ptr = &p;
        return *this;
    }

    [[gnu::always_inline]] constexpr T& operator*() const { return *ptr; }

    [[gnu::always_inline]] constexpr T* operator->() const { return ptr; }

    [[gnu::always_inline]] constexpr T* get() const { return ptr; }

    [[gnu::always_inline]] constexpr explicit operator bool() const {
        return true;
    }
};
