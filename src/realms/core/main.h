#pragma once

#include <boot/info.h>
#include <realms/mm/mem.h>
#include <sdk-meta/ranges.h>
#include <sdk-meta/res.h>

namespace Realms::Core {

Res<> validateAndDumpInfo(u64 magic, Boot::Info& info);

Res<> setupArch();

Res<> setupMemory(Ranges<MemoryRange> auto const& ranges);

Res<usize> setupMultitasking();

Res<> main(Boot::Info&);

} // namespace Realms::Core
