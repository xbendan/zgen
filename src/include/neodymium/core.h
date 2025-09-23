#pragma once

#include <sdk-io/traits.h>
#include <sdk-meta/rc.h>

namespace Sys {

using namespace Sdk;

Rc<Io::Reader> in();

Rc<Io::Writer> out();

Rc<Io::Writer> err();

} // namespace Sys
