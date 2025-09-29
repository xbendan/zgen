#pragma once

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

using uptr = unsigned long long;

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

// using FlatPtr = void*;

struct [[gnu::packed]] FlatPtr {
    FlatPtr() = default;

    FlatPtr(uptr ptr) : m_ptr(reinterpret_cast<void*>(ptr)) { }

    template <typename T>
    FlatPtr(T* ptr) : m_ptr(reinterpret_cast<void*>(ptr)) { }

    FlatPtr(FlatPtr const& other) : m_ptr(other.m_ptr) { }

    FlatPtr(FlatPtr&& other) : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }

    FlatPtr& operator=(FlatPtr const& other) {
        m_ptr = other.m_ptr;
        return *this;
    }

    FlatPtr& operator=(FlatPtr&& other) {
        m_ptr       = other.m_ptr;
        other.m_ptr = nullptr;
        return *this;
    }

    FlatPtr& operator=(void* ptr) {
        m_ptr = ptr;
        return *this;
    }

    FlatPtr& operator=(nullptr_t) {
        m_ptr = nullptr;
        return *this;
    }

    FlatPtr& operator=(uptr ptr) {
        m_ptr = reinterpret_cast<void*>(ptr);
        return *this;
    }

    operator uptr() const { return reinterpret_cast<uptr>(m_ptr); }

    operator void*() const { return reinterpret_cast<void*>(m_ptr); }

    operator FlatPtr*() const { return reinterpret_cast<FlatPtr*>(m_ptr); }

    bool operator<=(FlatPtr const& other) const { return m_ptr <= other.m_ptr; }

    bool operator>=(FlatPtr const& other) const { return m_ptr >= other.m_ptr; }

    template <typename T>
    operator T*() const {
        return reinterpret_cast<T*>(m_ptr);
    }

    void* m_ptr;
};
static_assert(sizeof(FlatPtr) == sizeof(void*), "FlatPtr size mismatch");

struct Empty {
    constexpr Empty() = default;

    explicit constexpr operator bool() const noexcept { return false; }

    template <typename T>
    constexpr operator T const*() const noexcept {
        return nullptr;
    }

    constexpr bool operator==(Empty const&) const = default;

    constexpr bool operator!=(Empty const&) const = default;
};

constexpr inline Empty EMPTY {};

template <typename T>
bool operator==(Empty, T* ptr) {
    return ptr == nullptr;
}

struct _Copy { };

struct _Move { };

constexpr inline auto Copy = _Copy {};

constexpr inline auto Move = _Move {};
