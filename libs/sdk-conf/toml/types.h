#pragma once

#include <sdk-meta/buf.h>
#include <sdk-meta/dict.h>
#include <sdk-meta/res.h>
#include <sdk-meta/slice.h>
#include <sdk-meta/union.h>
#include <sdk-text/str.h>

namespace Sdk::Conf::Toml {

struct Object;

using Array = Slice<Object>;

using InlineTable = Dict<Str, Object>;

struct Object {
    using Inner = Union<usize, double, bool, Str, Array>;

    Inner _inner;

    template <Meta::Contains<usize, double, bool, Str, Array> T>
    [[gnu::always_inline]] constexpr Res<T> as() const {
        if (not _inner.template is<T>()) [[unlikely]] {
            return Error::invalidData("Object::as: type mismatch");
        }
    };
};

} // namespace Sdk::Conf::Toml
