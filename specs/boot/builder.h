#pragma once

#include <boot/info.h>
#include <sdk-meta/buf.h>
#include <sdk-patterns/builder.h>

template <>
struct Builder<Boot::Info> {
    Bytes _buf;
    usize _size {};

    Boot::Record* _last { nullptr };

    Builder(Bytes buf) : _buf(buf) { }

    Boot::Info& build() { return *reinterpret_cast<Boot::Info*>(_buf.buf()); }

    void append(Boot::Record record) {
        if (record.size == 0) {
            return;
        }
    }
};
