module;

export module sdk:utility;

import :traits;
import :types;

export template <typename T>
constexpr T&& forward(RemoveRef<T>& t) noexcept {
    return static_cast<T&&>(t);
}

export template <typename T>
constexpr T&& forward(RemoveRef<T>&& t) noexcept {
    return static_cast<T&&>(t);
}

export template <typename T>
constexpr RemoveRef<T>&& move(T&& t) noexcept {
    return static_cast<RemoveRef<T>&&>(t);
}

export template <typename T>
constexpr void swap(T& a, T& b) noexcept {
    T temp(a);
    a = b;
    b = temp;
}

export template <typename T, typename U = T>
constexpr T exchange(T& obj, U&& nv) noexcept(MoveConstructible<T>) {
    T old = move(obj);
    obj   = forward<U>(nv);
    return old;
}
