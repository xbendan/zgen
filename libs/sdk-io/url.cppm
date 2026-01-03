module;

export module sdk.io:url;

import sdk;
import sdk.text;
import :path;

namespace Realms::Io {

struct Url {
    String   scheme;
    String   auth;
    String   host;
    Opt<u16> port;
    Path     path;
    String   query;
    String   fragment;
};
static_assert(sizeof(Url) == 112);

} // namespace Realms::Io
