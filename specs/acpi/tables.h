#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/traits.h>

namespace Acpi {

struct [[packed]] Rsdp {
    Array<char, 8> sign;
    u8             checksum;
    Array<char, 6> oemId;
    u8             revision;
    u32            rsdt;
};

struct [[packed]] Xsdp : public Rsdp {
    u32          len;
    u64          xsdt;
    u8           checksumExt;
    Array<u8, 3> reserved;
};

struct [[packed]] Desc {
    Array<char, 4> sign;
    u32            length;
    u8             revision;
    u8             checksum;
    Array<char, 6> oemId;
    Array<char, 8> oemTableId;
    u32            oemRev;
    u32            creatorId;
    u32            creatorRev;

    template <Meta::Extends<Desc> T>
    [[gnu::always_inline]] constexpr T* as() {
        return static_cast<T*>(this);
    }
};

struct [[packed]] Rsdt : public Desc {
    u32 tables[];
};

struct [[packed]] Xsdt : public Desc {
    u64 tables[];
};

struct [[packed]] addresspkg {
    /**
         * @brief address space indicates where to read data
         * 0: System Memory
         * 1: System I/O
         * 2: PCI Configuration Space
         * 3: Embedded Controller
         * 4: @deprecated System Management Bus
         * 5: System CMOS
         * 6: PCI Device BAR Target
         * 7: @deprecated Intelligent Platform Management Infrastructure
         * 8: @deprecated General Purpose I/O
         * 9: @deprecated Generic Serial Bus
         * 0x0a: @deprecated Platform Communication Channel
         * 0x0b-0x7f: reserved
         * 0x80-0xff: OEM defined
         */
    u8  addressSpace;
    u8  bitWidth;
    u8  bitOffset;
    u8  accessSize;
    u64 address;
};

struct [[packed]] Madt : public Desc {
    u32 address;
    u32 flags;
    struct [[packed]] Item {
        u8 type;
        u8 length;
    } _items[];

    struct [[packed]] LocalApic : public Item {
        u8  processorId;
        u8  apicId;
        u32 flags;
    };

    struct [[packed]] IoApic : public Item {
        u8  apicId;
        u8  __reserved__;
        u32 address;
        u32 gSiB;
    };

    struct [[packed]] Iso : public Item {
        u8  bus;
        u8  src;
        u32 gSi;
        u16 flags;
    };

    struct [[packed]] Nmi : public Item {
        u8  processorId;
        u16 flags;
        u8  lInt;
    };

    struct [[packed]] Localx2Apic : public Item {
        u16 __reserved__;
        u32 x2apicId;
        u32 flags;
        u32 uid;
    };

    struct [[packed]] Nmix2Apic : public Item {
        u16          flags;
        u32          gSi;
        u8           lInt;
        Array<u8, 3> __reserved__;
    };
};

struct [[packed]] Hpet : public Desc {
    u8         hwrevId;
    u8         info;
    u16        vendorId;
    addresspkg address;
    u8         num;
    u16        minTick;
    u8         protect;
};

struct [[packed]] Mcfg : public Desc {
    u64 __reserved__;
    struct [[packed]] Packet {
        u64 base;
        u16 pseg;
        u8  busStart;
        u8  busEnd;
        u32 __reserved__;
    } packets[];
};

struct [[packed]] Fadt : public Desc {
    u32 fwctrl;
    u32 dsdt;
};

} // namespace Acpi
