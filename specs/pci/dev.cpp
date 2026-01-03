#include <pci/dev.h>
#include <pci/spec.h>
#include <sdk-meta/res.h>
#include <sdk-text/format.h>

namespace Pci {

using namespace Realms;
using namespace Sdk;

Res<usize> Id::in(usize offset, usize size) {
    // u32 addr = (0x8000'0000
    //             | (bus << 16)
    //             | (slot << 11)
    //             | (func << 8)
    //             | (offset & 0xfc));

    return Error::notImplemented();
}

Res<> Id::out(usize offset, usize size, usize val) {
    // u32 addr = (0x8000'0000
    //             | (bus << 16)
    //             | (slot << 11)
    //             | (func << 8)
    //             | (offset & 0xfc));

    return Error::notImplemented();
}

Dev::Dev(u8 bus, u8 slot, u8 func)
    : Id(bus, slot, func),
      Core::Io::Dev(
          Text::format("pci-dev-{:02x}-{:02x}-{:02x}", bus, slot, func),
          Text::format("/pci/{:02x}-{:02x}-{:02x}:ven={:04x},dev={:04x}",
                       bus,
                       slot,
                       func,
                       vendorId().unwrapOr(0xffff),
                       deviceId().unwrapOr(0xffff)),
          Core::Io::Dev::Type::Unknown) {
}

Dev::Dev(u8 bus, u8 slot, u8 func, u16 vendorId, u16 deviceId)
    : Dev(bus, slot, func) {
    _vendorId = vendorId;
    _deviceId = deviceId;
}

Dev::Dev(Id const& id) : Dev(id.bus, id.slot, id.func) {
}

Dev::Dev(Id const& id, String name, Realms::Sys::Io::Dev::Type type)
    : Id(id),
      Core::Io::Dev(
          name,
          Text::format("/pci/{:02x}-{:02x}-{:02x}", id.bus, id.slot, id.func),
          type) {
}

} // namespace Pci
