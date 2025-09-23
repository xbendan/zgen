#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/iter.h>
#include <sdk-meta/types.h>
#include <sdk-meta/vec.h>

namespace Sys {

struct PrekernelInfo {
    u64   magic;
    char* agentName;
    char* agentVersion;
    i64   timestamp;
    i64   startupTime;

    struct _MemmapEntry {
        u64 base;
        u64 size;
        enum : u32 {
            Usable      = 0,
            Reserved    = 1,
            Reclaimable = 2,
            AcpiNvs     = 3,
            BadMemory   = 4,
            Framebuffer = 5,
        } type;
    };

    InlineVec<struct _MemmapEntry, 64> memmap;
};

inline PrekernelInfo prekernel;

} // namespace Sys
