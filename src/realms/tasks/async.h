#pragma once

#include <realms/tasks/future.h>
#include <sdk-meta/callable.h>
#include <sdk-meta/res.h>
#include <sdk-meta/time.h>
#include <sdk-meta/types.h>

namespace Realms::Sys {

struct CancellationToken {
    bool _cancelled = false;

    [[gnu::always_inline]] constexpr operator bool() const {
        return _cancelled;
    }

    [[gnu::always_inline]] constexpr void cancel() { _cancelled = true; }
};

template <typename Fn>
struct Task {
    enum struct Status {
        Ready,
        Running,
        Blocked,
        Completed,
        Cancelled,
        Faulted
    };

    Fn     _runnable;
    Status _status;
    u32    _id;

    Res<decltype(auto)> continueWith(auto f) {
        if (_status != Status::Ready) {
            // Error: Task not ready
            return Error::invalidState("Task not ready");
        }
        auto n = Task([=, _runnable = ::move(_runnable)]() {
            return f(_runnable());
        }, Status::Ready, _id);
        return Ok(n);
    }

    Res<decltype(auto)> delay(u32 ms) {
        if (_status != Status::Ready) {
            // Error: Task not ready
            return Error::invalidState("Task not ready");
        }
        auto n = Task([=, _runnable = ::move(_runnable)]() {
            // Sleep for ms milliseconds
            auto result = _runnable();
            // TODO: handle sleep interruption
            return result;
        }, Status::Ready, _id);
        return Ok(n);
    }

    auto whenAll(auto... tasks);

    auto whenAny(auto... tasks);

    void wait();

    bool waitTimeout(u32 ms);

    bool waitTimeout(TimeSpan span);

    void waitAll(Task... tasks);

    void waitAny(Task... tasks);

    Future<Ret<Fn>> start();
};

} // namespace Realms::Sys
