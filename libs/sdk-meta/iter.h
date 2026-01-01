#pragma once

#include <sdk-meta/buf.h>
#include <sdk-meta/callable.h>
#include <sdk-meta/index.h>
#include <sdk-meta/math.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/tuple.h>
#include <sdk-meta/types.h>

namespace Meta {

template <typename T, typename R = typename T::R>
concept Iterator = requires(T& it) {
    typename T::R;
    typename T::V;

    { it != NONE } -> Meta::Boolean;
    { ++it } -> Meta::Same<T&>;
    { *it } -> Meta::Same<R>;
    { *::declval<R>() } -> Meta::Same<typename T::V&>;
};

enum class Iteration {
    Forward,
    Backward,
    Bidirectional,
    RandomAccess,
};

template <typename T>
struct Arrays {
    using E = T;

    E const* _begin;
    E const* _end;

    [[gnu::always_inline]] constexpr Arrays(E const* begin, E const* end)
        : _begin(begin),
          _end(end) { }

    [[gnu::always_inline]] constexpr usize size() const {
        return static_cast<usize>(_end - _begin);
    }

    template <Iteration It>
    struct _;

    template <>
    struct _<Iteration::Forward> {
        using R = Opt<T&>;
        using V = T&;

        T*       _curr;
        T const* _end;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != _end;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            ++_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_curr == _end) {
                return NONE;
            }
            T& ref = *_curr;
            return R(ref);
        }
    };

    template <>
    struct _<Iteration::Backward> {
        using R = Opt<T&>;
        using V = T&;

        T*       _curr;
        T const* _begin;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != _begin;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            --_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_curr == _begin) {
                return NONE;
            }
            return *(_curr - 1);
        }
    };

    template <>
    struct _<Iteration::Bidirectional> {
        using R = Opt<T&>;
        using V = T&;

        T*       _curr;
        T const* _begin;
        T const* _end;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != _end;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            ++_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr _& operator--() {
            --_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_curr == _end or _curr == _begin) {
                return NONE;
            }
            return *_curr;
        }
    };

    [[gnu::always_inline]] constexpr auto begin() {
        return _<Iteration::Forward> { const_cast<T*>(_begin), _end };
    }

    [[gnu::always_inline]] constexpr auto end() -> None { return NONE; }

    [[gnu::always_inline]] constexpr auto rbegin() {
        return _<Iteration::Backward> { const_cast<T*>(_end), _begin };
    }

    [[gnu::always_inline]] constexpr auto rend() -> None { return NONE; }
};

} // namespace Meta

// namespace Meta {
// template <typename Fn>
// struct Iter {
//     Fn next;
//     // Opt<T>, T*, or any type with operator bool() and operator*()
//     using Item = decltype(next());
//     using V    = Meta::RemoveCvRef<decltype(*::declval<Item>())>;

//     // MARK: - [Aggregation]

//     constexpr auto reduce(Meta::Callable<V, V> auto f)
//         -> Opt<Meta::Ret<decltype(f), V, V>> {
//         auto acc = next();
//         if (not acc) {
//             return None {};
//         }

//         while (auto n = this->next()) {
//             acc = f(*acc, *n);
//         }
//         return acc;
//     };

// #define reduce$(expr) reduce([&](auto x, auto y) { return expr; })

//     constexpr auto reduce(Meta::Callable<V, V> auto f, V initial)
//         -> Opt<Meta::Ret<decltype(f), V, V>> {
//         auto acc = initial;

//         while (auto n = this->next()) {
//             acc = f(*acc, *n);
//         }
//         return acc;
//     };

//     constexpr auto reduce(Meta::Callable<V, V> auto f,
//                           Meta::Callable<V> auto    sel)
//         -> Opt<Meta::Ret<decltype(sel), Item>> {
//         auto acc = next();
//         if (not acc) {
//             return None {};
//         }

//         while (auto n = this->next()) {
//             acc = f(*acc, *n);
//         }
//         return sel(*acc);
//     };

//     constexpr auto reduce(Meta::Callable<V, V> auto f,
//                           V                         initial,
//                           Meta::Callable<V> auto    sel)
//         -> Opt<Meta::Ret<decltype(sel), V>> {
//         auto acc = initial;

//         while (auto n = this->next()) {
//             acc = f(*acc, *n);
//         }
//         return sel(acc);
//     };

//     constexpr auto all(Meta::Callable<V> auto pred) -> bool
//         requires(Meta::Boolean<Meta::Ret<decltype(pred), Item>>)
//     {
//         for (auto item = next(); item; item = next()) {
//             if (not pred(*item)) {
//                 return false;
//             }
//         }
//         return true;
//     }

// #define all$(expr) all([&](auto it) { return expr; })

//     constexpr auto any(Meta::Callable<V> auto pred) -> bool
//         requires(Meta::Boolean<Meta::Ret<decltype(pred), Item>>)
//     {
//         for (auto item = next(); item; item = next()) {
//             if (pred(*item)) {
//                 return true;
//             }
//         }
//         return false;
//     }

// #define any$(expr) any([&](auto it) { return expr; })

//     // MARK: - [Mathematical]

//     constexpr auto avg() -> Item
//         requires(Meta::Computable<V>)
//     {
//         Item  sum {};
//         usize count = 0;
//         for (auto item = next(); item; item = next()) {
//             if (not sum) {
//                 sum = *item;
//             } else {
//                 sum = *sum + *item;
//             }
//             ++count;
//         }
//         if (count == 0) {
//             return None {};
//         }
//         return sum / count;
//     }

//     constexpr auto avg(Meta::Callable<V> auto sel) -> decltype(auto)
//         requires(Meta::Computable<Meta::Ret<decltype(sel), V>>)
//     {
//         using R = Meta::Ret<decltype(sel), V>;

//         R     sum {};
//         usize count = 0;
//         for (auto item = next(); item; item = next()) {
//             if (not sum) {
//                 sum = sel(*item);
//             } else {
//                 sum = sum + sel(*item);
//             }
//             ++count;
//         }
//         if (count == 0) {
//             return NONE;
//         }
//         return sum / count;
//     }

// #define avg$(expr) avg([&](auto it) { return expr; })

//     constexpr auto sum() -> Item
//         requires(Meta::Computable<V>)
//     {
//         Item sum {};
//         for (auto item = next(); item; item = next()) {
//             if (not sum) {
//                 sum = *item;
//             } else {
//                 sum = *sum + *item;
//             }
//         }
//         return sum;
//     }

//     constexpr auto sum(Meta::Callable<V> auto sel) -> decltype(auto)
//         requires(Meta::Computable<Meta::Ret<decltype(sel), V>>)
//     {
//         using R = Meta::Ret<decltype(sel), V>;

//         R sum {};
//         for (auto item = next(); item; item = next()) {
//             if (not sum) {
//                 sum = sel(*item);
//             } else {
//                 sum = sum + sel(*item);
//             }
//         }
//         return sum;
//     }

// #define sum$(expr) sum([&](auto it) { return expr; })

//     constexpr auto max() -> Item
//         requires(Meta::Comparable<V>)
//     {
//         Item max {};
//         for (auto item = next(); item; item = next()) {
//             if (not max) {
//                 max = *item;
//             } else {
//                 max = ::max(max, *item);
//             }
//         }
//         return max;
//     }

//     constexpr auto max(Meta::Callable<V> auto sel) -> Item
//         requires(Meta::Comparable<Meta::Ret<decltype(sel), V>>)
//     {
//         using R = Meta::Ret<decltype(sel), V>;

//         Item max {};
//         R    maxKey {};
//         for (auto item = next(); item; item = next()) {
//             if (not max) {
//                 max    = *item;
//                 maxKey = sel(*item);
//             } else {
//                 auto key = sel(*item);
//                 if (key > maxKey) {
//                     max    = *item;
//                     maxKey = key;
//                 }
//             }
//         }
//         return max;
//     }

// #define max$(expr) max([&](auto it) { return expr; })

//     constexpr auto min() -> Item
//         requires(Meta::Comparable<V>)
//     {
//         Item min {};
//         for (auto item = next(); item; item = next()) {
//             if (not min) {
//                 min = *item;
//             } else {
//                 min = ::min(min, *item);
//             }
//         }
//         return min;
//     }

//     constexpr auto min(Meta::Callable<V> auto sel) -> Item
//         requires(Meta::Comparable<Meta::Ret<decltype(sel), V>>)
//     {
//         using R = Meta::Ret<decltype(sel), V>;

//         Item min {};
//         R    minKey {};
//         for (auto item = next(); item; item = next()) {
//             if (not min) {
//                 min    = *item;
//                 minKey = sel(*item);
//             } else {
//                 auto key = sel(*item);
//                 if (key < minKey) {
//                     min    = *item;
//                     minKey = key;
//                 }
//             }
//         }
//         return min;
//     }

// #define min$(expr) min([&](auto it) { return expr; })

//     // MARK: - [Projection]

//     template <typename U>
//         requires(Meta::Convertible<V, U>)
//     constexpr auto cast() {
//         auto n = [=, *this]() mutable -> Opt<U> {
//             if (auto item = next()) {
//                 return static_cast<U>(*item);
//             }
//             return NONE;
//         };
//         return Iter<decltype(n)> { n };
//     }

//     constexpr auto chunk(usize size) {
//         if (size == 0) [[unlikely]] {
//             panic("Iter::chunk: size must be greater than 0");
//         }

//         // TODO: validate algorithm for correctness and efficiency
//         auto n
//             = [=, *this, buf = Buf<V>(size), count = 0] mutable -> Opt<Buf<V>> {
//             while (count < size) {
//                 if (auto item = next()) {
//                     buf.emplace(count++, *item);
//                 } else
//                     break;
//             }

//             if (count == 0) {
//                 return NONE;
//             }

//             if (count == size) {
//                 count = 0;
//                 return ::move(buf);
//             }
//         };
//         return Iter<decltype(n)> { n };
//     }

//     constexpr auto concat(auto it)
//         requires(Meta::Same<Item, typename decltype(it)::Item>)
//     {
//         auto n = [=, *this, consumed = false]() mutable -> Item {
//             if (not consumed) {
//                 if (auto item = next(); item) {
//                     return item;
//                 }
//                 consumed = true;
//             }

//             return it.next();
//         };
//         return Iter<decltype(n)> { n };
//     }

//     constexpr auto prepend(Item& v) {
//         auto n = [=, *this, consumed = false]() mutable -> Item {
//             if (not consumed) {
//                 if (auto item = next(); item) {
//                     return item;
//                 }
//                 consumed = true;
//             }

//             return v;
//         };
//         return Iter<decltype(n)> { n };
//     }

//     constexpr auto append(Item& v) {
//         auto n = [=, *this, consumed = false]() mutable -> Item {
//             if (not consumed) {
//                 if (auto item = next(); item) {
//                     return item;
//                 }
//                 consumed = true;
//             }

//             return v;
//         };
//         return Iter<decltype(n)> { n };
//     }

//     constexpr auto select(Meta::Callable<V> auto f) {
//         auto n = [=, *this]() mutable -> Opt<Meta::Ret<decltype(f), V>> {
//             if (auto item = next()) {
//                 return f(*item);
//             }
//             return NONE;
//         };
//         return Iter<decltype(n)> { n };
//     }

// #define select$(expr) select([&](auto it) -> decltype(expr) { return expr; })

//     constexpr auto select(Meta::Callable<usize, Meta::Index> auto f) {
//         using R = Meta::Ret<decltype(f), usize, Meta::Index>;

//         auto n = [=, *this, index = usize { 0 }] mutable -> Opt<R> {
//             if (auto item = next()) {
//                 return f(index++, Index { index - 1, false });
//             }
//             return NONE;
//         };
//         return Iter<decltype(n)> { n };
//     }

// #define select$i(expr) select([&](auto it, auto index) { return expr; })

//     constexpr auto filter(Meta::Callable<V> auto pred) {
//         auto n = [=, *this]() mutable -> Item {
//             auto v = next();
//             if (not v) {
//                 return NONE;
//             }

//             while (v and not pred(*v)) {
//                 v = next();
//             }
//             return v;
//         };
//         return Iter<decltype(n)> { n };
//     }

//     // TODO: implement selectMany

//     constexpr auto selectMany(Meta::Callable<Item> auto f) const;

//     constexpr auto selectMany(Meta::Callable<usize, Meta::Index> auto f) const;

//     constexpr auto filter(Meta::Callable<usize, V> auto pred) {
//         auto n = [=, *this, index = usize { 0 }] mutable -> Item {
//             while (true) {
//                 auto item = next();
//                 if (not item) {
//                     return NONE;
//                 }
//                 if (pred(index++, *item)) {
//                     return item;
//                 }
//             }
//         };
//         return Iter<decltype(n)> { n };
//     }

// #define filter$i(expr) filter([&](auto it, auto index) { return expr; })

//     constexpr auto distinct() const;

//     // MARK: - [Consumption]

//     constexpr auto exists() const -> bool { return next() != NONE; }

//     constexpr auto count() -> usize {
//         usize cnt = 0;
//         for (auto item = next(); item; item = next()) {
//             ++cnt;
//         }
//         return cnt;
//     }

//     constexpr auto count(Meta::Predicate<Item> auto pred) -> usize {
//         usize cnt = 0;
//         for (auto item = next(); item; item = next()) {
//             if (pred(*item)) {
//                 ++cnt;
//             }
//         }
//         return cnt;
//     }

// #define count$(expr) count([&](auto it) { return expr; })

//     constexpr auto defaultIfEmpty(V const& def) const -> Item {
//         if (auto v = next()) {
//             return v;
//         }
//         return def;
//     }

//     constexpr auto defaultIfEmpty(Meta::Callable<> auto def) const -> Item
//         requires(Meta::Same<Item, decltype(def())>)
//     {
//         if (auto v = next()) {
//             return v;
//         }
//         return def();
//     }

// #define defaultIfEmpty$(expr) defaultIfEmpty([&]() { return expr; })

//     constexpr auto defaultIfEmpty() -> Item {
//         if (auto v = next()) {
//             return v;
//         }
//         return None {};
//     }

//     constexpr auto sequenceEquals(auto it)
//         requires(Meta::Same<Item, typename decltype(it)::Item>
//                  and Meta::Equatable<V>)
//     {
//         while (true) {
//             auto a = next();
//             auto b = it.next();

//             if (not a and not b) {
//                 return true;
//             }

//             if (not a or not b) {
//                 return false;
//             }

//             if (*a != *b) {
//                 return false;
//             }
//         }
//     }

//     constexpr auto forEach(auto f) -> void {
//         for (auto item = next(); item; item = next()) {
//             f(*item);
//         }
//     }

// #define forEach$(expr) forEach([&](auto it) { expr; })

//     constexpr auto forEachIndexed(auto f) -> void {
//         usize index = 0;
//         for (auto item = next(); item; item = next()) {
//             f(index++, *item);
//         }
//     }

// #define forEachIndexed$(expr)                                                  \
//     forEachIndexed([&](usize index, auto it) { expr; })

//     // MARK: - [Index Access]

//     constexpr auto elementAt(usize index) -> Item {
//         if (index < 0) [[unlikely]] {
//             return NONE;
//         }
//         usize count = 0;
//         for (auto item = next(); item; item = next()) {
//             if (count == index) {
//                 return item;
//             }
//             ++count;
//         }
//         return None {};
//     }

//     constexpr auto elementAt(Meta::Index index) -> Item {
//         usize i = index.val;
//         if (index.inverse) {
//             decltype(*this) copy = *this;

//             i = copy.count() - i - 1;
//         }
//         return elementAt(i);
//     }

//     constexpr auto elementAtOrDefault(usize index, Item const& def) const
//         -> Item {
//         if (index < 0) [[unlikely]] {
//             // TODO: change to log warning
//             panic("Iter::elementAtOrDefault: index out of bounds");
//         }

//         usize count = 0;
//         for (auto item = next(); item; item = next()) {
//             if (count == index) {
//                 return *item;
//             }
//             ++count;
//         }

//         return def;
//     }

//     constexpr auto elementAtOrDefault(Meta::Index index, Item const& def) const
//         -> Item {
//         usize i = index.val;
//         if (index.inverse) {
//             decltype(*this) copy = *this;

//             i = copy.count() - i - 1;
//         }
//         return elementAtOrDefault(i, def);
//     }

//     // MARK: - [Union & Intersection]

//     constexpr auto except(auto it) const
//         requires(Meta::Same<Item, typename decltype(it)::Item>
//                  and Meta::Comparable<V>)
//     {
//         // TODO: implement
//     }

//     constexpr auto intersect(auto it) const
//         requires(Meta::Same<Item, typename decltype(it)::Item>
//                  and Meta::Comparable<V>)
//     {
//         // TODO: implement
//     }

//     // MARK: - [First & Last]

//     constexpr auto first() -> Item { return next(); }

//     constexpr auto first(auto f) -> Item {
//         for (auto item = next(); item; item = next()) {
//             if (f(*item)) {
//                 return item;
//             }
//         }
//         return NONE;
//     }

// #define first$(expr) first([&](auto& it) -> bool { return expr; })

//     constexpr auto last() -> Opt<V> {
//         Opt<V> last {};
//         for (auto item = next(); item; item = next()) {
//             last = *item;
//         }
//         return last;
//     }

//     constexpr auto last(Meta::Predicate<Item> auto f) -> Opt<V> {
//         Opt<V> last {};
//         for (auto item = next(); item; item = next()) {
//             if (f(*item)) {
//                 last = *item;
//             }
//         }
//         return last;
//     }

// #define last$(expr) last([&](auto it) { return expr; })

//     constexpr auto lastOrDefault(V const& def) -> V {
//         Opt<V> last {};
//         for (auto item = next(); item; item = next()) {
//             last = *item;
//         }
//         return last.orElse(def);
//     }

//     // MARK: - [Grouping]

//     constexpr auto groupBy(Meta::Callable<Item> auto keySelector) const;

//     constexpr auto groupBy(Meta::Callable<Item> auto keySelector,
//                            Meta::Callable<Item> auto elementSelector) const;

//     constexpr auto groupJoin() const;

//     constexpr auto index() {
//         auto n = [=,
//                   *this,
//                   index = usize { 0 }] mutable -> Opt<Meta::Pair<usize, V>> {
//             if (auto item = next()) {
//                 return Meta::Pair { index++, *item };
//             }
//             return NONE;
//         };
//         return Iter<decltype(n)> { n };
//     }

//     // MARK: - [Sorting]

//     constexpr auto order() const -> decltype(auto)
//         requires(Meta::Comparable<V>);

//     constexpr auto order(Meta::Comparator<V> auto cmp) const -> decltype(auto);

//     constexpr auto orderBy(Meta::Callable<Item> auto keySelector) const
//         -> decltype(auto)
//         requires(Meta::Comparable<Meta::Ret<decltype(keySelector), V>>);

//     constexpr auto orderBy(
//         Meta::Callable<Item> auto                                  keySelector,
//         Meta::Comparator<Meta::Ret<decltype(keySelector), V>> auto cmp) const
//         -> decltype(auto);

//     constexpr auto orderDescending() const -> decltype(auto)
//         requires(Meta::Comparable<V>);

//     constexpr auto orderDescending(Meta::Comparator<V> auto cmp) const
//         -> decltype(auto);

//     constexpr auto orderByDescending(
//         Meta::Callable<Item> auto keySelector) const -> decltype(auto)
//         requires(Meta::Comparable<Meta::Ret<decltype(keySelector), V>>);

//     constexpr auto orderByDescending(
//         Meta::Callable<Item> auto                                  keySelector,
//         Meta::Comparator<Meta::Ret<decltype(keySelector), V>> auto cmp) const
//         -> decltype(auto);

//     // MARK: - [Skip & Limit]

//     constexpr auto skip(usize count) {
//         auto n = [=, *this, skipped = usize { 0 }] mutable -> Item {
//             while (skipped < count) {
//                 auto item = next();
//                 if (not item) {
//                     return NONE;
//                 }
//                 ++skipped;
//             }
//             return next();
//         };
//         return Iter<decltype(n)> { n };
//     }

//     constexpr auto limit(usize count) {
//         auto n = [=, *this, taken = usize { 0 }] mutable -> Item {
//             if (taken >= count) {
//                 return NONE;
//             }
//             ++taken;
//             return next();
//         };
//         return Iter<decltype(n)> { n };
//     }

//     constexpr auto repeat(usize count) {
//         auto n = [start = *this, curr = *this, i = 0, count] mutable {
//             auto v = curr.next();

//             if (not v and i < count) {
//                 curr = start;
//                 i++;
//                 v = curr.next();
//             }

//             return v;
//         };
//         return Iter<decltype(n)> { n };
//     }

//     constexpr auto cycle() {
//         auto n = [start = *this, curr = *this]() mutable -> Item {
//             auto v = curr.next();

//             if (not v) {
//                 curr = start;
//                 v    = curr.next();
//             }

//             return v;
//         };
//         return Iter<decltype(n)> { n };
//     }

//     // MARK: - [begin, end]

//     struct It {
//         Item curr;
//         Iter iter;

//         constexpr auto& operator*() { return *curr; }

//         constexpr auto operator++() {
//             curr = iter.next();
//             return *this;
//         }

//         constexpr bool operator!=(None) { return curr != NONE; }
//     };

//     constexpr It begin() { return It { next(), *this }; }

//     constexpr None end() { return NONE; }
// };

// // MARK: - Instantiations

// constexpr auto single(auto value) {
//     auto n = [value, end = false] mutable {
//         if (end) {
//             return NONE;
//         }
//         end = true;
//         return value;
//     };
//     return Iter<decltype(n)> { n };
// }

// constexpr auto repeat(auto value, usize count) {
//     auto n = [value, count] mutable -> Opt<decltype(value)> {
//         if (count == 0) {
//             return {};
//         }
//         count--;
//         return value;
//     };
//     return Iter<decltype(n)> { n };
// }

// template <typename T>
// constexpr auto range(T start, T end, T step) {
//     if (step == 0) [[unlikely]] {
//         panic("Iter::range: step must be non-zero");
//     }

//     auto n = [value = start, end, step]() mutable -> Opt<T> {
//         if ((step > 0 and value >= end) or (step < 0 and value <= end)) {
//             return NONE;
//         }
//         T current = value;
//         value += step;
//         return current;
//     };
//     return Iter<decltype(n)> { n };
// }

// template <typename T>
// constexpr auto range(T end) {
//     return range(static_cast<T>(0), end, static_cast<T>(1));
// }

// template <typename T>
// constexpr auto range(T start, T end) {
//     return range(start, end, static_cast<T>(1));
// }

// // template <Sliceable S>
// // constexpr auto iter(S& slice) {
// //     return Iter([&slice, i = 0uz] mutable -> typename S::Inner* {
// //         if (i >= slice.len()) {
// //             return nullptr;
// //         }

// //         return &slice.buf()[i++];
// //     });
// // }

// template <Sliceable S>
// constexpr auto foreach (S& slice) {
//     return Iter([&slice, i = 0uz] mutable -> Opt<typename S::Inner&> {
//         if (i >= slice.len()) {
//             return NONE;
//         }

//         return &slice.buf()[i++];
//     });
// }

// } // namespace Meta

// using Meta::foreach;
// using Meta::Iter;
// using Meta::range;
// using Meta::repeat;
// using Meta::single;
