#pragma once

#include <neodymium/init/prekernel.h>
#include <sdk-meta/array.h>
#include <sdk-meta/res.h>
#include <sdk-meta/tuple.h>
#include <sdk-meta/types.h>
#include <sdk-text/str.h>

namespace Sys {

Res<> validateAndDumpInfo(usize magic, PrekernelInfo* info);

enum struct Firmware {
    Unknown,
    Bios,
    Uefi32,
    Uefi64,
    Uefi64Csm,
    Sbi
};

Res<> initArch(PrekernelInfo*);

Res<> setupMemory(PrekernelInfo*);

Res<> main(u64, PrekernelInfo*);

} // namespace Sys

