cmake_minimum_required(VERSION 3.28)

# =========================================================
#  [核心修复] 交叉编译系统配置 (必须在 project() 之前!)
# =========================================================

# 1. 告诉 CMake 我们不是在构建 macOS 程序，而是裸机 (Generic)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# 2. 强制禁用 macOS 自动注入的 flag (-arch arm64, -isysroot)
set(CMAKE_OSX_SYSROOT "")
set(CMAKE_OSX_ARCHITECTURES "")

# 3. [关键] 告诉 CMake 不要尝试链接测试程序
# 因为裸机环境没有 libc/_start，默认的链接测试一定会失败。
# 设置为 STATIC_LIBRARY 后，CMake 只要能编译出 .o 文件就算测试通过。
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# =========================================================
#  编译器与扫描器配置
# =========================================================

set(CMAKE_CXX_COMPILER "/opt/llvm/21.1.8/bin/clang++")
set(CMAKE_ASM_NASM_COMPILER "nasm")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 显式指定模块依赖扫描器
set(CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS "/opt/llvm/21.1.8/bin/clang-scan-deps")

# 强制开启模块支持
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_MODULE_MAP_FORMAT "clang") 
set(CMAKE_CXX_MODULE_MAP_FLAG "@<MODULE_MAP_FILE>")
set(CMAKE_CXX_EXTENSIONS OFF)

# =========================================================
#  项目定义
# =========================================================

project(realms LANGUAGES CXX ASM_NASM)

# --- 2. 源代码与模块自动探测 ---
# 定义搜索路径 [cite: 2] src = src/realms/ specs/ ...
set(SEARCH_DIRS 
    "src/realms" 
    "specs" 
    "src/arch/${TARGET_ARCH}")

file(GLOB_RECURSE LIB_MODULES
    "libs/*.cppm"
)

file(GLOB_RECURSE SPEC_MODULES
    "specs/*.cppm"
)

# 自动探测 .cpp 源文件
file(GLOB_RECURSE CPP_SOURCES 
    "src/arch/${TARGET_ARCH}/*.cpp"
    "src/realms/*.cpp"
)

# 自动探测 .s 汇编文件
file(GLOB_RECURSE ASM_SOURCES "src/arch/${TARGET_ARCH}/*.s")

# 自动探测 .cppm 模块接口文件 (这是 CMake 模块化的关键)
file(GLOB_RECURSE MOD_SOURCES 
)

# --- 3. 定义目标 (Target) ---
add_executable(realms)

# 添加普通源文件和汇编文件
# [关键步骤] 添加 C++20 模块文件
# CMake 会自动扫描这些文件的 export module / import 语句并建立依赖图
target_sources(realms 
    # PRIVATE 
    #     ${CPP_SOURCES} ${ASM_SOURCES}
    PUBLIC
        FILE_SET CXX_MODULES FILES ${LIB_MODULES}
)

target_compile_features(realms PRIVATE
    cxx_std_26)

# --- 4. 编译选项 (对应 Makefile cxxflags) ---
# [cite: 1, 2]
target_compile_options(realms PRIVATE
    -target x86_64-pc-none-elf
    -isystem /opt/llvm/21.1.8/x86_64-pc-none-elf/include/c++/v1
    -O0
    -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare
    -Waddress-of-packed-member -Wno-unknown-warning-option
    -Wno-user-defined-literals
    -fno-omit-frame-pointer
    -ffreestanding -fno-rtti -fno-exceptions -fno-stack-protector
    -fno-unwind-tables -fno-asynchronous-unwind-tables
    -fno-pic
    -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4.1 -mno-sse4.2
    -mno-avx -mno-avx2 -mno-mmx
    -mcmodel=kernel -mno-red-zone
)

# 定义宏 [cite: 2]
target_compile_definitions(realms PRIVATE
    __meta_enable_ranges_deduction
    __meta_enable_global_namespace
)

# 头文件路径 [cite: 1, 2] includes = libs/ src/ specs/ ...
target_include_directories(realms PRIVATE
    libs src specs
    "${SYSROOT}/include/c++/v1"
)

# 汇编器选项 (NASM) [cite: 3] -f elf64
set(CMAKE_ASM_NASM_FLAGS "-f elf64")

# --- 5. 链接选项 (对应 Makefile ldflags) ---
# [cite: 2]
target_link_options(realms PRIVATE
    --sysroot=${SYSROOT}
    -static
    -L${SYSROOT}/lib
    -L${SYSROOT}/lib/baremetal
    -L/opt/llvm/21.1.8/lib
    -L/opt/llvm/21.1.8/lib/clang/21/lib/x86_64-unknown-none-elf
    "LINKER:--wrap=malloc" 
    "LINKER:--wrap=free" 
    "LINKER:--wrap=calloc" 
    "LINKER:--wrap=realloc"
    -z max-page-size=0x1000
    -T ${LINKER_SCRIPT}
)

# 链接库 [cite: 2]
target_link_libraries(realms PRIVATE
    c++ c++abi c m clang_rt.builtins-${TARGET_ARCH}
)

# --- 6. 生成 ISO 镜像 (对应 Makefile kernel-x86_64 部分) ---
set(BUILD_BIN_DIR "${CMAKE_BINARY_DIR}/bin")
set(ISO_DIR "${CMAKE_BINARY_DIR}/iso_root")
set(FINAL_ISO "${BUILD_BIN_DIR}/realms-${TARGET_ARCH}-v1.0-release.iso")

add_custom_command(TARGET realms POST_BUILD
    COMMENT "Generating bootable ISO..."
    # 准备目录结构
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ISO_DIR}/boot/grub
    # 复制 grub.cfg [cite: 4]
    COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_SOURCE_DIR}/build/target/${TARGET_ARCH}/grub.cfg 
        ${ISO_DIR}/boot/grub/grub.cfg
    # 复制内核 ELF 文件 [cite: 3]
    COMMAND ${CMAKE_COMMAND} -E copy 
        $<TARGET_FILE:realms> 
        ${ISO_DIR}/boot/realms-${TARGET_ARCH}-v1.0-release.bin
    # 生成 ISO [cite: 4]
    COMMAND x86_64-elf-grub-mkrescue -o ${FINAL_ISO} ${ISO_DIR}
)

# --- 7. 运行目标 (对应 Makefile run-x86_64) ---
# [cite: 4]
add_custom_target(run
    COMMAND qemu-system-x86_64.exe
        -cdrom ${FINAL_ISO}
        -serial stdio
        -cpu "qemu64,+ssse3,+sse4.1,+sse4.2"
        -smp 4 -m 128m
    DEPENDS realms
    COMMENT "Running in QEMU..."
    USES_TERMINAL
)