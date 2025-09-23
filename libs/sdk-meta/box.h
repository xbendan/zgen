#pragma once

#include <sdk-meta/panic.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>
#include <sdk-meta/utility.h>

template <typename T>
struct Box {
    T* _ptr;

    constexpr Box() = delete;

    constexpr Box(_Move, T* ptr) : _ptr(ptr) { }

    constexpr Box(T const& v) : _ptr(new T(v)) { }

    constexpr Box(T&& v) : _ptr(new T(Meta::forward<T>(v))) { }

    template <Meta::Derive<T> U>
    constexpr Box(Box<U>&& other) : _ptr(other._ptr) {
        other._ptr = nullptr;
    }

    constexpr ~Box() {
        if (_ptr) {
            delete _ptr;
        }
        _ptr = nullptr;
    }

    constexpr Box& operator=(Box const& other) {
        *this = Box(other);
        return *this;
    }

    template <Meta::Derive<T> U>
    constexpr Box& operator=(Box<U>&& other) {
        if (this != (void*) &other) {
            if (_ptr) {
                delete _ptr;
            }
            _ptr       = other._ptr;
            other._ptr = nullptr;
        }
        return *this;
    }

    template <typename U>
        requires Meta::Equatable<T, U>
    constexpr bool operator==(U const& other) const {
        return unwrap() == other;
    }

    template <typename U>
        requires Meta::Comparable<T, U>
    constexpr auto operator<=>(U const& other) const {
        return unwrap() <=> other;
    }

    template <typename U>
        requires Meta::Equatable<T, U>
    constexpr bool operator==(Box<U> const& other) const {
        if (_ptr == other._ptr)
            return true;
        return unwrap() == other.unwrap();
    }

    template <typename U>
        requires Meta::Comparable<T, U>
    constexpr auto operator<=>(Box<U> const& other) const {
        return unwrap() <=> other.unwrap();
    }

    constexpr T* operator->() { return &unwrap(); }

    constexpr T& operator*() { return unwrap(); }

    constexpr T const* operator->() const { return &unwrap(); }

    constexpr T const& operator*() const { return unwrap(); }

    constexpr operator T&() { return unwrap(); }

    constexpr operator T const&() const { return unwrap(); }

    constexpr T const& unwrap() const {
        if (not _ptr) [[unlikely]]
            panic("Box<T>::unwrap(): deferencing moved from Box<T>");
        return *_ptr;
    }

    constexpr T& unwrap() {
        if (not _ptr) [[unlikely]]
            panic("Box<T>::unwrap(): deferencing moved from Box<T>");
        return *_ptr;
    }
};

template <typename T, typename... Args>
constexpr static Box<T> makeBox(Args... args) {
    return { Move, new T(::forward<Args>(args)...) };
}
