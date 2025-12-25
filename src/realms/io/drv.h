#pragma once

#include <realms/io/dev.h>
#include <sdk-meta/rc.h>
#include <sdk-meta/res.h>
#include <sdk-text/str.h>

namespace Realms::Core::Io {

struct Drv {
    Str name;

    virtual Res<bool> match(Rc<Dev> dev) = 0;

    virtual Res<> onInit(Rc<Dev> dev) = 0;

    virtual Res<> onRemove(Rc<Dev> dev) = 0;

    virtual Res<> onSuspend(Rc<Dev> dev) = 0;

    virtual Res<> onResume(Rc<Dev> dev) = 0;
};

} // namespace Realms::Core::Io
