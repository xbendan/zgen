#pragma GCC diagnostic ignored "-Wcomment"

#include <realms/core/api.mem.h>
#include <realms/core/main.h>
#include <realms/hal/arch.h>
#include <sdk-logs/logger.h>
#include <sdk-meta/literals.h>
#include <sdk-meta/res.h>
#include <sdk-meta/types.h>
#include <sdk-text/str.h>

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

//   ____                   _
//  |  _ \    ___    __ _  | |  _ __ ___    ___
//  | |_) |  / _ \  / _` | | | | '_ ` _ \  / __|
//  |  _ <  |  __/ | (_| | | | | | | | | | \__ \
//  |_| \_\  \___|  \__,_| |_| |_| |_| |_| |___/

#define KERNEL_SPLASH                                                          \
    "The Realms Core \n"                                                       \
    "  ____                   _                   \n"                          \
    " |  _ \\    ___    __ _  | |  _ __ ___    ___ \n"                         \
    " | |_) |  / _ \\  / _` | | | | '_ ` _ \\  / __|\n"                        \
    " |  _ <  |  __/ | (_| | | | | | | | | | \\__ \\\n"                        \
    " |_| \\_\\  \\___|  \\__,_| |_| |_| |_| |_| |___/"
namespace Realms::Core {

Res<> main(Boot::Info& info) {
    try$(Hal::init());

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
            "https://github.com/xbendan/realms.git"s);
    logInfo("\n");

    logInfo("Validating and dumping prekernel info...\n");
    logInfo(
        "Prekernel Info:\n"
        "         - Magic:          {:#x}\n"
        "         - Agent Name:     {}\n"
        "         - Agent Version:  {}\n",
        info.magic,
        info.agent,
        info.version);

    try$(setupMemory(info
                     | filter$(it.tag == Boot::Tag::Memory)
                     | select$(it.template as<Boot::Tag::Memory>())));

    return Ok();
}

} // namespace Realms::Core

[[noreturn]] void panic([[maybe_unused]] char const* message) {
    __builtin_unreachable();
}
