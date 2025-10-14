#pragma once

#include <sdk-meta/list.h>
#include <sdk-meta/ref.h>
#include <zgen/hal/kmm.h>
#include <zgen/hal/pmm.h>
#include <zgen/hal/vmm.h>
#include <zgen/init/prekernel.h>
#include <zgen/mm/kmm.slub.h>

namespace Zgen::Core {

struct PmmBuddy : public Hal::Pmm {
    struct [[gnu::packed]] Data : public Meta::Lnode<Data> {
        u8    order;
        uflat addr;

        union [[gnu::packed]] {
            Data* head;
            uflat priv;

#if defined(NeodymiumKmmUseSlub) or defined(NeodymiumKmmUseSlab)
            KmmSlub::Cache* kmm;
#endif
        };

        struct [[gnu::packed]] {
            struct [[gnu::packed]] {
                u32 inuse: 16;
                u32 objects: 15;
                u32 frozen: 1;
            };
            void** freelist;
        };
    };
    static_assert(sizeof(Data) % 8 == 0);

    Res<Hal::PmmRange> alloc(u64                  size,
                             Flags<Hal::PmmFlags> flags = {}) override;

    Res<> free(Hal::PmmRange range) override;

    Res<> take(Hal::PmmRange range) override;
};

} // namespace Zgen::Core
