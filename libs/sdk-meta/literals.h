#pragma once

#include <sdk-meta/types.h>

constexpr inline u64 operator""_KiB(u64 x) {
    return x * 1024;
}

constexpr inline u64 operator""_MiB(u64 x) {
    return x * 1024 * 1024;
}

constexpr inline u64 operator""_GiB(u64 x) {
    return x * 1024 * 1024 * 1024;
}

constexpr inline u64 operator""_TiB(u64 x) {
    return x * 1024 * 1024 * 1024 * 1024;
}

constexpr inline u64 operator""_PiB(u64 x) {
    return x * 1024 * 1024 * 1024 * 1024 * 1024;
}

constexpr inline u64 operator""_KB(u64 x) {
    return x * 1000;
}

constexpr inline u64 operator""_MB(u64 x) {
    return x * 1000 * 1000;
}

constexpr inline u64 operator""_GB(u64 x) {
    return x * 1000 * 1000 * 1000;
}

constexpr inline u64 operator""_TB(u64 x) {
    return x * 1000 * 1000 * 1000 * 1000;
}

constexpr inline u64 operator""_PB(u64 x) {
    return x * 1000 * 1000 * 1000 * 1000 * 1000;
}
