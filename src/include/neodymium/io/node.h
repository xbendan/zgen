#pragma once

#include <neodymium/io/device.h>
#include <sdk-io/path.h>
#include <sdk-meta/rc.h>

namespace Sys::Io {

using Sdk::Io::Path;

struct Node {
    Rc<Device> device;
    Path       path;
};

} // namespace Sys::Io
