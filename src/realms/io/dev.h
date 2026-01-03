#pragma once

#include <sdk-io/traits.h>
#include <sdk-meta/dict.h>
#include <sdk-meta/types.h>
#include <sdk-text/str.h>

namespace Realms::Sys::Io {

struct Bus;
struct Drv;

struct Dev {
    enum struct Type {
        Unknown,

        // MARK: - Processing & Storage
        Processor,
        DisplayAdapter,
        StorageDrive,
        StorageController,

        // MARK: - Input
        HumanInterfaceDevice,
        KeyboardDevice,
        PointerDevice,
        Scanner,
        Printer,
        Camera,
        Microphone,

        // MARK: - Output
        AudioController,
        AudioOutput,
        DisplayDevice,
        Monitor,
        Projector,

        // MARK: - Communication
        NetworkAdapter,
        Modem,
        BluetoothAdapter,
        WirelessAdapter,

        // MARK: - Other
        PowerSupply,
        Battery,
        UsbController,
        UsbDevice,
        SerialPort,
        ParallelPort,
        VirtualDevice,
        SoftwareDevice
    };

    enum struct Status {
        Disabled,
        Enabled,
        Initialized,
        Unavailable
    };

    struct Prop {
        Str                         key;
        Union<Str, u32, u64, Bytes> value;
    };

    String    _name;
    String    _path;
    Type      _type;
    Opt<Drv&> _drv;
    Status    _status;

    Dev(String    name,
        String    path,
        Type      type = Type::Unknown,
        Opt<Drv&> drv  = NONE)
        : _name(name),
          _path(path),
          _type(type),
          _drv(drv),
          _status(Status::Disabled) { };
};

} // namespace Realms::Sys::Io
