#pragma once

#include <sdk-meta/lock.h>
#include <sdk-meta/ref.h>
#include <sdk-text/str.h>
#include <zgen/hal/kmm.h>
#include <zgen/mm/pmm.buddy.h>

namespace Zgen::Core {

struct KmmSlub final : public Hal::Kmm {
    struct Cache {
        Str  name;
        Lock lock;
        u32  size;
        u32  align;
    };

    struct [[gnu::packed]] Data {
        struct [[gnu::packed]] {
            u32 inuse: 16;
            u32 objects: 15;
            u32 frozen: 1;
        };
        void** freelist;
        Cache* cache;
    };
    static_assert(sizeof(Data) == 20);

    KmmSlub();
    ~KmmSlub() final = default;

    Res<Hal::KmmRange> alloc(usize                     size,
                             Flags<Hal::KmmAllocFlags> flags) override;

    Res<> free(Hal::KmmRange range) override;
};

} // namespace Zgen::Core
