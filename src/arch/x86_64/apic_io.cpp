#include <arch/x86_64/apic.h>
#include <sdk-meta/res.h>

namespace Zgen::Hal::x86_64::Apic {

Res<usize> GenericDevice::in(usize offset, usize size) {
    switch (size) {
        case 4:  return in32(offset).map<usize>();
        case 8:  return in64(offset).map<usize>();
        default: return Error::invalidArgument("invalid value size");
    }
}

Res<> GenericDevice::out(usize offset, usize size, usize val) {
    switch (size) {
        case 4:  try$(out32(offset, (u32) val)); break;
        case 8:  try$(out64(offset, (u64) val)); break;
        default: return Error::invalidArgument("invalid value size");
    }
    return Error::unknown();
}

Res<u32> GenericDevice::in32(usize offset) {
    *_io.sel = offset;
    return Ok(*_io.win);
}

Res<> GenericDevice::out32(usize offset, u32 value) {
    *_io.sel = offset;
    *_io.win = value;
    return Ok();
}

Res<u64> GenericDevice::in64(usize offset) {
    u32 lo = in32(offset).unwrap();
    u32 hi = in32(offset + 4).unwrap();
    return Ok(((u64) hi << 32) | lo);
}

Res<> GenericDevice::out64(usize offset, u64 value) {
    u32 lo = value & 0xFFFF'FFFF, hi = value >> 32;

    out32(offset, lo).unwrap();
    out32(offset + 1, hi).unwrap();
    return Ok();
}

} // namespace Zgen::Hal::x86_64::Apic
