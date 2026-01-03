#pragma once

#include <sdk-meta/manual.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/types.h>

namespace Meta {

template <typename T>
struct Queue { };

template <typename T, usize Capacity>
struct CircularQueue {
    using E = T;

    Manual<T> _buf[Capacity];
    usize     _size;
    usize     _begin;

    CircularQueue() noexcept : _size(0), _begin(0) { }

    Opt<T&> enqueue(T const& value)
        requires(Meta::CopyConstructible<T>)
    {
        return enqueue(::move(value));
    }

    Opt<T&> enqueue(T&& value)
        requires(Meta::MoveConstructible<T>)
    {
        if (_size == Capacity) {
            return NONE;
        }

        usize index = (_begin + _size) % Capacity;
        _buf[index].ctor(move(value));
        _size++;
        return &_buf[index].unwrap();
    }

    template <typename... Args>
    Opt<T&> emplace(Args&&... args)
        requires(Meta::Constructible<T, Args...>)
    {
        if (_size == Capacity) {
            return NONE;
        }

        usize index = (_begin + _size) % Capacity;
        _buf[index].ctor(forward<Args>(args)...);
        _size++;
        return &_buf[index].unwrap();
    }

    Opt<T> dequeue() {
        if (_size == 0) {
            return NONE;
        }

        T v    = _buf[_begin].take();
        _begin = (_begin + 1) % Capacity;
        _size--;
        return v;
    }

    Opt<T&> peek() {
        if (_size == 0) {
            return NONE;
        }

        return _buf[_begin].unwrap();
    }
};

} // namespace Meta
