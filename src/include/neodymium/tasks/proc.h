#pragma once

#include <neodymium/hal/vmm.h>
#include <sdk-meta/list.h>
#include <sdk-meta/rc.h>
#include <sdk-text/str.h>

namespace Sys::Sched {

struct Thread {
    enum State {
        Running,
        Ready,
        Blocked,
        Sleeping,
        Stopped,
        Zombie
    } state;

    struct Blocker { };
};

struct Process final {
    u32           processId;
    u32           parent;
    Str           name;
    Thread const& mainThread;
    List<Thread*> childrenThreads;
    Rc<Hal::Vmm>  vmm;
};

} // namespace Sys::Sched
