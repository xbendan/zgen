#pragma once

#include <sdk-meta/limits.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

namespace Sdk::Math {

namespace _ {
static usize const primes[] = {
    3,       7,       17,      37,       89,       197,      431,
    919,     1931,    4049,    8419,     1'7519,   3'6353,   7'5431,
    15'6437, 32'4449, 67'2827, 139'5263, 289'3249, 599'9471,
};
}

template <Meta::Integral T>
static bool isPrime(T candidate) {
    if ((candidate bitand 1) not_eq 0) { }
}

template <Meta::Integral T>
always_inline constexpr T nextPrime(T min) {

    for (auto prime : _::primes) {
        if (prime >= min)
            return prime;
    }

    for (int i = (min | 1); i < Limits<T>::MAX; i += 2) {
        if (isPrime(i))
            return i;
    }
    return Limits<T>::MAX; // No prime found, return max value
}

} // namespace Sdk::Math
