#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/types.h>
#include <sdk-meta/uuid.h>

#ifndef LIMINE_API_REVISION
#    define LIMINE_API_REVISION 0
#endif

#if LIMINE_API_REVISION > 3
#    error "limine.h API revision unsupported"
#endif

#define LIMINE_BASE_REVISION(N)                                                \
    u64 limine_base_revision[3]                                                \
        = { 0xf956'2b2d'5c95'a6c8, 0x6a7b'3849'4453'6bdc, (N) };
#define LIMINE_COMMON_MAGIC 0xc7b1'dd30'df4c'8b88, 0x0a82'e883'a194'f07b

namespace limine {

static constexpr inline u64 magic = 0x0a82'e883'a194'f07b;

struct file {
    u64   revision;
    void* address;
    usize size;
    char* path;
    char* cmdline;
    enum : u32 {
        Generic = 0,
        Optical = 1,
        Tftp    = 2,
    } media_type;
    u32  unused;
    u32  tftp_lp;
    u32  tftp_port;
    u32  partition_index;
    u32  mbr_disk_id;
    Uuid gpt_disk_uuid;
    Uuid gpt_part_uuid;
    Uuid part_uuid;
};

/* Boot info */

#define LIMINE_BOOTLOADER_INFO_REQUEST                                         \
    { LIMINE_COMMON_MAGIC, 0xf550'38d8'e2a1'202f, 0x2794'26fc'f5f5'9740 }

struct bootloader_info_response {
    u64   revision;
    char* name;
    char* version;
};

struct bootloader_info_request {
    u64                       id[4];
    u64                       revision;
    bootloader_info_response* response;
};

// MARK: - Executable Command Line

#define LIMINE_EXECUTABLE_CMDLINE_REQUEST                                      \
    { LIMINE_COMMON_MAGIC, 0x4b16'1536'e598'651e, 0xb390'ad4a'2f1f'303a }

struct exec_cmdline_response {
    u64   revision;
    char* cmdline;
};

struct exec_cmdline_request {
    u64                    id[4];
    u64                    revision;
    exec_cmdline_response* response;
};

// MARK: - Firmware Type

#define LIMINE_FIRMWARE_TYPE_REQUEST                                           \
    { LIMINE_COMMON_MAGIC, 0x8c2f'75d9'0bef'28a8, 0x7045'a468'8eac'00c3 }

#define LIMINE_FIRMWARE_TYPE_X86BIOS 0
#define LIMINE_FIRMWARE_TYPE_UEFI32  1
#define LIMINE_FIRMWARE_TYPE_UEFI64  2
#define LIMINE_FIRMWARE_TYPE_SBI     3

enum struct firmware : u64 {
    x86Bios = 0,
    Uefi32  = 1,
    Uefi64  = 2,
    Sbi     = 3,
    Unknown = 0xFFFF'FFFF'FFFF'FFFF
};

struct firmware_type_response {
    u64      revision;
    firmware firmware_type;
};

struct firmware_type_request {
    u64                     id[4];
    u64                     revision;
    firmware_type_response* response;
};

// MARK: - Stack Size

#define LIMINE_STACK_SIZE_REQUEST                                              \
    { LIMINE_COMMON_MAGIC, 0x224e'f046'0a8e'8926, 0xe1cb'0fc2'5f46'ea3d }

struct stack_size_response {
    u64 revision;
};

struct stack_size_request {
    u64                  id[4];
    u64                  revision;
    stack_size_response* response;
    u64                  stack_size;
};

// MARK: - Higher Half Direct Map

#define LIMINE_HHDM_REQUEST                                                    \
    { LIMINE_COMMON_MAGIC, 0x48dc'f1cb'8ad2'b852, 0x6398'4e95'9a98'244b }

struct hhdm_response {
    u64 revision;
    u64 offset;
};

struct hhdm_request {
    u64            id[4];
    u64            revision;
    hhdm_response* response;
};

// MARK: - Framebuffer

#define LIMINE_FRAMEBUFFER_REQUEST                                             \
    { LIMINE_COMMON_MAGIC, 0x9d58'27dc'd881'dd75, 0xa314'8604'f6fa'b11b }

struct video_mode {
    u64 pitch;
    u64 width;
    u64 height;
    u16 bpp;
    u8  memory_model;
    u8  red_mask_size;
    u8  red_mask_shift;
    u8  green_mask_size;
    u8  green_mask_shift;
    u8  blue_mask_size;
    u8  blue_mask_shift;
};

struct framebuffer {
    void*        address;
    u64          width;
    u64          height;
    u64          pitch;
    u16          bpp;
    u8           memory_model;
    u8           red_mask_size;
    u8           red_mask_shift;
    u8           green_mask_size;
    u8           green_mask_shift;
    u8           blue_mask_size;
    u8           blue_mask_shift;
    u8           unused[7];
    u64          edid_size;
    void*        edid;
    /* Response revision 1 */
    u64          mode_count;
    video_mode** modes;
};

struct framebuffer_response {
    u64           revision;
    usize         count;
    framebuffer** buffers;
};

struct framebuffer_request {
    u64                   id[4] LIMINE_FRAMEBUFFER_REQUEST;
    u64                   revision;
    framebuffer_response* response;
};

/* Terminal */

// TODO: Didn't used

// MARK: - Paging Mode

#define LIMINE_PAGING_MODE_REQUEST                                             \
    { LIMINE_COMMON_MAGIC, 0x95c1'a0ed'ab09'44cb, 0xa4e5'cb38'42f7'488a }

#if defined(__x86_64__) || defined(__i386__)
#    define LIMINE_PAGING_MODE_X86_64_4LVL 0
#    define LIMINE_PAGING_MODE_X86_64_5LVL 1
#    define LIMINE_PAGING_MODE_MIN         LIMINE_PAGING_MODE_X86_64_4LVL
#    define LIMINE_PAGING_MODE_DEFAULT     LIMINE_PAGING_MODE_X86_64_4LVL
#elif defined(__aarch64__)
#    define LIMINE_PAGING_MODE_AARCH64_4LVL 0
#    define LIMINE_PAGING_MODE_AARCH64_5LVL 1
#    define LIMINE_PAGING_MODE_MIN          LIMINE_PAGING_MODE_AARCH64_4LVL
#    define LIMINE_PAGING_MODE_DEFAULT      LIMINE_PAGING_MODE_AARCH64_4LVL
#elif defined(__riscv) && (__riscv_xlen == 64)
#    define LIMINE_PAGING_MODE_RISCV_SV39 0
#    define LIMINE_PAGING_MODE_RISCV_SV48 1
#    define LIMINE_PAGING_MODE_RISCV_SV57 2
#    define LIMINE_PAGING_MODE_MIN        LIMINE_PAGING_MODE_RISCV_SV39
#    define LIMINE_PAGING_MODE_DEFAULT    LIMINE_PAGING_MODE_RISCV_SV48
#elif defined(__loongarch__) && (__loongarch_grlen == 64)
#    define LIMINE_PAGING_MODE_LOONGARCH64_4LVL 0
#    define LIMINE_PAGING_MODE_MIN              LIMINE_PAGING_MODE_LOONGARCH64_4LVL
#    define LIMINE_PAGING_MODE_DEFAULT          LIMINE_PAGING_MODE_LOONGARCH64_4LVL
#else
#    error Unknown architecture
#endif

struct paging_mode_response {
    u64 revision;
    u64 mode;
};

struct paging_mode_request {
    u64                   id[4] = LIMINE_PAGING_MODE_REQUEST;
    u64                   revision;
    paging_mode_response* response;
};

// MARK: - Memory Map

#define LIMINE_MEMMAP_REQUEST                                                  \
    { LIMINE_COMMON_MAGIC, 0x67cf'3d9d'378a'806f, 0xe304'acdf'c50c'3c62 }

struct memmap_entry {
    u64 base;
    u64 length;
    enum Type : u64 {
        Usable    = 0,
        Reserved  = 1,
        AcpiRec   = 2,
        AcpiNvs   = 3,
        BadMemory = 4,
        BootRec   = 5,
#if LIMINE_API_REVISION >= 2
        ExecAndMods = 6,
#else
        KernelAndMods = 6,
#endif
        Framebuffer = 7,
        Unknown     = 0xFFFF'FFFF'FFFF'FFFF,
    } type;
};

struct memmap_response {
    u64            revision;
    u64            count;
    memmap_entry** entries;
};

struct memmap_request {
    u64              id[4] = LIMINE_MEMMAP_REQUEST;
    u64              revision;
    memmap_response* response;
};

// MARK: - Entry point

#define LIMINE_ENTRY_POINT_REQUEST                                             \
    { LIMINE_COMMON_MAGIC, 0x13d8'6c03'5a1c'd3e1, 0x2b0c'aa89'd8f3'026a }

typedef void (*entry_point)(void);

struct entry_point_response {
    u64 revision;
};

struct entry_point_request {
    u64                   id[4];
    u64                   revision;
    entry_point_response* response;
    entry_point           entry;
};

// MARK: - Executable file

#define LIMINE_EXECUTABLE_FILE_REQUEST                                         \
    { LIMINE_COMMON_MAGIC, 0xad97'e90e'83f1'ed67, 0x31eb'5d1c'5ff2'3b69 }

struct executable_file_response {
    u64   revision;
    file* executable_file;
};

struct executable_file_request {
    u64                       id[4] = LIMINE_EXECUTABLE_FILE_REQUEST;
    u64                       revision;
    executable_file_response* response;
};

// MARK: - Module

#define LIMINE_MODULE_REQUEST                                                  \
    { LIMINE_COMMON_MAGIC, 0x3e7e'2797'02be'32af, 0xca1c'4f3b'd128'0cee }

struct module {
    char const* path;
#if LIMINE_API_REVISION >= 3
    char const* string;
#else
    char const* cmdline;
#endif
    u64 flags;
};

struct module_response {
    u64    revision;
    u64    module_count;
    file** modules;
};

struct module_request {
    u64              id[4] = LIMINE_MODULE_REQUEST;
    u64              revision;
    module_response* response;
    u64              internal_module_count;
    module**         internal_modules;
};

// MARK: - EFI system table

#define LIMINE_EFI_SYSTEM_TABLE_REQUEST                                        \
    { LIMINE_COMMON_MAGIC, 0x5ceb'a516'3eaa'f6d6, 0x0a69'8161'0cf6'5fcc }

struct efi_system_table_response {
    u64 revision;
    u64 address;
};

struct efi_system_table_request {
    u64                        id[4];
    u64                        revision;
    efi_system_table_response* response;
};

// MARK: - EFI memory map

#define LIMINE_EFI_MEMMAP_REQUEST                                              \
    { LIMINE_COMMON_MAGIC, 0x7df6'2a43'1d68'72d5, 0xa4fc'dfb3'e573'06c8 }

struct efi_memmap_response {
    u64   revision;
    void* address;
    u64   memmap_size;
    u64   desc_size;
    u64   desc_version;
};

struct efi_memmap_request {
    u64                  id[4];
    u64                  revision;
    efi_memmap_response* response;
};

// MARK: - Boot time

#define LIMINE_BOOT_TIME_REQUEST                                               \
    { LIMINE_COMMON_MAGIC, 0x5027'46e1'84c0'88aa, 0xfbc5'ec83'e632'7893 }

struct boot_time_response {
    u64 revision;
    i64 timestamp;
    i64 boot_time;
};

struct boot_time_request {
    u64                 id[4];
    u64                 revision;
    boot_time_response* response;
};

/* Device Tree Blob */

#define LIMINE_DTB_REQUEST                                                     \
    { LIMINE_COMMON_MAGIC, 0xb40d'db48'fb54'bac7, 0x5450'8149'3f81'ffb7 }

struct dtb_response {
    u64   revision;
    void* dtb_ptr;
};

struct dtb_request {
    u64           id[4];
    u64           revision;
    dtb_response* response;
};

/* RISC-V Boot Hart ID */

#define LIMINE_RISCV_BSP_HARTID_REQUEST                                        \
    { LIMINE_COMMON_MAGIC, 0x1369'359f'0255'25f9, 0x2ff2'a561'7839'1bb6 }

struct riscv_bsp_hartid_response {
    u64 revision;
    u64 bsp_hartid;
};

struct riscv_bsp_hartid_request {
    u64                        id[4];
    u64                        revision;
    riscv_bsp_hartid_response* response;
};

Res<> parse();

} // namespace limine
