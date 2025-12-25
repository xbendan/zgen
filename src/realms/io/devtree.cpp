#include <realms/io/devtree.h>

namespace Realms::Core::Io {

Devtree::Devtree()
    : _root(makeBox<Node>(
          makeRc<Dev>("device-tree-root"s, "/"s, Dev::Type::SoftwareDevice),
          NONE,
          8)),
      _count(1),
      _table(16) {
}

Devtree::Devtree(InitializerList<Rc<Dev>> devices) : Devtree() {
    for (auto& dev : devices)
        mount(dev);
}

Devtree::Node& Devtree::mount(Rc<Dev> device) {
    LockScoped lk(_lock);
}

Opt<Rc<Dev>> Devtree::find(Str name) {
    LockScoped lk(_lock);

    return _table[name].value.mapTo$(it.dev);
}

} // namespace Realms::Core::Io
