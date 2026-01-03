module;

export module sdk:defer;

import :types;
import :traits;

/// Defer a computation until the end of the current scope.
/// The computation is executed in the reverse order of the defer calls.
export template <typename F>
struct Defer : Pinned {
    F _f;

    [[gnu::always_inline]] Defer(F f) : _f(f) { }

    [[gnu::always_inline]] ~Defer() { _f(); }
};

export template <typename F>
Defer(F) -> Defer<F>;

/// Defer a computation until the end of the current scope.
/// The computation is executed in the reverse order of the defer calls.
/// Can be disarm which will prevent the computation from being executed.
export template <typename F>
struct ArmedDefer : Uncopyable {
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

export template <typename F>
ArmedDefer(F) -> ArmedDefer<F>;
