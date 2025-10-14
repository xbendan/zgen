#pragma once

#include <sdk-meta/res.h>
#include <zgen/init/prekernel.h>

namespace Zgen::Hal {

Res<> init(Core::PrekernelInfo* info);

}
