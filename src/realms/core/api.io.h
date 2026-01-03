#pragma once

#include <realms/io/dev.h>
#include <realms/io/devtree.h>
#include <sdk-meta/list.h>

namespace Realms::Sys {

Opt<Io::Devtree&> devtree();

} // namespace Realms::Sys
