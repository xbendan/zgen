#pragma once

#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

namespace Meta {

struct [[packed]] Flat {
    u64 _ptr;

    constexpr Flat() = default;
    constexpr Flat(Meta::Convertible<u64> auto ptr)
        : _ptr(static_cast<u64>(ptr)) { }

    [[clang::always_inline]] constexpr Flat(Flat const& other)
        : _ptr(other._ptr) { }
    [[clang::always_inline]] constexpr Flat(Flat&& other) = delete;

    [[clang::always_inline]] constexpr Flat& operator=(Flat const& other) {
        _ptr = other._ptr;
        return *this;
    }

    [[clang::always_inline]] constexpr Flat& operator=(Flat&& other) = delete;

    [[clang::always_inline]] constexpr Flat& operator=(nullptr_t) {
        _ptr = 0;
        return *this;
    }

    [[clang::always_inline]] constexpr Flat& operator=(
        Meta::Convertible<u64> auto ptr) {
        _ptr = static_cast<u64>(ptr);
        return *this;
    }

    [[clang::always_inline]] constexpr operator u64() const { return _ptr; }

    [[clang::always_inline]] constexpr auto operator<=>(
        Meta::Convertible<u64> auto other) const {
        return _ptr <=> u64(other);
    }

    [[clang::always_inline]] constexpr bool operator==(
        Meta::Convertible<u64> auto other) const {
        return _ptr == u64(other);
    }

    [[clang::always_inline]] constexpr Flat& operator+=(u64 off) {
        _ptr += off;
        return *this;
    }

    [[clang::always_inline]] constexpr Flat& operator-=(u64 off) {
        _ptr -= off;
        return *this;
    }
};

struct [[packed]] uflat {
    [[gnu::always_inline]] constexpr uflat() = default;

    [[gnu::always_inline]] constexpr uflat(Meta::Convertible<u64> auto ptr)
        : m_ptr(ptr) { }

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

    [[gnu::always_inline]] constexpr bool operator==(
        Meta::Convertible<u64> auto other) const {
        return m_ptr == u64(other);
    }

    [[gnu::always_inline]] constexpr auto operator<=>(uflat other) const {
        return m_ptr <=> other.m_ptr;
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

    [[gnu::always_inline]] constexpr u64 operator*() { return m_ptr; }

    [[gnu::always_inline]] constexpr u64 operator*() const { return m_ptr; }

    [[gnu::always_inline]] constexpr operator u64() const { return m_ptr; }

    template <typename T>
    [[gnu::always_inline]] constexpr operator T*() const {
        return reinterpret_cast<T*>(m_ptr);
    }

    template <typename T>
    [[gnu::always_inline]] constexpr T* as() const {
        return reinterpret_cast<T*>(m_ptr);
    }

    u64 m_ptr;
};
static_assert(sizeof(uflat) == sizeof(void*), "uflat size mismatch");

} // namespace Meta

using Meta::uflat;
