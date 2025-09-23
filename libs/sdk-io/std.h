#pragma once

#include <sdk-io/text.h>
#include <sdk-io/traits.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/rc.h>

namespace Sdk {

Opt<Io::TextReader*> in();

Opt<Io::TextWriter*> out();

Opt<Io::TextWriter*> err();

} // namespace Sdk
