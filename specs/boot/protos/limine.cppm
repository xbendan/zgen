module;
/* BSD Zero Clause License */

/* Copyright (C) 2022-2025 Mintsuki and contributors.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <boot/builder.h>
#include <boot/info.h>
#include <sdk-meta/array.h>
#include <sdk-meta/flags.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>
#include <sdk-meta/uuid.h>

export module Boot.Limine;

export namespace Limine {

constexpr u32 Revision = 3;
#if LIMINE_API_REVISION > 3
#    error "limine.h API revision unsupported"
#endif

#define CommonMagic 0xc7b1'dd30'df4c'8b88, 0x0a82'e883'a194'f07b

constexpr Array<u64, 4> RequestStartMarker = {
    0xf6b8'f4b3'9de7'd1ae,
    0xfab9'1a69'40fc'b9cf,
    0x785c'6ed0'15d3'e316,
    0x181e'920a'7852'b9d9,
};

constexpr Array<u64, 2> RequestEndMarker = {
    0xadc0'e053'1bb1'0d03,
    0x9572'709f'3176'4c62,
};

constexpr Array<u64, 3> BaseRevision = {
    0xf956'2b2d'5c95'a6c8,
    0x6a7b'3849'4453'6bdc,
    Revision,
};

constexpr bool BaseRevisionSupported() {
    return BaseRevision[2] == 0;
}

enum struct Media : u32 {
    Generic,
    Optical,
    Tftp
};

struct File {
    u64   revision;
    void* address;
    u64   size;
    char* path;
    char* string;

    Media media;
    u32   unused;
    u32   tftpIp;
    u32   tftpPort;
    u32   partitionIndex;
    u32   mbrDiskId;
    Uuid  gptDiskUuid;
    Uuid  gptPartUuid;
    Uuid  partUuid;
};

#define BootloaderInfoCode                                                     \
    { CommonMagic, 0xf550'38d8'e2a1'202f, 0x2794'26fc'f5f5'9740 }

[[gnu::used, gnu::section(".limine_requests")]] struct BootloaderInfo {
    struct _ {
        u64   revision;
        char* name;
        char* version;
    };

    Array<u64, 4> id { BootloaderInfoCode };
    u64           revision { 0 };
    _*            response { nullptr };
};

#define CommandLineCode                                                        \
    { CommonMagic, 0x4b16'1536'e598'651e, 0xb390'ad4a'2f1f'303a }

struct CommandLine {
    struct _ {
        u64   revision;
        char* cmdline;
    };

    Array<u64, 4> id { CommandLineCode };
    u64           revision { 0 };
    _*            response { nullptr };
};

enum struct Firmware : u64 {
    x86Bios = 0,
    Uefi32  = 1,
    Uefi64  = 2,
    Sbi     = 3
};

#define FirmwareTypeCode                                                       \
    { CommonMagic, 0x8c2f'75d9'0bef'28a8, 0x7045'a468'8eac'00c3 }

struct FirmwareType {
    struct _ {
        u64      revision;
        Firmware firmwareType;
    };

    Array<u64, 4> id { FirmwareTypeCode };
    u64           revision { 0 };
    _*            response { nullptr };
};

#define StackSizeCode                                                          \
    { CommonMagic, 0x224e'f046'0a8e'8926, 0xe1cb'0fc2'5f46'ea3d }

struct StackSize {
    struct _ {
        u64 revision;
    };

    [[gnu::always_inline]] constexpr StackSize(u64 size) : size(size) { }

    Array<u64, 4> id { StackSizeCode };
    u64           revision { 0 };
    _*            response { nullptr };
    u64           size;
};

#define HhdmCode { CommonMagic, 0x48dc'f1cb'8ad2'b852, 0x6398'4e95'9a98'244b }

struct Hhdm {
    struct _ {
        u64 revision;
        u64 offset;
    };

    Array<u64, 4> id { HhdmCode };
    u64           revision { 0 };
    _*            response { nullptr };
};

#define FramebufferCode                                                        \
    { CommonMagic, 0x9d58'27dc'd881'dd75, 0xa314'8604'f6fa'b11b }

struct VideoMode {
    u64 pitch;
    u64 width;
    u64 height;
    u16 bpp;
    u8  model;
    struct {
        u8 size;
        u8 shift;
    } r, g, b;
};

struct FramebufInfo {
    void* address;
    u64   width;
    u64   height;
    u64   pitch;
    u16   bpp;
    u8    model;
    struct {
        u8 size;
        u8 shift;
    } r, g, b;
    Array<u8, 7> unused;
    u64          edidSize;
    void*        edid;
    struct {
        u64         count;
        VideoMode** entries;
    } mode;
};

struct Framebuf {
    struct _ {
        u64            revision;
        u64            framebufferCount;
        FramebufInfo** framebuffers;
    };

    Array<u64, 4> id { FramebufferCode };
    u64           revision { 0 };
    _*            response { nullptr };
};

#define PagingCode { CommonMagic, 0x95c1'a0ed'ab09'44cb, 0xa4e5'cb38'42f7'488a }

struct Paging {
#if defined(__x86_64__) || defined(__i386__)
    enum struct Mode : u64 {
        X86_64_4Lvl = 0,
        X86_64_5Lvl = 1
    };
    constexpr static Mode Default = Mode::X86_64_4Lvl;
    constexpr static Mode Min     = Mode::X86_64_4Lvl;
#elif defined(__aarch64__)
    enum struct Mode : u64 {
        Aarch64_4Lvl = 0,
        Aarch64_5Lvl = 1
    };
    constexpr static Mode Default = Mode::Aarch64_4Lvl;
    constexpr static Mode Min     = Mode::Aarch64_4Lvl;
#elif defined(__riscv) && (__riscv_xlen == 64)
    enum struct Mode : u64 {
        Riscv_Sv39 = 0,
        Riscv_Sv48 = 1,
        Riscv_Sv57 = 2
    };
    constexpr static Mode Default = Mode::Riscv_Sv48;
    constexpr static Mode Min     = Mode::Riscv_Sv39;
#elif defined(__loongarch__) && (__loongarch_grlen == 64)
    enum struct Mode : u64 {
        Loongarch64_4Lvl = 0
    };
    constexpr static Mode Default = Mode::Loongarch64_4Lvl;
    constexpr static Mode Min     = Mode::Loongarch64_4Lvl;
#else
#    error Unknown architecture
#endif

    struct _ {
        u64 revision;
        u64 mode;
    };

    [[gnu::always_inline]] constexpr Paging(Mode mode,
                                            Mode max = Default,
                                            Mode min = Min)
        : mode { mode, max, min } { }

    Array<u64, 4> id { PagingCode };
    u64           revision { Revision };
    _*            response { nullptr };
    struct {
        Mode requests;
        Mode max;
        Mode min;
    } mode;
};

#define MultiprocessingCode                                                    \
    { CommonMagic, 0x95a6'7b81'9a1b'857e, 0xa0b6'1b72'3b6a'73e0 }

struct Core;

using Goto = void (*)(struct Limine::Core*);

#if defined(__x86_64__) || defined(__i386__)
struct Core {
    u32   id;
    u32   apicId;
    u64   reserved;
    Goto* entry;
    u64   args;
};
#elif defined(__aarch64__)
struct Core {
    u32   id;
    u32   reserved1;
    u64   mpidr;
    u64   reserved2;
    Goto* entry;
    u64   args;
};
#elif defined(__riscv) && (__riscv_xlen == 64)
struct Core {
    u64   id;
    u64   hartid;
    u64   reserved;
    Goto* entry;
    u64   args;
};
#elif defined(__loongarch__) && (__loongarch_grlen == 64)
struct Core {
    u64   id;
    u64   hartid;
    u64   reserved;
    Goto* entry;
    u64   args;
};
#endif

struct Multiprocessing {

#if defined(__x86_64__) || defined(__i386__)
    struct _ {
        u64    revision;
        u32    flags;
        u32    apicId;
        u64    count;
        Core** cpus;
    };
#elif defined(__aarch64__)
    struct _ {
        u64    revision;
        u32    flags;
        u32    reserved;
        u64    count;
        Core** cpus;
    };
#elif defined(__riscv) && (__riscv_xlen == 64)
    struct _ {
        u64    revision;
        u64    flags;
        u64    bspHartid;
        u64    count;
        Core** cpus;
    };
#elif defined(__loongarch__) && (__loongarch_grlen == 64)
    struct _ {
        u64    revision;
        u64    flags;
        u64    bspHartid;
        u64    count;
        Core** cpus;
    };
#endif

    Array<u64, 4> id { MultiprocessingCode };
    u64           revision { Revision };
    _*            response { nullptr };
    u64           flags { 0 };
};

#define MemoryMapCode                                                          \
    { CommonMagic, 0xdeb1'7a3d'4a8a'6f0c, 0x9e5d'1f4d'2c3e'8b5a }

enum struct MemoryType : u64 {
    Usable                = 0,
    Reserved              = 1,
    AcpiReclaimable       = 2,
    AcpiNvs               = 3,
    BadMemory             = 4,
    BootloaderReclaimable = 5,
    ExecutableAndModules  = 6,
    Framebuffer           = 7
};

struct MemoryMapEntry {
    u64        base;
    u64        length;
    MemoryType type;
};

struct MemoryMap {
    struct _ {
        u64              revision;
        u64              count;
        MemoryMapEntry** entries;
    };

    Array<u64, 4> id { MemoryMapCode };
    u64           revision { 0 };
    _*            response { nullptr };

    auto slice() const { return Slice(response->entries, response->count); }
};

#define EntryCode { CommonMagic, 0x13d8'6c03'5a1c'd3e1, 0x2b0c'aa89'd8f3'026a }

struct Entry {
    struct _ {
        u64 revision;
    };

    [[gnu::always_inline]] Entry(void (*func)(void)) : entry(func) { }

    Array<u64, 4> id { EntryCode };
    u64           revision { Revision };
    _*            response { nullptr };
    void (*entry)(void);
};

#define ExecutableFileCode                                                     \
    { CommonMagic, 0xad97'e90e'83f1'ed67, 0x31eb'5d1c'5ff2'3b69 }

struct ExecutableFile {
    struct _ {
        u64   revision;
        File* file;
    };

    Array<u64, 4> id { ExecutableFileCode };
    u64           revision { Revision };
    _*            response { nullptr };
};

#define ModuleCode { CommonMagic, 0x3e7e'2797'02be'32af, 0xca1c'4f3b'd128'0cee }

enum struct ModuleFlags : u64 {
    Required   = 1 << 0,
    Compressed = 1 << 1
};
MakeFlags$(ModuleFlags);

struct InternalModule {
    char const*        path;
    char const*        string;
    Flags<ModuleFlags> flags;
};

struct Module {
    struct _ {
        u64    revision;
        u64    count;
        File** modules;
    };

    [[gnu::always_inline]] constexpr Module(usize internalModuleCount,
                                            InternalModule** internalModules)
        : count(internalModuleCount),
          modules(internalModules) { }

    Array<u64, 4> id { ModuleCode };
    u64           revision { Revision };
    _*            response { nullptr };

    u64              count;
    InternalModule** modules;
};

#define RsdpCode { CommonMagic, 0xc5e7'7b6b'397e'7b43, 0x2763'7845'accd'cf3c }

struct Rsdp {
    struct _ {
        u64 revision;
        u64 address;
    };

    Array<u64, 4> id { RsdpCode };
    u64           revision { Revision };
    _*            response { nullptr };
};

#define SmbiosCode { CommonMagic, 0x9e90'46f1'1e09'5391, 0xaa4a'520f'efbd'e5ee }

struct Smbios {
    struct _ {
        u64 revision;
        u64 entry32;
        u64 entry64;
    };

    Array<u64, 4> id { SmbiosCode };
    u64           revision { Revision };
    _*            response { nullptr };
};

#define EfiSystemTableCode                                                     \
    { CommonMagic, 0x5ceb'a516'3eaa'f6d6, 0x0a69'8161'0cf6'5fcc }

struct EfiSystemTable {
    struct _ {
        u64 revision;
        u64 address;
    };

    Array<u64, 4> id { EfiSystemTableCode };
    u64           revision { Revision };
    _*            response { nullptr };
};

#define EfiMemmapCode                                                          \
    { CommonMagic, 0x7df6'2a43'1d68'72d5, 0xa4fc'dfb3'e573'06c8 }

struct EfiMemmap {
    struct _ {
        u64   revision;
        void* memmap;
        u64   memmapSize;
        u64   descSize;
        u64   descVersion;
    };

    Array<u64, 4> id { EfiMemmapCode };
    u64           revision { Revision };
    _*            response { nullptr };
};

#define BootTimeCode                                                           \
    { CommonMagic, 0x5027'46e1'84c0'88aa, 0xfbc5'ec83'e632'7893 }

struct BootTime {
    struct _ {
        u64 revision;
        i64 timestamp;
    };

    Array<u64, 4> id { BootTimeCode };
    u64           revision { Revision };
    _*            response { nullptr };
};

#define ExecutableAddressCode                                                  \
    { CommonMagic, 0x71ba'7686'3cc5'5f63, 0xb264'4a48'c516'a487 }

struct ExecutableAddress {
    struct _ {
        u64 revision;
        u64 physicalBase;
        u64 virtualBase;
    };

    Array<u64, 4> id { ExecutableAddressCode };
    u64           revision { Revision };
    _*            response { nullptr };
};

#define DeviceTreeBlobCode                                                     \
    { CommonMagic, 0xb40d'db48'fb54'bac7, 0x5450'8149'3f81'ffb7 }

struct DeviceTreeBlob {
    struct _ {
        u64   revision;
        void* dtbPtr;
    };

    Array<u64, 4> id { DeviceTreeBlobCode };
    u64           revision { Revision };
    _*            response { nullptr };
};

#define RiscVBootHartIdCode                                                    \
    { CommonMagic, 0x1369'359f'0255'25f9, 0x2ff2'a561'7839'1bb6 }

struct RiscVBootHartId {
    struct _ {
        u64 revision;
        u64 bspHartid;
    };

    Array<u64, 4> id { RiscVBootHartIdCode };
    u64           revision { Revision };
    _*            response { nullptr };
};

#define BootloaderPerformanceCode                                              \
    { CommonMagic, 0x6b50'ad9b'f36d'13ad, 0xdc4c'7e88'fc75'9e17 }

struct BootloaderPerformance {
    struct _ {
        u64 revision;
        u64 resetUsec;
        u64 initUsec;
        u64 execUsec;
    };

    Array<u64, 4> id { BootloaderPerformanceCode };
    u64           revision { Revision };
    _*            response { nullptr };
};

namespace {

static auto const BaseRev = BaseRevision;
static BootloaderInfo volatile binfo;
static BootTime volatile btime;
static FirmwareType volatile fwtype;
static StackSize volatile stacksize(0x1'0000);
static MemoryMap volatile memmap;
static Hhdm volatile hhdm;
static ExecutableAddress volatile address;

} // namespace

Res<> parse(Builder<Boot::Info>& info) {
    info.magic(0xc7b1'dd30'df4c'8b88)
        .agent(binfo.response->name)
        .version(Revision);

    memmap.slice()
        | forEach$(info.append(_Record<Tag::Memory> {
            .range = Range<uflat>(it->base, it->length),
            .type  = match<MemoryType, u32>(
                it->type,
                1,
                _case(MemoryType::Usable, 0),
                _case(MemoryType::Reserved, 1),
                _case(MemoryType::AcpiReclaimable, 2),
                _case(MemoryType::AcpiNvs, 4),
                _case(MemoryType::BadMemory, 1),
                _case(MemoryType::BootloaderReclaimable, 3),
                _case(MemoryType::ExecutableAndModules, 5),
                _case(MemoryType::Framebuffer, 6), ),
        }));
}

} // namespace Limine
