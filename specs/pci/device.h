#pragma once

#include <specs/pci/info.h>
#include <zgen/io/device.h>

namespace Pci {

struct Device : public Zgen::Core::Io::Device, public Identifier { };
} // namespace Pci
