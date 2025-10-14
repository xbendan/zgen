#pragma once

#include <sdk-io/path.h>
#include <sdk-io/seek.h>
#include <sdk-io/traits.h>
#include <sdk-meta/flags.h>
#include <sdk-meta/rc.h>
#include <sdk-meta/res.h>
#include <sdk-meta/time.h>
#include <sdk-text/str.h>
#include <zgen/io/device.h>
#include <zgen/mm/buffer.h>

namespace Zgen::Core::Io {

using Sdk::Io::Path;
using Sdk::Io::Seek;
using Sdk::Io::Whence;

struct Node {
    Rc<Device> device;
    Path       path;
};

struct File;

enum struct FileAccess {
    Read    = (1 << 0),
    Write   = (1 << 1),
    Execute = (1 << 2),

    ReadWrite = Read | Write,
    All       = Read | Write | Execute,
};
MakeFlags$(FileAccess);

enum struct FileMode {
    /*
     * If the file exists, open and seek to the beginning, otherwise create a new 
     * file. This requires write permission and could only be used with FileAccess::Write.
     * Error(s):
     *  - ACCESS_VIOLATION <- Access to location other than the beginning of file.
     *  - INVALID_OPERATION <- Read operation raises InvalidOperationException.
     */
    Append,
    /*
     * Create a new file. Requires write permission, if the file exists, truncate it.
     * Otherwise use CreateNew instead. 
     */
    Create,
    /*
     * Create a new file. Requires write permission, if the file exists, raise 
     * Error(s):
     *  - ALREADY_EXISTS <- The file already exists.
     */
    CreateNew,
    /*
     * Open an existing file. Requires read permission.
     * Error(s):
     *  - NOT_FOUND <- The file does not exist.
     *  - ACCESS_VIOLATION <- The file is a directory.
     */
    Open,
    /*
     * Open or create a file. Requires read and write permission.
     * If the file exists, open and seek to the beginning, otherwise create a new
     * file.
     */
    OpenOrCreate,
    Truncate
};

enum struct FileSource {
    Local,
    Remote,
    Cloud,
    Virtual,
    Device
};

struct FileHandle : Sdk::Io::Reader, Sdk::Io::Writer, Sdk::Io::Seeker {
    Rc<Io::File> const file;
    Seek               offset;
};

struct File : public Node {
    Str      name;
    Path     path;
    usize    size;
    DateTime created;
    DateTime modified;
    DateTime accessed;

    virtual ~File() = default;

    /**
     * @brief 
     * 
     * @param mode 
     * @param access 
     * @return Res<Rc<FileHandle>> 
     */
    virtual Res<Rc<FileHandle>> open(FileMode mode, Flags<FileAccess> access);

    /**
     * @brief 
     * 
     * @param handle 
     * @return Res<> 
     */
    virtual Res<> close(FileHandle& handle);

    virtual bool canRead(FileHandle& handle) const;

    virtual bool canWrite(FileHandle& handle) const;

    /**
     * @brief 
     * 
     * @param handle 
     * @param whence 
     * @param buf 
     * @param size 
     * @return Res<usize> 
     */
    virtual Res<usize> read(FileHandle&         handle,
                            Seek                whence,
                            UserOrKernelBuffer& buf,
                            usize               size);

    /**
     * @brief 
     * 
     * @param handle 
     * @param whence 
     * @param buf 
     * @param size 
     * @return Res<usize> 
     */
    virtual Res<usize> write(FileHandle&               handle,
                             Seek                      whence,
                             UserOrKernelBuffer const& buf,
                             usize                     size);
};

struct Directory : public Node {
    Vec<Str> items;
};

struct Symlink : public Node { };

struct Library : public Node { };

} // namespace Zgen::Core::Io
