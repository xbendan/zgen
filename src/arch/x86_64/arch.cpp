#include <acpi/bus.h>
#include <arch/x86_64/apic.h>
#include <arch/x86_64/com.h>
#include <arch/x86_64/cpu.h>
#include <arch/x86_64/cpuid.h>
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/idt.h>
#include <arch/x86_64/regs.h>
#include <pci/bus.h>
#include <realms/init/prekernel.h>
#include <realms/io/devtree.h>
#include <realms/mm/mem.h>
#include <sdk-logs/logger.h>
#include <sdk-meta/iter.h>
#include <sdk-meta/manual.h>
#include <sdk-meta/res.h>

extern "C" {
    unsigned const smp_trampoline_entry = 0x2000;

    usize smp_trampoline_start;
    usize smp_trampoline_end;
}

namespace Realms::Hal {

extern "C" void _intDispatch(int num, x86_64::Regs* regs) {
    logInfo("Interrupt: {}, regs: {:#x}\n", num, (uflat) regs);
    __asm__ __volatile__("cli; hlt");
    __builtin_unreachable();
}

x86_64::Idt              _idt;
Manual<x86_64::CpuLocal> _cpuLocal;

Res<> init(Core::PrekernelInfo* info) {
    static auto _com = x86_64::com1();
    Sdk::out         = { &_com };
    Sdk::err         = { &_com };
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

} // namespace Realms::Hal

namespace Realms::Core {

[[noreturn]] void mpinitEntry(u16 id) {
    Hal::x86_64::halt();
}

Hal::x86_64::Gdt::Pack* s_gdtPtr = (Hal::x86_64::Gdt::Pack*) 0x1010;

extern "C" Hal::x86_64::Gdt::Pack GdtPack64;

u16 volatile* s_magic        = (u16*) 0x1000;
u16 volatile* s_trampolineId = (u16*) 0x1002;
u64 volatile* s_pagemap      = (u64*) 0x1020;
u64 volatile* s_stack        = (u64*) 0x1028;
u64 volatile* s_entrypoint   = (u64*) 0x1030;
bool volatile s_doneInit     = false;

Res<usize> setupMultitasking() {
    memcpy((void*) smp_trampoline_entry,
           &smp_trampoline_start,
           (usize) &smp_trampoline_end - (usize) &smp_trampoline_start);

    for (auto it : Hal::x86_64::Apic::units()) {
        if (it.id() == 0) [[unlikely]] {
            try$(it.send(Hal::x86_64::Apic::Dest::Others,
                         Hal::x86_64::Apic::Message::Init,
                         0));
            continue;
        }

        logInfo("Hal::setupMultitasking: starting CPU {}\n", it.id());
        *s_magic        = 0;
        *s_trampolineId = it.id();
        *s_entrypoint   = (u64) mpinitEntry; // TODO: check whether to use &
        *s_stack
            = (u64) Core::pmm()
                  .alloc(0x1'0000)
                  .unwrap("Hal::setupMultitasking: failed to allocate stack")
                  .end();
        *s_gdtPtr = GdtPack64;

        asm volatile("mfence" ::: "memory");
        asm volatile(
            "mov %%cr3, %%rax\n\t"
            "mov %%rax, %0"
            : "=m"(*s_pagemap)
            :
            : "rax");

        while (*s_magic != 0xb33f) {
            try$(it.send(Hal::x86_64::Apic::Dest::Self,
                         Hal::x86_64::Apic::Message::Startup,
                         (smp_trampoline_entry >> 12)));
            // sleep 100ms
        }

        while (not s_doneInit)
            asm volatile("pause");

        s_doneInit = false;
    }

    return Ok(Hal::x86_64::Apic::units().len());
}

static Opt<Io::Devtree&> s_devtree = NONE;

Io::Devtree& createDevtree() {
    if (not s_devtree) {
        s_devtree = new Io::Devtree(
            { makeRc<Acpi::BusDevice>(), makeRc<Pci::BusDevice>() });
    }
    return *s_devtree;
}

Opt<Io::Devtree&> devtree() {
    return s_devtree;
}

} // namespace Realms::Core
