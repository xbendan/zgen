#pragma once

#include <sdk-meta/res.h>
#include <sdk-meta/types.h>

namespace Realms::Hal {

struct Smp {
    virtual Res<usize> count() = 0;

    virtual Res<> init() = 0;

    virtual Res<> pause(usize id) = 0;

    virtual Res<> boot(usize id, void* stack, void (*entry)()) = 0;
};

} // namespace Realms::Hal
