#pragma once

#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

namespace Meta {

struct [[gnu::packed]] uflat {
    [[gnu::always_inline]] constexpr uflat() = default;

    [[gnu::always_inline]] constexpr uflat(u64 ptr) : m_ptr(ptr) { }

    template <typename T>
    [[gnu::always_inline]] constexpr uflat(T* ptr)
        : m_ptr(reinterpret_cast<u64>(ptr)) { }

    [[gnu::always_inline]] constexpr uflat(uflat const& other)
        : m_ptr(other.m_ptr) { }

    [[gnu::always_inline]] constexpr uflat(uflat&& other) : m_ptr(other.m_ptr) {
        other.m_ptr = 0ull;
    }

    [[gnu::always_inline]] constexpr uflat& operator=(uflat const& other) {
        m_ptr = other.m_ptr;
        return *this;
    }

    [[gnu::always_inline]] constexpr uflat& operator=(uflat&& other) {
        m_ptr       = other.m_ptr;
        other.m_ptr = 0ull;
        return *this;
    }

    [[gnu::always_inline]] constexpr uflat& operator=(void* ptr) {
        m_ptr = (u64) ptr;
        return *this;
    }

    [[gnu::always_inline]] constexpr uflat& operator=(nullptr_t) {
        m_ptr = 0;
        return *this;
    }

    [[gnu::always_inline]] constexpr uflat& operator=(u64 ptr) {
        m_ptr = ptr;
        return *this;
    }

    [[gnu::always_inline]] constexpr operator u64() const { return m_ptr; }

    [[gnu::always_inline]] constexpr operator void*() const {
        return reinterpret_cast<void*>(m_ptr);
    }

    [[gnu::always_inline]] constexpr uflat& operator+=(u64 off) {
        m_ptr += off;
        return *this;
    }

    [[gnu::always_inline]] constexpr uflat& operator-=(u64 off) {
        m_ptr -= off;
        return *this;
    }

    [[gnu::always_inline]] constexpr uflat& operator*=(u64 off) {
        m_ptr *= off;
        return *this;
    }

    [[gnu::always_inline]] constexpr uflat& operator/=(u64 off) {
        m_ptr /= off;
        return *this;
    }

    [[gnu::always_inline]] constexpr auto operator<=>(
        Meta::Convertible<u64> auto other) const {
        return m_ptr <=> u64(other);
    }

    // [[gnu::always_inline]] constexpr auto operator<=>(u64 other) const {
    //     return m_ptr <=> other;
    // }

    [[gnu::always_inline]] constexpr u64 operator*() { return m_ptr; }

    [[gnu::always_inline]] constexpr u64 operator*() const { return m_ptr; }

    template <typename T>
    [[gnu::always_inline]] constexpr operator T*() const {
        return reinterpret_cast<T*>(m_ptr);
    }

    u64 m_ptr;
};
static_assert(sizeof(uflat) == sizeof(void*), "uflat size mismatch");

} // namespace Meta

using Meta::uflat;
