module;

export module sdk:res;

import :error;
import :opt;
import :types;
import :_union;

export template <typename T = None>
struct Ok {
    T inner;

    template <typename... Args>
    [[gnu::always_inline]] constexpr Ok(Args&&... args)
        : inner(forward<Args>(args)...) { }

    [[gnu::always_inline]] explicit operator bool() const { return true; }

    [[gnu::always_inline]] auto operator<=>(Ok const&) const
        requires Comparable<T>
    = default;

    T take() { return move(inner); }
};

export template <typename... Args>
Ok(Args&&...) -> Ok<RemoveCvRef<Args>...>;

export template <typename V = None, typename E = Error>
struct [[nodiscard]] Res {
    using Inner = Union<Ok<V>, E>;

    Inner _inner;

    [[gnu::always_inline]] constexpr Res(Ok<V> const& ok) : _inner(ok) { }

    [[gnu::always_inline]] constexpr Res(Ok<V>&& ok) : _inner(move(ok)) { }

    template <typename U>
    [[gnu::always_inline]] constexpr Res(Ok<U> ok)
        : _inner(Ok<V> { ok.inner }) { }

    [[gnu::always_inline]] constexpr Res(E err) : _inner(err) { }

    [[gnu::always_inline]] constexpr Res(None)
        : _inner(Error::unknown("unexpected none")) { }

    template <typename U>
    [[gnu::always_inline]] constexpr Res(Res<U, E> other)
        : _inner(other._inner) { }

    [[gnu::always_inline]] constexpr explicit operator bool() const {
        return _inner.template is<Ok<V>>();
    }

    [[gnu::always_inline]] constexpr Opt<V> ok() const {
        if (_inner.template is<E>()) [[unlikely]]
            return None {};
        return _inner.template unwrap<Ok<V>>().inner;
    }

    [[gnu::always_inline]] constexpr Opt<E> err() const {
        if (not _inner.template is<E>()) [[unlikely]]
            return None {};
        return _inner.template unwrap<E>();
    }

    [[gnu::always_inline]] constexpr bool has() const {
        return _inner.template is<Ok<V>>();
    }

    [[gnu::always_inline]] constexpr E const& none() const {
        if (not _inner.template is<E>()) [[unlikely]]
            panic("Res<V, E>::none: called on an ok");

        return _inner.template unwrap<E>();
    }

    [[gnu::always_inline]] constexpr V& unwrap(
        char const* msg = "Res<V, E>::unwrap: unwraping an error") {
        if (not _inner.template is<Ok<V>>()) [[unlikely]]
            panic(msg);

        return _inner.template unwrap<Ok<V>>().inner;
    }

    [[gnu::always_inline]] constexpr V const& unwrap(
        char const* msg = "Res<V, E>::unwrap: unwraping an error") const {
        if (not _inner.template is<Ok<V>>()) [[unlikely]]
            panic(msg);

        return _inner.template unwrap<Ok<V>>().inner;
    }

    [[gnu::always_inline]] constexpr V const& unwrapOr(V const& other) const {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>().inner;
        return other;
    }

    [[gnu::always_inline]] constexpr V unwrapOrDefault(V other) const {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>().inner;
        return other;
    }

    [[gnu::always_inline]] constexpr V unwrapOrElse(auto f) const {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>().inner;
        return f();
    }

    [[gnu::always_inline]] constexpr V take(
        char const* msg = "Res<V, E>::take: called on an error") {
        if (not _inner.template is<Ok<V>>()) [[unlikely]]
            panic(msg);

        return _inner.template unwrap<Ok<V>>().take();
    }

    template <typename U>
    [[gnu::always_inline]] constexpr Res<V, U> mapErr(auto f) {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>();
        return f(_inner.template unwrap<E>());
    }

    template <typename U>
    [[gnu::always_inline]] constexpr Res<V, U> mapErr() {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>();
        return U {};
    }

    template <typename U>
    [[gnu::always_inline]] constexpr Res<U, E> map(auto f) {
        if (_inner.template is<Ok<V>>())
            return Ok(f(_inner.template unwrap<Ok<V>>().inner));
        return _inner.template unwrap<E>();
    }

    template <typename U>
    [[gnu::always_inline]] constexpr Res<U, E> map() {
        if (_inner.template is<Ok<V>>())
            return Ok(_inner.template unwrap<Ok<V>>().inner);
        return _inner.template unwrap<E>();
    }

    [[gnu::always_inline]] auto operator<=>(Res const&) const
        requires Comparable<Inner>
    = default;

    [[gnu::always_inline]] bool operator==(bool b) const { return has() == b; }
};
