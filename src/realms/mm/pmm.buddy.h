#pragma once

#include <realms/hal/kmm.h>
#include <realms/hal/pmm.h>
#include <realms/hal/vmm.h>
#include <realms/mm/kmm.slub.h>
#include <sdk-meta/list.h>

namespace Realms::Sys {

struct PmmBuddy : public Hal::Pmm {
    struct [[gnu::packed]] Item : LinkedTrait<Item> {
        u8    order;
        uflat addr;

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

} // namespace Realms::Sys
