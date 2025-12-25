#pragma once

#include <realms/hal/kmm.h>
#include <realms/hal/pmm.h>
#include <realms/hal/vmm.h>
#include <realms/init/prekernel.h>
#include <realms/mm/kmm.slub.h>
#include <sdk-meta/list.h>
#include <sdk-meta/ref.h>

namespace Realms::Core {

struct PmmBuddy : public Hal::Pmm {
    struct [[packed]] Item : LinkedTrait<Item> {
        u8    order;
        uflat addr;

        union [[packed]] {
            Item* head;
            uflat priv;
        };
    };

    Res<Hal::PmmRange> alloc(u64                  size,
                             Flags<Hal::PmmFlags> flags = {}) override;

    Res<> free(Hal::PmmRange range) override;

    Res<> take(Hal::PmmRange range) override;
};

} // namespace Realms::Core
