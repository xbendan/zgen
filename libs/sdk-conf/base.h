#pragma once

#include <sdk-io/traits.h>
#include <sdk-meta/box.h>
#include <sdk-meta/res.h>
#include <sdk-text/runes.h>

namespace Sdk::Conf {

using Text::Runes;

template <typename T>
concept Object = requires(T const& t) {

};

template <typename T>
concept Parser = requires(T const& t, Runes& rs, Io::Reader& reader) {
    typename T::Instance;
    { T::parse(rs) } -> Meta::Same<Res<Box<typename T::Instance>>>;
    { T::parse(reader) } -> Meta::Same<Res<Box<typename T::Instance>>>;
};

} // namespace Sdk::Conf
