#pragma once

#include <zgen/hal/intr.h>
#include <zgen/hal/io.h>

namespace Zgen::Hal::x86_64 {

struct Pic : public Hal::Intr {
    Hal::PortIo _io;

    static constexpr inline Hal::Reg<u8, 0> Command;
    static constexpr Hal::Reg<u8, 1>        Data = {};

    [[gnu::always_inline]] void wait() {
        __asm__ __volatile__("jmp 1f; 1: jmp 1f; 1:");
    }

    Res<> fin() override {
        try$(_io.out8(1, 0x20));
        return Ok();
    }

    Res<> pause() override {
        try$(_io.out8(1, 0x20));
        return Ok();
    }

    Res<> command(u8 cmd) { return _io.out8(0, cmd); }

    Res<> data(u8 data) { return _io.out8(1, data); }
};

} // namespace Zgen::Hal::x86_64
