module;

import Boot.Limine;

#include <boot/builder.h>
#include <boot/info.h>
#include <realms/core/main.h>
#include <realms/hal/vmm.h>
#include <sdk-meta/error.h>
#include <sdk-meta/match.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>

extern "C" u64 __kernel_start;
extern "C" u64 __kernel_end;

InlineBuf<u8, 0x1000> build;

extern "C" [[noreturn]] void kinit_limine(void) {
    Builder<Boot::Info> bootInfo(bytes(build));

    if (not Limine::parse(bootInfo)) {
        panic("Failed to resolve boot config");
    }

    if (not Realms::Sys::main(bootInfo.build())) {
        // BSOD

        panic("System exited unexpectedly");
    }
    __builtin_unreachable();
}

namespace Limine {

// namespace {
// Limine::Entry ep = {
//     .id       = LIMINE_ENTRY_POINT_REQUEST,
//     .revision = 0,
//     .response = nullptr,
//     .entry    = &kinit_limine,
// };

} // namespace Limine

