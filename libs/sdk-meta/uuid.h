#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/panic.h>
#include <sdk-meta/types.h>

union Uuid {
    struct [[gnu::packed]] {
        u32 low;
        u16 mid;
        u16 hi;
        u16 hi2;
        u64 dat: 48;
    };
    Array<u8, 16> bytes;
    Array<u64, 2> quad;

    static constexpr Uuid none() { return Uuid(); }

    constexpr Uuid() : low(0), mid(0), hi(0), hi2(0), dat(0) { }

    constexpr Uuid(u32 low, u16 mid, u16 hi, u16 hi2, u64 dat)
        : low(low),
          mid(mid),
          hi(hi),
          hi2(hi2),
          dat(dat) {
        if (dat >= (1ULL << 48)) [[unlikely]] {
            // panic("Uuid::Uuid: dat out of range: {} >= {}", dat, 1ULL << 48);
        }
    }

    constexpr Uuid(Uuid const&) = default;

    constexpr Uuid(Uuid&&) = default;

    constexpr ~Uuid() = default;

    constexpr bool operator==(Uuid const& other) const {
        return quad[0] == other.quad[0] && quad[1] == other.quad[1];
    }

    constexpr auto operator<=>(Uuid const&) const = delete;
};
