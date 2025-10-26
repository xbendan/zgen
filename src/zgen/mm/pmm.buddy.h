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
    struct [[gnu::packed]] Item {
        Lnode<Item> lnode;
        u8          order;
        uflat       addr;

        union [[gnu::packed]] {
            Item* head;
            uflat priv;
        };
    };

    Res<Hal::PmmRange> alloc(u64                  size,
                             Flags<Hal::PmmFlags> flags = {}) override;

    Res<> free(Hal::PmmRange range) override;

    Res<> take(Hal::PmmRange range) override;
};

} // namespace Zgen::Core
