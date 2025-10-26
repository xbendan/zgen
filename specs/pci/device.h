#pragma once

#include <pci/spec.h>
#include <zgen/io/dev.h>

namespace Pci {

struct Dev : public Zgen::Core::Io::Dev, public Id {
    Dev(u8 bus, u8 slot, u8 func);
    Dev(u8 bus, u8 slot, u8 func, u16 vendorId, u16 deviceId);
    Dev(Id const& id);
    Dev(Id const& id, Str name, Zgen::Core::Io::Dev::Type type);

    Dev(Dev const&)            = delete;
    Dev& operator=(Dev const&) = delete;

    [[gnu::always_inline]] bool barIsMmio(u8 i) {
        return in32(Regs::Bar0 + i * 4).unwrapOr(0) & 0x1;
    }

    [[gnu::always_inline]] Res<u8> interruptLine() {
        return in8(Regs::InterruptLine);
    }

    [[gnu::always_inline]] Res<u8> interruptPin() {
        return in8(Regs::InterruptPin);
    }

    [[gnu::always_inline]] Res<u8> command() { return in8(Regs::Command); }

    [[gnu::always_inline]] Res<> command(u8 val) {
        return out8(Regs::Command, val);
    }

    [[gnu::always_inline]] Res<u8> status() { return in8(Regs::Status); }

    [[gnu::always_inline]] Res<u8> headerType() {
        return in8(Regs::HeaderType);
    }

    [[gnu::always_inline]] Res<> enableBusMastering() {
        return out8(Regs::Command,
                    try$(in8(Regs::Command)) | Command::BusMaster);
    }

    [[gnu::always_inline]] Res<> enableInterrupts() {
        return out8(Regs::Command,
                    try$(in8(Regs::Command)) & ~Command::IntDisable);
    }

    [[gnu::always_inline]] Res<> disableInterrupts() {
        return out8(Regs::Command,
                    try$(in8(Regs::Command)) | Command::IntDisable);
    }

    [[gnu::always_inline]] Res<> enableMemorySpace() {
        return out8(Regs::Command,
                    try$(in8(Regs::Command)) | Command::MemorySpace);
    }

    [[gnu::always_inline]] Res<> enableIoSpace() {
        return out8(Regs::Command, try$(in8(Regs::Command)) | Command::IoSpace);
    }
};
} // namespace Pci
