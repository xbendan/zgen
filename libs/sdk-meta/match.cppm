module;

export module sdk:match;

import :array;
import :tuple;

export template <typename K, typename V, typename... Entries>
constexpr V match(K const& key, V defaultValue, Entries... entries) {
    V                     result = defaultValue;
    [[maybe_unused]] bool _
        = ((key == entries.v0 ? (result = entries.v1, true) : false) || ...);
    return result;
}

export template <typename K, typename V>
constexpr Pair<K, V> _case(K const& key, V const& value) {
    return { key, value };
}
