#pragma once

#include <sdk-meta/limits.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

namespace Sdk::Math {

template <Meta::Integral T>
struct RandomIdGen {
    u64 _seed;
    T   _cur = Limits<T>::MIN;
};

struct DiscreteIdGen { };

template <Meta::Integral T>
struct LoopIdGen {
    T _cur = Limits<T>::MIN;

    T next(T limit = Limits<T>::max()) {
        if (_cur == Limits<T>::MAX) {
            _cur = Limits<T>::MIN;
        }
        return (_cur++ % limit);
    }
};

} // namespace Sdk::Math
