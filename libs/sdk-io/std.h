#pragma once

#include <sdk-io/text.h>
#include <sdk-io/traits.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/rc.h>

namespace Sdk {

static inline Io::Null             _null;
static inline Opt<Io::TextReader&> in;
static inline Opt<Io::TextWriter&> out;
static inline Opt<Io::TextWriter&> err;

} // namespace Sdk
