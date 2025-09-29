#pragma once

#include <arch/x86_64/regs.h>
#include <neodymium/hal/interrupt.h>
#include <neodymium/hal/io.h>
#include <neodymium/io/device.h>
#include <sdk-meta/vec.h>
#include <specs/acpi/tables.h>

namespace Hal::x86_64::Apic {

using ApicId              = Hal::Reg<u32, 0x20>;
using ApicVersion         = Hal::Reg<u32, 0x30>;
using TaskPriority        = Hal::Reg<u32, 0x80>;
using ArbitrationPriority = Hal::Reg<u32, 0x90>;
using ProcessorPriority   = Hal::Reg<u32, 0xA0>;
using InterruptReset      = Hal::Reg<u32, 0xB0>;
using RemoteRead          = Hal::Reg<u32, 0xC0>;
using LogicalDest         = Hal::Reg<u32, 0xD0>;
using DestFormat          = Hal::Reg<u32, 0xE0>;
using Spurious            = Hal::Reg<u32, 0xF0>;
using InService           = Hal::Reg<u32, 0x100>;
using TriggerMode         = Hal::Reg<u32, 0x180>;
using InterruptRequest    = Hal::Reg<u32, 0x200>;
using ErrorStatus         = Hal::Reg<u32, 0x280>;
using IcrLow              = Hal::Reg<u32, 0x300>;
using IcrHigh             = Hal::Reg<u32, 0x310>;
using TimerInitial        = Hal::Reg<u32, 0x380>;
using TimerCurrent        = Hal::Reg<u32, 0x390>;

using Base = Hal::Reg<u64, 0xF0>;

enum struct Lvt {
    Timer       = 0x320,
    Thermal     = 0x330,
    Performance = 0x340,
    Lint0       = 0x350,
    Lint1       = 0x360,
    Error       = 0x370
};

enum struct Message {
    Fixed       = 0x0,
    LowPriority = (0x1 << 8),
    Smi         = (0x2 << 8),
    RemoteRead  = (0x3 << 8),
    Nmi         = (0x4 << 8),
    Init        = (0x5 << 8),
    Startup     = (0x6 << 8),
    External    = (0x7 << 8),
    Pending     = 0x1000
};

enum struct Dest {
    Normal = 0x0,
    Self   = (1 << 18),
    All    = (2 << 18),
    Others = (3 << 18)
};

using Sys::Io::Device;

struct Local;

struct GenericDevice : public Device, public Hal::Interrupts, public Hal::Io {
    Vec<Nonnull<Local>>   _devices;
    Vec<Acpi::Madt::Iso*> _isos;

    struct {
        uptr          base, vbase;
        u32           intr;
        u32 volatile* sel;
        u32 volatile* win;
    } _io;

    Res<usize> in(usize offset, usize size) override;

    Res<> out(usize offset, usize size, usize val) override;

    Opt<uptr> base();

    void base(uptr addr);
};

struct Local : public Device, public Hal::Io {
    u8   _processorId;
    u8   _apicId;
    uptr _base, _vbase;

    Nonnull<GenericDevice> _device;

    Res<> init();

    Res<usize> in(usize offset, usize size) override;

    Res<> out(usize offset, usize size, usize val) override;

    Res<> send(u32 vec);

    Res<> send(Dest dest, Message message, u8 vec);
};

struct TimerDevice : public Device {
    Local& _local;
    u32    _busSpeed, _irqSrc;
};

} // namespace Hal::x86_64::Apic
