#include <sdk-meta/res.h>
#include <sdk-meta/types.h>
#include <zgen/mm/mem.h>

void* operator new(usize size) {
    return Zgen::Core::kmm()
        .alloc(size, Zgen::Hal::KmmAllocFlags::KERNEL)
        .unwrap()
        ._start;
}

void* operator new[](usize size) {
    return Zgen::Core::kmm()
        .alloc(size, Zgen::Hal::KmmAllocFlags::KERNEL)
        .unwrap()
        ._start;
}

void operator delete(void* p) noexcept {
    Zgen::Core::kmm().free(p).unwrap();
}

void operator delete[](void* p) noexcept {
    Zgen::Core::kmm().free(p).unwrap();
}

void operator delete(void* p, usize) noexcept {
    Zgen::Core::kmm().free(p).unwrap();
}

void operator delete[](void* p, usize) noexcept {
    Zgen::Core::kmm().free(p).unwrap();
}
