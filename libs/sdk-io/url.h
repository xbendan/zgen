#pragma once

#include <sdk-io/path.h>
#include <sdk-meta/opt.h>
#include <sdk-text/str.h>

namespace Sdk::Io {

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

} // namespace Sdk::Io
