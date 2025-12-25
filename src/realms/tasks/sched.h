#pragma once

#include <realms/tasks/proc.h>
#include <sdk-meta/array.h>
#include <sdk-meta/dict.h>
#include <sdk-meta/list.h>
#include <sdk-meta/lock.h>
#include <sdk-meta/rc.h>
#include <sdk-meta/types.h>

namespace Realms::Core {

struct ReadyQueue {
    virtual ~ReadyQueue() = default;

    virtual Res<usize> add(Arc<Thread> thread) = 0;

    virtual Res<usize> remove(Arc<Thread> thread) = 0;

    virtual Opt<Arc<Thread>> next() = 0;
};

struct Sched {
    static constexpr inline usize MaxPriority = 8;

    ReadyQueue const&      _readyQueue;
    Dict<u32, Arc<Thread>> _runnings;
    Dict<u32, Arc<Thread>> _pendings;

    Res<> schedule();
};
static_assert(sizeof(Sched) == 80 * 2 + 8);

struct MultiLevelFeedbackQueue : ReadyQueue {
    static constexpr inline usize MaxPriority = Sched::MaxPriority;

    Lock              _lock;
    List<Arc<Thread>> _priorityQueues[MaxPriority];
    List<Arc<Thread>> _blockedQueue;

    MultiLevelFeedbackQueue() = default;

    ~MultiLevelFeedbackQueue() = default;

    Res<usize> add(Arc<Thread> thread) override;

    Res<usize> remove(Arc<Thread> thread) override;

    Opt<Arc<Thread>> next() override;
};

} // namespace Realms::Core
