#pragma once

#include <realms/hal/vmm.h>
#include <sdk-meta/list.h>
#include <sdk-meta/rc.h>
#include <sdk-meta/types.h>
#include <sdk-text/str.h>

namespace Realms::Core {

struct Process;

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

    Process const& _process;
    u32            _id;
    u8             _priority;
};

struct Process final {
    Str           _name;
    u32           _id;
    u32           _parent;
    Thread const& _mainThread;
    List<Thread*> _childrenThreads;
    Rc<Hal::Vmm>  _vmm;

    struct {
        uflat entry;
        uflat stack;
        uflat heap;
    } _program;
};

} // namespace Realms::Core
