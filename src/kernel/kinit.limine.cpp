#include <neodymium/init/boot.h>
#include <neodymium/init/prekernel.h>
#include <limine.h>
#include <sdk-meta/try.h>

extern "C" [[noreturn]] void kinit_limine(void) {
    if (not limine::parse()) {
        panic("Failed to resolve boot config");
    }
    if (not Sys::main(limine::magic, &Sys::prekernel)) {
        // BSOD

        panic("System exited unexpectedly");
    }
    __builtin_unreachable();
}

// clang-format off

#define limine_request __attribute__((used, section(".limine_requests"))) static volatile

namespace limine {

limine_request LIMINE_BASE_REVISION(3);
limine_request limine::bootloader_info_request blInfo       = { .id = LIMINE_BOOTLOADER_INFO_REQUEST, .revision = 0 };
limine_request limine::boot_time_request       blTime       = { .id = LIMINE_BOOT_TIME_REQUEST, .revision = 0 };
limine_request limine::firmware_type_request   firmwareType = { .id = LIMINE_FIRMWARE_TYPE_REQUEST, .revision = 0 };
limine_request limine::stack_size_request      stackSize    = { .id = LIMINE_STACK_SIZE_REQUEST, .revision = 0, .stack_size = 0x10000 };
limine_request limine::memmap_request          memmap       = { .id = LIMINE_MEMMAP_REQUEST, .revision = 0 };
limine_request limine::entry_point_request     entryPoint   = { .id = LIMINE_ENTRY_POINT_REQUEST, .revision = 0, .entry = &kinit_limine };

Res<> parse() {
    auto* info = &Sys::prekernel;

    if (not info) {
        return Error::notFound("Failed to get prekernel info");
    }
    info->magic        = limine::magic;
    info->agentName    = blInfo.response->name;
    info->agentVersion = blInfo.response->version;
    info->timestamp    = blTime.response->timestamp;
    info->startupTime  = blTime.response->boot_time;

    limine::memmap_entry* entry = memmap.response->entries[0];
    for (usize i = 0; 
         i < memmap.response->count; 
         i++, entry = memmap.response->entries[i]) {
        u32 type;
        switch (entry->type) {
            case memmap_entry::Usable: { type = 0; break; }
            case memmap_entry::Reserved: { type = 1; break; }
            case memmap_entry::AcpiRec: { type = 2; break; }
            case memmap_entry::AcpiNvs: { type = 3; break; }
            case memmap_entry::BadMemory: { type = 1; break; }
            case memmap_entry::BootRec: { type = 2; break; }
            case memmap_entry::KernelAndMods: { type = 1; break; }
            case memmap_entry::Framebuffer: { type = 5; break; }
            
            default: { type = 1; break; }
        }

        info->memmap.pushBack((Sys::PrekernelInfo::_MemmapEntry) {
            .base   = entry->base,
            .size   = entry->length,
            .type   = (decltype(Sys::PrekernelInfo::_MemmapEntry::type)) type,
        });
    }

    return Ok();
}

// clang-format on

} // namespace limine

