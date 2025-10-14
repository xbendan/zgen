#pragma once

#include <sdk-io/traits.h>
#include <sdk-meta/types.h>
#include <sdk-text/str.h>

namespace Zgen::Core::Io {

struct Device {
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

    enum struct Bus {
        None,
        Pci,
        PciExpress,
        Usb
    };

    enum struct Status {
        Disabled,
        Enabled,
        Initialized,
        Unavailable
    };

    Str    name;
    u32    id;
    Type   type;
    Bus    bus;
    Status status;

    virtual Res<> onInit() { return Ok(); }

    virtual Res<> onStart() { return Ok(); }

    virtual Res<> onStop() { return Ok(); }
};

} // namespace Zgen::Core::Io
