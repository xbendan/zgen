#pragma once

#include <sdk-meta/box.h>
#include <sdk-meta/dict.h>
#include <sdk-meta/lock.h>
#include <sdk-meta/res.h>
#include <sdk-text/str.h>

namespace Realms::Sys {

template <typename K, typename V>
using DictionaryStrategy = Meta::Dict<K, V>;

template <typename T>
struct Metadata;

template <typename K,
          typename V,
          template <typename, typename> class Strategy = DictionaryStrategy>
struct Registry {
    ReadWriteLock            lock;
    Strategy<K, Metadata<V>> strategy;

    Res<> add(K const& key, Metadata<V> metadata) { }

    Res<> addIfAbsent(K const& key, Metadata<V> metadata) { }

    Res<> remove(K const& key) { };
};

template <typename R, typename T>
struct Registration {
    [[maybe_unused]]
    static inline bool const _
        = []() {
        Metadata<R> metadata = {
            .name       = T::name,
            .priority   = T::priority,
            .requisites = T::requisites,
            .build      = []() -> Opt<Rc<R>> { return makeRc<T>(); },
        };
        return true;
    }();
};

} // namespace Realms::Sys
