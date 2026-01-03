#pragma once

#include <realms/hal/kmm.h>
#include <realms/mm/pmm.buddy.h>
#include <sdk-meta/list.h>
#include <sdk-meta/lock.h>
#include <sdk-meta/pc.h>
#include <sdk-text/str.h>

namespace Realms::Sys {

struct KmmSlub final : public Sys::Kmm {
    struct Block : public LinkedTrait<Block> {
        struct {
            u32 inuse: 16;
            u32 objects: 15;
            u32 frozen: 1;
        };
        void** ptr;
    };
    static_assert(ILinked<Block>);
    static_assert(sizeof(Block) == 32);

    struct Node {
        Block*      curr;
        List<Block> partial;
    };

    struct Kind : LinkedTrait<Kind> {
        Str         name;
        Lock        lock;
        u32         size;
        u32         align;
        List<Block> full;
        List<Block> partial;
        Pc<Node>    percpu;
        Vec<Node>   nodes;
    };
    static_assert(sizeof(Kind) <= 192);

    static constexpr Array<usize, 16> sizes = {
        (8),   (16),  (24),  (32),  (48),  (64),   (96),   (128),
        (192), (256), (384), (512), (768), (1024), (1536), (2048),
    };

    Slice<Kind>  _kinds;
    Slice<Block> _blocks;

    KmmSlub(Hal::KmmRange kindsRange, Hal::KmmRange blocksRange);

    Res<Hal::KmmRange> alloc(usize size, Flags<Hal::KmmFlags> flags) override;

    Opt<uflat> alloc(Kind& kind, Node& node, Flags<Hal::KmmFlags> flags);

    Res<> free(uflat addr) override;

    Res<> free(Hal::KmmRange range) override;

    Res<> nonnull(Kind& kind, Node& node);
};

} // namespace Realms::Sys
