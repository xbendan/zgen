#pragma once

#include <sdk-io/text.h>
#include <sdk-meta/range.h>
#include <sdk-meta/slice.h>
#include <sdk-text/fmt/base.h>
#include <sdk-text/runes.h>

namespace Sdk::Text {

template <Sliceable T>
struct Formatter<T> {
    Formatter<typename T::Inner> inner;

    void parse(Runes& rs) {
        if constexpr (requires() { inner.parse(rs); }) {
            inner.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, T const& val) {
        try$(writer.writeStr("["s));
        for (usize i = 0; i < val.len(); i++) {
            if (i != 0)
                try$(writer.writeStr(", "s));
            try$(inner.format(writer, val[i]));
        }
        return Ok(try$(writer.writeStr("]"s)));
    }
};

template <typename T, typename Tag>
struct Formatter<Range<T, Tag>> {

    Formatter<T> inner;

    void parse(Runes& rs) {
        if constexpr (requires() { inner.parse(rs); }) {
            inner.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Range<T, Tag> const& val) {
        try$(writer.writeStr("["s));
        try$(inner.format(writer, val.start));
        try$(writer.writeStr("-"s));
        try$(inner.format(writer, val.end()));
        try$(writer.writeStr("]"s));
        return Ok();
    }
};

} // namespace Sdk::Text
