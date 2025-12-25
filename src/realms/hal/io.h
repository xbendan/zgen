#pragma once

#include <sdk-meta/range.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>

namespace Realms::Hal {

template <typename T, usize _OFF>
struct Reg {
    using Type                 = T;
    static constexpr usize OFF = _OFF;
};

struct Io {
    virtual ~Io() = default;

    virtual Res<usize> in(usize offset, usize size) = 0;

    virtual Res<> out(usize offset, usize size, usize val) = 0;

    virtual Res<> read(usize offset, Bytes buf) {
        for (usize i = 0; i < buf.len(); i++) {
            buf[i] = try$(in(offset + i, 1));
        }
        return Ok();
    }

    virtual Res<> write(usize offset, Bytes buf) {
        for (usize i = 0; i < buf.len(); i++) {
            try$(out(offset + i, 1, buf[i]));
        }
        return Ok();
    }

    template <typename R>
    Res<typename R::Type> read() {
        return Ok(
            (typename R::Type)(try$(in(R::OFF, sizeof(typename R::Type)))));
    }

    template <typename R>
    Res<> write(typename R::Type value) {
        return out(R::OFF, sizeof(typename R::Type), (usize) value);
    }

    Res<u8> in8(usize offset) { return Ok((u8) try$(in(offset, 1))); }

    Res<> out8(usize offset, u8 value) { return out(offset, 1, value); }

    Res<u16> in16(usize offset) { return Ok((u16) try$(in(offset, 2))); }

    Res<> out16(usize offset, u16 value) { return out(offset, 2, value); }

    Res<u32> in32(usize offset) { return Ok((u32) try$(in(offset, 4))); }

    Res<> out32(usize offset, u32 value) { return out(offset, 4, value); }

    Res<u64> in64(usize offset) { return Ok((u64) try$(in(offset, 8))); }

    Res<> out64(usize offset, u64 value) { return out(offset, 8, value); }
};

using PortRange = Range<usize, struct _PortRangeTag>;

using MmioRange = Range<usize, struct _MmioRangeTag>;

namespace Pmio {

Res<u8> in8(usize);

Res<> out8(usize, u8);

Res<u16> in16(usize);

Res<> out16(usize, u16);

Res<u32> in32(usize);

Res<> out32(usize, u32);

Res<u64> in64(usize);

Res<> out64(usize, u64);

} // namespace Pmio

struct PortIo : public Io {
    PortRange _range;

    PortIo(PortRange range) : _range(range) { }

    Res<usize> in(usize offset, usize size) override {
        u16 addr = _range._start + offset;

        if (not _range.contains(addr)) {
            return Error::outOfBounds("PortIo::in: address out of bounds");
        }

        switch (size) {
            case 1:  return Pmio::in8(addr).map<usize>();
            case 2:  return Pmio::in16(addr).map<usize>();
            case 4:  return Pmio::in32(addr).map<usize>();
            case 8:  return Pmio::in64(addr).map<usize>();
            default: return Error::invalidArgument("PortIo::in: invalid size");
        }
    }

    Res<> out(usize offset, usize size, usize val) override {
        u16 addr = _range._start + offset;

        if (not _range.contains(addr)) {
            return Error::outOfBounds("PortIo::out: address out of bounds");
        }

        switch (size) {
            case 1:  return Pmio::out8(addr, (u8) val);
            case 2:  return Pmio::out16(addr, (u16) val);
            case 4:  return Pmio::out32(addr, (u32) val);
            case 8:  return Pmio::out64(addr, (u64) val);
            default: return Error::invalidArgument("PortIo::out: invalid size");
        }
    }
};

struct Mmio : public Io {
    MmioRange _range;

    Res<usize> in(usize offset, usize size) override {
        usize addr = _range._start + offset;

        if (not _range.contains(addr)) {
            return Error::outOfBounds("Mmio::in: address out of bounds");
        }

        switch (size) {
            case 1:  return Ok(*(u8 volatile*) addr);
            case 2:  return Ok(*(u16 volatile*) addr);
            case 4:  return Ok(*(u32 volatile*) addr);
            case 8:  return Ok(*(u64 volatile*) addr);
            default: return Error::invalidArgument("Mmio::in: invalid size");
        }
    }

    Res<> out(usize offset, usize size, usize val) override {
        usize addr = _range._start + offset;

        if (not _range.contains(addr)) {
            return Error::outOfBounds("Mmio::out: address out of bounds");
        }

        switch (size) {
            case 1:  *(u8 volatile*) addr = (u8) val; return Ok();
            case 2:  *(u16 volatile*) addr = (u16) val; return Ok();
            case 4:  *(u32 volatile*) addr = (u32) val; return Ok();
            case 8:  *(u64 volatile*) addr = (u64) val; return Ok();
            default: return Error::invalidArgument("Mmio::out: invalid size");
        }
    }

    Res<> read(usize offset, Bytes buf) override {
        usize addr = _range._start + offset;
        if (not _range.contains(addr))
            return Error::invalidInput("out of range");

        for (usize i = 0; i < buf.len(); i++) {
            buf[i] = *((u8 volatile*) (addr + i));
        }

        return Ok();
    }

    Res<> write(usize offset, Bytes buf) override {
        usize addr = _range._start + offset;
        if (not _range.contains(addr))
            return Error::invalidInput("out of range");

        for (usize i = 0; i < buf.len(); i++) {
            *((u8 volatile*) (addr + i)) = buf[i];
        }

        return Ok();
    }
};

} // namespace Realms::Hal
