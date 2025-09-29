#pragma once

#include <neodymium/io/device.h>
#include <sdk-meta/dict.h>
#include <sdk-meta/ref.h>
#include <sdk-text/str.h>
#include <specs/acpi/tables.h>

namespace Acpi {

using Sys::Io::Device;

struct ControlDevice : public Device {
    Dict<Str, _Desc*> tables;
    Rsdt*             rsdt;
    Xsdt*             xsdt;
    int               revision;

    /**
     * @brief Initialize the ACPI controller device.
     * 
     * @return Res<> 
     * @retval Ok<> on success
     * @retval Error::notFound if no Rsdp was found in memory.
     * @retval Error::unsupported if unknown revision was read.
     */
    Res<> onInit() override;

    Res<> onStart() override;

    Res<> onStop() override;

    Res<_Desc*> lookupTable(Str name);
};

} // namespace Acpi
