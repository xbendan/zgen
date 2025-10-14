#pragma GCC diagnostic ignored "-Wcomment"

#include <sdk-logs/logger.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>
#include <sdk-text/str.h>
#include <zgen/core/api.io.h>
#include <zgen/core/api.mem.h>
#include <zgen/hal/arch.h>
#include <zgen/init/boot.h>
#include <zgen/init/prekernel.h>

#if defined(__GNUG) or defined(__GNUC__)
#    define _STRINGIFY(x) #x
#    define STRINGIFY(x)  _STRINGIFY(x)
#    define COMPILER_NAME "x86_64-elf-g++"
#    define COMPILER_VERSION                                                   \
        STRINGIFY(__GNUC__.__GNUC_MINOR__.__GNUC_PATCHLEVEL__)
#elif defined(__clang__)
#    define STRINGIFY(x)  #x
#    define COMPILER_NAME "clang++"
#    define COMPILER_VERSION                                                   \
        STRINGIFY(__clang_major__.__clang_minor__.__clang_patchlevel__)
#elif !(defined(KCoreSuppressUnknownCompilerWarning))
#    error                                                                     \
        "Unknown compiler used, this project use gnu extensions." \
        "If you want to suppress this error, define KCoreSuppressUnknownCompilerWarning macro."
#endif

namespace {

Str CompilerName    = COMPILER_NAME;
Str CompilerVersion = COMPILER_VERSION;
Str BuildDate       = __DATE__ " " __TIME__;

} // namespace

//  ______
// |___  /
//    / /      __ _   ___  _ __
//   / /      / _` | / _ \| '_ \ 
// ./ /___ _ | (_| ||  __/| | | |
// \_____/(_) \__, | \___||_| |_|
//             __/ |
//            |___/

#define KERNEL_SPLASH                                                          \
    "The System Core Designed for Z-Generation \n"                             \
    " ______                            _____                     \n"          \
    "|___  /                           /  __ \\                    \n"         \
    "   / /      __ _   ___  _ __      | /  \\/  ___   _ __   ___  \n"         \
    "  / /      / _` | / _ \\| '_ \\     | |     / _ \\ | '__| / _ \\ \n"      \
    "./ /___ _ | (_| ||  __/| | | |    | \\__/\\| (_) || |   |  __/ \n"        \
    "\\_____/(_) \\__, | \\___||_| |_|     \\____/ \\___/ |_|    \\___| \n"    \
    "            __/ |                                         \n"             \
    "           |___/                                          \n"

namespace Zgen::Core {

Res<> main(u64 magic, PrekernelInfo* info) {
    try$(Hal::init(info));

    logInfo(KERNEL_SPLASH);

    logInfo("Initializing kernel...\n");
    logInfo(
        "Core info: \n"
        "         - Compiler Name:    {}\n"
        "         - Compiler Version: {}\n"
        "         - Build Date:       {}\n",
        CompilerName,
        CompilerVersion,
        BuildDate);
    logInfo("For further information, check this repo: {}\n",
            "https://github.com/xbendan/zgen.git"s);
    logInfo("\n");

    logInfo("Validating and dumping prekernel info...\n");
    logInfo(
        "Prekernel Info:\n"
        "         - Magic:          {:#x}\n"
        "         - Agent Name:     {}\n"
        "         - Agent Version:  {}\n"
        "         - Stack Pointer:  {:#x}\n",
        info->magic,
        Str { info->agentName },
        Str { info->agentVersion },
        info->stack);

    try$(setupMemory(info));

    return Ok();
}

static Opt<Io::TextReader&> _in  = NONE;
static Opt<Io::TextWriter&> _out = NONE;
static Opt<Io::TextWriter&> _err = NONE;

static Io::Null _null;

Io::TextReader& in() {
    return _in.unwrapOrElse(_null);
}

void in(Io::TextReader& reader) {
    _in = { reader };
}

Io::TextWriter& out() {
    return _out.unwrapOrElse(_null);
}

void out(Io::TextWriter& writer) {
    _out = { writer };
}

Io::TextWriter& err() {
    return _err.unwrapOrElse(_null);
}

void err(Io::TextWriter& writer) {
    _err = { writer };
}

} // namespace Zgen::Core

namespace Sdk {

Opt<Io::TextReader&> in() {
    return &Zgen::Core::in();
}

Opt<Io::TextWriter&> out() {
    return &Zgen::Core::out();
}

Opt<Io::TextWriter&> err() {
    return &Zgen::Core::err();
}

} // namespace Sdk
