#pragma once

#include <acpi/tables.h>
#include <sdk-meta/dict.h>
#include <sdk-meta/ref.h>
#include <sdk-text/str.h>
#include <zgen/io/dev.h>

namespace Acpi {

using Zgen::Core::Io::Dev;

struct ControlDevice : public Dev {
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
