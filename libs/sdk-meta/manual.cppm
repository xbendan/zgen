module;

export module sdk:manual;

import :traits;
import :types;
import :utility;

// Manual is a type that allows manual construction and destruction of an object
// of type T. It provides a way to manage the lifetime of an object without
// using dynamic memory allocation.

export template <typename T>
struct Manual {
    alignas(alignof(T)) byte _inner[sizeof(T)];

    template <typename... Args>
    [[gnu::always_inline]] void ctor(Args&&... args) {
        new (&unwrap()) T(forward<Args>(args)...);
    }

    [[gnu::always_inline]] void dtor() { unwrap().~T(); }

    [[gnu::always_inline]] T& unwrap() {
        return *reinterpret_cast<T*>(&_inner);
    }

    [[gnu::always_inline]] T const& unwrap() const {
        return *reinterpret_cast<T const*>(&_inner);
    }

    [[gnu::always_inline]] T take() {
        T v = move(unwrap());
        dtor();
        return v;
    }

    template <typename... Args>
    [[gnu::always_inline]] T* operator()(Args&&... args) {
        new (&unwrap()) T(forward<Args>(args)...);

        return &unwrap();
    }

    T* operator->() { return &unwrap(); }

    T const* operator->() const { return &unwrap(); }

    T& operator*() { return unwrap(); }

    T const& operator*() const { return unwrap(); }
};
static_assert(Trivial<Manual<isize>>);
static_assert(sizeof(Manual<isize>) == sizeof(isize));
static_assert(alignof(Manual<isize>) == alignof(isize));
