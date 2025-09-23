#pragma once

#include <sdk-io/traits.h>
#include <sdk-meta/types.h>
#include <sdk-text/str.h>

namespace Sys::Io {

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
    };

    enum struct Bus {
        None,
        Pci,
        PciExpress,
        Usb
    };

    Str  name;
    u32  id;
    Type type;
    Bus  bus;
};

} // namespace Sys::Io
