#include <sdk-meta/panic.h>

extern "C" int __cxa_atexit(void (*f)(void*), void* obj, void* dso) {
    return 0;
}

extern "C" void __cxa_pure_virtual() {
    panic("pure virtual function called");
}

extern "C" {
    void* __dso_handle __attribute__((__weak__)) = 0;
}

namespace __cxxabiv1 {

__extension__ typedef int __guard __attribute__((mode(__DI__)));

extern "C" int __cxa_guard_acquire(__guard* g) {
    return !*(char*) (g);
}

extern "C" void __cxa_guard_release(__guard* g) {
    *(char*) g = 1;
}

extern "C" void __cxa_guard_abort(__guard*) {
}

} // namespace __cxxabiv1
