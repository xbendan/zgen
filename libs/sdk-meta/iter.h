#pragma once

#include <sdk-meta/box.h>
#include <sdk-meta/callable.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/tuple.h>
#include <sdk-meta/types.h>

template <typename Fn>
struct Iter {
    Fn next;
    using Item = decltype(next()); // Opt<T> or T*
    using V    = Meta::RemoveCvRef<decltype(*Meta::declval<Item>())>; // T

    // MARK: - Aggregation

    constexpr auto reduce(auto f) -> Opt<V>
        requires(Meta::Same<Meta::Ret<decltype(f), Item, Item>, V>)
    {
        auto acc = next();
        if (not acc) {
            return Empty {};
        }

        Opt<V> v = *acc;
        for (auto item = next(); item; item = next()) {
            v = f(v, item);
        }
        return v;
    }

#define reduce$(expr) reduce([&](auto x, auto y) { return expr; })

    constexpr auto all(auto pred) const -> bool
        requires(Meta::Boolean<Meta::Ret<decltype(pred), V>>)
    {
        for (auto item = next(); item; item = next()) {
            if (not pred(*item)) {
                return false;
            }
        }
        return true;
    }

#define all$(expr) all([&](auto x) { return expr; })

    constexpr auto any(auto pred) const -> bool
        requires(Meta::Boolean<Meta::Ret<decltype(pred), V>>)
    {
        for (auto item = next(); item; item = next()) {
            if (pred(*item)) {
                return true;
            }
        }
        return false;
    }

#define any$(expr) any([&](auto x) { return expr; })

    constexpr auto count() const -> usize {
        usize cnt = 0;
        for (auto item = next(); item; item = next()) {
            ++cnt;
        }
        return cnt;
    }

    constexpr auto contains(Item const& item) const -> bool
        requires(Meta::Equatable<V>)
    {
        for (auto v = next(); v; v = next()) {
            if (*v == *item) {
                return true;
            }
        }
        return false;
    }

    constexpr void forEach(auto f) {
        for (auto item = next(); item; item = next()) {
            f(*item);
        }
    }

#define forEach$(expr) forEach([&](auto x) { expr; })

    // MARK: - Arithmetic

    constexpr auto sum() const -> Item
        requires(Meta::Computable<V>)
    {
        Item sum {};
        for (auto item = next(); item; item = next()) {
            if (not sum) {
                sum = *item;
            } else {
                sum = *sum + *item;
            }
        }
        return sum;
    }

    constexpr auto avg() const -> Item
        requires(Meta::Computable<V>)
    {
        Item  sum {};
        usize count = 0;
        for (auto item = next(); item; item = next()) {
            if (not sum) {
                sum = *item;
            } else {
                sum = *sum + *item;
            }
            ++count;
        }
        if (count == 0) {
            return Empty {};
        }
        return sum / count;
    }

    constexpr auto max() -> Item
        requires(Meta::Comparable<V>)
    {
        Item maxItem {};
        for (auto item = next(); item; item = next()) {
            if (not maxItem or *item > *maxItem) {
                maxItem = *item;
            }
        }
        return maxItem;
    }

    constexpr auto min() -> Item
        requires(Meta::Comparable<V>)
    {
        Item minItem {};
        for (auto item = next(); item; item = next()) {
            if (not minItem or *item < *minItem) {
                minItem = *item;
            }
        }
        return minItem;
    }

    constexpr auto max(auto f) -> Item
        requires(Meta::Computable<Meta::Ret<decltype(f), V>>)
    {
        Item                           maxItem {};
        Opt<Meta::Ret<decltype(f), V>> maxKey {};

        for (auto item = next(); item; item = next()) {
            auto key = f(*item);
            if (not maxKey or key > *maxKey) {
                maxKey  = key;
                maxItem = item;
            }
        }
        return maxItem;
    }

#define max$(expr) max([&](auto x) { return expr; })

    constexpr auto min(auto f) -> Item
        requires(Meta::Computable<Meta::Ret<decltype(f), V>>)
    {
        Item                           minItem {};
        Opt<Meta::Ret<decltype(f), V>> minKey {};

        for (auto item = next(); item; item = next()) {
            auto key = f(*item);
            if (not minKey or key < *minKey) {
                minKey  = key;
                minItem = item;
            }
        }
        return minItem;
    }

#define min$(expr) min([&](auto x) { return expr; })

    // MARK: - Projection

    constexpr auto map(auto f) {
        using R = Meta::Ret<decltype(f), V>;

        auto n = [=, *this] mutable -> Opt<R> {
            auto item = next();
            if (not item) {
                return Empty {};
            }

            return f(*item);
        };
        return Iter<decltype(n)> { n };
    }

#define map$(expr) map([&](auto x) { return expr; })

    constexpr auto filter(auto pred) {
        auto n = [=, *this] mutable -> Item {
            auto v = next();
            if (not v) {
                return Empty {};
            }

            while (not pred(*v)) {
                v = next();
                if (not v) {
                    return Empty {};
                }
            }

            return v;
        };

        return Iter<decltype(n)> { n };
    }

#define filter$(expr) filter([&](auto x) { return expr; })

    constexpr auto index() const {
        auto n = [=, *this, i = usize { 0 }] mutable -> Opt<Pair<usize, V>> {
            auto item = next();
            if (not item) {
                return Empty {};
            }
            return Pair { i++, *item };
        };
        return Iter<decltype(n)> { n };
    }

    constexpr auto skip(usize count) const {
        auto n = [=, *this, skipped = false] mutable -> Item {
            if (skipped) {
                return next();
            }

            for (usize i = 0; i < count; ++i) {
                auto item = next();
                if (not item) {
                    return Empty {};
                }
            }
            skipped = true;
            return next();
        };
        return Iter<decltype(n)> { n };
    }

    constexpr auto limit(usize count) const {
        auto n = [=, *this, taken = usize { 0 }] mutable -> Item {
            if (taken >= count) {
                return Empty {};
            }

            auto item = next();
            if (not item) {
                return Empty {};
            }

            ++taken;
            return item;
        };
        return Iter<decltype(n)> { n };
    }

    constexpr auto repeat(usize n) const {
        return Iter { [start = *this, curr = *this, i = 0, n] mutable {
            auto v = curr.next();

            if (not v and i < n) {
                curr = start;
                i++;
                v = curr.next();
            }

            return v;
        } };
    }

    // constexpr auto prepend(V const& v) {
    //     auto n = [=, *this, first = true] mutable -> Item {
    //         if (first) {
    //             first = false;
    //             return Item { v };
    //         }

    //         return next();
    //     };
    //     return Iter<decltype(n)> { n };
    // }

    constexpr auto prepend(Iter iter) {
        auto n = [=, *this, l = iter, consumed = false] mutable -> Item {
            if (not consumed) {
                auto item = l.next();
                if (item) {
                    return item;
                } else {
                    consumed = true;
                }
            }

            return next();
        };
        return Iter<decltype(n)> { n };
    }

    // constexpr auto append(V const& v) {
    //     auto n = [=, *this, last = false] mutable -> Item {
    //         if (last) {
    //             return Empty {};
    //         }

    //         auto item = next();
    //         if (not item) {
    //             last = true;
    //             return Item { v };
    //         }

    //         return item;
    //     };
    //     return Iter<decltype(n)> { n };
    // }

    constexpr auto append(auto it) {
        auto n = [=, *this, consumed = false] mutable -> Item {
            if (not consumed) {
                auto item = next();
                if (item) {
                    return item;
                }
                consumed = true;
            }

            return it.next();
        };
        return Iter<decltype(n)> { n };
    }

    // MARK: - Index Access

    constexpr auto elementAt(usize index) const -> Item {
        if (index < 0) [[unlikely]] {
            panic("Iter::elementAt: index out of bounds");
        }

        usize count = 0;
        for (auto item = next(); item; item = next()) {
            if (count == index) {
                return *item;
            }
            ++count;
        }

        return Empty {};
    }

    constexpr auto elementAtOrDefault(usize index, Item const& def) const
        -> Item {
        if (index < 0) [[unlikely]] {
            panic("Iter::elementAtOrDefault: index out of bounds");
        }

        usize count = 0;
        for (auto item = next(); item; item = next()) {
            if (count == index) {
                return *item;
            }
            ++count;
        }

        return def;
    }

    // MARK: - First & Last

    constexpr auto first() const -> Opt<V> {
        auto item = next();
        if (item) {
            return *item;
        }
        return Empty {};
    }

    constexpr auto first(auto f) -> Opt<V> {
        for (auto item = next(); item; item = next()) {
            if (f(*item)) {
                return *item;
            }
        }
        return Empty {};
    }

#define first$(expr) first([&](auto it) { return expr; })

    constexpr auto firstOrDefault(Item const& def) const -> Item {
        auto item = next();
        if (item) {
            return *item;
        }
        return def;
    }

    constexpr auto last() const -> Item {
        Item lastItem;
        for (auto item = next(); item; item = next()) {
            lastItem = *item;
        }
        return lastItem;
    }

    constexpr auto lastOrDefault(Item const& def) const -> Item {
        Item lastItem;
        for (auto item = next(); item; item = next()) {
            lastItem = *item;
        }
        return lastItem.unwrapOrElse(def);
    }

    // MARK: - Collect

    template <typename C>
    constexpr auto collect(C& c) -> void {
        forEach([&](auto v) { c.pushBack(v); });
    }

    template <typename C>
    constexpr auto collect() -> Opt<Box<C>> {
        auto c = makeBox<C>();
    }

    struct It {
        Item curr;
        Iter iter;

        constexpr auto& operator*() { return *curr; }

        constexpr auto operator++() {
            curr = iter.next();
            return *this;
        }

        constexpr bool operator!=(Empty) { return curr != Empty {}; }
    };

    constexpr It begin() { return It { next(), *this }; }

    constexpr Empty end() { return Empty {}; }
};

template <typename T>
constexpr auto single(T value) {
    return Iter<Empty> { [value, end = false] mutable {
        if (end) {
            return Empty {};
        }

        end = true;
        return value;
    } };
}

template <typename T>
constexpr auto repeat(T value, usize count) {
    return Iter { [value, count] mutable -> Opt<T> {
        if (count == 0) {
            return {};
        }

        count--;
        return value;
    } };
}

template <typename T>
constexpr auto range(T end) {
    return Iter { [value = static_cast<T>(0), end] mutable -> Opt<T> {
        if (value >= end)
            return Empty {};
        return value++;
    } };
}

template <typename T>
constexpr auto range(T start, T end) {
    return Iter { [value = start, start, end] mutable -> Opt<T> {
        if (value >= end) {
            return {};
        }

        auto result = value;
        if (start < end) {
            value++;
        } else {
            value++;
        }
        return result;
    } };
}

template <typename T>
constexpr auto range(T start, T end, T step) {
    return Iter { [value = start, start, end, step] mutable -> Opt<T> {
        if (value >= end) {
            return {};
        }

        auto result = value;
        if (start < end) {
            value += step;
        } else {
            value -= step;
        }
        return result;
    } };
}
