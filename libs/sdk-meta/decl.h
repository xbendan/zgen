#pragma once

namespace Meta {

template <typename T>
constexpr T
declval() noexcept;

template <typename T>
constexpr T&
declref() noexcept;

} // namespace Meta
