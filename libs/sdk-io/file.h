#pragma once

#include <sdk-io/traits.h>
#include <sdk-meta/rc.h>

namespace Sdk::Io {

struct File : Io::Seeker, Io::Flusher, Meta::Uncopyable { };

} // namespace Sdk::Io
