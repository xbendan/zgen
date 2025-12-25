#include <arch/x86_64/apic.h>
#include <sdk-meta/res.h>

namespace Realms::Hal::x86_64::Apic {

uflat         base, vbase;
u32           intr;
u32 volatile* sel;
u32 volatile* win;

Res<u32> in32(usize offset) {
    *sel = offset;
    return Ok(*win);
}

Res<> out32(usize offset, u32 value) {
    *sel = offset;
    *win = value;
    return Ok();
}

Res<u64> in64(usize offset) {
    u32 lo = try$(in32(offset));
    u32 hi = try$(in32(offset + 4));
    return Ok(((u64) hi << 32) | lo);
}

Res<> out64(usize offset, u64 value) {
    u32 lo = value & 0xFFFF'FFFF, hi = value >> 32;

    try$(out32(offset, lo));
    try$(out32(offset + 4, hi));
    return Ok();
}

} // namespace Realms::Hal::x86_64::Apic
