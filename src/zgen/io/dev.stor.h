#pragma once

#include <sdk-io/seek.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/rc.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>
#include <sdk-meta/vec.h>
#include <sdk-text/str.h>
#include <zgen/io/dev.h>

namespace Zgen::Core::Io {

using Sdk::Io::Seek;
using Sdk::Io::Whence;

struct StorDev : public Dev {
    usize _blockSize { 512 };
    usize _blockCount { 0 };

    virtual Res<> onInit() override;

    virtual Res<usize> read(Seek seek, Bytes& buf) = 0;

    virtual Res<usize> write(Seek seek, Bytes const& buf) = 0;
};

} // namespace Zgen::Core::Io
