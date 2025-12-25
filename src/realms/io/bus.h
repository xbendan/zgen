#pragma once

#include <realms/io/dev.h>
#include <realms/io/drv.h>
#include <sdk-meta/rc.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>
#include <sdk-text/str.h>

namespace Realms::Core::Io {

struct Bus {
    Str       _name;
    Vec<Drv*> _drivers = {};

    Bus(Str name, Vec<Drv*> drvs = { 8 })
        : _name(name),
          _drivers(move(drvs)) { }

    virtual Res<> onInit() = 0;

    virtual Res<String> path(Rc<Dev> dev) = 0;

    virtual Res<Slice<Rc<Dev>>> probe() = 0;

    virtual Res<Slice<Rc<Dev>>> devices() = 0;

    virtual Res<> remove(Rc<Dev> dev) = 0;
};

} // namespace Realms::Core::Io
