#pragma once

#include <realms/init/prekernel.h>
#include <realms/io/devtree.h>
#include <sdk-meta/res.h>

namespace Realms::Hal {

Res<> init(Core::PrekernelInfo* info);

} // namespace Realms::Hal
