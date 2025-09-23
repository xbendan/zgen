#pragma once

#include <sdk-meta/callable.h>
#include <sdk-meta/panic.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>
#include <sdk-meta/utility.h>

namespace Meta {

template <typename T>
struct Opt {

    using Inner = T;

    union {
        Empty _none;
        T     _value;
    };
    bool _present { false };

    always_inline constexpr Opt() : _none(), _present(false) { }

    always_inline constexpr Opt(Empty) : _none(), _present(false) { }

    template <typename U = T>
        requires(not Same<RemoveCvRef<U>, Opt<T>> and CopyConstructible<T, U>)
    always_inline constexpr Opt(U const& value)
        : _value(value),
          _present(true) { }

    template <typename U = T>
        requires(not Same<RemoveCvRef<U>, Opt<T>> and MoveConstructible<T, U>)
    always_inline constexpr Opt(U&& value)
        : _value(static_cast<U&&>(value)),
          _present(true) { }

    always_inline constexpr Opt(Opt const& other)
        requires(Meta::CopyConstructible<T>)
        : _present(other._present) {
        if (_present)
            new (&_value) T(other._value);
    }

    always_inline constexpr Opt(Opt&& other)
        requires(Meta::MoveConstructible<T>)
        : _present(other._present) {
        if (_present)
            new (&_value) T(Meta::forward<T>(other._value));
    }

    template <typename U>
    always_inline constexpr Opt(Opt<U> const& other)
        requires(Meta::CopyConstructible<T, U>)
        : _present(other._present) {
        if (_present)
            new (&_value) T(other.unwrap());
    }

    template <typename U>
        requires(Meta::MoveConstructible<T, U>)
    always_inline constexpr Opt(Opt<U>&& other) : _present(other._present) {
        if (_present)
            new (&_value) T(Meta::forward<U>(other.unwrap()));
    }

    ~Opt() { clear(); }

    always_inline constexpr Opt& operator=(Empty) {
        clear();
        return *this;
    }

    template <typename U = T>
        requires(not Meta::Same<Meta::RemoveCvRef<U>, Opt<T>>
                 and Meta::Convertible<U, T>)
    always_inline constexpr Opt& operator=(U const& value) {
        clear();
        _present = true;
        new (&_value) T(value);
        return *this;
    }

    template <typename U = T>
        requires(not Meta::Same<Meta::RemoveCvRef<U>, Opt<T>>
                 and Meta::MoveConstructible<T, U>)
    always_inline constexpr Opt& operator=(U&& value) {
        clear();
        _present = true;
        new (&_value) T(::forward<U>(value));
        return *this;
    }

    always_inline constexpr Opt& operator=(Opt const& other)
        requires(Meta::CopyConstructible<T>)
    {
        *this = Opt(other);
        return *this;
    }

    always_inline constexpr Opt& operator=(Opt&& other)
        requires(Meta::MoveConstructible<T>)
    {
        clear();
        if (other._present) {
            _present = true;
            new (&_value) T(Meta::forward<T>(other.take()));
        }
        return *this;
    }

    template <typename U = T>
    always_inline constexpr Opt& operator=(Opt<U> const& other)
        requires(Meta::CopyConstructible<T, U>)
    {
        *this = Opt(other);
        return *this;
    }

    template <typename U>
    always_inline constexpr Opt& operator=(Opt<U>&& other)
        requires(Meta::MoveConstructible<T, U>)
    {
        clear();
        if (other._present) {
            _present = true;
            new (&_value) T(::forward<U>(other.take()));
        }
        return *this;
    }

    always_inline constexpr explicit operator bool() const noexcept {
        return _present;
    }

    always_inline constexpr T* operator->() {
        if (not _present) [[unlikely]] {
            panic("Opt::operator-> on Empty");
        }
        return &_value;
    }

    always_inline constexpr T const* operator->() const {
        if (not _present) [[unlikely]] {
            panic("Opt::operator->: on Empty");
        }
        return &_value;
    }

    always_inline constexpr T& operator*() {
        if (not _present) [[unlikely]] {
            panic("Opt::operator*: on Empty");
        }
        return _value;
    }

    always_inline constexpr T const& operator*() const {
        if (not _present) [[unlikely]] {
            panic("Opt::operator*: on Empty");
        }
        return _value;
    }

    template <typename... Args>
    always_inline constexpr T& emplace(Args&&... args) {
        if (_present) {
            _value.~T();
        }
        new (&_value) T(Meta::forward<Args>(args)...);
        _present = true;
        return _value;
    }

    always_inline constexpr void clear() {
        if (_present) {
            _value.~T();
            _present = false;
        }
    }

    always_inline constexpr T& unwrap(char const* msg
                                      = "Opt::unwrap(): on Empty") {
        if (not _present) [[unlikely]] {
            panic(msg);
        }
        return _value;
    }

    always_inline constexpr T const&
    unwrap(char const* msg = "Opt::unwrap(): unwrap on Empty") const {
        if (not _present) [[unlikely]] {
            panic(msg);
        }
        return _value;
    }

    always_inline constexpr T const& unwrapOrElse(T const& def) const {
        if (_present) {
            return _value;
        }
        return def;
    }

    always_inline constexpr T unwrapOrElse(Callable<T> auto&& func) const {
        if (_present) {
            return _value;
        }
        return func();
    }

    always_inline constexpr T take(char const* msg
                                   = "Opt::take(): take on Empty") {
        if (not _present) [[unlikely]] {
            panic(msg);
        }
        T value = move(_value);
        _value.~T();
        _present = false;
        return value;
    }

    always_inline constexpr auto mapTo(Callable<T> auto&& func)
        -> Opt<decltype(func(unwrap()))> {
        if (not _present) {
            return Empty {};
        }
        return func(unwrap());
    }

#define mapTo$(expr) mapTo([&](auto x) { return expr; })

    always_inline constexpr bool operator==(Empty) const {
        return not _present;
    }

    template <typename U>
        requires Equatable<T, U>
    always_inline constexpr bool operator==(U const& other) const {
        if (not _present) {
            return false;
        }
        return _value == other;
    }

    static constexpr Empty none() { return Empty {}; }
};

} // namespace Meta

using Meta::Opt;
