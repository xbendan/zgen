#pragma once

#include <sdk-io/text.h>

namespace Zgen::Core {

using namespace Sdk;

Io::TextReader& in();

void in(Io::TextReader& reader);

Io::TextWriter& out();

void out(Io::TextWriter& writer);

Io::TextWriter& err();

void err(Io::TextWriter& writer);

} // namespace Zgen::Core
