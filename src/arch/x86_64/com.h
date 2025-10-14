/**
 * @file com.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-08-31
 * 
 * @copyright Copyright (c) 2025
 * 
 * @ref wiki.osdev.org "https://wiki.osdev.org/Serial_Ports"
 */

#pragma once

#include <sdk-io/text.h>
#include <sdk-io/traits.h>
#include <zgen/hal/io.h>

namespace Zgen::Hal::x86_64 {

using Sdk::Io::TextEncoderBase;

struct Com : public TextEncoderBase<> {
    PortIo _io;

    enum Regs {
        DATA                    = 0,
        INTERRUPT               = 1,
        BAUD_RATE_LOW           = 0,
        BAUD_RATE_HIGH          = 1,
        INTERRUPT_IDENTIFICATOR = 2,
        FIFO_CONTROLLER         = 2,
        LINE_CONTROL            = 3,
        MODEM_CONTROL           = 4,
        LINE_STATUS             = 5,
        MODEM_STATUS            = 6,
        SCRATCH_REGISTER        = 7,
    };

    enum LineCtrls {
        DATA_SIZE_5 = 0,
        DATA_SIZE_6 = 1,
        DATA_SIZE_7 = 2,
        DATA_SIZE_8 = 3,
        DLAB_STATUS = 1 << 7,
    };

    enum ModemCtrls {
        DATA_TERMINAL_READY = 1 << 0,
        REQUEST_TO_SEND     = 1 << 1,
        AUX_OUTPUT_1        = 1 << 2,
        AUX_OUTPUT_2        = 1 << 3,
        LOOPBACK_MODE       = 1 << 4,
    };

    enum LineStatus {
        DATA_READY            = 1 << 0,
        OVERRUN_ERROR         = 1 << 1,
        PARITY_ERROR          = 1 << 2,
        FRAMING_ERROR         = 1 << 3,
        BREAK_INDICATOR       = 1 << 4,
        TRANSMITTER_BUF_EMPTY = 1 << 5,
        TRANSMITTER_EMPTY     = 1 << 6,
        IMPENDING_ERROR       = 1 << 7,
    };

    enum ModemStatus {
        DELTA_CLEAR_TO_SEND          = 1 << 0,
        DELTA_DATA_SET_READY         = 1 << 1,
        TRAILING_EDGE_RING_INDICATOR = 1 << 2,
        DELTA_CARRIER_DETECT         = 1 << 3,
        CLEAR_TO_SEND                = 1 << 4,
        DATA_SET_READY               = 1 << 5,
        RING_INDICATOR               = 1 << 6,
        CARRIER_DETECT               = 1 << 7,
    };

    enum Int {
        WHEN_DATA_AVAILABLE    = 1 << 0,
        WHEN_TRANSMITTER_EMPTY = 1 << 1,
        WHEN_BREAK_ERROR       = 1 << 2,
        WHEN_STATUS_UPDATE     = 1 << 3,
    };

    Com(PortIo io) : _io(io) { }

    Res<> init(u32 baud = 9600) {
        try$(writeReg(INTERRUPT, 0));

        // Turn on dlab for setting baud rate
        try$(writeReg(LINE_CONTROL, DLAB_STATUS));

        // Set bauds
        u32 div = 11'5200 / baud;
        try$(writeReg(BAUD_RATE_LOW, div & 0xff));
        try$(writeReg(BAUD_RATE_HIGH, (div >> 8) & 0xff));

        // we want 8bit caracters + clear dlab
        try$(writeReg(LINE_CONTROL, DATA_SIZE_8));

        // enable fifo, clear them, with 14-byte threshold
        try$(writeReg(FIFO_CONTROLLER, 0b1100'0111));

        // turn on communication + redirect UART interrupt into ICU
        try$(writeReg(MODEM_CONTROL,
                      DATA_TERMINAL_READY | REQUEST_TO_SEND | AUX_OUTPUT_2));

        // flush and test serial chip
        try$(writeReg(
            MODEM_CONTROL,
            REQUEST_TO_SEND | AUX_OUTPUT_1 | AUX_OUTPUT_2 | LOOPBACK_MODE));
        try$(writeReg(DATA, 0xAE));
        if (try$(readReg(DATA)) != 0xAE) {
            return Error::invalidData("Com::init: serial port not detected");
        }

        // not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled
        try$(writeReg(MODEM_CONTROL, 0x0f));
        try$(writeReg(INTERRUPT, WHEN_DATA_AVAILABLE));

        try$(writeStr("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"));

        return Ok();
    }

    Res<> writeReg(Regs reg, u8 value) { return _io.out8(reg, value); }

    Res<u8> readReg(Regs reg) { return _io.in8(reg); }

    Res<bool> canRead() {
        return Ok<bool>(try$(readReg(LINE_STATUS)) & DATA_READY);
    }

    Res<bool> canWrite() {
        return Ok<bool>(try$(readReg(LINE_STATUS)) & TRANSMITTER_BUF_EMPTY);
    }

    Res<> waitWrite() {
        while (not try$(canWrite())) { }
        return Ok();
    }

    Res<> waitRead() {
        while (not try$(canRead())) { }
        return Ok();
    }

    Res<usize> putByte(u8 c) {
        try$(waitWrite());
        try$(writeReg(DATA, static_cast<u8>(c)));

        return Ok(1uz);
    }

    Res<> write(byte b) override {
        try$(putByte(b));
        return Ok();
    }

    Res<usize> write(Bytes bytes) override {
        for (auto b : iter(bytes)) {
            try$(putByte(b));
        }

        return Ok(bytes.len());
    }
};

static constexpr Com com1() {
    return { PortIo({ 0x3f8, 8 }) };
}

static constexpr Com com2() {
    return { PortIo({ 0x2f8, 8 }) };
}

static constexpr Com com3() {
    return { PortIo({ 0x3e8, 8 }) };
}

static constexpr Com com4() {
    return { PortIo({ 0x2e8, 8 }) };
}

} // namespace Zgen::Hal::x86_64
