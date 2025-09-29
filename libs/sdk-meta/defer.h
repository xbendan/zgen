#pragma once

#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

/// Defer a computation until the end of the current scope.
/// The computation is executed in the reverse order of the defer calls.
template <typename F>
struct Defer : Meta::Pinned {
    F _f;

    [[gnu::always_inline]] Defer(F f) : _f(f) { }

    [[gnu::always_inline]] ~Defer() { _f(); }
};

template <typename F>
Defer(F) -> Defer<F>;

/// Defer a computation until the end of the current scope.
/// The computation is executed in the reverse order of the defer calls.
/// Can be disarm which will prevent the computation from being executed.
template <typename F>
struct ArmedDefer : Meta::Uncopyable {
    F    _f;
    bool _armed = true;

    [[gnu::always_inline]] ArmedDefer(F f) : _f(f) { }

    [[gnu::always_inline]] ~ArmedDefer() {
        if (_armed)
            _f();
    }

    [[gnu::always_inline]] void disarm() { _armed = false; }

    [[gnu::always_inline]] void arm() { _armed = true; }
};

template <typename F>
ArmedDefer(F) -> ArmedDefer<F>;
