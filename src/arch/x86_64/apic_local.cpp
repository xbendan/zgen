#include <arch/x86_64/apic.h>
#include <sdk-logs/logger.h>

namespace Zgen::Hal::x86_64::Apic {

Res<> Local::init() {
    u64 base;
    base = _device->read<Base>().unwrapOr(0xfee0'0000);
    base |= (1ul << 11);
    try$(_device->write<Base>(base));

    try$(write<Spurious>(try$(read<Spurious>()) | 0x1ff));
    return Ok();
}

Res<usize> Local::in(usize offset, usize size) {
    if (size != 4) {
        logWarn("apic_local::in: only 4-byte accesses are supported");
    }
    return Ok(*((u32 volatile*) (_vbase + offset)));
}

Res<> Local::out(usize offset, usize size, usize val) {
    if (size != 4) {
        logWarn("apic_local::out: only 4-byte accesses are supported");
    }
    *((u32 volatile*) (_vbase + offset)) = (u32) val;
    return Ok();
}

Res<> Local::send(u32 vec) {
    try$(write<IcrHigh>(((u32) _apicId) << 24));
    try$(write<IcrLow>(vec));

    while ((try$(read<IcrLow>()) & (u32) Message::Pending) != 0)
        ;
    return Ok();
}

Res<> Local::send(Dest dest, Message message, u8 vec) {
    u32 val = (u32) dest | (u32) message | (u32) vec;
    return send(val);
}

} // namespace Zgen::Hal::x86_64::Apic
