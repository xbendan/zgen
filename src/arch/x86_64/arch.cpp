#include <arch/x86_64/com.h>
#include <arch/x86_64/cpu.h>
#include <arch/x86_64/cpuid.h>
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/idt.h>
#include <arch/x86_64/regs.h>
#include <sdk-logs/logger.h>
#include <sdk-meta/manual.h>
#include <sdk-meta/res.h>
#include <zgen/core/api.io.h>
#include <zgen/init/prekernel.h>

namespace Zgen::Hal {

extern "C" void _intDispatch(int num, x86_64::Regs* regs) {
    logInfo("Interrupt: {}, regs: {:#x}\n", num, (uflat) regs);
    __asm__ __volatile__("cli; hlt");
    __builtin_unreachable();
}

x86_64::Idt              _idt;
Manual<x86_64::CpuLocal> _cpuLocal;

Res<> init(Core::PrekernelInfo* info) {
    static auto _com = x86_64::com1();
    Core::out(_com);
    Core::err(_com);
    try$(_com.init());

    auto cpuid = x86_64::capabilities();
    if (!(cpuid.sse4_2 and cpuid.fpu and cpuid.msr and cpuid.apic)) {
        logError("x86_64: unsupported CPU features");
        return Error::notSupported();
    }

    for (usize i = 0; i < x86_64::Idt::LEN; i++) {
        _idt.entries[i] = { x86_64::_intVec[i], 0, x86_64::Idt::INTR };
    }

    _cpuLocal(0, _idt);
    _cpuLocal->gdtPtr.load();
    _cpuLocal->idtPtr.load();

    return Ok();
}

} // namespace Zgen::Hal
