#include <acpi/bus.h>
#include <realms/hal/pmm.h>
#include <realms/hal/vmm.h>
#include <realms/mm/mem.h>
#include <sdk-logs/logger.h>
#include <sdk-meta/iter.h>

namespace Acpi {

static Str sign = "RSD PTR ";

Res<> BusDevice::onInit() {
    auto addr
        = range<u64>(0x0, 0x7c00, 0x10)
              .concat(range<u64>(0x8'0000, 0xa'0000, 0x10))
              .concat(range<u64>(0xe'0000, 0x10'0000, 0x10))
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
    switch (Rsdp* p = addr.take(); _revision = p->revision) {
        case 0: {
            logInfo(
                "acpi::onInit: found acpi 1.0 \n"
                "   - Rsdp at {:#x}\n"
                "   - Rsdt at {:#x}\n"
                "   - Oem ID: {}",
                addr,
                Str(p->oemId));
            _rsdt = Realms::Core::mmapVirtIo(p->rsdt).take();
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
            Xsdp* xsdp = Realms::Core::mmapVirtIo(uflat(p)).take();
            _rsdt      = Realms::Core::mmapVirtIo(xsdp->rsdt).take();
            _xsdt      = Realms::Core::mmapVirtIo(xsdp->xsdt).take();
            break;
        }
        default: {
            logError("acpi::onInit: unknown acpi revision: {}, abort",
                     p->revision);
            return Error::unsupported("acpi::onInit: unknown acpi revision");
        }
    }

    return Ok();
}

Res<String> BusDevice::path(Rc<Io::Dev> dev) {
    return Error::notImplemented();
}

Res<Slice<Rc<Io::Dev>>> BusDevice::probe() {
    // TODO: implement ACPI device probing
    return Error::notImplemented();
}

Res<Slice<Rc<Io::Dev>>> BusDevice::devices() {
    return Error::notImplemented();
}

Res<> BusDevice::remove(Rc<Dev> dev) {
    return Error::notImplemented();
}

Opt<Desc&> BusDevice::lookupTable(Str name) {
    // if (auto table = tables.iter().first$(it._key == name); table) {
    //     return table->_value;
    // }

    if (name == "DSDT") {
        auto fadt = lookupTable("FACP");
        if (not fadt)
            return NONE;

        return Realms::Core::mmapVirtIo((fadt->as<Acpi::Fadt>()->dsdt))
            .take()
            .as<Acpi::Desc>();
    }

    usize entries = _revision
                      ? (_xsdt->length - sizeof(Acpi::Desc)) / sizeof(u64)
                      : (_rsdt->length - sizeof(Acpi::Desc)) / sizeof(u32);
    for (int i = 0; i < entries; i++) {
        u64 ent = _revision ? _xsdt->tables[i] : _rsdt->tables[i];

        auto* desc = (Acpi::Desc*) try$(Realms::Core::mmapVirtIo(ent));
        if (cstrEq(desc->sign.buf(), name.buf())) {
            logInfo("acpi::lookupTable: found table {} at {:#x}", name, ent);
            // tables[name] = desc;

            return desc;
        }
    }

    logWarn("acpi::lookupTable: table {} not found", name);
    return NONE;
}

} // namespace Acpi
