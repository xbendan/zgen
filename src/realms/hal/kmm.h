#pragma once

#include <sdk-meta/flags.h>
#include <sdk-meta/ptr.h>
#include <sdk-meta/range.h>
#include <sdk-meta/res.h>

namespace Realms::Sys {

enum struct KmmFlags {
    USER,
    KERNEL,
    CONTIGUOUS,
    NO_FAIL, // Repeatly try to allocate until success
    NO_WAIT, // No interrupt or sleep during allocation
    NO_RETRY // Abort immediately if first allocation fails
};
MakeFlags$(KmmFlags);

using KmmRange = Range<uflat, struct _KmmRangeTag>;

struct Kmm {
    ~Kmm() = default;

    Res<KmmRange> alloc(usize size, Flags<KmmFlags> flags);

    Res<> free(uflat addr);

    Res<> free(KmmRange range);
};
} // namespace Realms::Sys
