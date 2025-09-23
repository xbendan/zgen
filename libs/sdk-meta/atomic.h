#pragma once

#include <sdk-meta/types.h>
#include <sdk-meta/utility.h>

enum MemoryOrder : u8 {
    Relaxed                = 0,
    Acquire                = 1,
    Release                = 2,
    AcquireRelease         = 3,
    SequentiallyConsistent = 4
};

inline void placeMemoryBarrier(MemoryOrder order = SequentiallyConsistent) {
    __atomic_thread_fence(order);
}

template <typename T>
struct Atomic {
    T _val {};

    always_inline constexpr Atomic() = default;

    always_inline constexpr ~Atomic() = default;

    always_inline Atomic(T const& val) : _val(val) { }

    always_inline Atomic(T&& val) : _val(move(val)) { }

    always_inline T xchg(T           desired,
                         MemoryOrder order = SequentiallyConsistent) {
        return __atomic_exchange_n(&_val, desired, order);
    }

    always_inline bool cmpxchg(T expected, T desired,
                               MemoryOrder order = SequentiallyConsistent) {
        if (order == AcquireRelease or order == Relaxed)
            return __atomic_compare_exchange_n(
                &_val, &expected, desired, false, Relaxed, Acquire);

        return __atomic_compare_exchange_n(
            &_val, &expected, desired, false, order, order);
    }

    always_inline T fetchAdd(T           desired,
                             MemoryOrder order = SequentiallyConsistent) {
        return __atomic_fetch_add(&_val, desired, order);
    }

    always_inline T fetchSub(T           desired,
                             MemoryOrder order = SequentiallyConsistent) {
        return __atomic_fetch_sub(&_val, desired, order);
    }

    always_inline T fetchInc(MemoryOrder order = SequentiallyConsistent) {
        return __atomic_fetch_add(&_val, 1, order);
    }

    always_inline void inc(MemoryOrder order = SequentiallyConsistent) {
        __atomic_add_fetch(&_val, 1, order);
    }

    always_inline void dec(MemoryOrder order = SequentiallyConsistent) {
        __atomic_sub_fetch(&_val, 1, order);
    }

    always_inline T fetchDec(MemoryOrder order = SequentiallyConsistent) {
        return __atomic_fetch_sub(&_val, 1, order);
    }

    always_inline T load(MemoryOrder order = SequentiallyConsistent) {
        return __atomic_load_n(&_val, order);
    }

    always_inline void store(T           desired,
                             MemoryOrder order = SequentiallyConsistent) {
        __atomic_store_n(&_val, desired, order);
    }

    always_inline bool lockFree() { return __atomic_is_lock_free(&_val); }
};

