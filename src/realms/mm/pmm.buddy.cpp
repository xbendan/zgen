#include <realms/mm/pmm.buddy.h>

namespace Realms::Core {

Res<Hal::PmmRange> PmmBuddy::alloc(u64 size, Flags<Hal::PmmFlags> flags) {
    return Error::notImplemented();
}

Res<> PmmBuddy::free(Hal::PmmRange range) {
    return Error::notImplemented();
}

Res<> PmmBuddy::take(Hal::PmmRange range) {
    return Error::notImplemented();
}

} // namespace Realms::Core
