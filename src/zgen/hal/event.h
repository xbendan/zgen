#pragma once

#include <sdk-meta/rc.h>
#include <sdk-meta/ref.h>
#include <sdk-meta/res.h>

namespace Zgen::Hal {

struct Events {

    virtual Res<> fin() = 0;

    virtual Res<> pause() = 0;

    virtual Res<> unpause() = 0;
};

} // namespace Zgen::Hal
