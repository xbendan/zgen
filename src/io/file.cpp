#include <neodymium/io/file.h>

namespace Sys::Io {

Res<FileHandle*> File::open(FileMode mode, Flags<FileAccess> access) {
}

Res<> File::close(FileHandle& handle) {
    return EMPTY;
}

} // namespace Sys::Io
