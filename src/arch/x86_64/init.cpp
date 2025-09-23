#include <arch/x86_64/com.h>
#include <arch/x86_64/cpu.h>
#include <arch/x86_64/cpuid.h>
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/idt.h>
#include <neodymium/init/prekernel.h>
#include <sdk-meta/manual.h>
#include <sdk-meta/res.h>

namespace Hal {

Hal::x86_64::Idt idt;

Manual<Hal::x86_64::CpuLocal> cpuLocal;

} // namespace Hal

namespace Sys {

using namespace Hal;

Res<> initArch(PrekernelInfo* info) {
    cpuLocal(0, idt);
    cpuLocal->gdtPtr.load();
    cpuLocal->idtPtr.load();

    try$(x86_64::com1().init());

    auto cpuid = x86_64::capabilities();
    if (!(cpuid.sse4_2 and cpuid.fpu and cpuid.msr and cpuid.apic)) {
        return Error::notSupported();
    }

    return Ok();
}

} // namespace Sys
