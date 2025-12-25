#pragma once

#include <realms/io/dev.h>
#include <sdk-meta/box.h>
#include <sdk-meta/lock.h>

namespace Realms::Core::Io {

struct Devtree final {
    struct Node {
        Rc<Dev>    dev;
        Opt<Node&> parent;
        Vec<Node>  childrens;
    };

    Box<Node>        _root;
    usize            _count;
    usize            _version;
    Lock             _lock;
    Dict<Str, Node&> _table;

    Devtree();

    Devtree(InitializerList<Rc<Dev>> devices);

    Node& root();

    Node& mount(Rc<Dev> device);

    template <Meta::Extends<Dev> D>
    [[gnu::always_inline]] Opt<Rc<D>> find(Str name) {
        auto rc = find(name);
        if (not rc)
            return NONE;

        return rc->template cast<D>();
    }

    Opt<Rc<Dev>> find(Str name);
};

} // namespace Realms::Core::Io
