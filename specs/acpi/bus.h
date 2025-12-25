#pragma once

#include <acpi/tables.h>
#include <realms/io/bus.h>
#include <realms/io/dev.h>
#include <realms/io/drv.h>
#include <sdk-meta/dict.h>
#include <sdk-meta/ref.h>
#include <sdk-text/str.h>

namespace Acpi {
using namespace Realms::Core;

struct BusDevice : public Io::Bus, public Io::Dev {
    // Dict<Str, Desc*> tables;
    Vec<Rc<Dev>> _devices;
    Rsdt*        _rsdt;
    Xsdt*        _xsdt;
    int          _revision;

    /**
     * @brief Initialize the ACPI controller device.
     * 
     * @return Res<> 
     * @retval Ok<> on success
     * @retval Error::notFound if no Rsdp was found in memory.
     * @retval Error::unsupported if unknown revision was read.
     */

    BusDevice() : Io::Bus("acpi"s), Io::Dev("acpi-bus-device"s, ""s) { }

    Res<> onInit() override;

    Res<String> path(Rc<Dev> dev) override;

    Res<Slice<Rc<Io::Dev>>> probe() override;

    Res<Slice<Rc<Io::Dev>>> devices() override;

    Res<> remove(Rc<Dev> dev) override;

    Opt<Desc&> lookupTable(Str name);
};

} // namespace Acpi
