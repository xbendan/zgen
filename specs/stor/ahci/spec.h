#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/types.h>

namespace Ahci {

enum Fis : u8 {
    HostToDevice = 0x27,
    DeviceToHost = 0x34,
    DmaAct       = 0x39,
    DmaSetup     = 0x41,
    Data         = 0x46,
    Bist         = 0x58,
    PioSetup     = 0x5F,
    DevBits      = 0xA1
};

template <Fis>
struct Command;

template <>
struct [[gnu::packed]] Command<Fis::HostToDevice> {
    u8 id { Fis::HostToDevice };

    u8 pm: 4;
    u8 __reserved__0: 3;
    u8 mode: 1;

    u8 command;
    u8 featLo;

    struct {
        u8 lba0, lba1, lba2;
        u8 device;
    };
    struct {
        u8 lba3, lba4, lba5;
        u8 featHi;
    };
    struct {
        u8 countLo;
        u8 countHi;
        u8 icc;
        u8 ctl;
    };
    u8 __reserved__1[4];
};
static_assert(sizeof(Command<Fis::HostToDevice>) == 20);

template <>
struct [[gnu::packed]] Command<Fis::DeviceToHost> {
    u8 id { Fis::DeviceToHost };

    u8 pm: 4;
    u8 __reserved__0: 2;
    u8 interrupt: 1;
    u8 __reserved__1: 1;

    u8 status;
    u8 error;
    struct {
        u8 lba0, lba1, lba2;
        u8 device;
    };
    struct {
        u8 lba3, lba4, lba5;
        u8 __reserved__2;
    };
    struct {
        u8 countLo;
        u8 countHi;
        u8 __reserved__3[2];
    };

    u8 __reserved__4[4];
};
static_assert(sizeof(Command<Fis::DeviceToHost>) == 0x14);

template <>
struct [[gnu::packed]] Command<Fis::PioSetup> {
    u8 id { Fis::PioSetup };

    u8 pm: 4;
    u8 __reserved__0: 1;
    u8 direction: 1;
    u8 interrupt: 1;
    u8 __reserved__1: 1;

    u8 status;
    u8 error;
    struct {
        u8 lba0, lba1, lba2;
        u8 device;
    };
    struct {
        u8 lba3, lba4, lba5;
        u8 __reserved__2;
    };
    struct {
        u8 countLo;
        u8 countHi;
        u8 __reserved__3;
        u8 eStatus;
    };

    u16 tc;
    u8  __reserved__4[2];
};
static_assert(sizeof(Command<Fis::PioSetup>) == 0x14);

template <>
struct [[gnu::packed]] Command<Fis::DmaSetup> {
    u8 id { Fis::DmaSetup };

    u8 pm: 4;
    u8 __reserved__0: 1;
    u8 direction: 1;
    u8 interrupt: 1;
    u8 auto_: 1;

    u8 __reserved__1[2];

    u64 bufferId;
    u32 __reserved__2;

    u32 offset; // byte offset into buffer, first 2 bits must be 0
    u32 count;  // byte count, bit 0 must be 0

    u32 __reserved__3;
};
static_assert(sizeof(Command<Fis::DmaSetup>) == 28);

template <>
struct [[gnu::packed]] Command<Fis::Data> {
    u8 id { Fis::Data };
    u8 pm;

    u16 __reserved__0;

    u32 data[];
};

template <>
struct [[gnu::packed]] Command<Fis::DevBits> {
    u8  id { Fis::DevBits };
    u8  pm;
    u8  status;
    u8  error;
    u32 __reserved__0;
};

namespace Hba {

struct [[gnu::packed]] Capability {
    u32 np: 5;
    u32 sxs: 1;
    u32 ems: 1;
    u32 cccs: 1;
    u32 ncs: 5;
    u32 psc: 1;
    u32 ssc: 1;
    u32 pmd: 1;
    u32 fbss: 1;
    u32 pm: 1;
    u32 ahciMo: 1;
    u32 __reserved__0: 1;
    enum class LinkSpeed : u32 {
        Reserved = 0b00,
        Gen1     = 0b01, // 1.5 Gb/s
        Gen2     = 0b10, // 3.0 Gb/s
        Gen3     = 0b11  // 6.0 Gb/s
    } speed: 4;
    u32 clo: 1;
    u32 activityLed: 1;
    u32 alpm: 1;
    u32 ssu: 1;
    u32 mpswtch: 1;
    u32 sn: 1;
    u32 ncq: 1;
    u32 bits64: 1;
};

struct [[gnu::packed]] CapabilityExt {
    u32 bioshc: 1;
    u32 nvmhci: 1;
    u32 asmt: 1;
};

struct [[gnu::packed]] _PortRegs {
    u32           clb;
    u32           clbu;
    u32           fb;
    u32           fbu;
    u32           is;
    u32           ie;
    u32           cmd;
    u32           __reserved__0;
    u32           tfd;
    u32           sig;
    u32           ssts;
    u32           sctl;
    u32           serr;
    u32           sact;
    u32           ci;
    u32           sntf;
    u32           fbs;
    u32           __reserved__1[11];
    Array<u32, 4> ven;
};
using PortRegs = _PortRegs volatile;

struct [[gnu::packed]] _MemoryRegs {
    Capability          hostCaps;
    u32                 ghc;
    u32                 is;
    u32                 pi;
    u32                 version;
    u32                 cccctl;
    u32                 cccpts;
    u32                 emloc;
    u32                 emctl;
    CapabilityExt       hostCapsExt;
    u32                 bohc;
    u8                  __reserved__0[0x74];
    Array<char, 0x60>   vendor;
    Array<PortRegs, 32> ports;
};
using MemoryRegs = _MemoryRegs volatile;

struct Received {
    Command<Fis::DmaSetup>     dsfis;
    u8                         __reserved__0[4];
    Command<Fis::PioSetup>     psfis;
    u8                         __reserved__1[12];
    Command<Fis::DeviceToHost> rfis;
    u8                         __reserved__2[4];
    Command<Fis::DevBits>      sdbfis;
    Array<u8, 0x40>            ufis;
    u8                         __reserved__3[0x100 - 0xA0];
};

struct [[gnu::packed]] _CommandHeader {
    u8 cfl: 5;
    u8 atapi: 1;
    u8 write: 1;
    u8 prefetch: 1;
    u8 reset: 1;
    u8 bist: 1;
    u8 clearBusy: 1;
    u8 __reserved__0: 1;
    u8 pmport: 4;

    u16 prdtl;
    u32 volatile prdbc;
    u32 ctba;
    u32 ctbau;
    u32 __reserved__1[4];
};
using CommandHeader = _CommandHeader volatile;

struct [[gnu::packed]] PrdtEntry {
    u32 dba;
    u32 dbau;
    u32 __reserved__0;
    u32 dbc: 22;
    u32 __reserved__1: 9;
    u32 i: 1;
};

struct [[gnu::packed]] _CommandTable {
    Array<u8, 0x40> cfis;
    Array<u8, 0x10> atapiCmd;
    u8              __reserved__0[0x30];
    PrdtEntry       entries[1];
};
using CommandTable = _CommandTable volatile;

} // namespace Hba

}; // namespace Ahci
