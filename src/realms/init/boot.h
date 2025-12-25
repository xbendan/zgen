#pragma once

#include <realms/init/prekernel.h>
#include <realms/io/devtree.h>
#include <sdk-meta/array.h>
#include <sdk-meta/res.h>
#include <sdk-meta/tuple.h>
#include <sdk-meta/types.h>
#include <sdk-text/str.h>

namespace Realms::Core {

Res<> validateAndDumpInfo(usize magic, PrekernelInfo* info);

enum struct Firmware {
    Unknown,
    Bios,
    Uefi32,
    Uefi64,
    Uefi64Csm,
    Sbi
};

Res<> setupMemory(PrekernelInfo*);

Res<usize> setupMultitasking();

Io::Devtree& createDevtree();

Res<> main(u64, PrekernelInfo*);

} // namespace Realms::Core

