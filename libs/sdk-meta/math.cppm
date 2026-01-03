module;

export module sdk:math;

import :types;
import :limits;
import :traits;

inline constexpr f64 Pi  = 3.14159265358979323846;
inline constexpr f64 Tau = 6.28318530717958647692; // Tau = 2 * Pi
inline constexpr f64 E   = 2.71828182845904523536; // Euler's number

inline constexpr f64 DegToRad = Pi / 180.0;
inline constexpr f64 RadToDeg = 180.0 / Pi;

inline constexpr f64 GoldenRatio = 1.618033988749895;
inline constexpr f64 Sqrt2       = 1.41421356237309504880; // Square root of 2
inline constexpr f64 Sqrt3       = 1.73205080756887729352; // Square root of 3

inline constexpr f64 Log2E  = 1.44269504088896340736; // log base 2 of e
inline constexpr f64 Log10E = 0.43429448190325182765; // log base 10 of e

inline f64 const Nan    = 0.0 / 0.0;  // Not-a-Number (NaN)
inline f64 const PosInf = 1.0 / 0.0;  // Positive Infinity
inline f64 const NegInf = -1.0 / 0.0; // Negative Infinity

inline constexpr u32 HashPrime = 101;

export [[gnu::always_inline]] constexpr auto max(auto v) {
    return v;
}

export [[gnu::always_inline]] constexpr auto max(auto v, auto... vs) {
    auto rhs = max(vs...);
    return (v > rhs) ? v : rhs;
}

export [[gnu::always_inline]] constexpr auto min(auto v) {
    return v;
}

export [[gnu::always_inline]] constexpr auto min(auto v, auto... vs) {
    auto rhs = min(vs...);
    return (v < rhs) ? v : rhs;
}

export [[gnu::always_inline]] constexpr auto clamp(auto v,
                                                   auto min_v,
                                                   auto max_v) {
    return (v < min_v) ? min_v : (v > max_v) ? max_v : v;
}

export [[gnu::always_inline]] constexpr auto abs(auto v) {
    return (v < 0) ? -v : v;
}

// Align the nearest _lower_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 8
// ex: 7 with align = 8 -> 0
export [[gnu::always_inline]] constexpr usize alignDown(usize addr,
                                                        usize align) {
    return addr & ~(align - 1);
}

// Align the nearest _upper_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 8
export [[gnu::always_inline]] constexpr usize alignUp(usize addr, usize align) {
    return (addr + align - 1) & ~(align - 1);
}

export [[gnu::always_inline]] constexpr bool isAlign(usize addr, usize align) {
    return alignDown(addr, align) == addr;
}

export template <Signed T>
constexpr auto abs(T x) -> T {
    if (x == Limits<T>::MIN) {
        panic("abs(x): cannot take absolute value of Limits<T>::MIN");
    }
    return (x < T {}) ? -x : x;
}

export constexpr auto lerp(auto a, auto b, auto t) {
    if (t < 0 || t > 1) {
        panic("lerp(t): t must be in the range [0, 1]");
    }
    return a + (b - a) * t;
}

export constexpr auto isNan(Float auto x) -> bool {
    return __builtin_isnan(x);
}

export constexpr auto isInf(Float auto x) -> bool {
    return __builtin_isinf(x);
}

export constexpr auto isNegInf(Float auto x) -> bool {
    return __builtin_isinf(x) && x < 0;
}

export constexpr auto isPosInf(Float auto x) -> bool {
    return __builtin_isinf(x) && x > 0;
}

export constexpr isize floori(Arithmetic auto x) {
    using T = decltype(x);

    isize inum = (isize) x;
    if ((T) inum == x)
        return inum;
    if (x < T(0))
        return inum - 1;
    return inum;
}

export constexpr isize ceili(Arithmetic auto x) {
    using T = decltype(x);

    isize inum = (isize) x;
    if ((T) inum == x)
        return inum;
    if (x < T(0))
        return inum;
    return inum + 1;
}

export constexpr isize roundi(Arithmetic auto x) {
    using T = decltype(x);

    if (x < T(0))
        return (long) (x - T(.5));
    return (long) (x + T(.5));
}

export constexpr auto floor(Arithmetic auto x) -> decltype(x) {
    return static_cast<decltype(x)>(floori(x));
}

export constexpr auto ceil(Arithmetic auto x) -> decltype(x) {
    return static_cast<decltype(x)>(ceili(x));
}

export constexpr auto round(Arithmetic auto x) -> decltype(x) {
    return static_cast<decltype(x)>(roundi(x));
}

export constexpr auto sqrt(Float auto x) -> decltype(x) {
    // TODO: add SSE/AVX implementation
    // return __builtin_sqrt(x);
}

export constexpr auto rsqrt(Float auto x) -> decltype(x) {
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

export constexpr auto cos(Float auto x) -> decltype(x) {
    using T = decltype(x);

    constexpr T tp = 1. / (2. * Pi);
    x *= tp;
    x -= T(.25) + floor(x + T(.25));
    x *= T(16.) * (abs(x) - T(.5));
    return x;
}

export constexpr auto fcos(Float auto x) -> decltype(x) {
    using T = decltype(x);

    constexpr T tp = 1. / (2. * Pi);
    x *= tp;
    x -= T(.25) + floor(x + T(.25));
    x *= T(16.) * (abs(x) - T(.5));
    x += T(.225) * x * (abs(x) - T(1.));
    return x;
}

inline constexpr usize primes[] = {
    3,       7,       17,      37,       89,       197,      431,
    919,     1931,    4049,    8419,     1'7519,   3'6353,   7'5431,
    15'6437, 32'4449, 67'2827, 139'5263, 289'3249, 599'9471,
};

static bool isPrime(Integral auto candidate) {
    // TODO: add SSE/AVX implementation
    // if ((candidate bitand 1) != 0) {
    //     int limit = (int) sqrt(candidate);
    //     for (int divisor = 3; divisor <= limit; divisor += 2) {
    //         if ((candidate % divisor) == 0)
    //             return false;
    //     }
    //     return true;
    // }
    // return candidate == 2;
    return true; // --- IGNORE ---
}

export template <Integral T>
[[gnu::always_inline]] constexpr T nextPrime(T min) {

    for (auto prime : primes) {
        if (prime >= min)
            return prime;
    }

    for (int i = (min | 1); i < Limits<T>::MAX; i += 2) {
        if (isPrime(i) and ((i - 1) % HashPrime != 0))
            return i;
    }
    return Limits<T>::MAX; // No prime found, return max value
}
