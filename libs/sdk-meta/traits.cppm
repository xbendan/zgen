module;

export module sdk:traits;

import :types;
import :decl;
import :cond;

export template <typename T, typename... Args>
concept Constructible = requires { ::new T(declval<Args>()...); };

export template <typename Extendsd, typename Base>
concept Extends = __is_base_of(Base, Extendsd);

export template <typename T>
concept DefaultConstructible = Constructible<T>;

export template <typename T, typename U = T>
concept CopyConstructible = Constructible<T, U const&>;

export template <typename T, typename U = T>
concept MoveConstructible = Constructible<T, U&&>;

export template <typename From, typename To>
concept Convertible = requires { declval<void (*)(To)>()(declval<From>()); };

export template <typename T, typename U>
inline constexpr bool _Same = false;

export template <typename T>
inline constexpr bool _Same<T, T> = true;

export template <typename T, typename U>
concept Same = _Same<T, U>;

export template <typename T>
concept Trivial = __is_trivial(T);

export template <typename T>
concept StandardLayout = __is_standard_layout(T);

export template <typename T>
concept Pod = Trivial<T> and StandardLayout<T>;

export template <typename T>
concept TrivialyCopyable = __is_trivially_copyable(T);

export template <typename T, typename... Ts>
concept Contains = (Same<T, Ts> or ...);

export template <typename T>
constexpr inline bool _Integral = requires(T t, T* p, void (*f)(T)) {
    reinterpret_cast<T>(t);
    f(0);
    p + t;
};

export template <typename T>
concept Integral = _Integral<T>;

export template <typename T>
inline constexpr bool _Boolean = false;

export template <>
inline constexpr bool _Boolean<bool> = true;

export template <typename T>
concept Boolean = _Boolean<T>;

export template <typename T>
concept Float = Contains<T, f32, f64, f128>;

export template <typename T>
concept Arithmetic = Integral<T> or Float<T>;

export template <typename T>
concept Void = Same<T, void>;

/// A type is comparable if it can be compared using the <=> operator.
/// Comparable does not imply Equatable.
export template <typename T, typename U = T>
concept Comparable = requires(T const& a, U const& b) {
    { a < b } -> Boolean;
    { a > b } -> Boolean;
    { a <= b } -> Boolean;
    { a >= b } -> Boolean;
};

/// A type is equatable if it can be compared for equality.
export template <typename T, typename U = T>
concept Equatable = requires(T const& a, U const& b) {
    { a == b } -> Same<decltype(a == b)>;
    { a != b } -> Same<decltype(a != b)>;
};

export template <typename T, typename U = T>
concept Computable = requires(T const& a, U const& b) {
    { a + b } -> Same<decltype(a + b)>;
    { a - b } -> Same<decltype(a - b)>;
    { a * b } -> Same<decltype(a * b)>;
    { a / b } -> Same<decltype(a / b)>;
    { a % b } -> Same<decltype(a % b)>;
};

export template <typename Tp, typename T, typename U = T>
concept Comparator = requires(Tp t, T const& a, U const& b) {
    { t(a, b) } -> Same<bool>;
};

namespace _ {

export template <typename T>
constexpr inline bool Const = false;

export template <typename T>
constexpr inline bool Const<T const> = true;

export template <typename T>
constexpr inline bool Volatile = false;

export template <typename T>
constexpr inline bool Volatile<T volatile> = true;

export template <typename T>
struct RemoveConst {
    using Type = T;
};

export template <typename T>
struct RemoveConst<T const> {
    using Type = T;
};

export template <typename T>
struct RemoveVolatile {
    using Type = T;
};

export template <typename T>
struct RemoveVolatile<T volatile> {
    using Type = T;
};
} // namespace _

export template <typename T>
concept Const = _::Const<T>;

export template <typename T>
concept Volatile = _::Volatile<T>;

export template <typename T>
concept Cv = Const<T> or Volatile<T>;

export template <typename T>
using RemoveConst = typename _::RemoveConst<T>::Type;

export template <typename T>
using RemoveVolatile = typename _::RemoveVolatile<T>::Type;

export template <typename T>
using RemoveCv = RemoveConst<RemoveVolatile<T>>;

export template <typename Src, typename Dest>
using CopyConst = Cond<Const<Src>, Dest const, RemoveConst<Dest>>;

namespace _ {

export template <typename T>
inline constexpr bool Ptr = false;

export template <typename T>
inline constexpr bool Ptr<T*> = true;

export template <typename T>
inline constexpr bool Field = false;

export template <typename T, typename U>
inline constexpr bool Field<T U::*> = true;

export template <typename T>
struct RemovePtr {
    using Type = T;
};

export template <typename T>
struct RemovePtr<T*> {
    using Type = T;
};

} // namespace _

export template <typename T>
concept Ptr = _::Ptr<T>;

export template <typename T>
concept Field = _::Field<T>;

export template <typename T>
using RemovePtr = typename _::RemovePtr<RemoveCv<T>>::Type;

export template <typename>
inline constexpr bool _LvalueRef = false;

export template <typename T>
inline constexpr bool _LvalueRef<T&> = true;

export template <typename T>
concept LvalueRef = _LvalueRef<T>;

export template <typename>
inline constexpr bool _RvalueRef = false;

export template <typename T>
inline constexpr bool _RvalueRef<T&&> = true;

export template <typename T>
concept RvalueRef = _RvalueRef<T>;

export template <typename T>
struct _RemoveRef {
    using Type = T;
};

export template <typename T>
struct _RemoveRef<T&> {
    using Type = T;
};

export template <typename T>
struct _RemoveRef<T&&> {
    using Type = T;
};

export template <typename T>
using RemoveRef = typename _RemoveRef<T>::Type;

export template <typename T>
using RemoveCvRef = RemoveCv<RemoveRef<T>>;

export template <typename T>
concept Signed = Comparable<T> and (static_cast<T>(-1) < static_cast<T>(0));

export template <typename T>
concept Unsigned = Comparable<T> and (static_cast<T>(0) < static_cast<T>(-1));

export template <typename T>
concept Class = __is_class(T);

export template <typename T>
concept Enum = __is_enum(T);

namespace _ {
void test(...);
void test(int) = delete;
} // namespace _

export template <typename T>
concept ScopedEnum = Enum<T> and requires { _::test(T {}); };

export template <typename T>
concept Fundamental = Arithmetic<T> or Void<T>;

export template <typename T>
concept Scalar = Fundamental<T> or Ptr<T> or Field<T> or Enum<T>;

export template <typename T>
using UnderlyingType = __underlying_type(T);

export template <usize Start, typename T, typename... Ts>
    requires(Contains<T, Ts...>)
consteval usize _indexOf() {
    if constexpr (Same<T, Ts...[Start]>)
        return Start;
    else if constexpr (sizeof...(Ts) == 0)
        return -1;
    else
        return _indexOf<Start + 1, T, Ts...>();
}

export template <typename T, typename... Ts>
    requires(Contains<T, Ts...>)
consteval usize indexOf() {
    return _indexOf<0, T, Ts...>();
}

export template <typename...>
struct _IndexCast;

export template <typename Data, typename T>
struct _IndexCast<Data, T> {
    [[gnu::always_inline]] static inline auto eval(usize,
                                                   Data* ptr,
                                                   auto  func) {
        using U = CopyConst<Data, T>;
        return func(*reinterpret_cast<U*>(ptr));
    }
};

export template <typename Data, typename T, typename... Ts>
struct _IndexCast<Data, T, Ts...> {
    [[gnu::always_inline]] static inline auto eval(usize index,
                                                   Data* ptr,
                                                   auto  func) {
        using U = CopyConst<Data, T>;

        return index == 0 ? func(*reinterpret_cast<U*>(ptr))
                          : _IndexCast<Data, Ts...>::eval(index - 1, ptr, func);
    }
};

export template <typename... Ts>
[[gnu::always_inline]] inline auto indexCast(usize index,
                                             auto* ptr,
                                             auto  func) {
    return _IndexCast<RemoveRef<decltype(*ptr)>, Ts...>::eval(index, ptr, func);
}

namespace _ {
export template <typename>
inline constexpr bool Func = false;

export template <typename Ret, typename... Args>
inline constexpr bool Func<Ret(Args...)> = true;

export template <typename Ret, typename... Args>
inline constexpr bool Func<Ret(Args..., ...)> = true;

} // namespace _

export template <typename T>
concept Func = _::Func<T>;

export template <typename T>
concept FuncPtr = Ptr<T> and Func<RemovePtr<T>>;

export template <typename U, typename... Args>
using Ret = decltype(::declval<U>()(forward<Args>(::declval<Args>())...));

export template <typename F, typename... Args>
concept Callable = requires(F f) { f(declval<Args>()...); };

export template <typename F, typename... Args>
concept Predicate = Callable<F, Args...> and Boolean<Ret<F, Args...>>;

export template <typename F, typename... Args>
concept Action = Callable<F, Args...> and Void<Ret<F, Args...>>;

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
