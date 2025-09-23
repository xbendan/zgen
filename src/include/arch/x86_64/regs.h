#pragma once

#include <sdk-meta/types.h>

namespace Hal::x86_64 {

struct [[gnu::packed]] Regs {
    u64 r15, r14, r13, r12, r11, r10, r9, r8;
    u64 rbp, rdi, rsi, rdx, rcx, rbx, rax;
    u64 err;
    u64 rip, cs, rflags, rsp, ss;
};

} // namespace Hal::x86_64
