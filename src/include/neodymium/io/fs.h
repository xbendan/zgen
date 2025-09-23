#pragma once

#include <neodymium/io/device.h>
#include <neodymium/io/file.h>
#include <neodymium/io/node.h>
#include <sdk-meta/rc.h>
#include <sdk-meta/vec.h>
#include <sdk-text/str.h>

namespace Sys::Io {

struct Fs {
    Str           name;
    Vec<Rc<Node>> opened;

    virtual Res<> create(Path path) = 0;

    virtual Res<Rc<Node>> retrieve(Path path) = 0;
};

} // namespace Sys::Io
