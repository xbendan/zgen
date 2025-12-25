#include <realms/io/file.h>

namespace Realms::Core::Io {

Res<Rc<FileHandle>> File::open(FileMode mode, Flags<FileAccess> access) {
    return Error::notImplemented("File::open: not implemented");
}

Res<> File::close(FileHandle& handle) {
    return Error::notImplemented("File::close: not implemented");
}

bool File::canRead(FileHandle& handle) const {
    return false;
}

bool File::canWrite(FileHandle& handle) const {
    return false;
}

Res<usize> File::read(FileHandle&         handle,
                      Seek                whence,
                      UserOrKernelBuffer& buf,
                      usize               size) {
    return Error::notImplemented("File::read: not implemented");
}

Res<usize> File::write(FileHandle&               handle,
                       Seek                      whence,
                       UserOrKernelBuffer const& buf,
                       usize                     size) {
    return Error::notImplemented("File::write: not implemented");
}

} // namespace Realms::Core::Io
