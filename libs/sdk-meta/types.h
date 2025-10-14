#pragma once

#include <compare>
#include <coroutine>
#include <initializer_list>

#define alignat(x) __attribute__((aligned(x)))

using u8   = unsigned char;
using u16  = unsigned short;
using u32  = unsigned int;
using u64  = unsigned long long;
using u128 = unsigned __int128;

using i8   = signed char;
using i16  = signed short;
using i32  = signed int;
using i64  = signed long long;
using i128 = signed __int128;

using byte  = u8;
using word  = u16;
using dword = u32;
using qword = u64;

using usize = long unsigned int;
using isize = long int;

using f32  = float;
using f64  = double;
using f128 = long double;

using nullptr_t = decltype(nullptr);

struct None {
    constexpr None() = default;

    explicit constexpr operator bool() const noexcept { return false; }

    template <typename T>
    constexpr operator T const*() const noexcept {
        return nullptr;
    }

    constexpr bool operator==(None const&) const = default;

    constexpr bool operator!=(None const&) const = default;
};

constexpr inline None NONE {};

template <typename T>
bool operator==(None, T* ptr) {
    return ptr == nullptr;
}

struct _Copy { };

struct _Move { };

constexpr inline auto Copy = _Copy {};

constexpr inline auto Move = _Move {};

template <typename T>
using InitializerList = std::initializer_list<T>;

[[gnu::always_inline]] inline void* operator new(usize size, void* p) noexcept {
    return p;
}

[[gnu::always_inline]] inline void operator delete(void*, void*) noexcept {
    // do nothing
}
