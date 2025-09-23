#pragma once

#include <sdk-meta/res.h>
#include <sdk-text/fmt/base.h>
#include <sdk-text/runes.h>
#include <sdk-text/str.h>

namespace Sdk::Text {

enum struct Case {
    DEFAULT,
    CAMEL,
    CAPITAL,
    CONSTANT,
    DOT,
    HEADER,
    NO,
    PARAM,
    PASCAL,
    PATH,
    SENTENCE,
    SNAKE,
    TITLE,
    SWAP,
    LOWER,
    LOWER_FIRST,
    UPPER,
    UPPER_FIRST,
    SPONGE,
};

Res<String> toDefaultCase(Str str);

Res<String> toCamelCase(Str str);

Res<String> toCapitalCase(Str str);

Res<String> toConstantCase(Str str);

Res<String> toDotCase(Str str);

Res<String> toHeaderCase(Str str);

Res<String> toNoCase(Str str);

Res<String> toParamCase(Str str);

Res<String> toPascalCase(Str str);

Res<String> toPathCase(Str str);

Res<String> toSentenceCase(Str str);

Res<String> toSnakeCase(Str str);

Res<String> toTitleCase(Str str);

Res<String> toSwapCase(Str str);

Res<String> toLowerCase(Str str);

Res<String> toLowerFirstCase(Str str);

Res<String> toUpperCase(Str str);

Res<String> toUpperFirstCase(Str str);

Res<String> toSpongeCase(Str str);

Res<String> changeCase(Str str, Case toCase);

template <typename T>
struct Cased {
    T    _inner;
    Case _case;
};

inline auto cased(auto inner, Case cased) {
    return Cased<decltype(inner)> { inner, cased };
}

template <typename T>
struct Formatter<Cased<T>> {
    Formatter<T> _innerFmt {};

    void parse(Runes& rs) {
        if constexpr (requires() { _innerFmt.parse(rs); }) {
            _innerFmt.parse(rs);
        }
    }

    Res<> format(Io::TextWriter& writer, Cased<T> val) {
        Io::StringWriter sw;
        try$(_innerFmt.format(sw, val._inner));
        String result = try$(changeCase(sw.str(), val._case));
        try$(writer.writeStr(result.str()));
        return Ok();
    }
};

} // namespace Sdk::Text
