#include <acpi/bus.h>
#include <pci/bus.h>
#include <realms/core/api.io.h>
#include <sdk-logs/logger.h>
#include <sdk-meta/ranges.h>

namespace Pci {

using namespace Realms::Sys;

BusDevice::BusDevice()
    : Io::Bus("pci"s),
      Io::Dev("pci-bus-device"s, "pci/pci-bus-device"s),
      _mode(Pci::Mode::Legacy) {
}

BusDevice::~BusDevice() {
    // TODO: cleanup devices
}

Res<> BusDevice::onInit() {
    auto dev = devtree()->find<Acpi::BusDevice>("acpi-bus-device"s);

    if (dev and dev.unwrap()->lookupTable("MCFG"s)) {
        _mode = Pci::Mode::Enhanced;
    }

    return Ok();
}

Res<String> BusDevice::path(Rc<Io::Dev> dev) {
    return Error::notImplemented();
}

Res<Slice<Rc<Io::Dev>>> BusDevice::probe() {
    for (u16 i = 0; i < 256; i++) {
        for (u16 j = 0; j < 32; j++) {
            Id id = { (u8) i, (u8) j, 0 };
            if (not check(id)) {
                continue;
            }

            _devices.pushBack(create(id)->cast<Pci::Dev>().take());
            if (try$(id.in8(Regs::HeaderType)) & 0x80) {
                // Multifunction device
                logInfo(
                    "PciBus::probe: multifunction device found at "
                    "bus {:02x}, slot {:02x}",
                    i,
                    j);
                for (u16 k = 1; k < 8; k++) {
                    id.func = k;
                    if (not check(id)) {
                        continue;
                    }

                    create(id);
                }
            }
        }
    }
    return Ok(slice(_devices).cast<Rc<Io::Dev>>());
}

Res<Slice<Rc<Io::Dev>>> BusDevice::devices() {
    return Ok(slice(_devices).cast<Rc<Io::Dev>>());
}

Res<> BusDevice::remove(Rc<Io::Dev> dev) {
    return Error::notImplemented();
}

bool BusDevice::check(u8 bus, u8 slot, u8 func) {
    Id id = { bus, slot, func };
    return check(id);
}

bool BusDevice::check(Id& id) {
    return id.vendorId().unwrapOr(0xffff) != 0xffff;
}

bool BusDevice::check(u16 deviceId, u16 vendorId) {
    return false;
}

bool BusDevice::check(u8 classcode, u8 subclass) {
    return false;
}

Opt<Pci::Dev&> BusDevice::find(u16 deviceId, u16 vendorId) {
    return NONE;
}

Opt<Rc<Pci::Dev>> BusDevice::create(Id& id) {
    if (not check(id)) {
        return NONE;
    }

    return _devices
         | filter$(it.unwrap() == id)
         | first()
         | supply$(makeRc<Pci::Dev>(id));
}

} // namespace Pci
