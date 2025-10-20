/*   multiboot2.h - Multiboot 2 header file. */
/*   Copyright (C) 1999,2003,2007,2008,2009,2010  Free Software Foundation, Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL ANY
 *  DEVELOPER OR DISTRIBUTOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/iter.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/ptr.h>
#include <sdk-meta/res.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

/*  How many bytes from the start of the file we search for the header. */
#define MULTIBOOT_SEARCH       3'2768
#define MULTIBOOT_HEADER_ALIGN 8

/*  The magic field should contain this. */
#define MULTIBOOT2_HEADER_MAGIC 0xe852'50d6

/*  This should be in %eax. */
#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d7'6289

/*  Alignment of multiboot modules. */
#define MULTIBOOT_MOD_ALIGN 0x0000'1000

/*  Alignment of the multiboot info structure. */
#define MULTIBOOT_INFO_ALIGN 0x0000'0008

/*  Flags set in the ’flags’ member of the multiboot header. */

#define MULTIBOOT_TAG_ALIGN               8
#define MULTIBOOT_TAG_TYPE_END            0
#define MULTIBOOT_TAG_TYPE_CMDLINE        1
#define MULTIBOOT_TAG_TYPE_AGENT_NAME     2
#define MULTIBOOT_TAG_TYPE_MODULE         3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO  4
#define MULTIBOOT_TAG_TYPE_BOOTDEV        5
#define MULTIBOOT_TAG_TYPE_MMAP           6
#define MULTIBOOT_TAG_TYPE_VBE            7
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER    8
#define MULTIBOOT_TAG_TYPE_ELF_SECTIONS   9
#define MULTIBOOT_TAG_TYPE_APM            10
#define MULTIBOOT_TAG_TYPE_EFI32          11
#define MULTIBOOT_TAG_TYPE_EFI64          12
#define MULTIBOOT_TAG_TYPE_SMBIOS         13
#define MULTIBOOT_TAG_TYPE_ACPI_OLD       14
#define MULTIBOOT_TAG_TYPE_ACPI_NEW       15
#define MULTIBOOT_TAG_TYPE_NETWORK        16
#define MULTIBOOT_TAG_TYPE_EFI_MMAP       17
#define MULTIBOOT_TAG_TYPE_EFI_BS         18
#define MULTIBOOT_TAG_TYPE_EFI32_IH       19
#define MULTIBOOT_TAG_TYPE_EFI64_IH       20
#define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR 21

#define MULTIBOOT_HEADER_TAG_END                 0
#define MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST 1
#define MULTIBOOT_HEADER_TAG_ADDRESS             2
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS       3
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS       4
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER         5
#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN        6
#define MULTIBOOT_HEADER_TAG_EFI_BS              7
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI32 8
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI64 9
#define MULTIBOOT_HEADER_TAG_RELOCATABLE         10

#define MULTIBOOT_ARCHITECTURE_I386   0
#define MULTIBOOT_ARCHITECTURE_MIPS32 4
#define MULTIBOOT_HEADER_TAG_OPTIONAL 1

#define MULTIBOOT_LOAD_PREFERENCE_NONE 0
#define MULTIBOOT_LOAD_PREFERENCE_LOW  1
#define MULTIBOOT_LOAD_PREFERENCE_HIGH 2

#define MULTIBOOT_CONSOLE_FLAGS_CONSOLE_REQUIRED   1
#define MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED 2

namespace multiboot2 {

static constexpr u32 InfoMagic = 0xe852'50d6;
static constexpr u32 BootMagic = 0x36d7'6289;

struct Header {
    /*  Must be MULTIBOOT_MAGIC - see above. */
    u32 magic;

    /*  ISA */
    u32 architecture;

    /*  Total header length. */
    u32 header_length;

    /*  The above fields plus this one must equal 0 mod 2^32. */
    u32 checksum;
};

enum struct RespId : u32 {
    End              = 0,
    CommandLine      = 1,
    AgentName        = 2,
    Module           = 3,
    Meminfo          = 4,
    Bootdev          = 5,
    MemoryMap        = 6,
    Vbe              = 7,
    Framebuffer      = 8,
    ElfSections      = 9,
    Apm              = 10,
    Efi32            = 11,
    Efi64            = 12,
    Smbios           = 13,
    AcpiOld          = 14,
    AcpiNew          = 15,
    Network          = 16,
    EfiMemoryMap     = 17,
    EfiBootServices  = 18,
    Efi32ImageHandle = 19,
    Efi64ImageHandle = 20,
    LoadBaseAddress  = 21
};

struct Request {
    u16 type;
    u16 flags;
    u32 size;
};

struct InfoRequest : Request {
    u32 requests[0];
};

struct AddressRequest : Request {
    u32 header_addr;
    u32 load_addr;
    u32 load_end_addr;
    u32 bss_end_addr;
};

struct EntryRequest : Request {
    u32 entry_addr;
};

struct ConsoleFlagsRequest : Request {
    u32 console_flags;
};

struct FramebufferRequest : Request {
    u32 width;
    u32 height;
    u32 depth;
};

struct ModuleAlignRequest : Request { };

struct EfiBsRequest : Request { };

struct EntryAddressEfi32Request : Request {
    u32 entry_addr;
};
struct EntryAddressEfi64Request : Request {
    u64 entry_addr;
};

struct RelocatableRequest : Request {
    u16 min_addr;
    u16 max_addr;
    u32 align;
    u32 preference;
};

struct Color {
    u8 red;
    u8 green;
    u8 blue;
};

struct Response {
    u32 id;
    u32 size;
};

struct Info {
    u32      totalSize;
    u32      __reserved__;
    Response responses[0];
};

struct CommandLine : Response {
    char cmdline[0];
};

struct AgentName : Response {
    char name[0];
};

struct Module : Response {
    u32  mod_start;
    u32  mod_end;
    char cmdline[0];
};

struct Meminfo : Response {
    u32 lo;
    u32 hi;
};

struct Bootdev : Response {
    u32 biosdev;
    u32 slice;
    u32 part;
};

struct MemoryMap : Response {
    enum Type : u32 {
        Available       = 1,
        Reserved        = 2,
        AcpiReclaimable = 3,
        AcpiNvs         = 4,
        BadRam          = 5,
    };

    u32 entrySize;
    u32 entryVersion;
    struct Entry {
        u64  addr;
        u64  len;
        Type type;
        u32  zero;
    } entries[0];
};

struct Vbe : Response {
    u16 mode;
    u16 interface_seg;
    u16 interface_off;
    u16 interface_len;

    Array<u8, 512> control_info;
    Array<u8, 256> mode_info;
};

struct Framebuffer : Response {
    u64 addr;
    u32 pitch;
    u32 width;
    u32 height;
    u8  bpp;
    enum Type : u8 {
        Indexed = 0,
        Rgb     = 1,
        EgaText = 2,
    } type;
    union {
        struct {
            u16   color_nums;
            Color palette[0];
        };
        struct {
            u8 red_field_position;
            u8 red_mask_size;
            u8 green_field_position;
            u8 green_mask_size;
            u8 blue_field_position;
            u8 blue_mask_size;
        };
    };
};

struct ElfSections : Response {
    u32  num;
    u32  entsize;
    u32  shndx;
    char sections[0];
};

struct Apm : Response {
    u16 version;
    u16 cseg;
    u32 offset;
    u16 cseg_16;
    u16 dseg;
    u16 flags;
    u16 cseg_len;
    u16 cseg_16_len;
    u16 dseg_len;
};

struct Efi32 : Response {
    u32 pointer;
};

struct Efi64 : Response {
    u64 pointer;
};

struct Smbios : Response {
    u8 major;
    u8 minor;
    u8 reserved[6];
    u8 tables[0];
};

struct AcpiOld : Response {
    u8 rsdp[0];
};

struct AcpiNew : Response {
    u8 rsdp[0];
};

struct Network : Response {
    u8 dhcpack[0];
};

struct EfiMemoryMap : Response {
    u32 descr_size;
    u32 descr_vers;
    u8  efi_mmap[0];
};

struct Efi32ImageHandle : Response {
    u32 pointer;
};

struct Efi64ImageHandle : Response {
    u64 pointer;
};

struct LoadBaseAddress : Response {
    u32 load_base_addr;
};

Res<> parse(Info* info);

} // namespace multiboot2

constexpr auto iter(multiboot2::Info const& info) {
    auto n = [info,
              curr = (u8*) info.responses,
              end  = (u8*) &info
                  + info.totalSize] mutable -> Opt<multiboot2::Response&> {
        auto* resp = (multiboot2::Response*) curr;
        if (resp->id == 0 or curr >= end) {
            return NONE;
        }

        curr += alignUp(resp->size, MULTIBOOT_TAG_ALIGN);

        return resp;
    };
    return Meta::Iter<decltype(n)>(n);
}
