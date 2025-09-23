#pragma once

#include <neodymium/io/fs.h>
#include <sdk-meta/bits.h>

namespace Sys::Io {

struct RamFs : public Fs {
    Vec<FlatPtr> blocks;
    Bits         bits;
};

} // namespace Sys::Io
