module;

export module sdk.io:path;

import sdk;
import sdk.text;

namespace Realms::Io {

using Meta::Vec;
using Text::String;

struct Path {
    static constexpr auto sep = '/';

    Vec<String> comp;

    void normalize();

    String toString() const;

    String operator[](usize index) const { return comp[index]; }

    usize len() const { return comp.len(); }
};

} // namespace Realms::Io
