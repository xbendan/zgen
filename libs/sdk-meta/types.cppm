module;

#include <compare>
#include <initializer_list>

export module sdk:types;

export using u8  = unsigned char;
export using u16 = unsigned short;
export using u32 = unsigned int;
export using u64 = unsigned long long;
#if defined(__SIZEOF_INT128__)
export using u128 = unsigned __int128;
#endif

export using i8  = signed char;
export using i16 = signed short;
export using i32 = signed int;
export using i64 = signed long long;
#if defined(__SIZEOF_INT128__)
export using i128 = signed __int128;
#endif

export using byte  = u8;
export using word  = u16;
export using dword = u32;
export using qword = u64;

export using usize = __SIZE_TYPE__;
export using isize = __PTRDIFF_TYPE__;

export using f32 = float;
export using f64 = double;
#if defined(__SIZEOF_INT128__)
export using f128 = long double;
#endif

export using nullptr_t = decltype(nullptr);

export struct None {
    constexpr None() = default;

    explicit constexpr operator bool() const noexcept { return false; }

    template <class T>
    constexpr operator T const*() const noexcept {
        return nullptr;
    }

    constexpr bool operator==(None const&) const = default;
    constexpr bool operator!=(None const&) const = default;
};

export constexpr inline None NONE {};

export template <class T>
constexpr bool operator==(None, T const* ptr) {
    return ptr == nullptr;
}

export struct Copy { };

export struct Move { };

export constexpr inline auto COPY = Copy {};
export constexpr inline auto MOVE = Move {};

export template <typename T>
using Items = std::initializer_list<T>;

export template <usize... Ns>
struct Indices {
    static constexpr usize size() { return sizeof...(Ns); }
};

export template <usize N, usize... Ns>
struct _makeIndices : _makeIndices<N - 1, N - 1, Ns...> { };

export template <usize... Ns>
struct _makeIndices<0, Ns...> {
    using Type = Indices<Ns...>;
};

export template <usize N>
using makeIndices = typename _makeIndices<N>::Type;

export [[gnu::always_inline]] inline void* operator new(
    [[maybe_unused]] usize size,
    void*                  p) noexcept {
    return p;
}

export [[gnu::always_inline]] inline void operator delete(void*,
                                                          void*) noexcept {
    // do nothing
}

export [[noreturn]] void panic(char const* message);
