#pragma once

#include <compare>
#include <initializer_list>

#define alignat(x) __attribute__((aligned(x)))
#define section(x) __attribute__((section(x)))

#define packed gnu::packed

#if defined(__clang__)
// #    define always_inline [[clang::always_inline]]
#    define lifetimebound [[clang::lifetimebound]]
#elif defined(__GNUC__)
// #    define always_inline [[gnu::always_inline]]
#    define lifetimebound
#endif

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
#if defined(__SIZEOF_INT128__)
using u128 = unsigned __int128;
#endif

using i8  = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;
#if defined(__SIZEOF_INT128__)
using i128 = signed __int128;
#endif

using byte  = u8;
using word  = u16;
using dword = u32;
using qword = u64;

using usize = __SIZE_TYPE__;
using isize = __PTRDIFF_TYPE__;

using f32 = float;
using f64 = double;
#if defined(__SIZEOF_INT128__)
using f128 = long double;
#endif

using nullptr_t = decltype(nullptr);

struct None {
    constexpr None() = default;

    explicit constexpr operator bool() const noexcept { return false; }

    template <class T>
    constexpr operator T const*() const noexcept {
        return nullptr;
    }

    constexpr bool operator==(None const&) const = default;
    constexpr bool operator!=(None const&) const = default;
};

constexpr inline None NONE {};

template <class T>
constexpr bool operator==(None, T const* ptr) {
    return ptr == nullptr;
}

struct Copy { };

struct Move { };

constexpr inline auto COPY = Copy {};

constexpr inline auto MOVE = Move {};

template <typename T>
using InitializerList = std::initializer_list<T>;

[[gnu::always_inline]] inline void* operator new(usize size, void* p) noexcept {
    return p;
}

[[gnu::always_inline]] inline void operator delete(void*, void*) noexcept {
    // do nothing
}

[[noreturn]] void panic(char const* message);

// [[noreturn]] void panic(char const* fmt, auto&&... args);
