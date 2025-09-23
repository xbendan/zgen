#pragma once

#include <sdk-meta/flags.h>
#include <sdk-meta/range.h>
#include <sdk-meta/res.h>

namespace Hal {

enum struct KmmAllocFlags {
    USER,
    KERNEL,
    CONTIGUOUS,
    NO_FAIL, // Repeatly try to allocate until success
    NO_WAIT, // No interrupt or sleep during allocation
    NO_RETRY // Abort immediately if first allocation fails
};
MakeFlags$(KmmAllocFlags);

using KmmRange = Range<uptr, struct _KmmRangeTag>;

struct Kmm {
    virtual ~Kmm() = default;

    virtual Res<KmmRange> alloc(usize size, Flags<KmmAllocFlags> flags) = 0;

    virtual Res<> free(KmmRange range) = 0;
};
} // namespace Hal
