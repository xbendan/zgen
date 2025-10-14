#include <arch/x86_64/apic.h>

namespace Zgen::Hal::x86_64::Apic {

Opt<uflat> GenericDevice::base() {
    return _msr.read<MsrApic>().unwrap();
}

void GenericDevice::base(uflat addr) {
    _msr.write<MsrApic>(addr).unwrap();
}

Res<> Intr::fin() {
    return _cpuLocal.write<InterruptReset>(0);
}

Res<> Intr::pause() {
    return Error::notImplemented("Apic::Intr::pause: not implemented");
}

Res<> Intr::unpause() {
    return Error::notImplemented("Apic::Intr::unpause: not implemented");
}

Res<> Smp::init() {
    return Error::notImplemented("Apic::Smp::init: not implemented");
}

Res<> Smp::pause(usize id) {
    return Error::notImplemented("Apic::Smp::pause: not implemented");
}

Res<> Smp::boot(usize id, void* stack, void (*entry)()) {
    return Error::notImplemented("Apic::Smp::boot: not implemented");
}

} // namespace Zgen::Hal::x86_64::Apic
