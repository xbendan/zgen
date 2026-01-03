module;

#include <realms/hal/kmm.h>
#include <realms/hal/pmm.h>
#include <realms/mm/kmm.slub.h>
#include <realms/mm/mem.h>
#include <sdk-meta/ranges.h>

export module Realms.Core;

namespace Realms::Sys {

KmmSlub::KmmSlub(Sys::KmmRange kindsRange, Sys::KmmRange blocksRange)
    : _kinds({ kindsRange.start().as<Kind>(), sizes.len() }),
      _blocks({ blocksRange.start().as<Block>(),
                blocksRange.size() / sizeof(Block) }) {
    for (usize i = 0; i < sizes.len(); i++) { }
}

Res<Sys::KmmRange> KmmSlub::alloc(usize size, Flags<Sys::KmmFlags> flags) {
    if (size >= Sys::PAGE_SIZE / 2) {
        return Error::notImplemented();
    }

    auto kind = _kinds | first$(size <= it.size);
    if (not kind) {
        return Error::notFound("Kmm::alloc: no suitable kind found");
    }

    Opt<uflat> addr = 0;
    auto&      node = kind->percpu();
    if (addr = alloc(kind.unwrap(), node, flags); addr) {
        return Ok(Hal::KmmRange { *addr, kind->size });
    }

    LockScoped lk(kind->lock);
    if (addr = alloc(kind.unwrap(), kind->nodes[0], flags); addr) {
        return Ok(Hal::KmmRange { *addr, kind->size });
    }

    return Error::outOfMemory("Kmm::alloc: no memory available");
}

Opt<uflat> KmmSlub::alloc(Kind& kind, Node& node, Flags<Hal::KmmFlags> flags) {
    if (not nonnull(kind, node)) {
        return NONE;
    }

    auto* block = node.curr;
    uflat addr  = (uflat) block->ptr;
    block->ptr  = (void**) *block->ptr;

    if (++block->inuse == block->objects) {
        node.curr  = nullptr;
        block->ptr = nullptr;
    }

    return addr;
}

Res<> KmmSlub::free(uflat addr) {
    return Error::notImplemented();
}

Res<> KmmSlub::free(Hal::KmmRange range) {
    return Error::notImplemented();
}

Res<> KmmSlub::nonnull(Kind& kind, Node& node) {
    Block* block = nullptr;
    if ((block = node.curr) && (block->inuse < block->objects)) {
        return Ok();
    }

    if (not node.partial.isEmpty()) {
        node.curr = node.partial.head();
        node.partial.remove(*node.curr);
        return Ok();
    }

    auto pmm = Core::pmm()
                   .alloc(Hal::PAGE_SIZE, Hal::PmmFlags::Kernel)
                   .unwrap("Kmm::nonnull: pmm alloc failed");
    block          = &_blocks[pmm.start() / Hal::PAGE_SIZE];
    block->inuse   = 0;
    block->objects = (Hal::PAGE_SIZE / kind.size);

    uflat addr = try$(Core::mmapVirtIo(pmm.start()));
    block->ptr = (void**) addr;

    for (int i = 0; i < block->objects; i++)
        *((u64*) (addr + i * kind.size)) = (u64) (addr + (i + 1) * kind.size);

    node.curr = block;
    return Ok();
}

} // namespace Realms::Sys
