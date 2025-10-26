#pragma once

#include <sdk-meta/callable.h>
#include <sdk-meta/panic.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>
#include <sdk-meta/utility.h>

namespace Meta {

template <typename>
struct Opt;

// MARK: - Reference specialization

template <typename T>
    requires(Meta::Same<Meta::RemoveCvRef<T>&, T>)
struct Opt<T> {
    using Inner = Meta::RemoveCvRef<T>*;

    union {
        None  _none;
        Inner _value;
    };
    bool _present { false };

    [[gnu::always_inline]] constexpr Opt() : _none(), _present(false) { }

    [[gnu::always_inline]] constexpr Opt(None) : _none(), _present(false) { }

    template <typename U = T>
        requires(not Same<RemoveCvRef<U>, Opt<T>> and Convertible<U, T>)
    [[gnu::always_inline]] constexpr Opt(U const& value)
        : _value(&value),
          _present(true) { }

    template <typename U = Inner>
        requires(not Same<RemoveCvRef<U>, Opt<T>> and Convertible<U, Inner>)
    [[gnu::always_inline]] constexpr Opt(U value)
        : _value(value),
          _present(true) { }

    [[gnu::always_inline]] constexpr Opt(Opt const& other)
        : _present(other._present) {
        if (_present)
            _value = other._value;
    }

    template <typename U>
    [[gnu::always_inline]] constexpr Opt(Opt<U> const& other)
        requires(Convertible<U, T>)
        : _present(other._present) {
        if (_present)
            new (&_value) T(other.unwrap());
    }

    // template <typename U>
    //     requires(Meta::MoveConstructible<T, U>)
    // [[gnu::always_inline]] constexpr Opt(Opt<U>&& other) : _present(other._present) {
    //     if (_present)
    //         new (&_value) T(Meta::forward<U>(other.unwrap()));
    // }

    ~Opt() { clear(); }

    [[gnu::always_inline]] constexpr Opt& operator=(None) {
        clear();
        return *this;
    }

    template <typename U = T>
        requires(not Meta::Same<Meta::RemoveCvRef<U>, Opt<T>>
                 and Meta::Convertible<U, T>)
    [[gnu::always_inline]] constexpr Opt& operator=(U const& value) {
        clear();
        _present = true;
        _value   = &value;
        return *this;
    }

    // template <typename U = T>
    //     requires(not Meta::Same<Meta::RemoveCvRef<U>, Opt<T>>
    //              and Meta::MoveConstructible<T, U>)
    // [[gnu::always_inline]] constexpr Opt& operator=(U&& value) {
    //     clear();
    //     _present = true;
    //     new (&_value) T(::forward<U>(value));
    //     return *this;
    // }

    [[gnu::always_inline]] constexpr Opt& operator=(Opt const& other) {
        if (other._present) {
            _present = true;
            _value   = other._value;
        } else {
            clear();
        }

        return *this;
    }

    // [[gnu::always_inline]] constexpr Opt& operator=(Opt&& other)
    //     requires(Meta::MoveConstructible<T>)
    // {
    //     clear();
    //     if (other._present) {
    //         _present = true;
    //         new (&_value) T(Meta::forward<T>(other.take()));
    //     }
    //     return *this;
    // }

    template <typename U = T>
    [[gnu::always_inline]] constexpr Opt& operator=(Opt<U> const& other)
        requires(Meta::Convertible<U, T>)
    {
        *this = Opt(other);
        return *this;
    }

    // template <typename U>
    // [[gnu::always_inline]] constexpr Opt& operator=(Opt<U>&& other)
    //     requires(Meta::MoveConstructible<T, U>)
    // {
    //     clear();
    //     if (other._present) {
    //         _present = true;
    //         new (&_value) T(::forward<U>(other.take()));
    //     }
    //     return *this;
    // }

    [[gnu::always_inline]] constexpr explicit operator bool() const noexcept {
        return _present;
    }

    [[gnu::always_inline]] constexpr Meta::RemoveCvRef<T>* operator->() {
        if (not _present) [[unlikely]] {
            panic("Opt::operator-> on None");
        }
        return _value;
    }

    [[gnu::always_inline]] constexpr Meta::RemoveCvRef<T> const* operator->()
        const {
        if (not _present) [[unlikely]] {
            panic("Opt::operator->: on None");
        }
        return _value;
    }

    [[gnu::always_inline]] constexpr Meta::RemoveCvRef<T>& operator*() {
        if (not _present) [[unlikely]] {
            panic("Opt::operator*: on None");
        }
        return *_value;
    }

    [[gnu::always_inline]] constexpr Meta::RemoveCvRef<T> const& operator*()
        const {
        if (not _present) [[unlikely]] {
            panic("Opt::operator*: on None");
        }
        return *_value;
    }

    // template <typename... Args>
    // [[gnu::always_inline]] constexpr T& emplace(Args&&... args) {
    //     if (_present) {
    //         _value.~T();
    //     }
    //     new (&_value) T(Meta::forward<Args>(args)...);
    //     _present = true;
    //     return _value;
    // }

    [[gnu::always_inline]] constexpr void clear() {
        if (_present) {
            _value.~Inner();
            _present = false;
        }
    }

    [[gnu::always_inline]] constexpr T& unwrap(char const* msg
                                               = "Opt::unwrap(): on None") {
        if (not _present) [[unlikely]] {
            panic(msg);
        }
        return *_value;
    }

    [[gnu::always_inline]] constexpr T const& unwrap(
        char const* msg = "Opt::unwrap(): unwrap on None") const {
        if (not _present) [[unlikely]] {
            panic(msg);
        }
        return *_value;
    }

    [[gnu::always_inline]] constexpr T const& unwrapOrElse(T const& def) const {
        if (_present) {
            return *_value;
        }
        return def;
    }

    [[gnu::always_inline]] constexpr T unwrapOrElse(
        Callable<T> auto&& func) const {
        if (_present) {
            return *_value;
        }
        return func();
    }

    [[gnu::always_inline]] constexpr T take(char const* msg
                                            = "Opt::take(): take on None") {
        if (not _present) [[unlikely]] {
            panic(msg);
        }
        // T value = move(_value);
        // _value.~T();
        _present = false;
        return *_value;
    }

    [[gnu::always_inline]] constexpr auto mapTo(Callable<T> auto&& func)
        -> Opt<decltype(func(unwrap()))> {
        if (not _present) {
            return None {};
        }
        return func(unwrap());
    }

    template <typename U>
    [[gnu::always_inline]] constexpr auto mapTo() -> Opt<U>
        requires(Meta::Convertible<T, U>)
    {
        if (not _present) {
            return None {};
        }
        return static_cast<U>(unwrap());
    }

    // #define mapTo$(expr) mapTo([&](auto x) { return expr; })

    [[gnu::always_inline]] constexpr bool operator==(None) const {
        return not _present;
    }

    template <typename U>
        requires Equatable<T, U>
    [[gnu::always_inline]] constexpr bool operator==(U const& other) const {
        if (not _present) {
            return false;
        }
        return _value == other;
    }

    static constexpr None none() { return None {}; }
};

// MARK: - General case

template <typename T>
struct Opt {

    using Inner = T;

    union {
        None _none;
        T    _value;
    };
    bool _present { false };

    [[gnu::always_inline]] constexpr Opt() : _none(), _present(false) { }

    [[gnu::always_inline]] constexpr Opt(None) : _none(), _present(false) { }

    template <typename U = T>
        requires(not Same<RemoveCvRef<U>, Opt<T>> and CopyConstructible<T, U>)
    [[gnu::always_inline]] constexpr Opt(U const& value)
        : _value(value),
          _present(true) { }

    template <typename U = T>
        requires(not Same<RemoveCvRef<U>, Opt<T>> and MoveConstructible<T, U>)
    [[gnu::always_inline]] constexpr Opt(U&& value)
        : _value(static_cast<U&&>(value)),
          _present(true) { }

    [[gnu::always_inline]] constexpr Opt(Opt const& other)
        requires(Meta::CopyConstructible<T>)
        : _present(other._present) {
        if (_present)
            new (&_value) T(other._value);
    }

    [[gnu::always_inline]] constexpr Opt(Opt&& other)
        requires(Meta::MoveConstructible<T>)
        : _present(other._present) {
        if (_present)
            new (&_value) T(Meta::forward<T>(other._value));
    }

    template <typename U>
    [[gnu::always_inline]] constexpr Opt(Opt<U> const& other)
        requires(Meta::CopyConstructible<T, U>)
        : _present(other._present) {
        if (_present)
            new (&_value) T(other.unwrap());
    }

    template <typename U>
        requires(Meta::MoveConstructible<T, U>)
    [[gnu::always_inline]] constexpr Opt(Opt<U>&& other)
        : _present(other._present) {
        if (_present)
            new (&_value) T(Meta::forward<U>(other.unwrap()));
    }

    ~Opt() { clear(); }

    [[gnu::always_inline]] constexpr Opt& operator=(None) {
        clear();
        return *this;
    }

    template <typename U = T>
        requires(not Meta::Same<Meta::RemoveCvRef<U>, Opt<T>>
                 and Meta::Convertible<U, T>)
    [[gnu::always_inline]] constexpr Opt& operator=(U const& value) {
        clear();
        _present = true;
        new (&_value) T(value);
        return *this;
    }

    template <typename U = T>
        requires(not Meta::Same<Meta::RemoveCvRef<U>, Opt<T>>
                 and Meta::MoveConstructible<T, U>)
    [[gnu::always_inline]] constexpr Opt& operator=(U&& value) {
        clear();
        _present = true;
        new (&_value) T(::forward<U>(value));
        return *this;
    }

    [[gnu::always_inline]] constexpr Opt& operator=(Opt const& other)
        requires(Meta::CopyConstructible<T>)
    {
        *this = Opt(other);
        return *this;
    }

    [[gnu::always_inline]] constexpr Opt& operator=(Opt&& other)
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
    [[gnu::always_inline]] constexpr Opt& operator=(Opt<U> const& other)
        requires(Meta::CopyConstructible<T, U>)
    {
        *this = Opt(other);
        return *this;
    }

    template <typename U>
    [[gnu::always_inline]] constexpr Opt& operator=(Opt<U>&& other)
        requires(Meta::MoveConstructible<T, U>)
    {
        clear();
        if (other._present) {
            _present = true;
            new (&_value) T(::forward<U>(other.take()));
        }
        return *this;
    }

    [[gnu::always_inline]] constexpr explicit operator bool() const noexcept {
        return _present;
    }

    [[gnu::always_inline]] constexpr T* operator->() {
        if (not _present) [[unlikely]] {
            panic("Opt::operator-> on None");
        }
        return &_value;
    }

    [[gnu::always_inline]] constexpr T const* operator->() const {
        if (not _present) [[unlikely]] {
            panic("Opt::operator->: on None");
        }
        return &_value;
    }

    [[gnu::always_inline]] constexpr T& operator*() {
        if (not _present) [[unlikely]] {
            panic("Opt::operator*: on None");
        }
        return _value;
    }

    [[gnu::always_inline]] constexpr T const& operator*() const {
        if (not _present) [[unlikely]] {
            panic("Opt::operator*: on None");
        }
        return _value;
    }

    template <typename... Args>
    [[gnu::always_inline]] constexpr T& emplace(Args&&... args) {
        if (_present) {
            _value.~T();
        }
        new (&_value) T(Meta::forward<Args>(args)...);
        _present = true;
        return _value;
    }

    [[gnu::always_inline]] constexpr void clear() {
        if (_present) {
            _value.~T();
            _present = false;
        }
    }

    [[gnu::always_inline]] constexpr T& unwrap(char const* msg
                                               = "Opt::unwrap(): on None") {
        if (not _present) [[unlikely]] {
            panic(msg);
        }
        return _value;
    }

    [[gnu::always_inline]] constexpr T const& unwrap(
        char const* msg = "Opt::unwrap(): unwrap on None") const {
        if (not _present) [[unlikely]] {
            panic(msg);
        }
        return _value;
    }

    [[gnu::always_inline]] constexpr T const& unwrapOrElse(T const& def) const {
        if (_present) {
            return _value;
        }
        return def;
    }

    [[gnu::always_inline]] constexpr T unwrapOrElse(
        Callable<T> auto&& func) const {
        if (_present) {
            return _value;
        }
        return func();
    }

    [[gnu::always_inline]] constexpr T unwrapOrDefault(T other) const {
        if (_present) {
            return _value;
        }
        return other;
    }

    [[gnu::always_inline]] constexpr T take(char const* msg
                                            = "Opt::take(): take on None") {
        if (not _present) [[unlikely]] {
            panic(msg);
        }
        T value = move(_value);
        _value.~T();
        _present = false;
        return value;
    }

    [[gnu::always_inline]] constexpr auto mapTo(Callable<T> auto&& func)
        -> Opt<decltype(func(unwrap()))> {
        if (not _present) {
            return NONE;
        }
        return func(unwrap());
    }

#define mapTo$(expr) mapTo([&](auto it) { return expr; })

    template <typename U>
    [[gnu::always_inline]] constexpr auto mapTo() -> Opt<U>
        requires(Meta::Convertible<T, U>)
    {
        if (not _present) {
            return None {};
        }
        return static_cast<U>(unwrap());
    }

    [[gnu::always_inline]] constexpr void ifPresent(Callable<T> auto&& func) {
        if (_present) {
            func(_value);
        }
    }

    [[gnu::always_inline]] constexpr void ifPresent(
        Callable<T> auto&& func) const {
        if (_present) {
            func(_value);
        }
    }

#define ifPresent$(expr) ifPresent([&](auto it) { expr; })

    [[gnu::always_inline]] constexpr bool operator==(None) const {
        return not _present;
    }

    template <typename U>
        requires Equatable<T, U>
    [[gnu::always_inline]] constexpr bool operator==(U const& other) const {
        if (not _present) {
            return false;
        }
        return _value == other;
    }

    static constexpr None none() { return None {}; }
};

} // namespace Meta

using Meta::Opt;
