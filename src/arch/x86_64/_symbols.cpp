#include <sdk-meta/types.h>

using ptrdiff_t
    = decltype(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));

extern "C" {

    void _exit([[maybe_unused]] int status) {
        while (1) {
            // Infinite loop to halt execution
            __asm__ __volatile__("cli; hlt");
        }
    }

    // clang-format off

    void* __wrap_malloc(usize size) { return nullptr;}
    void __wrap_free(void* ptr) { }
    void* __wrap_realloc(void* ptr, usize size) { return nullptr; }
    void* __wrap_calloc(usize nmemb, usize size) { return nullptr; }

    // clang-format on

    void* sbrk(ptrdiff_t increment) {
        return nullptr;
    }

    void abort() {
        // 1. 禁用中断，防止进一步的混乱
        __asm__ volatile("cli");

        // 2. 可以在这里打印 "Kernel Aborted"

        // 3. 彻底停机
        while (1) {
            __asm__ volatile("hlt");
        }
    }

    void __abort_message(char const* format, ...) {
        // 在这里你可以直接调用你内核的串口打印或屏幕打印函数
        // kprintf("KERNEL PANIC: ");
        // (处理 format 字符串的逻辑...)

        // 最后必须停止系统
        abort();
    }
}
