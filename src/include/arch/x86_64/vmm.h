#pragma once

#include <neodymium/hal/vmm.h>
#include <sdk-meta/types.h>

namespace Hal::x86_64 {

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
        NO_EXECUTE    = (1ULL << 63),
    };
    static constexpr u64 ADDRESS_MASK = 0x000f'ffff'ffff'f000;
    static constexpr u64 FLAGS_MASK   = 0xfff0'0000'0000'0fff;
    u64                  data;

    // clang-format off

    Flags<Hal::VmmFlags> flags() {
        Flags<Hal::VmmFlags> f;

        if (data & PRESENT)       f |= Hal::VmmFlags::PRESENT;
        if (data & READWRITE)     f |= Hal::VmmFlags::WRITE; 
        if (data & USER)          f |= Hal::VmmFlags::USER;
        if (data & WRITE_THROUGH) f |= Hal::VmmFlags::WRITE_THROUGH;
        if (data & GLOBAL)        f |= Hal::VmmFlags::GLOBAL;
        if (data & NO_EXECUTE)    f |= Hal::VmmFlags::NO_EXECUTE;
        if (data & NO_CACHE)      f |= Hal::VmmFlags::UNCACHED;

        return f;
    }

    void flags(Flags<Hal::VmmFlags> f) {
        data &= ADDRESS_MASK;

        if (f[Hal::VmmFlags::PRESENT])       data |= PRESENT; 
        if (f[Hal::VmmFlags::WRITE])         data |= READWRITE; 
        if (f[Hal::VmmFlags::USER])          data |= USER; 
        if (f[Hal::VmmFlags::WRITE_THROUGH]) data |= WRITE_THROUGH; 
        if (f[Hal::VmmFlags::GLOBAL])        data |= GLOBAL; 
        if (f[Hal::VmmFlags::NO_EXECUTE])    data |= NO_EXECUTE; 
        if (f[Hal::VmmFlags::UNCACHED])      data |= NO_CACHE; 
    }

    // clang-format on

    u64 address() const { return data & ADDRESS_MASK; }

    void redirect(u64 addr) {
        data = (data & FLAGS_MASK) | (addr & ADDRESS_MASK);
    }

    bool const present() const { return data & PRESENT; }
};
static_assert(sizeof(Entry) == 8);

template <usize L>
struct [[gnu::packed]] Pml {
    static constexpr usize Level = L;
    static constexpr usize Len   = 512;

    Entry entries[Len];

    Entry& operator[](usize i) { return entries[i]; }

    Entry const& operator[](usize i) const { return entries[i]; }

    usize indexOf(u64 virt) const {
        return (virt >> (12 + (Level - 1) * 9)) & 0x1ff;
    }

    Opt<u64> findPhys(u64 virt) const {
        Entry e = entries[indexOf(virt)];

        if (not e.present()) {
            return EMPTY;
        }

        if constexpr (Level == 1)
            return e.address() | (virt & 0x1ff);
        else {
            auto* pml = (Pml<Level - 1>*) e.address();
            return pml->findPhys(virt);
        }
    }

    Entry at(u64 virt) const { return entries[indexOf(virt)]; }

    void replace(usize addr, Entry e) { entries[indexOf(addr)] = e; }

    bool none() const {
        for (auto e : entries) {
            if (e.present())
                return false;
        }
        return true;
    }
};
static_assert(sizeof(Pml<1>) == 0x1000);

} // namespace Hal::x86_64
