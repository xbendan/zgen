module;

export module sdk:_union;

import :cursor;
import :hash;
import :math;
import :traits;
import :types;

export template <typename... Ts>
struct Union {
    static_assert(sizeof...(Ts) > 0 and (sizeof...(Ts) < 256),
                  "Union must have at least one and at most 255 types");

    alignas(max(sizeof(Ts)...)) char _buf[max(sizeof(Ts)...)];
    u8 _index;

    [[gnu::always_inline]] Union()
        requires(Contains<None, Ts...>)
        : Union(None {}) { }

    template <Contains<Ts...> T>
    [[gnu::always_inline]] Union(T const& value) : _index(indexOf<T, Ts...>()) {
        new (_buf) T(value);
    }

    template <Contains<Ts...> T>
    [[gnu::always_inline]] Union(T&& value) : _index(indexOf<T, Ts...>()) {
        new (_buf) T(move(value));
    }

    [[gnu::always_inline]] Union(Union const& other) : _index(other._index) {
        indexCast<Ts...>(_index, other._buf, [this]<typename T>(T const& ptr) {
            new (_buf) T(ptr);
        });
    }

    [[gnu::always_inline]] Union(Union&& other) : _index(other._index) {
        indexCast<Ts...>(_index, other._buf, [this]<typename T>(T& ptr) {
            new (_buf) T(move(ptr));
        });
    }

    [[gnu::always_inline]] ~Union() {
        indexCast<Ts...>(_index, _buf, []<typename T>(T& ptr) { ptr.~T(); });
    }

    template <Contains<Ts...> T>
    [[gnu::always_inline]] Union& operator=(T const& value) {
        *this = Union(value);
        return *this;
    }

    template <Contains<Ts...> T>
    [[gnu::always_inline]] Union& operator=(T&& value) {
        indexCast<Ts...>(_index, _buf, []<typename U>(U& ptr) { ptr.~U(); });

        _index = indexOf<T, Ts...>();
        new (_buf) T(move(value));

        return *this;
    }

    [[gnu::always_inline]] Union& operator=(Union const& other) {
        *this = Union(other);
        return *this;
    }

    [[gnu::always_inline]] Union& operator=(Union&& other) {
        indexCast<Ts...>(_index, _buf, []<typename T>(T& ptr) { ptr.~T(); });

        _index = other._index;

        indexCast<Ts...>(_index, other._buf, [this]<typename T>(T& ptr) {
            new (_buf) T(move(ptr));
        });

        return *this;
    }

    template <Contains<Ts...> T>
    [[gnu::always_inline]] T& unwrap(char const* msg
                                     = "unwrapping wrong type") {
        if (_index != indexOf<T, Ts...>()) [[unlikely]]
            panic(msg);

        return *reinterpret_cast<T*>(_buf);
    }

    template <Contains<Ts...> T>
    [[gnu::always_inline]] T const& unwrap(char const* msg
                                           = "unwrapping wrong type") const {
        if (_index != indexOf<T, Ts...>()) [[unlikely]]
            panic(msg);

        return *reinterpret_cast<T const*>(_buf);
    }

    template <Contains<Ts...> T>
    [[gnu::always_inline]] T const& unwrapOr(T const& fallback) const {
        if (_index != indexOf<T, Ts...>())
            return fallback;

        return *reinterpret_cast<T const*>(_buf);
    }

    template <typename T, typename... Args>
    [[gnu::always_inline]] T& emplace(Args&&... args) {
        if (_index != indexOf<T, Ts...>()) {
            indexCast<Ts...>(
                _index, _buf, []<typename U>(U& ptr) { ptr.~U(); });

            _index = indexOf<T, Ts...>();
        }

        return *new (_buf) T(forward<Args>(args)...);
    }

    template <Contains<Ts...> T>
    [[gnu::always_inline]] T take(char const* msg = "taking wrong type") {
        if (_index != indexOf<T, Ts...>())
            panic(msg);
        return move(*reinterpret_cast<T*>(_buf));
    }

    [[gnu::always_inline]] auto visit(auto visitor) {
        return indexCast<Ts...>(_index, _buf, visitor);
    }

    [[gnu::always_inline]] auto visit(auto visitor) const {
        return indexCast<Ts...>(_index, _buf, visitor);
    }

    [[gnu::always_inline]] static auto any(auto visitor) {
        return any<Ts...>(visitor);
    }

    // template <Contains<Ts...> T>
    // [[gnu::always_inline]] MutCursor<T> is() {
    //     if (_index != indexOf<T, Ts...>())
    //         return nullptr;
    //     return (T*) _buf;
    // }

    template <Contains<Ts...> T>
    [[gnu::always_inline]] Cursor<T> is() const {
        if (_index != indexOf<T, Ts...>())
            return nullptr;
        return (T const*) _buf;
    }

    [[gnu::always_inline]] usize index() const { return _index; }

    [[gnu::always_inline]] bool valid() const { return _index < sizeof...(Ts); }

    // template <Contains<Ts...> T>
    // std::partial_ordering operator<=>(T const& other) const {
    //     if constexpr (Comparable<T>)
    //         if (is<T>())
    //             return unwrap<T>() <=> other;
    //     return std::partial_ordering::unordered;
    // }

    template <Contains<Ts...> T>
        requires Equatable<T>
    bool operator==(T const& other) const {
        if (is<T>())
            return unwrap<T>() == other;
        return false;
    }

    // std::partial_ordering operator<=>(Union const& other) const {
    //     if (_index == other._index)
    //         return visit([&]<typename T>(T const& ptr)
    //                          requires Comparable<T>
    //         { return ptr <=> other.unwrap<T>(); });
    //     return std::partial_ordering::unordered;
    // }

    bool operator==(Union const& other) const {
        if (_index == other._index)
            return visit([&]<typename T>(T const& ptr) {
                return ptr == other.unwrap<T>();
            });
        return false;
    }

    u64 hash() const {
        return hash(hash(_index),
                    visit([](auto const& v) { return ::hash(v); }));
    }
};
