#pragma once

#include <sdk-math/const.h>
#include <sdk-meta/limits.h>
#include <sdk-meta/panic.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

namespace Sdk::Math {

template <Meta::Signed T>
constexpr auto abs(T x) -> T {
    if (x == Limits<T>::MIN) {
        panic("abs(x): cannot take absolute value of Limits<T>::MIN");
    }
    return (x < T {}) ? -x : x;
}

constexpr auto lerp(auto a, auto b, auto t) {
    if (t < 0 || t > 1) {
        panic("lerp(t): t must be in the range [0, 1]");
    }
    return a + (b - a) * t;
}

static constexpr auto isNan(Meta::Float auto x) -> bool {
    return __builtin_isnan(x);
}

static constexpr auto isInf(Meta::Float auto x) -> bool {
    return __builtin_isinf(x);
}

static constexpr auto isNegInf(Meta::Float auto x) -> bool {
    return __builtin_isinf(x) && x < 0;
}

static constexpr auto isPosInf(Meta::Float auto x) -> bool {
    return __builtin_isinf(x) && x > 0;
}

constexpr isize floori(Meta::Arithmetic auto x) {
    using T = decltype(x);

    isize inum = (isize) x;
    if ((T) inum == x)
        return inum;
    if (x < T(0))
        return inum - 1;
    return inum;
}

static constexpr isize ceili(Meta::Arithmetic auto x) {
    using T = decltype(x);

    isize inum = (isize) x;
    if ((T) inum == x)
        return inum;
    if (x < T(0))
        return inum;
    return inum + 1;
}

constexpr isize roundi(Meta::Arithmetic auto x) {
    using T = decltype(x);

    if (x < T(0))
        return (long) (x - T(.5));
    return (long) (x + T(.5));
}

constexpr auto floor(Meta::Arithmetic auto x) -> decltype(x) {
    return static_cast<decltype(x)>(floori(x));
}

constexpr auto ceil(Meta::Arithmetic auto x) -> decltype(x) {
    return static_cast<decltype(x)>(ceili(x));
}

constexpr auto round(Meta::Arithmetic auto x) -> decltype(x) {
    return static_cast<decltype(x)>(roundi(x));
}

constexpr auto sqrt(Meta::Float auto x) -> decltype(x) {
    return __builtin_sqrt(x);
}

constexpr auto rsqrt(Meta::Float auto x) -> decltype(x) {
    if (x == 0) {
        panic("rsqrt(x): cannot take reciprocal square root of zero");
    }
    using T = decltype(x);

    long    i;
    T       xhalf, y;
    T const threehalfs = T(1.5);

    xhalf = x * T(0.5);
    y     = x;
    i     = *(long*) &y; // Bit-level hacking
    i     = 0x5f37'59df - (i >> 1);
    y     = *(float*) &i;
    y     = y * (threehalfs - (xhalf * y * y)); // 1st iteration
    //    y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can
    //    be removed

    return y;
}

static constexpr auto cos(Meta::Float auto x) -> decltype(x) {
    using T = decltype(x);

    constexpr T tp = 1. / (2. * Pi);
    x *= tp;
    x -= T(.25) + floor(x + T(.25));
    x *= T(16.) * (abs(x) - T(.5));
    return x;
}

static constexpr auto fcos(Meta::Float auto x) -> decltype(x) {
    using T = decltype(x);

    constexpr T tp = 1. / (2. * Pi);
    x *= tp;
    x -= T(.25) + floor(x + T(.25));
    x *= T(16.) * (abs(x) - T(.5));
    x += T(.225) * x * (abs(x) - T(1.));
    return x;
}

} // namespace Sdk::Math
