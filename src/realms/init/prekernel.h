#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/iter.h>
#include <sdk-meta/ptr.h>
#include <sdk-meta/range.h>
#include <sdk-meta/tuple.h>
#include <sdk-meta/types.h>
#include <sdk-meta/vec.h>

namespace Realms::Core {

struct PrekernelInfo {
    u64   magic;
    char* agentName;
    char* agentVersion;
    i64   timestamp;

    uflat offsetPhys;
    uflat offsetVirt;

    struct _MemmapEntry {
        Range<u64> range;
        enum : u32 {
            Usable      = 0,
            Reserved    = 1,
            Reclaimable = 2,
            TextOrData  = 3,
            AcpiNvs     = 4,
            BadMemory   = 5,
            Framebuffer = 6,
        } type;
    };

    InlineVec<_MemmapEntry, 64> memmap;
};

inline PrekernelInfo prekernel;

} // namespace Realms::Core
