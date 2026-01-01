#pragma once

#include <sdk-meta/iter.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/utility.h>

namespace Meta::Ranges {

template <typename T, typename E = typename RemoveCvRef<T>::E>
concept Ranges = requires(T& t) {
    typename RemoveCvRef<T>::E; // Element type

    { t.begin() } -> Iterator;
    { t.end() } -> Meta::Same<None>;
};

template <typename T>
concept Views = Ranges<T> and requires(T& t) {
    typename RemoveCvRef<T>::Ei;

    { t.begin() } -> Meta::Same<typename RemoveCvRef<T>::Ei>;
    { t.end() } -> Meta::Same<None>;
};

template <typename T, typename E = typename RemoveCvRef<T>::E>
concept Sequence = requires(T& t) {
    typename RemoveCvRef<T>::E;

    { t.begin() } -> Meta::Same<E*>;
    { t.end() } -> Meta::Same<E*>;
};

enum class Pipeline {
    // containers
    Referred,
    Owned,

    // views
    Filter,
    Select,
    Take,
    Skip,
    Reverse,
    Index,
    Peek,
    Chunk,
    Split,
    Flatten,
    Readonly,
    Cartesian,

    // aggregators / consumers
    AllMatch,
    AnyMatch,
    NoneMatch,
    Sum,
    Average,
    Max,
    Min,
    MaxBy,
    MinBy,
    Reduce,
    First,
    Last,
    ForEach,
};

static_assert(Ranges<Arrays<int>>); // Test

template <Pipeline P, typename T>
struct Closure {
    static constexpr Pipeline Pipeline = P;
    using Type                         = T;

    T t;
};

// MARK: - #-- Views:

template <Pipeline P, Ranges Rn, typename T>
struct View;

// MARK: - [Referred]

template <Ranges Rn>
struct View<Pipeline::Referred, Rn, None> {
    static_assert(not Views<Rn>);

    Rn& _range;

    using Ei = decltype(_range.begin());
    using E  = typename Rn::E;

    [[gnu::always_inline]] constexpr View(Rn& range) : _range(range) { }

    [[gnu::always_inline]] auto begin() { return _range.begin(); }

    [[gnu::always_inline]] auto end() { return NONE; }
};

// MARK: - [Owned]

template <Ranges Rn>
struct View<Pipeline::Owned, Rn, None> {
    static_assert(not Views<Rn>);

    Rn _range;

    using E  = typename Rn::E;
    using Ei = decltype(_range.begin());

    [[gnu::always_inline]] constexpr View(Rn&& range)
        : _range(::move(range)) { }

    View(View const&)            = delete;
    View& operator=(View const&) = delete;

    View(View&&)            = default;
    View& operator=(View&&) = default;

    [[gnu::always_inline]] auto begin() { return _range.begin(); }

    [[gnu::always_inline]] auto end() { return NONE; }
};

// MARK: - [Filter]

template <Ranges Rn, typename Fn>
struct View<Pipeline::Filter, Rn, Fn> {
    using E = typename Rn::E;
    Rn _range;
    Fn _func;

    struct _ {
        using B = decltype(::declval<Rn>().begin());
        using R = Opt<typename B::V&>;
        using V = typename B::V&;
        B   _curr;
        Fn& _func;

        void ensure() {
            while (_curr != NONE and not _func(*_curr))
                ++_curr;
        }

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != NONE;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            ++_curr;
            ensure();
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            ensure();
            return *_curr;
        }
    };
    static_assert(Iterator<_>);

    using Ei = _;

    [[gnu::always_inline]] constexpr View(Rn range, Fn func)
        : _range(::move(range)),
          _func(func) { }

    [[gnu::always_inline]] auto begin() { return _ { _range.begin(), _func }; }

    [[gnu::always_inline]] auto end() { return NONE; }
};

// MARK: - [Select, Fn]

template <Ranges Rn, typename Fn>
struct View<Pipeline::Select, Rn, Fn> {
    using R = decltype(*declval<Rn>().begin());
    using E = Meta::Ret<Fn, decltype(*declval<R>())>;
    Rn _range;
    Fn _func;

    struct _ {
        using B = decltype(::declval<Rn>().begin());
        using R = Opt<E>;
        using V = E;
        B  _curr;
        Fn _func;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != NONE;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            ++_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            auto obj = *_curr;
            if (obj != NONE) {
                return _func(*obj);
            }
            return NONE;
        }
    };
    static_assert(Iterator<_>);

    [[gnu::always_inline]] constexpr View(Rn range, Fn func)
        : _range(::move(range)),
          _func(func) { }

    [[gnu::always_inline]] auto begin() { return _ { _range.begin(), _func }; }

    [[gnu::always_inline]] auto end() { return NONE; }
};

// MARK: - [Select, ->]

template <Ranges Rn, Field F>
struct View<Pipeline::Select, Rn, F> {
    using Ri = typename RemoveCvRef<Rn>::E;
    using E  = decltype((*::declval<Ri>()).*declval<F>());
    Rn _range;
    F  _field;

    struct _ {
        using B = decltype(::declval<Rn>().begin());
        using R = Opt<E>;
        using V = E;
        B _curr;
        F _field;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != NONE;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            ++_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            return (*_curr)->*_field;
        }
    };
    static_assert(Iterator<_>);

    [[gnu::always_inline]] constexpr View(Rn range, F field)
        : _range(::move(range)),
          _field(field) { }

    [[gnu::always_inline]] auto begin() { return _ { _range.begin(), _field }; }

    [[gnu::always_inline]] auto end() { return NONE; }
};

// MARK: - [Take]

template <Ranges Rn>
struct View<Pipeline::Take, Rn, None> {
    using E = typename Rn::E;
    Rn    _range;
    usize _count;

    struct _ {
        using B = decltype(::declval<Rn>().begin());
        using R = typename B::R;
        using V = typename B::V;
        B     _curr;
        usize _rem;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != NONE and _rem > 0;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            if (not _rem)
                return NONE;

            --_rem;
            ++_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            return _rem > 0 ? _curr : NONE;
        }
    };
    static_assert(Iterator<_>);

    [[gnu::always_inline]] constexpr View(Rn range, usize count)
        : _range(::move(range)),
          _count(count) { }

    [[gnu::always_inline]] auto begin() { return _ { _range.begin(), _count }; }

    [[gnu::always_inline]] auto end() { return NONE; }
};

// MARK: - [Skip]

template <Ranges Rn>
struct View<Pipeline::Skip, Rn, None> {
    using E = typename Rn::E;
    Rn    _range;
    usize _count;

    struct _ {
        using B = decltype(::declval<Rn>().begin());
        using R = typename B::R;
        using V = typename B::V;
        B     _curr;
        usize _rem;

        void ensure() {
            while (_rem > 0 and _curr != NONE) {
                --_rem;
                ++_curr;
            }
        }

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            ensure();
            return _curr != NONE;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            ++_curr;
            ensure();
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            ensure();
            return _curr;
        }
    };
    static_assert(Iterator<_>);

    [[gnu::always_inline]] constexpr View(Rn range, usize count)
        : _range(::move(range)),
          _count(count) { }

    [[gnu::always_inline]] auto begin() { return _ { _range.begin(), _count }; }

    [[gnu::always_inline]] auto end() { return NONE; }
};

// MARK: - [Reverse]

// TODO: check logical correctness

template <Ranges Rn>
struct View<Pipeline::Reverse, Rn, None> {
    using E = typename Rn::E;
    Rn _range;

    struct _ {
        using B = decltype(::declval<Rn>().rbegin());
        using R = typename B::R;
        using V = typename B::V;
        B _curr;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != NONE;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            ++_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() { return _curr; }
    };

    [[gnu::always_inline]] constexpr View(Rn range) : _range(::move(range)) { }

    [[gnu::always_inline]] auto begin() { return _ { _range.rbegin() }; }

    [[gnu::always_inline]] auto end() { return NONE; }

    [[gnu::always_inline]] auto rbegin() { return _ { _range.begin() }; }

    [[gnu::always_inline]] auto rend() { return NONE; }
};

// MARK: - [Index]

template <Ranges Rn>
struct View<Pipeline::Index, Rn, None> {
    using E = Tuple<usize, typename Rn::E>;
    Rn _range;

    struct _ {
        using B = decltype(::declval<Rn>().begin());
        using R = Opt<E>;
        using V = E;
        B     _curr;
        usize _index = 0;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != NONE;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            ++_curr;
            ++_index;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            return { _index, *_curr };
        }
    };

    [[gnu::always_inline]] constexpr View(Rn&& range)
        : _range(::move(range)) { }
};

// MARK: - [Peek]

template <Ranges Rn, typename Fn>
struct View<Pipeline::Peek, Rn, Fn> {
    using E = typename Rn::E;
    Rn _range;
    Fn _func;

    struct _ {
        using B = decltype(::declval<Rn>().begin());
        using R = typename B::R;
        using V = typename B::V;
        B   _curr;
        Fn& _func;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != NONE;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            ++_curr;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            auto obj = *_curr;
            if (obj != NONE) {
                _func(*obj);
                return obj;
            }
            return NONE;
        }
    };

    [[gnu::always_inline]] constexpr View(Rn range, Fn func)
        : _range(::move(range)),
          _func(func) { }

    [[gnu::always_inline]] auto begin() { return _ { _range.begin(), _func }; }

    [[gnu::always_inline]] auto end() { return NONE; }
};

// MARK: - [Chunk]

template <Ranges Rn>
struct View<Pipeline::Chunk, Rn, None> {
    using T = typename Rn::E;

    struct Chunk {
        using B = decltype(::declval<Rn>().begin());
        using R = Opt<T>;
        using V = T;

        B     _begin;
        usize _rem;
        usize _size;

        [[gnu::always_inline]] constexpr Chunk(B begin, usize size)
            : _begin(begin),
              _rem(size),
              _size(size) { }

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _begin != NONE and _rem > 0;
        }

        [[gnu::always_inline]] constexpr Chunk& operator++() {
            if (_rem != 0 and _begin != NONE) {
                --_rem;
            }
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_rem == 0) {
                return NONE;
            }
        }

        [[gnu::always_inline]] constexpr void next() {
            while (_rem > 0 and _begin != NONE) {
                --_rem;
                ++_begin;
            }
        }
    };

    using E = Chunk;
    Rn    _range;
    usize _size;

    struct _ {
        using B = decltype(::declval<Rn>().begin());
        using R = Opt<E>;
        using V = E;

        B _curr;
        E _chunk;

        [[gnu::always_inline]] constexpr _(B curr, usize size)
            : _curr(curr),
              _chunk({ curr, size }) { }

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != NONE;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            if (_curr != NONE) {
                _chunk.next();
            }

            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_curr == NONE) {
                return NONE;
            }
            return _chunk;
        }
    };

    [[gnu::always_inline]] constexpr View(Rn&& range, usize size)
        : _range(::move(range)),
          _size(size) { }

    [[gnu::always_inline]] auto begin() { return _ { _range.begin(), _size }; }

    [[gnu::always_inline]] auto end() { return NONE; }
};

// MARK: - [Split]

template <Ranges Rn, typename Fn>
struct View<Pipeline::Split, Rn, Fn> {
    using T = typename Rn::E;

    struct Split {
        using B = decltype(::declval<Rn>().begin());
        using R = Opt<typename Rn::E>;
        using V = typename Rn::E;

        B  _curr;
        Fn _func;

        [[gnu::always_inline]] constexpr Split(B curr, Fn func)
            : _curr(curr),
              _func(func) { }

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != NONE and not _func(*_curr);
        }

        [[gnu::always_inline]] constexpr Split& operator++() {
            if (_curr != NONE and not _func(*_curr)) {
                ++_curr;
            }
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_curr == NONE) {
                return NONE;
            }
            return *_curr;
        }

        [[gnu::always_inline]] constexpr void next() {
            while (_curr != NONE and not _func(*_curr)) {
                ++_curr;
            }
        }
    };

    using E = Split;
    Rn _range;
    Fn _func;

    struct _ {
        using B = decltype(::declval<Rn>().begin());
        using R = Opt<E>;
        using V = E;

        B _curr;
        E _split;

        [[gnu::always_inline]] constexpr _(B curr, Fn func)
            : _curr(curr),
              _split(curr, func) { }

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _curr != NONE;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            if (_curr != NONE) {
                _split.next();
            }
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_curr == NONE) {
                return NONE;
            }
            return _split;
        }
    };

    [[gnu::always_inline]] constexpr View(Rn range, Fn func)
        : _range(::move(range)),
          _func(func) { }

    [[gnu::always_inline]] auto begin() { return _ { _range.begin(), _func }; }

    [[gnu::always_inline]] auto end() { return NONE; }
};

// MARK: - [Flatten]

template <Ranges Rn>
struct View<Pipeline::Flatten, Rn, None> { };

// MARK: - [Readonly]

template <Ranges Rn>
struct View<Pipeline::Readonly, Rn, None> { };

// MARK: - #-- Consumers:

template <Pipeline P, typename Fn>
struct Consume;

// MARK: - [Matches]

template <typename Fn>
struct Consume<Pipeline::AllMatch, Fn> {
    Fn _func;

    [[gnu::always_inline]] constexpr Consume(Fn func) : _func(func) { }

    [[gnu::always_inline]] constexpr auto operator()(Ranges auto const& range) {
        for (auto it = range.begin(); it != NONE; ++it) {
            if (not _func(*it)) {
                return false;
            }
        }
        return true;
    }
};

template <typename Fn>
struct Consume<Pipeline::AnyMatch, Fn> {
    Fn _func;

    [[gnu::always_inline]] constexpr Consume(Fn func) : _func(func) { }

    [[gnu::always_inline]] constexpr auto operator()(Ranges auto const& range) {
        for (auto it = range.begin(); it != NONE; ++it) {
            if (_func(*it)) {
                return true;
            }
        }
        return false;
    }
};

template <typename Fn>
struct Consume<Pipeline::NoneMatch, Fn> {
    Fn _func;

    [[gnu::always_inline]] constexpr Consume(Fn func) : _func(func) { }

    [[gnu::always_inline]] constexpr auto operator()(Ranges auto const& range) {
        for (auto it = range.begin(); it != NONE; ++it) {
            if (_func(*it)) {
                return false;
            }
        }
        return true;
    }
};

// MARK: - [Sum / Average]

template <>
struct Consume<Pipeline::Sum, None> {
    template <Ranges Rn, typename T = typename Rn::E>
        requires(Meta::Computable<T>)
    [[gnu::always_inline]] constexpr auto operator()(Rn const& range) {
        T sum = 0;
        for (auto it : range) {
            sum += *it;
        }
        return sum;
    }
};

template <>
struct Consume<Pipeline::Average, None> {
    template <Ranges Rn, typename T = typename Rn::E>
        requires(Meta::Computable<T>)
    [[gnu::always_inline]] constexpr auto operator()(Rn const& range) {
        T     sum   = 0;
        usize count = 0;
        for (auto it : range) {
            sum += *it;
            ++count;
        }
        if (count == 0) {
            return NONE;
        }
        return sum / count;
        ;
    }
};

// MARK: - [Max / Min]

template <>
struct Consume<Pipeline::Max, None> {
    template <Ranges Rn, typename T = typename Rn::E>
        requires(Meta::Comparable<T>)
    [[gnu::always_inline]] constexpr auto operator()(Rn const& range) {
        Opt<T> max = NONE;
        for (auto it : range) {
            if (not max or *it > *max) {
                max = *it;
            }
        }
        return max;
    }
};

template <>
struct Consume<Pipeline::Min, None> {
    template <Ranges Rn, typename T = typename Rn::E>
        requires(Meta::Comparable<T>)
    [[gnu::always_inline]] constexpr auto operator()(Rn const& range) {
        Opt<T> min = NONE;
        for (auto it : range) {
            if (not min or *it < *min) {
                min = *it;
            }
        }
        return min;
    }
};

// MARK: - [MaxBy / MinBy]

template <typename Fn>
struct Consume<Pipeline::MaxBy, Fn> {
    Fn _func;

    [[gnu::always_inline]] constexpr Consume(Fn func) : _func(func) { }

    template <Ranges Rn, typename T = typename Rn::E>
        requires(Meta::Comparable<Meta::Ret<Fn, T>>)
    [[gnu::always_inline]] constexpr auto operator()(Rn const& range) {
        Opt<T> max = NONE;
        for (auto it : range) {
            if (not max or _func(*it) > _func(*max)) {
                max = *it;
            }
        }
        return max;
    }
};

template <typename Fn>
struct Consume<Pipeline::MinBy, Fn> {
    Fn _func;

    [[gnu::always_inline]] constexpr Consume(Fn func) : _func(func) { }

    template <Ranges Rn, typename T = typename Rn::E>
        requires(Meta::Comparable<Meta::Ret<Fn, T>>)
    [[gnu::always_inline]] constexpr auto operator()(Rn const& range) {
        Opt<T> min = NONE;
        for (auto it : range) {
            if (not min or _func(*it) < _func(*min)) {
                min = *it;
            }
        }
        return min;
    }
};

// MARK: - [Reduce]

template <typename Fn>
struct Consume<Pipeline::Reduce, Fn> {
    Fn _func;

    [[gnu::always_inline]] constexpr Consume(Fn func) : _func(func) { }

    template <Ranges Rn>
    [[gnu::always_inline]] constexpr auto operator()(Rn&& range) {
        Opt<typename RemoveCvRef<Rn>::E> acc = NONE;
        for (auto it : range) {
            if (not acc) {
                acc = *it;
            } else {
                acc = _func(*acc, *it);
            }
        }
        return acc;
    }
};

// MARK: - [First / Last]

template <>
struct Consume<Pipeline::First, None> {
    template <Ranges Rn>
    [[gnu::always_inline]] constexpr auto operator()(Rn&& range)
        -> Opt<typename RemoveCvRef<Rn>::E> {
        for (auto it : range) {
            return *it;
        }
        return NONE;
    }
};

template <>
struct Consume<Pipeline::Last, None> {
    template <Ranges Rn>
    [[gnu::always_inline]] constexpr auto operator()(Rn&& range) {
        Opt<typename Rn::E> last = NONE;
        for (auto it : range) {
            last = *it;
        }
        return last;
    }
};

template <typename Fn>
struct Consume<Pipeline::First, Fn> {
    Fn _func;

    [[gnu::always_inline]] constexpr Consume(Fn func) : _func(func) { }

    template <Ranges Rn>
    [[gnu::always_inline]] constexpr auto operator()(Rn&& range) {
        for (auto it : range) {
            if (_func(*it)) {
                return *it;
            }
        }
        return NONE;
    }
};

template <typename Fn>
struct Consume<Pipeline::Last, Fn> {
    Fn _func;

    [[gnu::always_inline]] constexpr Consume(Fn func) : _func(func) { }

    template <Ranges Rn>
    [[gnu::always_inline]] constexpr auto operator()(Rn&& range) {
        Opt<typename Rn::E> last = NONE;
        for (auto it : range) {
            if (_func(*it)) {
                last = *it;
            }
        }
        return last;
    }
};

// MARK: - [ForEach]

template <typename Fn>
struct Consume<Pipeline::ForEach, Fn> {
    Fn _func;

    [[gnu::always_inline]] constexpr Consume(Fn func) : _func(func) { }

    template <Ranges Rn>
    [[gnu::always_inline]] constexpr void operator()(Rn&& range) {
        for (auto it : range) {
            _func(*it);
        }
    }
};

// MARK: - #-- Builders:

constexpr auto filter(auto func) {
    return Closure<Pipeline::Filter, decltype(func)> { func };
}

constexpr auto select(auto func) {
    return Closure<Pipeline::Select, decltype(func)> { func };
}

constexpr auto select(Field auto f) {
    return Closure<Pipeline::Select, decltype(f)> { f };
}

constexpr auto take(usize count) {
    return Closure<Pipeline::Take, usize> { count };
}

constexpr auto skip(usize count) {
    return Closure<Pipeline::Skip, usize> { count };
}

constexpr auto index() {
    return Closure<Pipeline::Index, None> {};
}

constexpr auto reverse() {
    return Closure<Pipeline::Reverse, None> {};
}

constexpr auto peek(auto func) {
    return Closure<Pipeline::Peek, decltype(func)> { func };
}

constexpr auto all(auto func) -> bool {
    return Consume<Pipeline::AllMatch, decltype(func)> { func };
}

constexpr auto any(auto func) -> bool {
    return Consume<Pipeline::AnyMatch, decltype(func)> { func };
}

constexpr auto none(auto func) -> bool {
    return Consume<Pipeline::NoneMatch, decltype(func)> { func };
}

constexpr auto sum() {
    return Consume<Pipeline::Sum, None> {};
}

constexpr auto average() {
    return Consume<Pipeline::Average, None> {};
}

constexpr auto max() {
    return Consume<Pipeline::Max, None> {};
}

constexpr auto min() {
    return Consume<Pipeline::Min, None> {};
}

constexpr auto maxBy(auto func) {
    return Consume<Pipeline::MaxBy, decltype(func)> { func };
}

constexpr auto minBy(auto func) {
    return Consume<Pipeline::MinBy, decltype(func)> { func };
}

constexpr auto reduce(auto func) {
    return Consume<Pipeline::Reduce, decltype(func)> { func };
}

constexpr auto first() {
    return Consume<Pipeline::First, None> {};
}

constexpr auto last() {
    return Consume<Pipeline::Last, None> {};
}

constexpr auto first(auto&& func) {
    return Consume<Pipeline::First, decltype(func)> { func };
}

constexpr auto last(auto&& func) {
    return Consume<Pipeline::Last, decltype(func)> { func };
}

constexpr auto forEach(auto&& func) {
    return Consume<Pipeline::ForEach, decltype(func)> { func };
}

#if defined(__meta_enable_ranges_deduction)
#    define all$(expr)    all([&](auto& it) -> bool { return expr; })
#    define any$(expr)    any([&](auto& it) -> bool { return expr; })
#    define none$(expr)   none([&](auto& it) -> bool { return expr; })
#    define filter$(expr) filter([&](auto& it) -> bool { return expr; })
#    define select$(expr)                                                      \
        select([&](auto& it) -> decltype(auto) { return expr; })
#    define reduce$(expr)  reduce([&](auto& x, auto& y) { return expr; })
#    define peek$(expr)    peek([&](auto const& it) { expr; })
#    define forEach$(expr) forEach([&](auto const& it) { expr; })
#    define apply$(expr)   forEach([&](auto const& it) { expr; })
#    define exists$()      first()
#endif

// MARK: - #-- Operands:

constexpr auto operator+(Ranges auto const& r1, Ranges auto const& r2) {
}

template <Sequence S, Pipeline P, typename T>
constexpr auto operator|(S&& s, Closure<P, T> closure) {
    return Arrays<typename RemoveCvRef<S>::E> { s.begin(), s.end() } | closure;
}

template <Ranges Rn, Pipeline P, typename T>
    requires(not Views<Rn>)
constexpr auto operator|(Rn&& r, Closure<P, T> closure) {
    if constexpr (Meta::LvalueRef<Rn>) {
        return View<Pipeline::Referred, RemoveCvRef<Rn>, None> { r } | closure;
    } else {
        return View<Pipeline::Owned, RemoveCvRef<Rn>, None> { ::forward<Rn>(r) }
             | closure;
    }
}

template <Views V, Pipeline P, typename T>
constexpr auto operator|(V&& v, Closure<P, T> closure) {
    return View<P, Meta::RemoveCvRef<V>, T> { ::forward<V>(v), closure.t };
}

template <Ranges Rn, Pipeline P, typename Fn>
constexpr auto operator|(Rn&& r, Consume<P, Fn>&& consume) {
    return consume(r);
}

// MARK: - #-- Creators:

constexpr auto single(auto value) {
    struct _ {
        using E = decltype(value);
        using R = Opt<E>;
        using V = E;

        E    _value;
        bool _done = false;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return not _done;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            _done = true;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_done) {
                return NONE;
            }
            return _value;
        }
    };

    return _ { value };
}

template <typename T>
    requires Meta::Arithmetic<T>
constexpr auto range(T start, T end, T step = { 1 }) {
    if ((step == 0)
        or ((end - start) % step != 0)
        or ((start - end) / step < 0)) {
        panic("Meta::Ranges::range: (end - start) must be divisible by step");
    }

    struct _ {
        using R = Opt<T>;
        using V = T;

        T _curr;
        T _end;
        T _step;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return (_step > 0) ? (_curr < _end) : (_curr > _end);
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            _curr += _step;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_curr == _end) {
                return NONE;
            }
            return _curr;
        }
    };

    return _ { start, end, step };
}

template <typename T>
constexpr auto repeat(T value, usize count) {
    struct _ {
        using R = Opt<T>;
        using V = T;

        T     _value;
        usize _rem;

        [[gnu::always_inline]] constexpr bool operator!=(None) {
            return _rem > 0;
        }

        [[gnu::always_inline]] constexpr _& operator++() {
            if (_rem != 0)
                --_rem;
            return *this;
        }

        [[gnu::always_inline]] constexpr R operator*() {
            if (_rem == 0) {
                return NONE;
            }
            return _value;
        }
    };

    return _ { value, count };
};

// MARK: - #-- Extensions:

template <typename T, typename Fn>
constexpr auto operator|(Opt<T> opt, Closure<Pipeline::Select, Fn> closure)
    -> Opt<Meta::Ret<Fn, T&>> {
    if (opt == NONE) {
        return NONE;
    }
    return closure.t(*opt);
}

template <typename T, typename Fn>
constexpr auto operator|(Opt<T> opt, Consume<Pipeline::ForEach, Fn> consume)
    -> Opt<T> {
    if (opt != NONE) {
        consume._func(*opt);
    }
    return opt;
}

template <typename T>
constexpr auto operator|(Opt<T> opt, Consume<Pipeline::First, None>) -> Opt<T> {
    return opt._present;
}

} // namespace Meta::Ranges

#if defined(__meta_enable_global_namespace)
using Meta::Ranges::Ranges;
using Meta::Ranges::Views;

using Meta::Ranges::all;
using Meta::Ranges::any;
using Meta::Ranges::average;
using Meta::Ranges::filter;
using Meta::Ranges::first;
using Meta::Ranges::index;
using Meta::Ranges::last;
using Meta::Ranges::max;
using Meta::Ranges::maxBy;
using Meta::Ranges::min;
using Meta::Ranges::minBy;
using Meta::Ranges::none;
using Meta::Ranges::peek;
using Meta::Ranges::range;
using Meta::Ranges::reduce;
using Meta::Ranges::repeat;
using Meta::Ranges::select;
using Meta::Ranges::single;
using Meta::Ranges::skip;
using Meta::Ranges::sum;
using Meta::Ranges::take;
#endif

// template <typename R = Range<usize>>
// struct Ranges {
//     Vec<R> _r;

//     Slice<R> ranges() const { return _r; }

//     void clear() { _r.clear(); }

//     void remove(R range) {
//         for (usize i = 0; i < _r.len(); i++) {
//             R curr = _r[i];

//             if (curr == range) {
//                 _r.removeAt(i);
//                 break;
//             }

//             if (curr.overlaps(range)) {
//                 R lh = curr.halfUnder(range);
//                 R uh = curr.halfOver(range);

//                 if (lh.size != 0) {
//                     _r[i] = lh;

//                     if (uh.size != 0) {
//                         add(uh);
//                     }
//                 } else if (uh.size != 0) {
//                     _r[i] = uh;
//                 }
//             }
//         }
//     }

//     Res<R> take(usize size) {
//         for (usize i = 0; i < _r.len(); i++) {
//             if (_r[i].size == size) {
//                 R result = _r[i];
//                 _r.removeAt(i);
//                 return Ok(result);
//             }

//             if (_r[i].size > size) {
//                 R result = { _r[i].start, size };
//                 _r[i].start += size;
//                 _r[i].size -= size;
//                 return Ok(result);
//             }
//         }

//         return Error::outOfMemory();
//     }

//     void _compress(usize i) {
//         while (i + 1 < _r.len() and _r[i].contigous(_r[i + 1])) {
//             _r[i] = _r[i].merge(_r[i + 1]);
//             _r.removeAt(i + 1);
//         }

//         while (i > 0 and _r[i].contigous(_r[i - 1])) {
//             _r[i] = _r[i].merge(_r[i - 1]);
//             _r.removeAt(i - 1);
//         }
//     }

//     void add(R range) {
//         for (usize i = 0; i < _r.len(); i++) {
//             if (_r[i].contigous(range)) {
//                 _r[i] = _r[i].merge(range);
//                 _compress(i);
//                 return;
//             }

//             if ((i + 1 < _r.len())
//                 && (_r[i].start < range.start)
//                 && (range.start < _r[i + 1].start)) {
//                 _r.insert(i + 1, range);
//                 return;
//             }
//         }

//         _r.pushBack(range);
//     }

//     bool contains(R range) const {
//         for (usize i = 0; i < _r.len(); i++) {
//             if (_r[i].contains(range)) {
//                 return true;
//             }
//         }

//         return false;
//     }

//     bool colides(R range) const {
//         for (usize i = 0; i < _r.len(); i++) {
//             if (_r[i].colides(range)) {
//                 return true;
//             }
//         }

//         return false;
//     }
// };
