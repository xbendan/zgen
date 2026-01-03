module;

export module sdk:iter;

import :math;
import :opt;
import :traits;
import :types;

export template <typename T, typename R = typename T::R>
concept Iter = requires(T& it) {
    typename T::R;
    typename T::V;

    { it != NONE } -> Boolean;
    { ++it } -> Same<T&>;
    { *it } -> Same<R>;
    { *::declval<R>() } -> Same<typename T::V&>;
};

enum class Iteration {
    Forward,
    Backward,
    Bidirectional,
    RandomAccess,
};

template <typename T>
struct Arrays {
    using E = T;

    E const* _begin;
    E const* _end;

    [[gnu::always_inline]] constexpr Arrays(E const* begin, E const* end)
        : _begin(begin),
          _end(end) { }

    [[gnu::always_inline]] constexpr usize size() const {
        return static_cast<usize>(_end - _begin);
    }

    template <Iteration It>
    struct _;

    template <>
    struct _<Iteration::Forward> {
        using R = Opt<T&>;
        using V = T&;

        T*       _curr;
        T const* _end;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != _end;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            ++_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_curr == _end) {
                return NONE;
            }
            T& ref = *_curr;
            return R(ref);
        }
    };

    template <>
    struct _<Iteration::Backward> {
        using R = Opt<T&>;
        using V = T&;

        T*       _curr;
        T const* _begin;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != _begin;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            --_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_curr == _begin) {
                return NONE;
            }
            return *(_curr - 1);
        }
    };

    template <>
    struct _<Iteration::Bidirectional> {
        using R = Opt<T&>;
        using V = T&;

        T*       _curr;
        T const* _begin;
        T const* _end;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != _end;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            ++_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr _& operator--() {
            --_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_curr == _end or _curr == _begin) {
                return NONE;
            }
            return *_curr;
        }
    };

    [[gnu::always_inline]] constexpr auto begin() {
        return _<Iteration::Forward> { const_cast<T*>(_begin), _end };
    }

    [[gnu::always_inline]] constexpr auto end() -> None { return NONE; }

    [[gnu::always_inline]] constexpr auto rbegin() {
        return _<Iteration::Backward> { const_cast<T*>(_end), _begin };
    }

    [[gnu::always_inline]] constexpr auto rend() -> None { return NONE; }
};
