#pragma once

#include <boot/record.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/types.h>
#include <sdk-text/str.h>

namespace Boot {

struct [[gnu::packed]] Info {
    using E = Record;

    u64    magic;
    Str    agent;
    u32    version;
    u32    length;
    Record records[];

    struct It {
        using R = Record*;
        using V = Record;

        Record* p;

        [[gnu::always_inline]] constexpr R operator*() { return p; }

        [[gnu::always_inline]] constexpr It& operator++() {
            if (p->tag != Tag::Empty) {
                p = (Record*) ((usize) p + p->size);
            }
            return *this;
        }

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return p->tag != Tag::Empty;
        }
    };

    [[gnu::always_inline]] constexpr auto begin() { return It { records }; }

    [[gnu::always_inline]] constexpr auto end() { return NONE; }
};

} // namespace Boot
