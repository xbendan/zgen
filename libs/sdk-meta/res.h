#pragma once

#include <sdk-meta/error.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/std.h>
#include <sdk-meta/types.h>
#include <sdk-meta/union.h>

template <typename T = Empty>
struct Ok {
    T inner;

    template <typename... Args>
    [[gnu::always_inline]] constexpr Ok(Args&&... args)
        : inner(forward<Args>(args)...) { }

    [[gnu::always_inline]] explicit operator bool() const { return true; }

    [[gnu::always_inline]] auto operator<=>(Ok const&) const
        requires Meta::Comparable<T>
    = default;

    T take() { return move(inner); }
};

template <typename... Args>
Ok(Args&&...) -> Ok<Meta::RemoveCvRef<Args>...>;

template <typename V = Empty, typename E = Error>
struct [[nodiscard]] Res {
    using Inner = Union<Ok<V>, E>;

    Inner _inner;

    [[gnu::always_inline]] constexpr Res(Ok<V> const& ok) : _inner(ok) { }

    [[gnu::always_inline]] constexpr Res(Ok<V>&& ok) : _inner(move(ok)) { }

    template <typename U>
    [[gnu::always_inline]] constexpr Res(Ok<U> ok)
        : _inner(Ok<V> { ok.inner }) { }

    [[gnu::always_inline]] constexpr Res(E err) : _inner(err) { }

    [[gnu::always_inline]] constexpr Res(Empty)
        : _inner(Error::unknown("unexpected none")) { }

    template <typename U>
    [[gnu::always_inline]] constexpr Res(Res<U, E> other)
        : _inner(other._inner) { }

    [[gnu::always_inline]] constexpr explicit operator bool() const {
        return _inner.template is<Ok<V>>();
    }

    [[gnu::always_inline]] constexpr Opt<V> ok() const {
        if (_inner.template is<E>()) [[unlikely]]
            return Empty {};
        return _inner.template unwrap<Ok<V>>().inner;
    }

    [[gnu::always_inline]] constexpr Opt<E> err() const {
        if (not _inner.template is<E>()) [[unlikely]]
            return Empty {};
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
        requires Meta::Comparable<Inner>
    = default;

    [[gnu::always_inline]] bool operator==(bool b) const { return has() == b; }
};

// MARK: Try -------------------------------------------------------------------

// Give us a symbole to break one when debbuging error handling.
// This is a no-op in release mode.
#if defined(TRY_FAIL_REDIRECT)
extern "C" void __try_failed();
#    define __tryFail() __try_failed()
#else
#    define __tryFail() /* NOP */
#endif

#define __try$(EXPR, RET, AWAIT, FINAL)                                        \
    ({                                                                         \
        auto __expr = AWAIT(EXPR);                                             \
        if (not static_cast<bool>(__expr)) [[unlikely]] {                      \
            __tryFail();                                                       \
            RET __expr.none();                                                 \
            FINAL;                                                             \
        }                                                                      \
        __expr.take();                                                         \
    })

// clang-format off
#define try$(EXPR)      __try$(EXPR,    return,          ,          )
#define co_try$(EXPR)   __try$(EXPR, co_return,          ,          )
#define co_trya$(EXPR)  __try$(EXPR, co_return,  co_await,          )
#define co_tryy$(EXPR)  __try$(EXPR,  co_yield,          , co_return)
#define co_tryya$(EXPR) __try$(EXPR,  co_yield,  co_await, co_return)
// clang-format on
