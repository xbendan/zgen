#pragma once

#include <acpi/tables.h>
#include <pci/dev.h>
#include <pci/spec.h>
#include <realms/io/bus.h>
#include <realms/io/dev.h>
#include <sdk-meta/list.h>
#include <sdk-meta/vec.h>

namespace Pci {

using namespace Realms::Sys;

struct BusDevice : public Io::Bus, public Io::Dev {
    Vec<Rc<Pci::Dev>>       _devices = {};
    Vec<Acpi::Mcfg::Packet> _packets = {};
    Pci::Mode               _mode;

    BusDevice();
    virtual ~BusDevice();

    Res<> onInit() override;

    Res<String> path(Rc<Io::Dev> dev) override;

    Res<Slice<Rc<Dev>>> probe() override;

    Res<Slice<Rc<Dev>>> devices() override;

    bool check(u8 bus, u8 slot, u8 func);

    bool check(Id& id);

    bool check(u16 deviceId, u16 vendorId);

    bool check(u8 classcode, u8 subclass);

    Opt<Pci::Dev&> find(u16 deviceId, u16 vendorId);

    Opt<Rc<Pci::Dev>> create(Id& id);

    Res<> remove(Rc<Io::Dev> dev) override;
};

} // namespace Pci
