#pragma once

#include <realms/io/fs.h>
#include <sdk-meta/range.h>
#include <sdk-meta/types.h>
#include <sdk-meta/uuid.h>
#include <sdk-text/str.h>

namespace Realms::Core::Io {

struct Volume {
    Str    name;
    Uuid   uuid;
    Rc<Fs> fs;

    Range<usize, struct _LbaRangeTag> range;
};

} // namespace Realms::Core::Io
