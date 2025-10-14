#include <limine.h>
#include <sdk-meta/error.h>
#include <sdk-meta/panic.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>
#include <zgen/init/boot.h>
#include <zgen/init/prekernel.h>

extern "C" [[noreturn]] void kinit_limine(void) {
    if (not limine::parse()) {
        panic("Failed to resolve boot config");
    }
    if (not Zgen::Core::main(0, &Zgen::Core::prekernel)) {
        // BSOD

        panic("System exited unexpectedly");
    }
    __builtin_unreachable();
}

// clang-format off

#define limine_request __attribute__((used, section(".limine_requests"))) static volatile

namespace limine {

limine_request LIMINE_BASE_REVISION(3);
limine_request limine_bootloader_info_request blInfo       = { .id = LIMINE_BOOTLOADER_INFO_REQUEST, .revision = 0, .response = nullptr };
limine_request limine_boot_time_request       blTime       = { .id = LIMINE_BOOT_TIME_REQUEST, .revision = 0, .response = nullptr };
limine_request limine_firmware_type_request   firmwareType = { .id = LIMINE_FIRMWARE_TYPE_REQUEST, .revision = 0, .response = nullptr };
limine_request limine_stack_size_request      stack        = { .id = LIMINE_STACK_SIZE_REQUEST, .revision = 0, .response = nullptr, .stack_size = 0x10000 };
limine_request limine_memmap_request          memmap       = { .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = nullptr };
limine_request limine_entry_point_request     entryPoint   = { .id = LIMINE_ENTRY_POINT_REQUEST, .revision = 0, .response = nullptr, .entry = &kinit_limine };

Res<> parse() {
    auto* info = &Zgen::Core::prekernel;

    if (not info) {
        return Error::notFound("Failed to get prekernel info");
    }
    info->magic        = 0;
    info->agentName    = blInfo.response->name;
    info->agentVersion = blInfo.response->version;
    info->timestamp    = blTime.response->boot_time;
    {
        usize sp;
        asm volatile("mov %%rsp, %0" : "=r"(sp));
        info->stack = sp;
    }

    limine_memmap_entry const* entry = memmap.response->entries[0];
    for (usize i = 0; 
         i < memmap.response->count; 
         i++, entry = memmap.response->entries[i]) {
        u32 type;
        switch (entry->type) {
            case LIMINE_MEMMAP_USABLE: { type = 0; break; }
            case LIMINE_MEMMAP_RESERVED: { type = 1; break; }
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE: { type = 2; break; }
            case LIMINE_MEMMAP_ACPI_NVS: { type = 3; break; }
            case LIMINE_MEMMAP_BAD_MEMORY: { type = 1; break; }
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: { type = 2; break; }
            case LIMINE_MEMMAP_KERNEL_AND_MODULES: { type = 1; break; }
            case LIMINE_MEMMAP_FRAMEBUFFER: { type = 5; break; }

            default: { type = 1; break; }
        }

        info->memmap.pushBack((Zgen::Core::PrekernelInfo::_MemmapEntry) {
            .range = {
                entry->base,
                entry->length,
            },
            .type   = (decltype(Zgen::Core::PrekernelInfo::_MemmapEntry::type)) type,
        });
    }

    return Ok();
}

// clang-format on

} // namespace limine

