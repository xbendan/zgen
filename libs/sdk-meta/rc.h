#pragma once

#include <sdk-meta/cursor.h>
#include <sdk-meta/hash.h>
#include <sdk-meta/id.h>
#include <sdk-meta/lock.h>
#include <sdk-meta/manual.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/panic.h>
#include <sdk-meta/types.h>

/// A reference-counted object heap cell.
template <typename L>
struct _Cell {
    i32                     _strong = 0;
    i32                     _weak   = 0;
    [[no_unique_address]] L _lock;

    virtual ~_Cell() = default;

    virtual void* _unwrap() = 0;

    virtual void clear() = 0;

    virtual Meta::Id id() = 0;

    void collectAndRelease() {
        _lock.release();
        if (_strong == 0 and _weak == 0)
            delete this;
    }

    _Cell* refStrong() {
        LockScoped scope(_lock);

        _strong++;
        if (_strong < 0) [[unlikely]]
            panic("_Cell::refStrong() overflow");

        return this;
    }

    void derefStrong() {
        _lock.acquire();

        if (_strong == 1)
            clear();

        _strong--;
        if (_strong < 0) [[unlikely]]
            panic("_Cell::derefStrong() underflow");

        collectAndRelease();
    }

    _Cell* refWeak() {
        LockScoped scope(_lock);

        _weak++;
        if (_weak < 0) [[unlikely]]
            panic("_Cell::refWeak() overflow");

        return this;
    }

    void derefWeak() {
        _lock.acquire();

        _weak--;
        if (_weak < 0) [[unlikely]]
            panic("_Cell::derefWeak() underflow");

        collectAndRelease();
    }

    template <typename T>
    T& unwrap() {
        return *static_cast<T*>(_unwrap());
    }
};

template <typename L, typename T>
struct Cell : _Cell<L> {
    Manual<T> _buf {};

    template <typename... Args>
    Cell(Args&&... args) {
        _buf.ctor(forward<Args>(args)...);
    }

    void* _unwrap() override { return &_buf.unwrap(); }

    Meta::Id id() override { return Meta::idOf<T>(); }

    void clear() override { _buf.dtor(); }
};

/// A strong reference to an object of type  `T`.
///
/// A strong reference keeps the object alive as long as the
/// reference is in scope. When the reference goes out of scope
/// the object is deallocated if there are no other strong
/// references to it.
template <typename L, typename T>
struct _Rc {
    _Cell<L>* _cell {};

    // MARK: Rule of Five ------------------------------------------------------

    constexpr _Rc() = delete;

    constexpr _Rc(_Move, _Cell<L>* ptr) : _cell(ptr->refStrong()) { }

    constexpr _Rc(_Rc const& other) : _cell(other._cell->refStrong()) { }

    constexpr _Rc(_Rc&& other) : _cell(exchange(other._cell, nullptr)) { }

    template <Meta::Derive<T> U>
    constexpr _Rc(_Rc<L, U> const& other) : _cell(other._cell->refStrong()) { }

    template <Meta::Derive<T> U>
    constexpr _Rc(_Rc<L, U>&& other) : _cell(exchange(other._cell, nullptr)) { }

    constexpr ~_Rc() {
        if (_cell) {
            _cell->derefStrong();
            _cell = nullptr;
        }
    }

    constexpr _Rc& operator=(_Rc const& other) {
        *this = _Rc(other);
        return *this;
    }

    constexpr _Rc& operator=(_Rc&& other) {
        swap(_cell, other._cell);
        return *this;
    }

    // MARK: Operators ---------------------------------------------------------

    constexpr T const* operator->() const { return &unwrap(); }

    constexpr T* operator->() { return &unwrap(); }

    constexpr T const& operator*() const { return unwrap(); }

    constexpr T& operator*() { return unwrap(); }

    // MARK: Methods -----------------------------------------------------------

    /// Returns the number of strong references to the object.
    constexpr usize strong() const { return _cell ? _cell->_strong : 0; }

    /// Returns the number of weak references to the object.
    constexpr usize weak() const { return _cell ? _cell->_weak : 0; }

    /// Returns the total number of references to the object.
    constexpr usize refs() const { return strong() + weak(); }

    constexpr void ensure() const {
        if (not _cell) [[unlikely]]
            panic("null dereference");
    }

    constexpr T const& unwrap() const {
        ensure();
        return _cell->template unwrap<T>();
    }

    constexpr T& unwrap() {
        ensure();
        return _cell->template unwrap<T>();
    }

    template <Meta::Derive<T> U>
    constexpr U const& unwrap() const {
        ensure();
        if (not is<U>()) [[unlikely]]
            panic("unwrapping T as U");

        return _cell->template unwrap<U>();
    }

    template <Meta::Derive<T> U>
    constexpr U& unwrap() {
        ensure();
        if (not is<U>()) [[unlikely]]
            panic("unwrapping T as U");

        return _cell->template unwrap<U>();
    }

    template <typename U>
    constexpr Cursor<U> is() {
        if (not _cell)
            return nullptr;

        if (not Meta::Same<T, U>
            and not Meta::Derive<T, U>
            and not(_cell->id() == Meta::idOf<U>())) {
            return nullptr;
        }

        return &_cell->template unwrap<U>();
    }

    template <typename U>
    constexpr Cursor<U> is() const {
        if (not _cell)
            return nullptr;

        if (not Meta::Same<T, U>
            and not Meta::Derive<T, U>
            and not(_cell->id() == Meta::idOf<U>())) {
            return nullptr;
        }

        return &_cell->template unwrap<U>();
    }

    Meta::Id id() const {
        ensure();
        return _cell->id();
    }

    u64 hash() const { return ::hash(unwrap()); }

    template <typename U>
    constexpr Opt<_Rc<L, U>> cast() {
        if (not is<U>()) {
            return Empty {};
        }

        return _Rc<L, U>(Move, _cell);
    }

    template <typename UL, Meta::Comparable<T> U>
    auto operator<=>(_Rc<UL, U> const& other) const
        requires Meta::Comparable<T>
    {
        if (_cell == other._cell)
            return _cell <=> other._cell;
        return unwrap() <=> other.unwrap();
    }

    template <typename UL, Meta::Equatable<T> U>
    bool operator==(_Rc<UL, U> const& other) const
        requires Meta::Equatable<T>
    {
        if (_cell == other._cell)
            return true;
        return unwrap() == other.unwrap();
    }

    auto operator<=>(Meta::Comparable<T> auto const& other) const {
        return unwrap() <=> other;
    }

    bool operator==(Meta::Equatable<T> auto const& other) const {
        return unwrap() == other;
    }
};

/// A weak reference to a an object of type `T`.
///
/// A weak reference does not keep the object alive, but can be
/// upgraded to a strong reference if the object is still alive.
template <typename L, typename T>
struct _Weak {
    _Cell<L>* _cell;

    constexpr _Weak() = delete;

    template <Meta::Derive<T> U>
    constexpr _Weak(_Rc<L, U> const& other) : _cell(other._cell->refWeak()) { }

    template <Meta::Derive<T> U>
    constexpr _Weak(_Weak<L, U> const& other)
        : _cell(other._cell->refWeak()) { }

    template <Meta::Derive<T> U>
    constexpr _Weak(_Weak<L, U>&& other)
        : _cell(exchange(other._cell, nullptr)) { }

    constexpr _Weak(_Move, _Cell<L>* ptr) : _cell(ptr->refWeak()) { }

    constexpr _Weak& operator=(_Rc<L, T> const& other) {
        *this = _Weak(other);
        return *this;
    }

    constexpr _Weak& operator=(_Weak const& other) {
        *this = _Weak(other);
        return *this;
    }

    constexpr _Weak& operator=(_Weak&& other) {
        swap(_cell, other._cell);
        return *this;
    }

    constexpr ~_Weak() {
        if (_cell) {
            _cell->derefWeak();
            _cell = nullptr;
        }
    }

    /// Upgrades the weak reference to a strong reference.
    ///
    /// Returns `NONE` if the object has been deallocated.
    Opt<_Rc<L, T>> upgrade() const {
        if (not _cell or _cell->_strong == 0)
            return Empty {};
        return _Rc<L, T>(Move, _cell);
    }
};

template <typename T>
using Rc = _Rc<Unlock, T>;

template <typename T>
using WeakRc = _Weak<Unlock, T>;

template <typename T>
using Arc = _Rc<Lock, T>;

template <typename T>
using WeakArc = _Weak<Lock, T>;

/// Allocates an object of type `T` on the heap and returns
/// a strong reference to it.
template <typename T, typename... Args>
constexpr static Rc<T> makeRc(Args&&... args) {
    return { Move, new Cell<Unlock, T>(Meta::forward<Args>(args)...) };
}

template <typename T, typename... Args>
constexpr static Arc<T> makeArc(Args&&... args) {
    return { Move, new Cell<Lock, T>(Meta::forward<Args>(args)...) };
}
