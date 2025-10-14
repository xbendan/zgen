#pragma once

#include <sdk-meta/range.h>
#include <sdk-meta/types.h>
#include <sdk-meta/uuid.h>
#include <sdk-text/str.h>
#include <zgen/io/fs.h>

namespace Zgen::Core::Io {

struct Volume {
    Str    name;
    Uuid   uuid;
    Rc<Fs> fs;

    Range<usize, struct _LbaRangeTag> range;
};

} // namespace Zgen::Core::Io
