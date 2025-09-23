#pragma once

#include <neodymium/io/device.h>
#include <specs/pci/info.h>

namespace Pci {

struct Device : public Sys::Io::Device, public Identifier { };
} // namespace Pci
