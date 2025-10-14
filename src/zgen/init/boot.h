#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/res.h>
#include <sdk-meta/tuple.h>
#include <sdk-meta/types.h>
#include <sdk-text/str.h>
#include <zgen/init/prekernel.h>

namespace Zgen::Core {

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

Res<> main(u64, PrekernelInfo*);

} // namespace Zgen::Core

