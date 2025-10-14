#include <zgen/hal/io.h>

namespace Zgen::Hal::Pmio {

Res<u8> in8(usize port) {
    u8 ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"((u16) port));
    return Ok(ret);
}

Res<> out8(usize port, u8 val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"((u16) port));
    return Ok();
}

Res<u16> in16(usize port) {
    u16 ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "Nd"((u16) port));
    return Ok(ret);
}

Res<> out16(usize port, u16 val) {
    asm volatile("outw %0, %1" : : "a"(val), "Nd"((u16) port));
    return Ok();
}

Res<u32> in32(usize port) {
    u32 ret;
    asm volatile("inl %1, %0" : "=a"(ret) : "Nd"((u16) port));
    return Ok(ret);
}

Res<> out32(usize port, u32 val) {
    asm volatile("outl %0, %1" : : "a"(val), "Nd"((u16) port));
    return Ok();
}

Res<u64> in64(usize port) {
    u64 ret { 0 };
    ret |= try$(in32(port));
    ret |= ((u64) try$(in32(port + 4)) << 32);
    return Ok(ret);
}

Res<> out64(usize port, u64 val) {
    try$(out32(port, (u32) (val & 0xFFFF'FFFF)));
    try$(out32(port + 4, (u32) ((val >> 32) & 0xFFFF'FFFF)));
    return Ok();
}

} // namespace Zgen::Hal::Pmio
