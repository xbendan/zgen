#include <arch/x86_64/apic.h>

namespace Realms::Hal::x86_64::Apic {

Vec<Local> _units;

Slice<Local> units() {
    return _units;
}

} // namespace Realms::Hal::x86_64::Apic
