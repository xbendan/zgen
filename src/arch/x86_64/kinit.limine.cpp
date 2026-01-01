#include <boot/protos/limine.h>
#include <realms/hal/vmm.h>
#include <sdk-meta/error.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>

extern "C" u64 __kernel_start;
extern "C" u64 __kernel_end;

extern "C" [[noreturn]] void kinit_limine(void) {
    if (not limine::parse()) {
        panic("Failed to resolve boot config");
    }
    if (not Realms::Core::main(0, &Realms::Core::prekernel)) {
        // BSOD

        panic("System exited unexpectedly");
    }
    __builtin_unreachable();
}

// clang-format off

#define limine_request __attribute__((used)) section(".limine_requests") static volatile

namespace limine {

limine_request LIMINE_BASE_REVISION(LIMINE_API_REVISION);
limine_request limine_bootloader_info_request    bootInfo     = { .id = LIMINE_BOOTLOADER_INFO_REQUEST, .revision = 0, .response = nullptr };
limine_request limine_date_at_boot_request       bootTime     = { .id = LIMINE_DATE_AT_BOOT_REQUEST, .revision = 0, .response = nullptr };
limine_request limine_firmware_type_request      firmwareType = { .id = LIMINE_FIRMWARE_TYPE_REQUEST, .revision = 0, .response = nullptr };
limine_request limine_stack_size_request         stack        = { .id = LIMINE_STACK_SIZE_REQUEST, .revision = 0, .response = nullptr, .stack_size = 0x10000 };
limine_request limine_memmap_request             memmap       = { .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = nullptr };
limine_request limine_entry_point_request        entryPoint   = { .id = LIMINE_ENTRY_POINT_REQUEST, .revision = 0, .response = nullptr, .entry = &kinit_limine };
limine_request limine_hhdm_request               hhdm         = { .id = LIMINE_HHDM_REQUEST, .revision = 0, .response = nullptr };
limine_request limine_executable_address_request address      = { .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST, .revision = 0, .response = nullptr };

Res<> parse() {
    auto* info = &Realms::Core::prekernel;

    if (not info) {
        return Error::notFound("Failed to get prekernel info");
    }

    usize kernelSize = (uflat) &__kernel_end - (uflat) &__kernel_start;
    memcpy((void*) (Realms::Hal::DIRECT_IO_REGION.start() + 0x100000),
           (void*) address.response->virtual_base,
           kernelSize);

    info->magic        = 0;
    info->agentName    = bootInfo.response->name;
    info->agentVersion = bootInfo.response->version;
    info->timestamp    = bootTime.response->timestamp;
    info->offsetPhys   = address.response->physical_base;
    info->offsetVirt   = address.response->virtual_base;

    limine_memmap_entry const* entry = memmap.response->entries[0];
    for (usize i = 0; 
         i < memmap.response->count; 
         i++, entry = memmap.response->entries[i]) {
        u32 type = 1;
        switch (entry->type) {
            case LIMINE_MEMMAP_USABLE: { type = 0; break; }
            case LIMINE_MEMMAP_RESERVED: { type = 1; break; }
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE: { type = 2; break; }
            case LIMINE_MEMMAP_ACPI_NVS: { type = 4; break; }
            case LIMINE_MEMMAP_BAD_MEMORY: { type = 1; break; }
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: { type = 2; break; }
            case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES: { type = 3; break; }
            case LIMINE_MEMMAP_FRAMEBUFFER: { type = 6; break; }

            default: { type = 1; break; }
        }

        info->memmap.pushBack((Realms::Core::PrekernelInfo::_MemmapEntry) {
            .range = {
                entry->base,
                entry->length,
            },
            .type   = (decltype(Realms::Core::PrekernelInfo::_MemmapEntry::type)) type,
        });
    }

    return Ok();
}

// clang-format on

} // namespace limine

