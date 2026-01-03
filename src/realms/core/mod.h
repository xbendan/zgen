#pragma once

#include <realms/core/registry.h>
#include <sdk-meta/res.h>
#include <sdk-text/str.h>

namespace Realms::Sys {

struct Mod {
    using Registry = Registry<Str, Mod>;

    virtual ~Mod()           = default;
    virtual Res<> onLoad()   = 0;
    virtual Res<> onUnload() = 0;
};

template <>
struct Metadata<Mod> {
    Str        name;
    usize      priority;
    Slice<Str> requisites;
    Opt<Rc<Mod>> (*build)();
};

} // namespace Realms::Sys
