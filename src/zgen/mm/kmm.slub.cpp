#include <zgen/mm/kmm.slub.h>

namespace Zgen::Core {

KmmSlub::KmmSlub() {
}

Res<Hal::KmmRange> KmmSlub::alloc(usize size, Flags<Hal::KmmAllocFlags> flags) {
    return Error::notImplemented();
}

Res<> KmmSlub::free(Hal::KmmRange range) {
    return Error::notImplemented();
}

} // namespace Zgen::Core
