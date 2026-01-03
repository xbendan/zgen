#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/res.h>
#include <sdk-meta/traits.h>
#include <sdk-text/str.h>

namespace Realms::Hal::x86_64 {

union Cpuid {
    struct {
        u32 eax, ebx, ecx, edx;
    };
    Array<u32, 4>   regs;
    Array<char, 16> str;

    struct Branding {
        Array<char, 12> _vendor {};
        Array<char, 48> _brand {};

        Str vendor() const { return _vendor; }
        Str brand() const { return _brand; }
    };

    struct Capability {
        union {
            struct [[gnu::packed]] {
                u32 sse3: 1;      // Streaming SIMD Extensions 3
                u32 pclmulqdq: 1; // PCLMULQDQ instruction
                u32 ds64: 1;      // 64-bit Debug Trace Enable
                u32 mowait: 1;    // Monitor/Mwait
                u32 dscpl: 1;     // CPL qualified debug store
                u32 vmx: 1;       // Virtual Machine eXtensions
                u32 smx: 1;       // Safer Mode Extensions
                u32 eist: 1;      // Enhanced Intel SpeedStep Technology
                u32 tm2: 1;       // Thermal Monitor 2
                u32 ssse3: 1;     // Supplemental Streaming SIMD Extensions 3
                u32 cid: 1;       // L1 Context ID
                u32 sdbg: 1;      // Silicon Debug
                u32 fma: 1;       // Fused Multiply-Add
                u32 xx16: 1;      // CMPXCHG16B instruction
                u32 xtpr: 1;      // Disable sending task priority messages
                u32 pdcm: 1;      // Perfmon & debug capability
                u32 _reserved_16: 1;
                u32 pcid: 1;     // Process Context Identifier
                u32 dca: 1;      // Direct Cache Access for DMA writes
                u32 sse4_1: 1;   // Streaming SIMD Extensions 4.1
                u32 sse4_2: 1;   // Streaming SIMD Extensions 4.2
                u32 x2apic: 1;   // 2nd Generation APIC
                u32 movbe: 1;    // MOVBE instruction
                u32 popcnt: 1;   // POPCNT instruction
                u32 tscval: 1;   // APIC one-shot operation using a TSC deadline
                                 // value
                u32 aes: 1;      // Advanced Encryption Standard
                u32 xsave: 1;    // XSAVE, XRESTOR, XSETBV, XSG
                u32 os_xsave: 1; // OS-enabled XSAVE
                u32 avx: 1;      // Advanced Vector Extensions
                u32 f16c: 1;     // 16-bit Floating Point Conversion
                u32 rdrand: 1;   // RDRAND instruction
                u32 hypervisor: 1; // Hypervisor environment
            };
            u32 ecx;
        };
        union {
            struct [[gnu::packed]] {
                u32 fpu: 1;  // Onboard x87 FPU
                u32 vme: 1;  // Virtual 8086 Mode Extension
                u32 de: 1;   // Debugging Extension
                u32 pse: 1;  // Page Size Extension
                u32 tsc: 1;  // Time Stamp Counter
                u32 msr: 1;  // Model-Specific Registers
                u32 pae: 1;  // Physical Address Extension
                u32 mce: 1;  // Machine Check Exception
                u32 cx8: 1;  // CMPXCHG8B instruction
                u32 apic: 1; // Advanced Programmable Interrupt Controller
                u32 __reserved__1: 1;
                u32 sep: 1;   // SYSENTER, SYSEXIT
                u32 mtrr: 1;  // Memory Type Range Registers
                u32 pge: 1;   // Page Global Enable bit in CR4
                u32 mca: 1;   // Machine Check Architecture
                u32 cmov: 1;  // Conditional Move Instruction
                u32 pat: 1;   // Page Attribute Table
                u32 pse36: 1; // Page Size Extension 36-bit
                u32 psn: 1;   // Processor Serial Number
                u32 clfsh: 1; // Cache Line Flush Instruction
                u32 __reserved__2: 1;
                u32 ds: 1;   // Debug Store
                u32 acpi: 1; // Onboard thermal control MSRs for APCI
                u32 mmx: 1;  // Multimedia Extensions
                u32 fxsr: 1; // FXSAVE/FXRSTOR instructions
                u32 sse: 1;  // Streaming SIMD Extensions
                u32 sse2: 1; // Streaming SIMD Extensions 2
                u32 ss: 1;   // Self Snoop Cache
                u32 htt: 1;  // HyperThreading
                u32 tm: 1;   // Thermal Monitor Auto
                u32 ia64: 1; // IA64
                u32 pbe: 1;  // Pending Break
            };
            u32 edx;
        };
        union {
            u32 xecx;
        };
        union {
            u32 xedx;
        };

        [[gnu::always_inline]] Capability(u32 c, u32 d, u32 cx, u32 dx)
            : ecx(c),
              edx(d),
              xecx(cx),
              xedx(dx) { }
    };
};

static inline Cpuid cpuid(u32 leaf, u32 subleaf = 0) {
    u32 maxLeaf = 0;

    asm volatile("cpuid"
                 : "=a"(maxLeaf)
                 : "a"(leaf & 0x8000'0000)
                 : "rbx", "rcx", "rdx");

    if (leaf > maxLeaf) [[unlikely]] {
        panic("cpuid leaf out of range");
    }

    Cpuid result {};

    asm volatile(
        "cpuid"
        : "=a"(result.eax), "=b"(result.ebx), "=c"(result.ecx), "=d"(result.edx)
        : "a"(leaf), "c"(subleaf));

    return result;
}

static Array<char, 12> _vendor() {
    union {
        Array<u32, 3>   regs;
        Array<char, 12> str;
    } buf {};

    Cpuid result = cpuid(0);
    buf.regs[0]  = result.ebx;
    buf.regs[1]  = result.edx;
    buf.regs[2]  = result.ecx;

    return { buf.str };
}

static Array<char, 48> _brand() {
    union [[gnu::packed]] {
        Array<Cpuid, 4> ids;
        Array<char, 48> str;
    } buf {};

    buf.ids[0] = cpuid(0x8000'0002);
    buf.ids[1] = cpuid(0x8000'0003);
    buf.ids[2] = cpuid(0x8000'0004);
    buf.ids[3] = cpuid(0x8000'0005);
    return buf.str;
}

[[maybe_unused]] static Cpuid::Branding branding() {
    return { _vendor(), _brand() };
}

[[maybe_unused]] static Cpuid::Capability capabilities() {
    auto feat  = cpuid(1);
    auto featx = cpuid(0x8000'0001);

    return { feat.ecx, feat.edx, featx.ecx, featx.edx };
}

} // namespace Realms::Hal::x86_64
