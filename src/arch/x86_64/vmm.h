#pragma once

#include <sdk-logs/logger.h>
#include <sdk-meta/bits.h>
#include <sdk-meta/index.h>
#include <sdk-meta/iter.h>
#include <sdk-meta/ref.h>
#include <sdk-meta/slice.h>
#include <sdk-meta/str.h>
#include <sdk-meta/types.h>
#include <zgen/hal/vmm.h>

namespace Zgen::Hal::x86_64 {

struct [[gnu::packed]] Entry {
    enum : u64 {
        PRESENT       = (1 << 0),
        READWRITE     = (1 << 1),
        USER          = (1 << 2),
        WRITE_THROUGH = (1 << 3),
        NO_CACHE      = (1 << 4),
        ACCESSED      = (1 << 5),
        DIRTY         = (1 << 6),
        PAGE_SIZE     = (1 << 7),
        GLOBAL        = (1 << 7),
        NO_EXECUTE    = (1ull << 63),
    };
    static constexpr inline u64 MaskAddress = 0x000f'ffff'ffff'f000;
    static constexpr inline u64 MaskFlags   = 0xfff0'0000'0000'0fff;

    u64 data;

    // clang-format off

    Flags<Hal::VmmFlags> flags() {
        Flags<Hal::VmmFlags> f;

        if (data & PRESENT)       f |= Hal::VmmFlags::PRESENT;
        if (data & READWRITE)     f |= Hal::VmmFlags::WRITE; 
        if (data & USER)          f |= Hal::VmmFlags::USER;
        if (data & WRITE_THROUGH) f |= Hal::VmmFlags::WRITE_THROUGH;
        if (data & PAGE_SIZE)     f |= Hal::VmmFlags::PAGE_SIZE;
        if (data & NO_EXECUTE)    f |= Hal::VmmFlags::NO_EXECUTE;
        if (data & NO_CACHE)      f |= Hal::VmmFlags::UNCACHED;

        return f;
    }

    Entry& with(Flags<Hal::VmmFlags> f) {
        data &= MaskAddress;

        if (f[Hal::VmmFlags::PRESENT])       data |= PRESENT; 
        if (f[Hal::VmmFlags::WRITE])         data |= READWRITE; 
        if (f[Hal::VmmFlags::USER])          data |= USER; 
        if (f[Hal::VmmFlags::WRITE_THROUGH]) data |= WRITE_THROUGH; 
        if (f[Hal::VmmFlags::PAGE_SIZE])     data |= PAGE_SIZE; 
        if (f[Hal::VmmFlags::NO_EXECUTE])    data |= NO_EXECUTE; 
        if (f[Hal::VmmFlags::UNCACHED])      data |= NO_CACHE; 

        return *this;
    }

    Entry& with(uflat addr) {
        data = (data & MaskFlags) | (addr & MaskAddress);
        return *this;
    }

    // clang-format on

    u64 addr() const { return data & MaskAddress; }

    bool present() const { return data & PRESENT; }
};
static_assert(sizeof(Entry) == 8);

template <usize L>
struct [[gnu::packed]] alignat(Hal::PAGE_SIZE) Pml {
    static constexpr usize Level = L;
    static constexpr usize Len   = 512;

    using Inner = Entry;
    Array<Entry, Len> entries;

    Entry& operator[](usize i) {
        return entries[i];
    }

    Entry const& operator[](usize i) const {
        return entries[i];
    }

    Index indexOf(u64 virt) const {
        return (virt >> (12 + (Level - 1) * 9)) & 0x1ff;
    }

    usize granularity() const {
        return 1 << (12 + (Level - 1) * 9);
    }

    Entry& at(u64 virt) const noexcept {
        return const_cast<Entry&>(entries[indexOf(virt)]);
    }

    Res<> map(uflat                virt,
              uflat                phys,
              Flags<Hal::VmmFlags> flags
              = { Hal::VmmFlags::PRESENT | Hal::VmmFlags::WRITE }) {
        return map(indexOf(virt), phys, flags);
    }

    Res<> map(Index                index,
              uflat                addr,
              Flags<Hal::VmmFlags> flags
              = { Hal::VmmFlags::PRESENT | Hal::VmmFlags::WRITE });

    Res<> map(VmmRange             virt,
              PmmRange             phys,
              Flags<Hal::VmmFlags> flags = { Hal::VmmFlags::PRESENT
                                             | Hal::VmmFlags::WRITE
                                             | Hal::VmmFlags::PAGE_SIZE });

    bool none() const {
        for (auto e : entries) {
            if (e.present())
                return false;
        }
        return true;
    }

    Entry const* buf() const {
        return entries.buf();
    }

    usize len() const {
        return entries.len();
    }
};
static_assert(sizeof(Pml<1>) == 0x1000);
static_assert(Sliceable<Pml<1>>);

struct Vmm : Hal::Vmm {
    Hal::Pmm& _pmm;
    Pml<4>*   _pml4;
    Bits      _bits; // Each 4KiB can be used to manage 128MiB of virtual memory

    Vmm(Hal::Pmm& pmm, Pml<4>* pml4, Slice<u8> bits = {})
        : _pmm(pmm),
          _pml4(pml4),
          _bits(bits) { }

    ~Vmm() override = default;

    template <usize L>
    Res<Pml<L - 1>*> pmlAt(Pml<L>& up, usize vaddr, bool alloc = false) {
        // auto& e = up.at(vaddr);

        // if (not e.present()) {
        //     if (not alloc) {
        //         return Error::notFound("Vmm::pmlAt: no entry found");
        //     }
        //     usize lo = try$(_pmm.alloc(Hal::PAGE_SIZE, {}))._start;
        //     memset((void*) _mapper.map(lo), 0, Hal::PAGE_SIZE);
        //     up.replace(vaddr, { lo, Entry::PRESENT | Entry::READWRITE });
        // }

        // return Ok((Pml<L - 1>*) _mapper.map(e.address()));

        return Error::notImplemented("Vmm::pmlAt: not implemented");
    }

    Res<VmmRange> alloc(Opt<VmmRange>   vrange,
                        usize           amount,
                        Flags<VmmFlags> flags) override;

    Res<> free(VmmRange range) override;

    template <usize L>
    Res<> map(Pml<L>*              pml,
              VmmRange             virt,
              PmmRange             phys,
              Flags<Hal::VmmFlags> flags
              = { Hal::VmmFlags::PRESENT | Hal::VmmFlags::WRITE },
              Hal::Granularity granularity = Hal::Granularity::PAGE_4K) {
        if (virt.size() != phys.size()) {
            logError(
                "Vmm::map: size of virt ({:#x}) and phys ({:#x}) do not "
                "match",
                virt.size(),
                phys.size());
            return Error::invalidArgument(
                "Vmm::map: size of virt and phys do not match");
        }

        if (not virt.aligned(granularity) or not phys.aligned(granularity)) {
            logError(
                "Vmm::map: virt {:#x} or phys {:#x} is not aligned to {:#x}",
                virt._start,
                phys._start,
                granularity);
            return Error::invalidArgument(
                "Vmm::map: virt or phys is not page aligned");
        }

        iter(*pml)
            .filter$(not it.present())
            .index()
            .forEach$(
                it.v1.with(flags).with(phys.start() + it.v0 * granularity));

        usize pageCount = virt.size() / granularity;

        for (usize i = 0; i < pageCount; i++) {
            auto e = pml->at(virt.start() + i * granularity);

            if (e.present()) {
                return Error::alreadyExists("Vmm::map: entry already exists");
            }

            e.with(flags).with(phys.start() + i * granularity);
        }

        logInfo("Vmm::map: mapped {:#d} {}-level entries -> {:#x}",
                virt.size() / granularity);

        return Ok();
    }

    Res<> map(VmmRange virt, PmmRange phys, Flags<VmmFlags> flags) override;

    Res<> unmap(VmmRange range) override;

    Res<VmmPage> at(usize address) override;

    usize count() const override { return _bits.used(); }

    Res<> load() override;
};

struct UserVmm : public Hal::Vmm { };

} // namespace Zgen::Hal::x86_64
