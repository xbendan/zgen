#pragma once

#include <sdk-math/funcs.h>
#include <sdk-meta/limits.h>
#include <sdk-meta/math.h>
#include <sdk-meta/types.h>
#include <sdk-meta/utility.h>

namespace Sdk::Math {

template <typename T>
union Vec2 {
    struct {
        T x, y;
    };

    struct {
        T u, v;
    };

    struct {
        T width, height;
    };

    static Vec2<T> const Zero;

    static Vec2<T> const One;

    static Vec2<T> const Max;

    constexpr Vec2(T x, T y) : x(x), y(y) { }

    constexpr Vec2(T v) : x(v), y(v) { }

    constexpr Vec2(Vec2 const& other) : x(other.x), y(other.y) { }

    constexpr Vec2(Vec2&& other) : x(::move(other.x)), y(::move(other.y)) { }

    constexpr Vec2& operator=(Vec2 const& other) {
        x = other.x;
        y = other.y;
        return *this;
    }

    constexpr Vec2& operator=(Vec2&& other) {
        x = ::move(other.x);
        y = ::move(other.y);
        return *this;
    }

    constexpr Vec2 operator+(Vec2 const& other) const {
        return { x + other.x, y + other.y };
    }

    constexpr Vec2 operator-(Vec2 const& other) const {
        return { x - other.x, y - other.y };
    }

    constexpr Vec2 operator*(T scalar) const {
        return { x * scalar, y * scalar };
    }

    constexpr Vec2 operator/(T scalar) const {
        return { x / scalar, y / scalar };
    }

    constexpr Vec2& operator+=(Vec2 const& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vec2& operator-=(Vec2 const& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Vec2& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr Vec2& operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    template <typename U>
    constexpr auto operator-(Vec2<U> const& other) const
        -> Vec2<decltype(T {} - U {})> {
        return { x - other.x, y - other.y };
    }

    template <typename U>
    constexpr auto operator*(Vec2<U> const& other) const
        -> Vec2<decltype(T {} * U {})> {
        return { x * other.x, y * other.y };
    }

    template <typename U>
    constexpr auto operator/(Vec2<U> const& other) const
        -> Vec2<decltype(T {} / U {})> {
        return { x / other.x, y / other.y };
    }

    constexpr Vec2 operator-() const { return { -x, -y }; }

    constexpr T min() const { return (x < y) ? x : y; }

    constexpr T max() const { return (x > y) ? x : y; }

    constexpr Vec2<T> min(Vec2<T> const& other) const {
        return { ::min(x, other.x), ::min(y, other.y) };
    }

    constexpr Vec2<T> max(Vec2<T> const& other) const {
        return { ::max(x, other.x), ::max(y, other.y) };
    }

    constexpr T dot(Vec2<T> const& other) const {
        return x * other.x + y * other.y;
    }

    constexpr T cross(Vec2<T> const& other) const {
        return x * other.y - y * other.x;
    }

    constexpr T len() const { return sqrt(x * x + y * y); }

    constexpr T lenSqr() const { return x * x + y * y; }

    constexpr T distance(Vec2<T> const& other) const {
        return (*this - other).len();
    }

    constexpr T unit() const { return Vec2<T> { x, y } / len(); }

    constexpr T angle() const { return atan2(y, x); }

    constexpr T angle(Vec2<T> const& other) const {
        auto r    = unit().dot(other.unit());
        auto sign = (x * other.y < y * other.x) ? -1.0 : 1.0;
        return sign * acos(r);
    }

    constexpr Vec2<T> rotate(T angle) const {
        T cosA = cos(angle);
        T sinA = sin(angle);
        return { x * cosA - y * sinA, x * sinA + y * cosA };
    }

    constexpr Vec2<T> normal() const { return { -y, x }; }

    template <typename U>
    constexpr Vec2<U> cast() const {
        return { static_cast<U>(x), static_cast<U>(y) };
    }

    Vec2 floor() { return { Math::floor(x), Math::floor(y) }; }

    Vec2 ceil() { return { Math::ceil(x), Math::ceil(y) }; }

    Vec2 round() { return { Math::round(x), Math::round(y) }; }

    bool hasNan() const { return isNan(x) or isNan(y); }

    bool operator==(Vec2 const& other) const {
        return x == other.x and y == other.y;
    }
};

template <typename T>
Vec2<T> operator+(T const& lhs, Vec2<T> const& rhs) {
    return rhs + lhs;
}

template <typename T>
Vec2<T> operator-(T const& lhs, Vec2<T> const& rhs) {
    return rhs - lhs;
}

template <typename T>
Vec2<T> operator*(T const& lhs, Vec2<T> const& rhs) {
    return rhs * lhs;
}

template <typename T>
Vec2<T> operator/(T const& lhs, Vec2<T> const& rhs) {
    return rhs / lhs;
}

template <typename T>
constexpr Vec2<T> Vec2<T>::Zero = {};

template <typename T>
constexpr Vec2<T> Vec2<T>::One = { 1 };

template <typename T>
constexpr Vec2<T> Vec2<T>::Max = { Limits<T>::MAX };

using Vec2i = Vec2<isize>;

using Vec2u = Vec2<usize>;

using Vec2f = Vec2<f64>;

} // namespace Sdk::Math
