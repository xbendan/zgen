#include <acpi/device.h>
#include <sdk-logs/logger.h>
#include <sdk-meta/iter.h>
#include <zgen/hal/pmm.h>
#include <zgen/hal/vmm.h>
#include <zgen/mm/mem.h>

namespace Acpi {

static Str sign = "RSD PTR ";

Res<> ControlDevice::onInit() {
    auto addr
        = range<uflat>(0x0, 0x7c00, 0x10)
              .concat(range<uflat>(0x8'0000, 0xa'0000, 0x10))
              .concat(range<uflat>(0xe'0000, 0x10'0000, 0x10))
              .first$(strncmp((char const*) it, sign.buf(), sign.len()) == 0)
              .mapTo<uflat>();
    if (not addr) {
        return Error::notFound("acpi::onInit: no rsdp found");
    }

    //
    // Handle Acpi Revisions
    // When Revision
    //  ->  2:         = version 2.0 or higher
    //                   contains an extended Xsdp table needs to be casted first
    //  ->  0:         = version 1.0
    //  ->  otherwise: = unknown version
    //
    switch (Rsdp* p = addr.take(); revision = p->revision) {
        case 0: {
            logInfo(
                "acpi::onInit: found acpi 1.0 \n"
                "   - Rsdp at {:#x}\n"
                "   - Rsdt at {:#x}\n"
                "   - Oem ID: {}",
                addr,
                Str(p->oemId.buf(), 6));
            rsdt = Zgen::Core::mmapVirtIo(p->rsdt).take();
            break;
        }
        case 2: {
            auto* x = reinterpret_cast<Xsdp*>(p);
            logInfo(
                "acpi::onInit: found acpi 2.0+ \n"
                "    - Xsdp at {:#x}\n"
                "    - Xsdt at {:#x}\n"
                "    - Oem ID: {}",
                addr,
                (u64) x->xsdt,
                Str(x->oemId.buf(), 6));
            Xsdp* xsdp = Zgen::Core::mmapVirtIo(uflat(p)).take();
            rsdt       = Zgen::Core::mmapVirtIo(xsdp->rsdt).take();
            xsdt       = Zgen::Core::mmapVirtIo(xsdp->xsdt).take();
            break;
        }
        default: {
            logError("acpi::onInit: unknown acpi revision: {}, abort",
                     p->revision);
            return Error::unsupported("acpi::onInit: unknown acpi revision");
        }
    }
    status = Status::Initialized;

    return Ok();
}

Res<> ControlDevice::onStart() {
    return Error::notImplemented("acpi::onStart: not implemented");
}

Res<> ControlDevice::onStop() {
    return Error::notSupported(
        "acpi::onStop: acpi service must keep running during system lifetime, "
        "abort.");
}

Res<_Desc*> ControlDevice::lookupTable(Str name) {
    if (auto table = tables.iter().first$(it._key == name); table) {
        return Ok(table->_value);
    }

    if (name == "DSDT") {
        auto* fadt = try$(lookupTable("FACP"));
        uflat dsdt = try$(Zgen::Core::mmapVirtIo(((Acpi::Fadt*) fadt)->dsdt));
        return Ok(dsdt);
    }

    usize entries = revision
                      ? (xsdt->length - sizeof(Acpi::_Desc)) / sizeof(u64)
                      : (rsdt->length - sizeof(Acpi::_Desc)) / sizeof(u32);
    for (int i = 0; i < entries; i++) {
        u64 ent = revision ? xsdt->tables[i] : rsdt->tables[i];

        auto desc = (Acpi::_Desc*) try$(Zgen::Core::mmapVirtIo(ent));
        if (cstrEq(desc->sign.buf(), name.buf())) {
            logInfo("acpi::lookupTable: found table {} at {:#x}", name, ent);
            tables[name] = desc;

            return Ok(desc);
        }
    }

    logWarn("acpi::lookupTable: table {} not found", name);
    return Error::notFound("acpi::lookupTable: table not found");
}

} // namespace Acpi
