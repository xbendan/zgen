#pragma once

#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

namespace Meta {

template <typename T>
constexpr T&& forward(Meta::RemoveRef<T>& t) noexcept {
    return static_cast<T&&>(t);
}

template <typename T>
constexpr T&& forward(Meta::RemoveRef<T>&& t) noexcept {
    return static_cast<T&&>(t);
}

template <typename T>
constexpr Meta::RemoveRef<T>&& move(T&& t) noexcept {
    return static_cast<Meta::RemoveRef<T>&&>(t);
}

template <typename T>
constexpr void swap(T& a, T& b) noexcept {
    T temp(a);
    a = b;
    b = temp;
}

template <typename T, typename U = T>
constexpr T exchange(T& obj, U&& nv) noexcept(Meta::MoveConstructible<T>) {
    T old = move(obj);
    obj   = forward<U>(nv);
    return old;
}

} // namespace Meta

using Meta::exchange;
using Meta::forward;
using Meta::move;
using Meta::swap;
