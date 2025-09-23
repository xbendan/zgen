#pragma once

#include <sdk-meta/str.h>
#include <sdk-meta/vec.h>
#include <sdk-text/runes.h>

namespace Sdk::Io {

struct Path {
    static constexpr auto sep = '/';

    Vec<String> comp;

    void normalize();

    String toString() const;

    String operator[](usize index) const { return comp[index]; }

    usize len() const { return comp.len(); }

    auto operator<=>(Path const& other) const = default;
};

} // namespace Sdk::Io
