set_project("krystal-cpp.xmake")
set_version("1.0.0")
set_languages("cxx23")
add_rules("mode.debug", "mode.release")

toolchain("gcc-x86_64-elf")
    set_kind("standalone")
    set_sdkdir("~/.toolchains/gcc-x86_64-elf")
    set_bindir("~/.toolchains/gcc-x86_64-elf/bin")
toolchain_end()

toolchain("clang-x86_64-elf")
    set_kind("standalone")
    set_sdkdir("/usr/lib/llvm-22")
    set_bindir("/usr/lib/llvm-22/bin")
toolchain_end()

target("kernel-x86_64")
    set_toolchains("clang-x86_64-elf", "nasm", { plat = "cross", arch="x64" })
    set_kind("binary")
    set_targetdir("$(builddir)", { bindir = "bin", libdir = "lib" })
    set_objectdir("$(builddir)/objs")
    set_filename("krystal.kernel-$(mode)-$(arch).elf")

    add_cxxflags(
        "-O0",
        "-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare",
        "-Wno-write-strings -Waddress-of-packed-member",
        "-ffreestanding -fno-rtti -fno-exceptions -fno-stack-protector",
        "-fno-pic -fno-builtin",
        "-mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4.1 -mno-sse4.2",
        "-mno-avx -mno-avx2 -mno-mmx",
        "-mcmodel=kernel",
        "--target=x86_64-pc-none-elf",
        "-march=x86-64",
        "-g"
    )
    add_ldflags(
        "-static",
        "-m elf_x86_64",
        "-nostdlib",
        "-z max-page-size=0x1000"
    )

    add_files("src/kernel/*.cpp")
    add_files("src/arch/x86_64/*.cpp")
    add_files("src/mm/*.cpp")


--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

