#pragma once

#include <acpi/tables.h>
#include <realms/core/mod.h>
#include <realms/io/bus.h>
#include <realms/io/dev.h>
#include <realms/io/drv.h>
#include <sdk-meta/dict.h>
#include <sdk-text/str.h>

namespace Acpi {
using namespace Realms::Sys;

struct BusDevice : public Mod, public Io::Bus, public Io::Dev {
    static constexpr Str  name       = "acpi-bus-device"s;
    static constexpr auto requisites = toArray({
        "core.pmm"s,
        "core.vmm"s,
        "core.io.bus"s,
    });

    // Dict<Str, Desc*> tables;
    Vec<Rc<Dev>> _devices;
    Rsdt*        _rsdt { nullptr };
    Xsdt*        _xsdt { nullptr };
    int          _revision { -1 };

    /**
     * @brief Initialize the ACPI controller device.
     * 
     * @return Res<> 
     * @retval Ok<> on success
     * @retval Error::notFound if no Rsdp was found in memory.
     * @retval Error::unsupported if unknown revision was read.
     */

    BusDevice() : Mod(), Io::Bus("acpi"s), Io::Dev("acpi-bus-device"s, ""s) { }

    virtual ~BusDevice() = default;

    Res<> onInit() override;

    Res<> onLoad() override;

    Res<> onUnload() override;

    Res<String> path(Rc<Dev> dev) override;

    Res<Slice<Rc<Io::Dev>>> probe() override;

    Res<Slice<Rc<Io::Dev>>> devices() override;

    Res<> remove(Rc<Dev> dev) override;

    Opt<Desc&> lookupTable(Str name);
};

} // namespace Acpi
