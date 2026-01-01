#pragma once

#include <sdk-meta/decl.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/utility.h>

namespace Meta {

namespace _ {
template <typename>
inline constexpr bool Func = false;

template <typename Ret, typename... Args>
inline constexpr bool Func<Ret(Args...)> = true;

template <typename Ret, typename... Args>
inline constexpr bool Func<Ret(Args..., ...)> = true;

} // namespace _

template <typename T>
concept Func = _::Func<T>;

template <typename T>
concept FuncPtr = Ptr<T> and Func<RemovePtr<T>>;

template <typename U, typename... Args>
using Ret = decltype(::declval<U>()(Meta::forward<Args>(::declval<Args>())...));

template <typename F, typename... Args>
concept Callable = requires(F f) { f(declval<Args>()...); };

template <typename F, typename... Args>
concept Predicate = Callable<F, Args...> and Boolean<Ret<F, Args...>>;

template <typename F, typename... Args>
concept Action = Callable<F, Args...> and Void<Ret<F, Args...>>;

} // namespace Meta
