#pragma once

#include <realms/hal/io.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>

namespace Pci {

enum Regs : u8 {
    VendorId                = 0x00,
    DeviceId                = 0x02,
    Command                 = 0x04,
    Status                  = 0x06,
    RevisionId              = 0x08,
    ProgIf                  = 0x09,
    SubclassCode            = 0x0A,
    ClassCode               = 0x0B,
    CacheLineSize           = 0x0C,
    LatencyTimer            = 0x0D,
    HeaderType              = 0x0E,
    Bist                    = 0x0F,
    Bar0                    = 0x10,
    Bar1                    = 0x14,
    Bar2                    = 0x18,
    Bar3                    = 0x1C,
    Bar4                    = 0x20,
    Bar5                    = 0x24,
    CardbusCISPointer       = 0x28,
    SubsystemVendorID       = 0x2C,
    SubsystemID             = 0x2E,
    ExpansionROMBaseAddress = 0x30,
    CapabilitiesPointer     = 0x34,
    InterruptLine           = 0x3C,
    InterruptPin            = 0x3D,
    MinGrant                = 0x3E,
    MaxLatency              = 0x3F
};

enum Mode {
    Legacy,
    Enhanced
};

enum class Class {
    Unclassified     = 0x00,
    MassStorage      = 0x01,
    Network          = 0x02,
    Display          = 0x03,
    Multimedia       = 0x04,
    Memory           = 0x05,
    Bridge           = 0x06,
    Communication    = 0x07,
    Peripheral       = 0x08,
    Input            = 0x09,
    Docking          = 0x0A,
    Processor        = 0x0B,
    SerialBus        = 0x0C,
    Wireless         = 0x0D,
    Intelligent      = 0x0E,
    Satellite        = 0x0F,
    Encryption       = 0x10,
    SignalProcessing = 0x11,
    Accelerator      = 0x12,
    NonEssential     = 0x13,
    CoProcessor      = 0x40,
    Reserved         = 0xFF
};

enum Command {
    IoSpace            = (1 << 0),
    MemorySpace        = (1 << 1),
    BusMaster          = (1 << 2),
    SpecialCycle       = (1 << 3),
    MemWriteInv        = (1 << 4),
    VgaPaletteSnooping = (1 << 5),
    ParityError        = (1 << 6),
    SteppingCtrl       = (1 << 7),
    SERR               = (1 << 8),
    FastBackToBack     = (1 << 9),
    IntDisable         = (1 << 10),
};

template <Class>
struct Subclass;

template <>
struct Subclass<Class::MassStorage> {
    enum : u8 {
        Scsi      = 0x00,
        Ide       = 0x01,
        Floppy    = 0x02,
        Ipi       = 0x03,
        Raid      = 0x04,
        Ata       = 0x05,
        SerialAta = 0x06,
        Nvme      = 0x08,
        Other     = 0x80
    };
};

template <>
struct Subclass<Class::Network> {
    enum : u8 {
        Ethernet   = 0x00,
        TokenRing  = 0x01,
        Fddi       = 0x02,
        Atm        = 0x03,
        Isdn       = 0x04,
        WorldFip   = 0x05,
        Picmg      = 0x06,
        Infiniband = 0x07,
        Other      = 0x80
    };
};

template <>
struct Subclass<Class::Display> {
    enum : u8 {
        Vga    = 0x00,
        Xga    = 0x01,
        ThreeD = 0x02,
        Other  = 0x80
    };
};

template <>
struct Subclass<Class::Multimedia> {
    enum : u8 {
        Video     = 0x00,
        Audio     = 0x01,
        Telephony = 0x02,
        Other     = 0x80
    };
};

template <>
struct Subclass<Class::Memory> {
    enum : u8 {
        Ram   = 0x00,
        Flash = 0x01,
        Other = 0x80
    };
};

template <>
struct Subclass<Class::Communication> {
    enum : u8 {
        Serial    = 0x00,
        Parallel  = 0x01,
        Multiport = 0x02,
        Modem     = 0x03,
        GPiB      = 0x04,
        SmartCard = 0x05,
        Other     = 0x80
    };
};

template <>
struct Subclass<Class::Peripheral> {
    enum : u8 {
        Pic     = 0x00,
        Dma     = 0x01,
        Timer   = 0x02,
        RTC     = 0x03,
        HotPlug = 0x04,
        Other   = 0x80
    };
};

template <>
struct Subclass<Class::Input> {
    enum : u8 {
        Keyboard = 0x00,
        Stylus   = 0x01,
        Mouse    = 0x02,
        Scanner  = 0x03,
        Gameport = 0x04,
        Other    = 0x80
    };
};

template <>
struct Subclass<Class::Bridge> {
    enum : u8 {
        Host            = 0x00,
        Isa             = 0x01,
        Eisa            = 0x02,
        MicroChannel    = 0x03,
        PciToPci        = 0x04,
        Pcmcia          = 0x05,
        NuBus           = 0x06,
        CardBus         = 0x07,
        RACEway         = 0x08,
        SemiTransparent = 0x09,
        Other           = 0x80
    };
};

using ConfigAddress = Realms::Hal::Reg<u32, 0xcf8>;
using ConfigData    = Realms::Hal::Reg<u32, 0xcfc>;

struct Id : public Realms::Hal::Io {
    u8  bus, slot, func;
    u16 _vendorId, _deviceId;
    u8  _class, _subclass, _progIf;

    Id(u8 bus, u8 slot, u8 func) : bus(bus), slot(slot), func(func) { }

    Res<usize> in(usize offset, usize size) override;

    Res<> out(usize offset, usize size, usize val) override;

    Res<u16> vendorId() {
        return _vendorId ? Ok(_vendorId)
                         : (_vendorId = try$(in(Regs::VendorId, 2).map<u16>()));
    }

    Res<u16> deviceId() {
        return _deviceId ? Ok(_deviceId)
                         : (_deviceId = try$(in(Regs::DeviceId, 2).map<u16>()));
    }

    Res<u8> clazz() {
        return _class ? Ok(_class)
                      : (_class = try$(in(Regs::ClassCode, 1).map<u8>()));
    }

    Res<u8> subclass() {
        return _subclass
                 ? Ok(_subclass)
                 : (_subclass = try$(in(Regs::SubclassCode, 1).map<u8>()));
    }

    Res<u8> progIf() {
        return _progIf ? Ok(_progIf)
                       : (_progIf = try$(in(Regs::ProgIf, 1).map<u8>()));
    }
};

} // namespace Pci
