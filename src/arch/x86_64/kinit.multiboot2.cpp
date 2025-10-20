#include <multiboot2.h>
#include <sdk-meta/panic.h>
#include <sdk-meta/types.h>
#include <zgen/init/boot.h>
#include <zgen/init/prekernel.h>

namespace multiboot2 {

Res<> parse(Info* info) {
    if (info->totalSize < sizeof(Info)) {
        return Error::invalidData("multiboot2 info too small");
    }

    auto* prekernel  = &Zgen::Core::prekernel;
    prekernel->magic = 0x36d7'6289;

    auto* resp = reinterpret_cast<Response*>(info->responses);

    while ((resp->id != MULTIBOOT_TAG_TYPE_END)
           and ((u64) resp < (u64) info + info->totalSize)) {
        switch (resp->id) {
            case MULTIBOOT_TAG_TYPE_CMDLINE: {

            } break;

            case MULTIBOOT_TAG_TYPE_AGENT_NAME: {
                prekernel->agentName = ((AgentName*) resp)->name;
            } break;

            case MULTIBOOT_TAG_TYPE_MODULE: {

            } break;

            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO: {

            } break;

            case MULTIBOOT_TAG_TYPE_BOOTDEV: {

            } break;

            case MULTIBOOT_TAG_TYPE_MMAP: {
                auto* mmap  = (MemoryMap*) resp;
                auto* entry = (MemoryMap::Entry*) mmap->entries;

                while ((u64) entry < ((u64) mmap + mmap->size)) {
                    u32 type;

                    switch (entry->type) {
                        case MemoryMap::Available:       type = 0; break;
                        case MemoryMap::Reserved:        type = 1; break;
                        case MemoryMap::AcpiReclaimable: type = 2; break;
                        case MemoryMap::AcpiNvs:         type = 4; break;
                        case MemoryMap::BadRam:          type = 5; break;

                        default: type = 1; break;
                    }
                    prekernel->memmap.pushBack((Zgen::Core::PrekernelInfo::_MemmapEntry) {
                        .range = {
                            entry->addr,
                            entry->len,
                        },
                        .type  = (decltype(Zgen::Core::PrekernelInfo::_MemmapEntry::type)) type,
                    });
                    entry = (MemoryMap::Entry*) ((u8*) entry + mmap->entrySize);
                }
            } break;

            case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR: {
                auto* lba             = (LoadBaseAddress*) resp;
                prekernel->offsetPhys = lba->load_base_addr;
                prekernel->offsetVirt
                    = lba->load_base_addr + 0xffff'ffff'8000'0000;
            }

            default: break;
        }

        resp = (Response*) ((u64) resp + alignUp(resp->size, 8));
    }

    return Ok();
}

} // namespace multiboot2

extern "C" [[noreturn]] void kinit_multiboot2(multiboot2::Info* info) {
    info = (multiboot2::Info*) ((u64) info + 0xffff'ffff'8000'0000);

    if (not multiboot2::parse(info)) {
        panic("Failed to resolve boot config");
    }

    if (not Zgen::Core::main(0, &Zgen::Core::prekernel)) {
        // BSOD

        panic("System exited unexpectedly");
    }
    __builtin_unreachable();
}
