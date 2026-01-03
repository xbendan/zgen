module;

export module sdk:flat;

import :traits;
import :types;

struct [[gnu::packed]] Flat {
    u64 _ptr;

    constexpr Flat() = default;
    constexpr Flat(Convertible<u64> auto ptr) : _ptr(static_cast<u64>(ptr)) { }

    [[gnu::always_inline]] constexpr Flat(Flat const& other)
        : _ptr(other._ptr) { }
    [[gnu::always_inline]] constexpr Flat(Flat&& other) = delete;

    [[gnu::always_inline]] constexpr Flat& operator=(Flat const& other) {
        _ptr = other._ptr;
        return *this;
    }

    [[gnu::always_inline]] constexpr Flat& operator=(Flat&& other) = delete;

    [[gnu::always_inline]] constexpr Flat& operator=(nullptr_t) {
        _ptr = 0;
        return *this;
    }

    [[gnu::always_inline]] constexpr Flat& operator=(
        Convertible<u64> auto ptr) {
        _ptr = static_cast<u64>(ptr);
        return *this;
    }

    [[gnu::always_inline]] constexpr operator u64() const { return _ptr; }

    [[gnu::always_inline]] constexpr auto operator<=>(
        Convertible<u64> auto other) const {
        return _ptr <=> u64(other);
    }

    [[gnu::always_inline]] constexpr bool operator==(
        Convertible<u64> auto other) const {
        return _ptr == u64(other);
    }

    [[gnu::always_inline]] constexpr Flat& operator+=(u64 off) {
        _ptr += off;
        return *this;
    }

    [[gnu::always_inline]] constexpr Flat& operator-=(u64 off) {
        _ptr -= off;
        return *this;
    }
};

struct [[gnu::packed]] uflat {
    [[gnu::always_inline]] constexpr uflat() = default;

    [[gnu::always_inline]] constexpr uflat(Convertible<u64> auto ptr)
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
        Convertible<u64> auto other) const {
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
