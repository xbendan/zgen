#pragma once

#include <sdk-meta/types.h>

namespace Ide {

enum Status : u8 {
    Busy         = (1 << 7),
    Ready        = (1 << 6),
    WriteFault   = (1 << 5),
    SeekComplete = (1 << 4),
    DataReady    = (1 << 3),
    Corrected    = (1 << 2),
    Index        = (1 << 1),
    Error        = (1 << 0),
};

enum Error : u8 {
    BadBlock           = (1 << 7),
    BadData            = (1 << 6),
    MediaChanged       = (1 << 5),
    IdNotFound         = (1 << 4),
    MediaChangeRequest = (1 << 3),
    Aborted            = (1 << 2),
    TrackZero          = (1 << 1),
    NoAddressMark      = (1 << 0),
};

enum Command : u8 {
    ReadPio        = 0x20,
    ReadPioExt     = 0x24,
    ReadDma        = 0xC8,
    ReadDmaExt     = 0x25,
    WritePio       = 0x30,
    WritePioExt    = 0x34,
    WriteDma       = 0xCA,
    WriteDmaExt    = 0x35,
    FlushCache     = 0xE7,
    FlushCacheExt  = 0xEA,
    IdentifyDrive  = 0xEC,
    IdentifyPacket = 0xA1,
    Packet         = 0xA0,
    // Atapi packet commands
    PI_Read        = 0xA8,
    PI_Eject       = 0x1B,
};

enum Channel : u8 {
    Primary   = 0,
    Secondary = 1,
};

enum Ata : u8 {
    Master = 0,
    Slave  = 1,
};

enum Regs {
    Data      = 0x00,
    Error     = 0x01,
    Feats     = 0x01,
    SecCnt0   = 0x02,
    Lba0      = 0x03,
    Lba1      = 0x04,
    Lba2      = 0x05,
    DevSel    = 0x06,
    Command   = 0x07,
    Status    = 0x07,
    SecCnt1   = 0x08,
    Lba3      = 0x09,
    Lba4      = 0x0A,
    Lba5      = 0x0B,
    Control   = 0x0C,
    AltStatus = 0x0C,
    DevAddr   = 0x0D,
};

struct ChannelRegs {
    u16 base;
    u16 ctrl;
    u16 bmide;
    u8  nIen { 0 };
};

struct Drive {
    u8 exists;
    enum {
        Primary   = 0,
        Secondary = 1
    } channel;
    enum {
        Master = 0,
        Slave  = 1
    } mode;
    bool atapi;
    u16  signature;
    u16  caps;
    u32  commands;
    u32  size;
    u8   model[41];
};

struct Port {
    u16 data;
    u16 error;
    u16 feats;
    u16 secCnt0;
    u16 lba0;
    u16 lba1;
    u16 lba2;
    u16 devSel;
    u16 command;
    u16 status;
    u16 secCnt1;
    u16 lba3;
    u16 lba4;
    u16 lba5;
    u16 control;
    u16 altStatus;
    u16 devAddr;
};

} // namespace Ide
