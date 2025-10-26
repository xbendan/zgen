#pragma once

#include <sdk-meta/opt.h>
#include <sdk-meta/panic.h>
#include <sdk-meta/range.h>

namespace Meta {
using BitsRange = Range<u64, struct _BitsRangeTag>;

struct Bits {
    u8*   _buf {};
    usize _len {};

    Bits(Slice<u8> slice) : _buf(slice.buf()), _len(slice.len()) { }

    Bits(Sliceable<u8> auto& slice) : _buf(slice.buf()), _len(slice.len()) { }

    Bits(u8* buf, usize len) : _buf(buf), _len(len) { }

    ~Bits() = default;

    bool operator[](usize index) { return get(index); }

    bool get(usize index) const {
        if (index >= len()) {
            panic("Bits::get: index out of range");
        }
        return (_buf[index / 8] & (1 << (index % 8))) != 0;
    }

    void set(usize index, bool used) {
        if (index >= len()) {
            panic("Bits::set: index out of range");
        }

        if (used) {
            _buf[index / 8] |= (1 << (index % 8));
        } else {
            _buf[index / 8] &= ~(1 << (index % 8));
        }
    }

    void setRange(BitsRange range, bool used) {
        if (range.end() > len()) {
            return;
        }
        if (used) {
            for (usize i = 0; i < range.size(); i++) {
                _buf[(range.start() + i) / 8]
                    |= (1 << ((range.start() + i) % 8));
            }
        } else {
            for (usize i = 0; i < range.size(); i++) {
                _buf[(range.start() + i) / 8]
                    &= ~(1 << ((range.start() + i) % 8));
            }
        }
        // TODO: Improve performance by categories operations
    }

    usize len() const { return _len * 8; }

    Opt<BitsRange> alloc(usize count, usize start) {
        start = min(start, len());

        if (not len() or not count) {
            return NONE;
        }

        BitsRange range = {};
        for (usize i = start; //
             i < len();
             i += 1) {

            if (get(i)) {
                range = {};
            } else {
                if (not range.size()) {
                    range.start(i);
                }

                range._size++;
            }

            if (range.size() == count) {
                setRange(range, true);
                return range;
            }
        }

        return NONE;
    }

    usize used() const {
        usize res = 0;
        for (usize i = 0; i < len(); ++i) {
            if (get(i)) {
                res++;
            }
        }
        return res;
    }

    Bytes bytes() const { return { _buf, _len }; }
};

} // namespace Meta

using Meta::Bits;
using Meta::BitsRange;
