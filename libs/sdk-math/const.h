#pragma once

#include <sdk-meta/types.h>

namespace Sdk::Math {
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
} // namespace Sdk::Math
