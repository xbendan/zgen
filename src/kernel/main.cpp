#include <neodymium/hal/vmm.h>
#include <neodymium/init/boot.h>
#include <sdk-logs/logger.h>
#include <sdk-meta/try.h>

#if defined(__GNU__)
#    define STRINGIFY(x)  #x
#    define COMPILER_NAME "g++"
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

Str CompilerName    = COMPILER_NAME;
Str CompilerVersion = COMPILER_VERSION;
Str BuildDate       = __DATE__ " " __TIME__;

//  _   __                 _       _____
// | | / /                | |     /  __ \               
// | |/ / _   _  __ _ _ __| |_ ___| /  \/ ___  _ __ ___
// |    \| | | |/ _` | '__| __|_  / |    / _ \| '__/ _ \ 
// | |\  \ |_| | (_| | |  | |_ / /| \__/\ (_) | | |  __/
// \_| \_/\__,_|\__,_|_|   \__/___|\____/\___/|_|  \___|

#define KERNEL_SPLASH                                                          \
    " _   __                     _         _____                     \n"       \
    "| | / /                    | |       /  __ \\                    \n"      \
    "| |/ /  _   _   __ _  _ __ | |_  ____| /  \\/  ___   _ __   ___  \n"      \
    "|    \\ | | | | / _` || '__|| __||_  /| |     / _ \\ | '__| / _ \\ \n"    \
    "| |\\  \\| |_| || (_| || |   | |_  / / | \\__/\\| (_) || |   |  __/ \n"   \
    "\\_| \\_/ \\__,_| \\__,_||_|    \\__|/___| \\____/ \\___/ |_|    \\___| " \
    "\n"

namespace Sys {

Res<> main(u64 magic, PrekernelInfo* info) {
    logInfo("Initializing kernel...");
    logInfo(
        "Core info: \n"
        " - Compiler Name:    {}\n"
        " - Compiler Version: {}\n"
        " - Build Date:       {}",
        CompilerName,
        CompilerVersion,
        BuildDate);
    logInfo("For further information, check this repo: {}");
    logInfo("");

    logInfo("Validating and dumping prekernel info...");
    try$(validateAndDumpInfo(magic, info));

    try$(initArch(info));
    try$(setupMemory(info));

    return Ok();
}

} // namespace Sys
