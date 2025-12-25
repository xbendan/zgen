#pragma once

#include <sdk-meta/types.h>

namespace Usb {

enum Class : u8 {
    Device               = 0x00,
    Audio                = 0x01,
    Communications       = 0x02,
    HumanInterfaceDevice = 0x03,
    Physical             = 0x05,
    Image                = 0x06,
    Printer              = 0x07,
    MassStorage          = 0x08,
    Hub                  = 0x09,
    CdcData              = 0x0a,
    SmartCard            = 0x0b,
    ContentSecurity      = 0x0d,
    Video                = 0x0e,
    PersonalHealthcare   = 0x0f,
    AudioVideoDevices    = 0x10,
    Billboard            = 0x11,
    Bridge               = 0x12,
    BulkDisplayProtocol  = 0x13,
    MctpEndpoint         = 0x14,
    I3c                  = 0x3c,
    Diagnostic           = 0xdc,
    WirelessController   = 0xe0,
    Miscellaneous        = 0xef,
    ApplicationSpecific  = 0xfe,
    VendorSpecific       = 0xff,
};

} // namespace Usb
