#pragma once

#include "_embed.h"

#include <sdk-meta/atomic.h>
#include <sdk-meta/traits.h>

template <typename T>
concept Lockable = requires(T& t) {
    { t.tryAcquire() };
    { t.acquire() };
    { t.release() };
};

struct [[nodiscard]] CritScope : Meta::Pinned { };

struct Lock : Meta::Pinned {
    Atomic<bool> _lock { false };

    bool _tryAcquire() {
        bool result = _lock.cmpxchg(false, true);
        threadfence();

        if (not result) {
            // TODO: leave critical section
        }

        return result;
    }

    bool tryAcquire() {
        // TODO: enter critical section
        return _tryAcquire();
    }

    void acquire() {
        // TODO: enter critical section
        while (not _tryAcquire())
            ; // TODO: relaxe
    }

    void release() {
        threadfence();
        _lock.store(false);
        // TODO: leave critical section
    }
};

struct Unlock : private Meta::Pinned {
    bool tryAcquire() { return true; }

    void acquire() { }

    void release() { }
};

template <Lockable L = Lock>
struct LockScoped : private Meta::Pinned {
    L& _lock;

    [[gnu::always_inline]] LockScoped(L& lock) : _lock(lock) {
        _lock.acquire();
    }

    [[gnu::always_inline]] ~LockScoped() { _lock.release(); }
};

template <Lockable L>
LockScoped(L&) -> LockScoped<L>;

template <typename T, Lockable L = Lock>
struct LockProtected {
    L _lock;
    T _val;

    LockProtected() = default;

    template <typename... Args>
    LockProtected(Args&&... args) : _val(forward<Args>(args)...) { }

    auto with(auto&& f) {
        LockScoped lock(_lock);
        return f(_val);
    }
};

template <typename T, Lockable L = Lock>
LockProtected(T, L&) -> LockProtected<T, L>;

struct ReadWriteLock : Meta::Pinned {
    Atomic<i32> _state { 0 };

    Lock          _lock;
    Atomic<isize> _pendings {};
    isize         _readers {};
    isize         _writers {};

    void acquireRead() {
        _Embed::enterCritical();

        while (not tryAcquireRead()) {
            _Embed::relaxe();
            threadfence();
        }
    }

    bool tryAcquireRead() {
        LockScoped scope(_lock);

        if (_pendings.load())
            return false;

        if (_writers)
            return false;

        ++_readers;

        _Embed::enterCritical();
        return true;
    }

    void releaseRead() {
        LockScoped scope(_lock);
        --_readers;
        _Embed::leaveCritical();
    }

    void acquireWrite() {
        _Embed::enterCritical();

        _pendings.inc();

        while (not tryAcquireWrite()) {
            _Embed::relaxe();
            threadfence();
        }

        _pendings.dec();
    }

    bool tryAcquireWrite() {
        LockScoped scope(_lock);

        if (_readers)
            return false;

        if (_writers)
            return false;

        ++_writers;
        _Embed::enterCritical();
        return true;
    }

    void releaseWrite() {
        LockScoped scope(_lock);
        --_writers;
        _Embed::leaveCritical();
    }
};

struct [[nodiscard]] ReadLockScope : Meta::Pinned {

    ReadWriteLock& _lock;

    ReadLockScope(ReadWriteLock& lock) : _lock(lock) { _lock.acquireRead(); }

    ~ReadLockScope() { _lock.releaseRead(); }
};

struct [[nodiscard]] WriteLockScope : Meta::Pinned {

    ReadWriteLock& _lock;

    WriteLockScope(ReadWriteLock& lock) : _lock(lock) { _lock.acquireWrite(); }

    ~WriteLockScope() { _lock.releaseWrite(); }
};
