#pragma once

#include <realms/io/fs.h>
#include <sdk-meta/bits.h>
#include <sdk-meta/ptr.h>

namespace Realms::Sys::Io {

struct RamFs : public Fs {
    Vec<uflat> blocks;
    Bits       bits;
};

} // namespace Realms::Sys::Io
