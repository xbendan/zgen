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

    struct Subscript {
        Bits& bits;
        usize index;

        Subscript(Bits& bits, usize index) : bits(bits), index(index) { }

        operator bool() const {
            return (bits._buf[index / 8] & (1 << (index % 8))) != 0;
        }

        Subscript& operator=(bool value) {
            if (value) {
                bits._buf[index / 8] |= (1 << (index % 8));
            } else {
                bits._buf[index / 8] &= ~(1 << (index % 8));
            }
            return *this;
        }
    };

    Subscript operator[](usize index) {
        if (index >= len()) {
            // panic("Bits index out of range: {} >= {}", index, _len);
        }
        return { *this, index };
    }

    bool get(usize index) const {
        if (index >= len()) {
            // panic("Bits index out of range: {} >= {}", index, _len);
        }
        return (_buf[index / 8] & (1 << (index % 8))) != 0;
    }

    void set(usize index, bool value) {
        if (index >= len()) {
            // panic("Bits index out of range: {} >= {}", index, _len);
        }
        if (value) {
            _buf[index / 8] |= (1 << (index % 8));

        } else {
            _buf[index / 8] &= ~(1 << (index % 8));
        }
    }

    void setRange(BitsRange range, bool value) {
        if (range.end() > len()) {
            // panic("Bits range out of bounds: {} + {} > {}", range._start,
            // range._size, _len * 8);
        }
        for (usize i = 0; i < range._size; ++i) {
            (*this)[range._start + i] = value;
        }
        // TODO: Improve performance by categories operations
    }

    usize len() const { return _len * 8; }

    Opt<BitsRange> alloc(usize count, usize start, bool upper = true) {
        start = min(start, len());

        if (not len() or not count) {
            return Empty {};
        }

        BitsRange range = {};
        for (usize i = start; //
             (upper ? i > 0 : i < len());
             (i += upper ? -1 : 1)) {

            if (get(i)) {
                range = {};
            } else {
                if (not range._size or upper) {
                    range._start = i;
                }

                range._size++;
            }

            if (range._size == count) {
                setRange(range, true);
                return range;
            }
        }

        return EMPTY;
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
