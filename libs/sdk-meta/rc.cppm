module;

export module sdk:rc;

import :cursor;
import :hash;
import :id;
import :limits;
import :lock;
import :manual;
import :opt;
import :types;

/// A reference-counted object heap cell.
export template <typename L>
struct _Cell {
    i32                     _strong = 0;
    i32                     _weak   = 0;
    [[no_unique_address]] L _lock;

    virtual ~_Cell() = default;

    virtual void* _unwrap() = 0;

    virtual void clear() = 0;

    virtual Id id() = 0;

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

    Id id() override { return idOf<T>(); }

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

    constexpr _Rc(Move, _Cell<L>* ptr) : _cell(ptr->refStrong()) { }

    constexpr _Rc(_Rc const& other) : _cell(other._cell->refStrong()) { }

    constexpr _Rc(_Rc&& other) : _cell(exchange(other._cell, nullptr)) { }

    template <Extends<T> U>
    constexpr _Rc(_Rc<L, U> const& other) : _cell(other._cell->refStrong()) { }

    template <Extends<T> U>
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

    template <Extends<T> U>
    constexpr U const& unwrap() const {
        ensure();
        if (not is<U>()) [[unlikely]]
            panic("unwrapping T as U");

        return _cell->template unwrap<U>();
    }

    template <Extends<T> U>
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

        if (not Same<T, U>
            and not Extends<T, U>
            and not(_cell->id() == idOf<U>())) {
            return nullptr;
        }

        return &_cell->template unwrap<U>();
    }

    template <typename U>
    constexpr Cursor<U> is() const {
        if (not _cell)
            return nullptr;

        if (not Same<T, U>
            and not Extends<T, U>
            and not(_cell->id() == idOf<U>())) {
            return nullptr;
        }

        return &_cell->template unwrap<U>();
    }

    Id id() const {
        ensure();
        return _cell->id();
    }

    u64 hash() const { return ::hash(unwrap()); }

    template <typename U>
    constexpr Opt<_Rc<L, U>> cast() {
        if (not is<U>()) {
            return NONE;
        }

        return _Rc<L, U>(MOVE, _cell);
    }

    template <typename UL, Comparable<T> U>
    auto operator<=>(_Rc<UL, U> const& other) const
        requires Comparable<T>
    {
        if (_cell == other._cell)
            return _cell <=> other._cell;
        return unwrap() <=> other.unwrap();
    }

    template <typename UL, Equatable<T> U>
    bool operator==(_Rc<UL, U> const& other) const
        requires Equatable<T>
    {
        if (_cell == other._cell)
            return true;
        return unwrap() == other.unwrap();
    }

    auto operator<=>(Comparable<T> auto const& other) const {
        return unwrap() <=> other;
    }

    bool operator==(Equatable<T> auto const& other) const {
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

    template <Extends<T> U>
    constexpr _Weak(_Rc<L, U> const& other) : _cell(other._cell->refWeak()) { }

    template <Extends<T> U>
    constexpr _Weak(_Weak<L, U> const& other)
        : _cell(other._cell->refWeak()) { }

    template <Extends<T> U>
    constexpr _Weak(_Weak<L, U>&& other)
        : _cell(exchange(other._cell, nullptr)) { }

    constexpr _Weak(Move, _Cell<L>* ptr) : _cell(ptr->refWeak()) { }

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
            return None {};
        return _Rc<L, T>(MOVE, _cell);
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
    return { MOVE, new Cell<Unlock, T>(forward<Args>(args)...) };
}

template <typename T, typename... Args>
constexpr static Arc<T> makeArc(Args&&... args) {
    return { MOVE, new Cell<Lock, T>(forward<Args>(args)...) };
}

template <typename T>
struct RefCounted {
    friend T;

    u32 mutable _refCount;

    [[gnu::always_inline]] void ref() const {
        if (!(_refCount > 0) or (_refCount + 1) == Limits<u32>::MAX)
            [[unlikely]] {
            panic("RefCounted::ref(): ref count overflow");
        }
        _refCount++;
    }

    [[gnu::always_inline]] bool unref() const {
        auto* that = const_cast<T*>(static_cast<T const*>(this));

        auto count = --that->_refCount;
        if (count == 0) {
            if constexpr (requires { that->destroy(); })
                that->destroy();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfree-nonheap-object"
            delete static_cast<T const*>(this);
#pragma GCC diagnostic pop
            return true;
        }
        return false;
    }

    [[gnu::always_inline]] u32 refCount() const { return _refCount; }

protected:
    RefCounted() : _refCount(1) { }
    ~RefCounted() {
        if (_refCount != 0) [[unlikely]] {
            panic(
                "RefCounted::~RefCounted(): destroying object with non-zero "
                "ref count");
        }
    }
};

template <typename T>
struct AtomicRefCounted {
    friend T;

    Atomic<u32> mutable _refCount;

    void ref() const {
        if (!(_refCount > 0) or (_refCount + 1) == Limits<u32>::MAX)
            [[unlikely]] {
            panic("AtomicRefCounted::ref(): ref count overflow");
        }

        _refCount.fetchAdd(1, MemoryOrder::Relaxed);
    }

    bool unref() const {
        auto* that = const_cast<T*>(static_cast<T const*>(this));

        auto count
            = that->_refCount.fetchSub(1, MemoryOrder::AcquireRelease) - 1;
        if (count == 0) {
            if constexpr (requires { that->destroy(); })
                that->destroy();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfree-nonheap-object"
            delete static_cast<T const*>(this);
#pragma GCC diagnostic pop
            return true;
        }
        return false;
    }

    u32 refCount() const { return _refCount.load(MemoryOrder::Relaxed); }

protected:
    AtomicRefCounted() : _refCount(1) { }
    ~AtomicRefCounted() {
        if (_refCount.load(MemoryOrder::Relaxed) != 0) [[unlikely]] {
            panic(
                "AtomicRefCounted::~AtomicRefCounted(): destroying object with "
                "non-zero ref count");
        }
    }
};
