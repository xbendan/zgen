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

#pragma once

#include <sdk-meta/res.h>
#include <sdk-meta/types.h>

#ifdef LIMINE_NO_POINTERS
#    define LIMINE_PTR(TYPE) u64
#else
#    define LIMINE_PTR(TYPE) TYPE
#endif

#ifndef LIMINE_API_REVISION
#    define LIMINE_API_REVISION 0
#endif

#if LIMINE_API_REVISION > 3
#    error "limine.h API revision unsupported"
#endif

#ifdef __GNUC__
#    define LIMINE_DEPRECATED __attribute__((__deprecated__))
#    define LIMINE_DEPRECATED_IGNORE_START                                     \
        _Pragma("GCC diagnostic push")                                         \
            _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#    define LIMINE_DEPRECATED_IGNORE_END _Pragma("GCC diagnostic pop")
#else
#    define LIMINE_DEPRECATED
#    define LIMINE_DEPRECATED_IGNORE_START
#    define LIMINE_DEPRECATED_IGNORE_END
#endif

#define LIMINE_REQUESTS_START_MARKER                                           \
    u64 limine_requests_start_marker[4] = { 0xf6b8'f4b3'9de7'd1ae,             \
                                            0xfab9'1a69'40fc'b9cf,             \
                                            0x785c'6ed0'15d3'e316,             \
                                            0x181e'920a'7852'b9d9 };
#define LIMINE_REQUESTS_END_MARKER                                             \
    u64 limine_requests_end_marker[2]                                          \
        = { 0xadc0'e053'1bb1'0d03, 0x9572'709f'3176'4c62 };

#define LIMINE_REQUESTS_DELIMITER LIMINE_REQUESTS_END_MARKER

#define LIMINE_BASE_REVISION(N)                                                \
    u64 limine_base_revision[3]                                                \
        = { 0xf956'2b2d'5c95'a6c8, 0x6a7b'3849'4453'6bdc, (N) };

#define LIMINE_BASE_REVISION_SUPPORTED (limine_base_revision[2] == 0)

#define LIMINE_LOADED_BASE_REV_VALID                                           \
    (limine_base_revision[1] != 0x6a7b'3849'4453'6bdc)
#define LIMINE_LOADED_BASE_REVISION (limine_base_revision[1])

#define LIMINE_COMMON_MAGIC 0xc7b1'dd30'df4c'8b88, 0x0a82'e883'a194'f07b

struct limine_uuid {
    u32 a;
    u16 b;
    u16 c;
    u8  d[8];
};

#define LIMINE_MEDIA_TYPE_GENERIC 0
#define LIMINE_MEDIA_TYPE_OPTICAL 1
#define LIMINE_MEDIA_TYPE_TFTP    2

struct limine_file {
    u64 revision;
    LIMINE_PTR(void*) address;
    u64 size;
    LIMINE_PTR(char*) path;
#if LIMINE_API_REVISION >= 3
    LIMINE_PTR(char*) string;
#else
    LIMINE_PTR(char*) cmdline;
#endif
    u32                media_type;
    u32                unused;
    u32                tftp_ip;
    u32                tftp_port;
    u32                partition_index;
    u32                mbr_disk_id;
    struct limine_uuid gpt_disk_uuid;
    struct limine_uuid gpt_part_uuid;
    struct limine_uuid part_uuid;
};

/* Boot info */

#define LIMINE_BOOTLOADER_INFO_REQUEST                                         \
    { LIMINE_COMMON_MAGIC, 0xf550'38d8'e2a1'202f, 0x2794'26fc'f5f5'9740 }

struct limine_bootloader_info_response {
    u64 revision;
    LIMINE_PTR(char*) name;
    LIMINE_PTR(char*) version;
};

struct limine_bootloader_info_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_bootloader_info_response*) response;
};

/* Executable command line */

#define LIMINE_EXECUTABLE_CMDLINE_REQUEST                                      \
    { LIMINE_COMMON_MAGIC, 0x4b16'1536'e598'651e, 0xb390'ad4a'2f1f'303a }

struct limine_executable_cmdline_response {
    u64 revision;
    LIMINE_PTR(char*) cmdline;
};

struct limine_executable_cmdline_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_executable_cmdline_response*) response;
};

/* Firmware type */

#define LIMINE_FIRMWARE_TYPE_REQUEST                                           \
    { LIMINE_COMMON_MAGIC, 0x8c2f'75d9'0bef'28a8, 0x7045'a468'8eac'00c3 }

#define LIMINE_FIRMWARE_TYPE_X86BIOS 0
#define LIMINE_FIRMWARE_TYPE_UEFI32  1
#define LIMINE_FIRMWARE_TYPE_UEFI64  2
#define LIMINE_FIRMWARE_TYPE_SBI     3

struct limine_firmware_type_response {
    u64 revision;
    u64 firmware_type;
};

struct limine_firmware_type_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_firmware_type_response*) response;
};

/* Stack size */

#define LIMINE_STACK_SIZE_REQUEST                                              \
    { LIMINE_COMMON_MAGIC, 0x224e'f046'0a8e'8926, 0xe1cb'0fc2'5f46'ea3d }

struct limine_stack_size_response {
    u64 revision;
};

struct limine_stack_size_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_stack_size_response*) response;
    u64 stack_size;
};

/* HHDM */

#define LIMINE_HHDM_REQUEST                                                    \
    { LIMINE_COMMON_MAGIC, 0x48dc'f1cb'8ad2'b852, 0x6398'4e95'9a98'244b }

struct limine_hhdm_response {
    u64 revision;
    u64 offset;
};

struct limine_hhdm_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_hhdm_response*) response;
};

/* Framebuffer */

#define LIMINE_FRAMEBUFFER_REQUEST                                             \
    { LIMINE_COMMON_MAGIC, 0x9d58'27dc'd881'dd75, 0xa314'8604'f6fa'b11b }

#define LIMINE_FRAMEBUFFER_RGB 1

struct limine_video_mode {
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

struct limine_framebuffer {
    LIMINE_PTR(void*) address;
    u64 width;
    u64 height;
    u64 pitch;
    u16 bpp;
    u8  memory_model;
    u8  red_mask_size;
    u8  red_mask_shift;
    u8  green_mask_size;
    u8  green_mask_shift;
    u8  blue_mask_size;
    u8  blue_mask_shift;
    u8  unused[7];
    u64 edid_size;
    LIMINE_PTR(void*) edid;
    /* Response revision 1 */
    u64 mode_count;
    LIMINE_PTR(struct limine_video_mode**) modes;
};

struct limine_framebuffer_response {
    u64 revision;
    u64 framebuffer_count;
    LIMINE_PTR(struct limine_framebuffer**) framebuffers;
};

struct limine_framebuffer_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_framebuffer_response*) response;
};

/* Terminal */

#define LIMINE_TERMINAL_REQUEST                                                \
    { LIMINE_COMMON_MAGIC, 0xc8ac'5931'0c2b'0844, 0xa68d'0c72'65d3'8878 }

#define LIMINE_TERMINAL_CB_DEC           10
#define LIMINE_TERMINAL_CB_BELL          20
#define LIMINE_TERMINAL_CB_PRIVATE_ID    30
#define LIMINE_TERMINAL_CB_STATUS_REPORT 40
#define LIMINE_TERMINAL_CB_POS_REPORT    50
#define LIMINE_TERMINAL_CB_KBD_LEDS      60
#define LIMINE_TERMINAL_CB_MODE          70
#define LIMINE_TERMINAL_CB_LINUX         80

#define LIMINE_TERMINAL_CTX_SIZE     ((u64) (-1))
#define LIMINE_TERMINAL_CTX_SAVE     ((u64) (-2))
#define LIMINE_TERMINAL_CTX_RESTORE  ((u64) (-3))
#define LIMINE_TERMINAL_FULL_REFRESH ((u64) (-4))

/* Response revision 1 */
#define LIMINE_TERMINAL_OOB_OUTPUT_GET ((u64) (-10))
#define LIMINE_TERMINAL_OOB_OUTPUT_SET ((u64) (-11))

#define LIMINE_TERMINAL_OOB_OUTPUT_OCRNL  (1 << 0)
#define LIMINE_TERMINAL_OOB_OUTPUT_OFDEL  (1 << 1)
#define LIMINE_TERMINAL_OOB_OUTPUT_OFILL  (1 << 2)
#define LIMINE_TERMINAL_OOB_OUTPUT_OLCUC  (1 << 3)
#define LIMINE_TERMINAL_OOB_OUTPUT_ONLCR  (1 << 4)
#define LIMINE_TERMINAL_OOB_OUTPUT_ONLRET (1 << 5)
#define LIMINE_TERMINAL_OOB_OUTPUT_ONOCR  (1 << 6)
#define LIMINE_TERMINAL_OOB_OUTPUT_OPOST  (1 << 7)

LIMINE_DEPRECATED_IGNORE_START

struct LIMINE_DEPRECATED limine_terminal;

typedef void (*limine_terminal_write)(struct limine_terminal*,
                                      char const*,
                                      u64);
typedef void (
    *limine_terminal_callback)(struct limine_terminal*, u64, u64, u64, u64);

struct LIMINE_DEPRECATED limine_terminal {
    u64 columns;
    u64 rows;
    LIMINE_PTR(struct limine_framebuffer*) framebuffer;
};

struct LIMINE_DEPRECATED limine_terminal_response {
    u64 revision;
    u64 terminal_count;
    LIMINE_PTR(struct limine_terminal**) terminals;
    LIMINE_PTR(limine_terminal_write) write;
};

struct LIMINE_DEPRECATED limine_terminal_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_terminal_response*) response;
    LIMINE_PTR(limine_terminal_callback) callback;
};

LIMINE_DEPRECATED_IGNORE_END

/* Paging mode */

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

struct limine_paging_mode_response {
    u64 revision;
    u64 mode;
};

struct limine_paging_mode_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_paging_mode_response*) response;
    u64 mode;
    u64 max_mode;
    u64 min_mode;
};

/* 5-level paging */

#define LIMINE_5_LEVEL_PAGING_REQUEST                                          \
    { LIMINE_COMMON_MAGIC, 0x9446'9551'da9b'3192, 0xebe5'e86d'b738'2888 }

LIMINE_DEPRECATED_IGNORE_START

struct LIMINE_DEPRECATED limine_5_level_paging_response {
    u64 revision;
};

struct LIMINE_DEPRECATED limine_5_level_paging_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_5_level_paging_response*) response;
};

LIMINE_DEPRECATED_IGNORE_END

/* MP */

#if LIMINE_API_REVISION >= 1
#    define LIMINE_MP_REQUEST                                                  \
        { LIMINE_COMMON_MAGIC, 0x95a6'7b81'9a1b'857e, 0xa0b6'1b72'3b6a'73e0 }
#    define LIMINE_MP(TEXT) limine_mp_##TEXT
#else
#    define LIMINE_SMP_REQUEST                                                 \
        { LIMINE_COMMON_MAGIC, 0x95a6'7b81'9a1b'857e, 0xa0b6'1b72'3b6a'73e0 }
#    define LIMINE_MP(TEXT) limine_smp_##TEXT
#endif

struct LIMINE_MP(info);

typedef void (*limine_goto_address)(struct LIMINE_MP(info) *);

#if defined(__x86_64__) || defined(__i386__)

#    if LIMINE_API_REVISION >= 1
#        define LIMINE_MP_X2APIC (1 << 0)
#    else
#        define LIMINE_SMP_X2APIC (1 << 0)
#    endif

struct LIMINE_MP(info) {
    u32 processor_id;
    u32 lapic_id;
    u64 reserved;
    LIMINE_PTR(limine_goto_address) goto_address;
    u64 extra_argument;
};

struct LIMINE_MP(response) {
    u64 revision;
    u32 flags;
    u32 bsp_lapic_id;
    u64 cpu_count;
    LIMINE_PTR(struct LIMINE_MP(info) **) cpus;
};

#elif defined(__aarch64__)

struct LIMINE_MP(info) {
    u32 processor_id;
    u32 reserved1;
    u64 mpidr;
    u64 reserved;
    LIMINE_PTR(limine_goto_address) goto_address;
    u64 extra_argument;
};

struct LIMINE_MP(response) {
    u64 revision;
    u64 flags;
    u64 bsp_mpidr;
    u64 cpu_count;
    LIMINE_PTR(struct LIMINE_MP(info) **) cpus;
};

#elif defined(__riscv) && (__riscv_xlen == 64)

struct LIMINE_MP(info) {
    u64 processor_id;
    u64 hartid;
    u64 reserved;
    LIMINE_PTR(limine_goto_address) goto_address;
    u64 extra_argument;
};

struct LIMINE_MP(response) {
    u64 revision;
    u64 flags;
    u64 bsp_hartid;
    u64 cpu_count;
    LIMINE_PTR(struct LIMINE_MP(info) **) cpus;
};

#elif defined(__loongarch__) && (__loongarch_grlen == 64)

struct LIMINE_MP(info) {
    u64 reserved;
};

struct LIMINE_MP(response) {
    u64 cpu_count;
    LIMINE_PTR(struct LIMINE_MP(info) **) cpus;
};

#else
#    error Unknown architecture
#endif

struct LIMINE_MP(request) {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct LIMINE_MP(response) *) response;
    u64 flags;
};

/* Memory map */

#define LIMINE_MEMMAP_REQUEST                                                  \
    { LIMINE_COMMON_MAGIC, 0x67cf'3d9d'378a'806f, 0xe304'acdf'c50c'3c62 }

#define LIMINE_MEMMAP_USABLE                 0
#define LIMINE_MEMMAP_RESERVED               1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE       2
#define LIMINE_MEMMAP_ACPI_NVS               3
#define LIMINE_MEMMAP_BAD_MEMORY             4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#if LIMINE_API_REVISION >= 2
#    define LIMINE_MEMMAP_EXECUTABLE_AND_MODULES 6
#else
#    define LIMINE_MEMMAP_KERNEL_AND_MODULES 6
#endif
#define LIMINE_MEMMAP_FRAMEBUFFER 7

struct limine_memmap_entry {
    u64 base;
    u64 length;
    u64 type;
};

struct limine_memmap_response {
    u64 revision;
    u64 count;
    LIMINE_PTR(struct limine_memmap_entry**) entries;
};

struct limine_memmap_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_memmap_response*) response;
};

/* Entry point */

#define LIMINE_ENTRY_POINT_REQUEST                                             \
    { LIMINE_COMMON_MAGIC, 0x13d8'6c03'5a1c'd3e1, 0x2b0c'aa89'd8f3'026a }

typedef void (*limine_entry_point)(void);

struct limine_entry_point_response {
    u64 revision;
};

struct limine_entry_point_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_entry_point_response*) response;
    LIMINE_PTR(limine_entry_point) entry;
};

/* Executable File */

#if LIMINE_API_REVISION >= 2
#    define LIMINE_EXECUTABLE_FILE_REQUEST                                     \
        { LIMINE_COMMON_MAGIC, 0xad97'e90e'83f1'ed67, 0x31eb'5d1c'5ff2'3b69 }
#else
#    define LIMINE_KERNEL_FILE_REQUEST                                         \
        { LIMINE_COMMON_MAGIC, 0xad97'e90e'83f1'ed67, 0x31eb'5d1c'5ff2'3b69 }
#endif

#if LIMINE_API_REVISION >= 2
struct limine_executable_file_response {
#else
struct limine_kernel_file_response {
#endif
    u64 revision;
#if LIMINE_API_REVISION >= 2
    LIMINE_PTR(struct limine_file*) executable_file;
#else
    LIMINE_PTR(struct limine_file*) kernel_file;
#endif
};

#if LIMINE_API_REVISION >= 2
struct limine_executable_file_request {
#else
struct limine_kernel_file_request {
#endif
    u64 id[4];
    u64 revision;
#if LIMINE_API_REVISION >= 2
    LIMINE_PTR(struct limine_executable_file_response*) response;
#else
    LIMINE_PTR(struct limine_kernel_file_response*) response;
#endif
};

/* Module */

#define LIMINE_MODULE_REQUEST                                                  \
    { LIMINE_COMMON_MAGIC, 0x3e7e'2797'02be'32af, 0xca1c'4f3b'd128'0cee }

#define LIMINE_INTERNAL_MODULE_REQUIRED   (1 << 0)
#define LIMINE_INTERNAL_MODULE_COMPRESSED (1 << 1)

struct limine_internal_module {
    LIMINE_PTR(char const*) path;
#if LIMINE_API_REVISION >= 3
    LIMINE_PTR(const char*) string;
#else
    LIMINE_PTR(const char*) cmdline;
#endif
    u64 flags;
};

struct limine_module_response {
    u64 revision;
    u64 module_count;
    LIMINE_PTR(struct limine_file**) modules;
};

struct limine_module_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_module_response*) response;

    /* Request revision 1 */
    u64 internal_module_count;
    LIMINE_PTR(struct limine_internal_module**) internal_modules;
};

/* RSDP */

#define LIMINE_RSDP_REQUEST                                                    \
    { LIMINE_COMMON_MAGIC, 0xc5e7'7b6b'397e'7b43, 0x2763'7845'accd'cf3c }

struct limine_rsdp_response {
    u64 revision;
#if LIMINE_API_REVISION >= 1
    u64 address;
#else
    LIMINE_PTR(void*) address;
#endif
};

struct limine_rsdp_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_rsdp_response*) response;
};

/* SMBIOS */

#define LIMINE_SMBIOS_REQUEST                                                  \
    { LIMINE_COMMON_MAGIC, 0x9e90'46f1'1e09'5391, 0xaa4a'520f'efbd'e5ee }

struct limine_smbios_response {
    u64 revision;
#if LIMINE_API_REVISION >= 1
    u64 entry_32;
    u64 entry_64;
#else
    LIMINE_PTR(void*) entry_32;
    LIMINE_PTR(void*) entry_64;
#endif
};

struct limine_smbios_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_smbios_response*) response;
};

/* EFI system table */

#define LIMINE_EFI_SYSTEM_TABLE_REQUEST                                        \
    { LIMINE_COMMON_MAGIC, 0x5ceb'a516'3eaa'f6d6, 0x0a69'8161'0cf6'5fcc }

struct limine_efi_system_table_response {
    u64 revision;
#if LIMINE_API_REVISION >= 1
    u64 address;
#else
    LIMINE_PTR(void*) address;
#endif
};

struct limine_efi_system_table_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_efi_system_table_response*) response;
};

/* EFI memory map */

#define LIMINE_EFI_MEMMAP_REQUEST                                              \
    { LIMINE_COMMON_MAGIC, 0x7df6'2a43'1d68'72d5, 0xa4fc'dfb3'e573'06c8 }

struct limine_efi_memmap_response {
    u64 revision;
    LIMINE_PTR(void*) memmap;
    u64 memmap_size;
    u64 desc_size;
    u64 desc_version;
};

struct limine_efi_memmap_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_efi_memmap_response*) response;
};

/* Date at boot */

#if LIMINE_API_REVISION >= 3
#    define LIMINE_DATE_AT_BOOT_REQUEST                                        \
        { LIMINE_COMMON_MAGIC, 0x5027'46e1'84c0'88aa, 0xfbc5'ec83'e632'7893 }
#else
#    define LIMINE_BOOT_TIME_REQUEST                                           \
        { LIMINE_COMMON_MAGIC, 0x5027'46e1'84c0'88aa, 0xfbc5'ec83'e632'7893 }
#endif

#if LIMINE_API_REVISION >= 3
struct limine_date_at_boot_response {
#else
struct limine_boot_time_response {
#endif
    u64 revision;
#if LIMINE_API_REVISION >= 3
    i64 timestamp;
#else
    i64 boot_time;
#endif
};

#if LIMINE_API_REVISION >= 3
struct limine_date_at_boot_request {
#else
struct limine_boot_time_request {
#endif
    u64 id[4];
    u64 revision;
#if LIMINE_API_REVISION >= 3
    LIMINE_PTR(struct limine_date_at_boot_response*) response;
#else
    LIMINE_PTR(struct limine_boot_time_response*) response;
#endif
};

/* Executable address */

#if LIMINE_API_REVISION >= 2
#    define LIMINE_EXECUTABLE_ADDRESS_REQUEST                                  \
        { LIMINE_COMMON_MAGIC, 0x71ba'7686'3cc5'5f63, 0xb264'4a48'c516'a487 }
#else
#    define LIMINE_KERNEL_ADDRESS_REQUEST                                      \
        { LIMINE_COMMON_MAGIC, 0x71ba'7686'3cc5'5f63, 0xb264'4a48'c516'a487 }
#endif

#if LIMINE_API_REVISION >= 2
struct limine_executable_address_response {
#else
struct limine_kernel_address_response {
#endif
    u64 revision;
    u64 physical_base;
    u64 virtual_base;
};

#if LIMINE_API_REVISION >= 2
struct limine_executable_address_request {
#else
struct limine_kernel_address_request {
#endif
    u64 id[4];
    u64 revision;
#if LIMINE_API_REVISION >= 2
    LIMINE_PTR(struct limine_executable_address_response*) response;
#else
    LIMINE_PTR(struct limine_kernel_address_response*) response;
#endif
};

/* Device Tree Blob */

#define LIMINE_DTB_REQUEST                                                     \
    { LIMINE_COMMON_MAGIC, 0xb40d'db48'fb54'bac7, 0x5450'8149'3f81'ffb7 }

struct limine_dtb_response {
    u64 revision;
    LIMINE_PTR(void*) dtb_ptr;
};

struct limine_dtb_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_dtb_response*) response;
};

/* RISC-V Boot Hart ID */

#define LIMINE_RISCV_BSP_HARTID_REQUEST                                        \
    { LIMINE_COMMON_MAGIC, 0x1369'359f'0255'25f9, 0x2ff2'a561'7839'1bb6 }

struct limine_riscv_bsp_hartid_response {
    u64 revision;
    u64 bsp_hartid;
};

struct limine_riscv_bsp_hartid_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_riscv_bsp_hartid_response*) response;
};

/* Bootloader Performance */

#define LIMINE_BOOTLOADER_PERFORMANCE_REQUEST                                  \
    { LIMINE_COMMON_MAGIC, 0x6b50'ad9b'f36d'13ad, 0xdc4c'7e88'fc75'9e17 }

struct limine_bootloader_performance_response {
    u64 revision;
    u64 reset_usec;
    u64 init_usec;
    u64 exec_usec;
};

struct limine_bootloader_performance_request {
    u64 id[4];
    u64 revision;
    LIMINE_PTR(struct limine_bootloader_performance_response*) response;
};

namespace limine {
Res<> parse();
}
