#pragma once

#include <boot/info.h>
#include <sdk-meta/buf.h>
#include <sdk-meta/str.h>
#include <sdk-patterns/builder.h>

template <>
struct Builder<Boot::Info> {
    Bytes _buf;
    usize _size {};

    Builder(Bytes buf) : _buf(buf) { }

    Boot::Info& unwrap() { return *reinterpret_cast<Boot::Info*>(_buf.buf()); }

    Boot::Info& build() {
        unwrap().length = (u32) _size;
        return unwrap();
    }

    template <Boot::Tag T>
    Builder& append(Boot::_Record<T> record) {
        if (record.size == 0) {
            return *this;
        }

        memcpy(_buf.buf() + _size, &record, record.size);
        _size += record.size;
        return *this;
    }

    Builder& magic(u64 magic) {
        unwrap().magic = magic;
        return *this;
    }

    Builder& agent(Str agent) {
        unwrap().agent = agent;
        return *this;
    }

    Builder& version(u32 version) {
        unwrap().version = version;
        return *this;
    }

    Builder& length(u32 length) {
        unwrap().length = length;
        return *this;
    }
};
