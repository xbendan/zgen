#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/types.h>

namespace Mbr {

struct [[gnu::packed]] Entry {
    u8           status;
    Array<u8, 3> chsBegin;
    u8           type;
    Array<u8, 3> chsEnd;
    u32          lbaBegin;
    u32          lbaLength;
};

struct [[gnu::packed]] Table {
    u8    bootCode[440];
    u32   diskIdentifier;
    u16   __reserved__0;
    Entry entries[4];
    u16   signature;
};

} // namespace Mbr
