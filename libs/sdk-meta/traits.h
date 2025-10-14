#pragma once

#include <sdk-meta/cond.h>
#include <sdk-meta/decl.h>
#include <sdk-meta/types.h>

namespace Meta {

template <typename Derived, typename Base>
concept Derive = __is_base_of(Base, Derived);

template <typename T, typename... Args>
concept Constructible = requires { ::new T(declval<Args>()...); };

template <typename T>
concept DefaultConstructible = Constructible<T>;

template <typename T, typename U = T>
concept CopyConstructible = Constructible<T, U const&>;

template <typename T, typename U = T>
concept MoveConstructible = Constructible<T, U&&>;

template <typename From, typename To>
concept Convertible = requires { declval<void (*)(To)>()(declval<From>()); };

template <typename T, typename U>
inline constexpr bool _Same = false;

template <typename T>
inline constexpr bool _Same<T, T> = true;

template <typename T, typename U>
concept Same = _Same<T, U>;

/// A type is comparable if it can be compared using the <=> operator.
/// Comparable does not imply Equatable.
template <typename T, typename U = T>
concept Comparable = requires(T const& a, U const& b) {
    { a <=> b } -> Same<decltype(a <=> b)>;
};

/// A type is equatable if it can be compared for equality.
template <typename T, typename U = T>
concept Equatable = requires(T const& a, U const& b) {
    { a == b } -> Same<decltype(a == b)>;
};

template <typename T, typename U = T>
concept Computable = requires(T const& a, U const& b) {
    { a + b } -> Same<decltype(a + b)>;
    { a - b } -> Same<decltype(a - b)>;
    { a * b } -> Same<decltype(a * b)>;
    { a / b } -> Same<decltype(a / b)>;
    { a % b } -> Same<decltype(a % b)>;
};

template <typename Tp, typename T, typename U = T>
concept Comparator = requires(Tp t, T const& a, U const& b) {
    { t(a, b) } -> Meta::Same<bool>;
};

template <typename T, typename... Ts>
concept Contains = (Same<T, Ts> or ...);

namespace _ {

template <typename T>
constexpr inline bool Const = false;

template <typename T>
constexpr inline bool Const<T const> = true;

template <typename T>
constexpr inline bool Volatile = false;

template <typename T>
constexpr inline bool Volatile<T volatile> = true;

template <typename T>
struct RemoveConst {
    using Type = T;
};

template <typename T>
struct RemoveConst<T const> {
    using Type = T;
};

template <typename T>
struct RemoveVolatile {
    using Type = T;
};

template <typename T>
struct RemoveVolatile<T volatile> {
    using Type = T;
};
} // namespace _

template <typename T>
concept Const = _::Const<T>;

template <typename T>
concept Volatile = _::Volatile<T>;

template <typename T>
concept Cv = Const<T> or Volatile<T>;

template <typename T>
using RemoveConst = typename _::RemoveConst<T>::Type;

template <typename T>
using RemoveVolatile = typename _::RemoveVolatile<T>::Type;

template <typename T>
using RemoveCv = RemoveConst<RemoveVolatile<T>>;

template <typename Src, typename Dest>
using CopyConst = Cond<Const<Src>, Dest const, RemoveConst<Dest>>;

namespace _ {

template <typename T>
inline constexpr bool Ptr = false;

template <typename T>
inline constexpr bool Ptr<T*> = true;

template <typename T>
inline constexpr bool MemberPtr = false;

template <typename T, typename U>
inline constexpr bool MemberPtr<T U::*> = true;

template <typename T>
struct RemovePtr {
    using Type = T;
};

template <typename T>
struct RemovePtr<T*> {
    using Type = T;
};

} // namespace _

template <typename T>
concept Ptr = _::Ptr<T>;

template <typename T>
concept MemberPtr = _::MemberPtr<T>;

template <typename T>
using RemovePtr = typename _::RemovePtr<RemoveCv<T>>::Type;

template <typename>
inline constexpr bool _LvalueRef = false;

template <typename T>
inline constexpr bool _LvalueRef<T&> = true;

template <typename T>
concept LvalueRef = _LvalueRef<T>;

template <typename>
inline constexpr bool _RvalueRef = false;

template <typename T>
inline constexpr bool _RvalueRef<T&&> = true;

template <typename T>
concept RvalueRef = _RvalueRef<T>;

template <typename T>
struct _RemoveRef {
    using Type = T;
};

template <typename T>
struct _RemoveRef<T&> {
    using Type = T;
};

template <typename T>
struct _RemoveRef<T&&> {
    using Type = T;
};

template <typename T>
using RemoveRef = typename _RemoveRef<T>::Type;

template <typename T>
using RemoveCvRef = RemoveCv<RemoveRef<T>>;

template <typename T>
concept Trivial = __is_trivial(T);

template <typename T>
concept StandardLayout = __is_standard_layout(T);

template <typename T>
concept Pod = Trivial<T> and StandardLayout<T>;

template <typename T>
concept TrivialyCopyable = __is_trivially_copyable(T);

template <typename T>
concept Signed = Comparable<T> and (static_cast<T>(-1) < static_cast<T>(0));

template <typename T>
concept Unsigned = Comparable<T> and (static_cast<T>(0) < static_cast<T>(-1));

template <typename T>
concept Class = __is_class(T);

template <typename T>
concept Union = __is_union(T);

template <typename T>
concept Enum = __is_enum(T);

namespace _ {
void test(...);
void test(int) = delete;
} // namespace _

template <typename T>
concept ScopedEnum = Enum<T> and requires { _::test(T {}); };

template <typename T>
constexpr inline bool _Integral = requires(T t, T* p, void (*f)(T)) {
    reinterpret_cast<T>(t);
    f(0);
    p + t;
};

template <typename T>
concept Integral = _Integral<T>;

template <typename T>
inline constexpr bool _Boolean = false;

template <>
inline constexpr bool _Boolean<bool> = true;

template <typename T>
concept Boolean = _Boolean<T>;

template <typename T>
concept Float = Same<T, f32> or Same<T, f64> or Same<T, f128>;

template <typename T>
concept Arithmetic = Integral<T> or Float<T>;

// template <typename T>
// concept Array = __is_array(T);

template <typename T>
concept Void = Same<T, void>;

template <typename T>
concept Fundamental = Arithmetic<T> or Void<T>;

template <typename T>
concept Scalar = Fundamental<T> or Ptr<T> or MemberPtr<T> or Enum<T>;

template <typename T>
using UnderlyingType = __underlying_type(T);

template <usize Start, typename T, typename... Ts>
    requires(Contains<T, Ts...>)
consteval usize _indexOf() {
    if constexpr (Same<T, Ts...[Start]>)
        return Start;
    else if constexpr (sizeof...(Ts) == 0)
        return -1;
    else
        return _indexOf<Start + 1, T, Ts...>();
}

template <typename T, typename... Ts>
    requires(Contains<T, Ts...>)
consteval usize indexOf() {
    return _indexOf<0, T, Ts...>();
}

template <typename...>
struct _IndexCast;

template <typename Data, typename T>
struct _IndexCast<Data, T> {
    [[gnu::always_inline]] static inline auto eval(usize,
                                                   Data* ptr,
                                                   auto  func) {
        using U = CopyConst<Data, T>;
        return func(*reinterpret_cast<U*>(ptr));
    }
};

template <typename Data, typename T, typename... Ts>
struct _IndexCast<Data, T, Ts...> {
    [[gnu::always_inline]] static inline auto eval(usize index,
                                                   Data* ptr,
                                                   auto  func) {
        using U = CopyConst<Data, T>;

        return index == 0 ? func(*reinterpret_cast<U*>(ptr))
                          : _IndexCast<Data, Ts...>::eval(index - 1, ptr, func);
    }
};

template <typename... Ts>
[[gnu::always_inline]] inline auto indexCast(usize index,
                                             auto* ptr,
                                             auto  func) {
    return _IndexCast<RemoveRef<decltype(*ptr)>, Ts...>::eval(index, ptr, func);
}

struct Uncopyable {
    Uncopyable()                             = default;
    Uncopyable(Uncopyable const&)            = delete;
    Uncopyable& operator=(Uncopyable const&) = delete;
    Uncopyable(Uncopyable&&)                 = default;
    Uncopyable& operator=(Uncopyable&&)      = default;
};

struct Unmovable {
    Unmovable()                            = default;
    Unmovable(Unmovable const&)            = default;
    Unmovable& operator=(Unmovable const&) = default;
    Unmovable(Unmovable&&)                 = delete;
    Unmovable& operator=(Unmovable&&)      = delete;
};

struct Pinned {
    Pinned()                         = default;
    Pinned(Pinned const&)            = delete;
    Pinned& operator=(Pinned const&) = delete;
    Pinned(Pinned&&)                 = delete;
    Pinned& operator=(Pinned&&)      = delete;
};

} // namespace Meta
