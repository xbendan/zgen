#pragma once

#include <sdk-meta/rc.h>
#include <sdk-meta/vec.h>
#include <sdk-text/str.h>
#include <zgen/io/device.h>
#include <zgen/io/file.h>

namespace Zgen::Core::Io {

struct Fs {
    Str           name;
    Vec<Rc<Node>> opened;

    virtual Res<> create(Path path) = 0;

    virtual Res<Rc<Node>> open(Path path) = 0;

    virtual Res<> close(Rc<Node> node) = 0;

    virtual Res<> remove(Path path) = 0;

    virtual Res<Path> move(Path src, Path dest) = 0;

    virtual Res<Path> copy(Path src, Path dest) = 0;

    virtual Res<Vec<Str>> listFiles(Path path) = 0;
};

} // namespace Zgen::Core::Io
