#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/endian.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>
#include <sdk-text/runes.h>
#include <sdk-text/str.h>

namespace Sdk::Net {

union IPv4 {
    struct {
        byte a, b, c, d;
    };
    Array<byte, 4> bytes;
    u32be          r;

    constexpr IPv4(byte a, byte b, byte c, byte d) : a(a), b(b), c(c), d(d) { }

    constexpr IPv4(Array<byte, 4> bytes) : bytes(bytes) { }

    constexpr static auto unknown() -> IPv4 { return { 0, 0, 0, 0 }; }

    constexpr static auto localhost() -> IPv4 { return { 127, 0, 0, 1 }; }

    constexpr static auto broadcast() -> IPv4 { return { 255, 255, 255, 255 }; }

    static Res<IPv4> parse(Text::Runes& rs);

    static Res<IPv4> parse(Str str);

    constexpr auto operator<=>(IPv4 const& other) const {
        return r <=> other.r;
    }

    constexpr bool operator==(IPv4 const& other) const { return r == other.r; }
};

union IPv6 {
    struct {
        word a, b, c, d, e, f, g, h;
    };
    Array<word, 8> words;
    Be<u128>       r;

    IPv6(word a, word b, word c, word d, word e, word f, word g, word h)
        : a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h) { }

    IPv6(Array<word, 8> words) : words(words) { }

    static constexpr auto unknown() -> IPv6 {
        return { 0, 0, 0, 0, 0, 0, 0, 0 };
    }

    static constexpr auto localhost() -> IPv6 {
        return { 0, 0, 0, 0, 0, 0, 0, 1 };
    }

    static Res<IPv6> parse(Text::Runes& rs);

    static Res<IPv6> parse(Str str);

    constexpr auto operator<=>(IPv6 const& other) const {
        return r <=> other.r;
    }

    constexpr bool operator==(IPv6 const& other) const { return r == other.r; }
};

struct IP : Union<IPv4, IPv6> {
    using Union<IPv4, IPv6>::Union;

    static Res<IP> parse(Text::Runes& rs);

    static Res<IP> parse(Str str);
};

struct Socket {
    IP  addr;
    u16 port;

    Socket(IP addr, u16 port) : addr(addr), port(port) { }

    static Res<Socket> parse(Text::Runes& rs);

    static Res<Socket> parse(Str str);

    auto operator<=>(Socket const&) const = default;

    bool operator==(Socket const&) const = default;
};

} // namespace Sdk::Net
