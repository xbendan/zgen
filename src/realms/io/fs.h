#pragma once

#include <realms/io/dev.h>
#include <realms/io/file.h>
#include <sdk-meta/rc.h>
#include <sdk-meta/vec.h>
#include <sdk-text/str.h>

namespace Realms::Sys::Io {

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

} // namespace Realms::Sys::Io
