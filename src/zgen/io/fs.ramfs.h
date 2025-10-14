#pragma once

#include <sdk-meta/bits.h>
#include <zgen/io/fs.h>

namespace Zgen::Core::Io {

struct RamFs : public Fs {
    Vec<uflat> blocks;
    Bits       bits;
};

} // namespace Zgen::Core::Io
