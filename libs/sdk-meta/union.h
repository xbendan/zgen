#pragma once

#include <sdk-meta/cursor.h>
#include <sdk-meta/hash.h>
#include <sdk-meta/math.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

template <typename... Ts>
struct Union {
    static_assert(sizeof...(Ts) > 0 and (sizeof...(Ts) < 256),
                  "Union must have at least one and at most 255 types");

    alignas(max(sizeof(Ts)...)) char _buf[max(sizeof(Ts)...)];
    u8 _index;

    [[gnu::always_inline]] Union()
        requires(Meta::Contains<None, Ts...>)
        : Union(None {}) { }

    template <Meta::Contains<Ts...> T>
    [[gnu::always_inline]] Union(T const& value)
        : _index(Meta::indexOf<T, Ts...>()) {
        new (_buf) T(value);
    }

    template <Meta::Contains<Ts...> T>
    [[gnu::always_inline]] Union(T&& value)
        : _index(Meta::indexOf<T, Ts...>()) {
        new (_buf) T(move(value));
    }

    [[gnu::always_inline]] Union(Union const& other) : _index(other._index) {
        Meta::indexCast<Ts...>(
            _index, other._buf, [this]<typename T>(T const& ptr) {
            new (_buf) T(ptr);
        });
    }

    [[gnu::always_inline]] Union(Union&& other) : _index(other._index) {
        Meta::indexCast<Ts...>(_index, other._buf, [this]<typename T>(T& ptr) {
            new (_buf) T(move(ptr));
        });
    }

    [[gnu::always_inline]] ~Union() {
        Meta::indexCast<Ts...>(
            _index, _buf, []<typename T>(T& ptr) { ptr.~T(); });
    }

    template <Meta::Contains<Ts...> T>
    [[gnu::always_inline]] Union& operator=(T const& value) {
        *this = Union(value);
        return *this;
    }

    template <Meta::Contains<Ts...> T>
    [[gnu::always_inline]] Union& operator=(T&& value) {
        Meta::indexCast<Ts...>(
            _index, _buf, []<typename U>(U& ptr) { ptr.~U(); });

        _index = Meta::indexOf<T, Ts...>();
        new (_buf) T(move(value));

        return *this;
    }

    [[gnu::always_inline]] Union& operator=(Union const& other) {
        *this = Union(other);
        return *this;
    }

    [[gnu::always_inline]] Union& operator=(Union&& other) {
        Meta::indexCast<Ts...>(
            _index, _buf, []<typename T>(T& ptr) { ptr.~T(); });

        _index = other._index;

        Meta::indexCast<Ts...>(_index, other._buf, [this]<typename T>(T& ptr) {
            new (_buf) T(move(ptr));
        });

        return *this;
    }

    template <Meta::Contains<Ts...> T>
    [[gnu::always_inline]] T& unwrap(char const* msg
                                     = "unwrapping wrong type") {
        if (_index != Meta::indexOf<T, Ts...>()) [[unlikely]]
            panic(msg);

        return *reinterpret_cast<T*>(_buf);
    }

    template <Meta::Contains<Ts...> T>
    [[gnu::always_inline]] T const& unwrap(char const* msg
                                           = "unwrapping wrong type") const {
        if (_index != Meta::indexOf<T, Ts...>()) [[unlikely]]
            panic(msg);

        return *reinterpret_cast<T const*>(_buf);
    }

    template <Meta::Contains<Ts...> T>
    [[gnu::always_inline]] T const& unwrapOr(T const& fallback) const {
        if (_index != Meta::indexOf<T, Ts...>())
            return fallback;

        return *reinterpret_cast<T const*>(_buf);
    }

    template <typename T, typename... Args>
    [[gnu::always_inline]] T& emplace(Args&&... args) {
        if (_index != Meta::indexOf<T, Ts...>()) {
            Meta::indexCast<Ts...>(
                _index, _buf, []<typename U>(U& ptr) { ptr.~U(); });

            _index = Meta::indexOf<T, Ts...>();
        }

        return *new (_buf) T(forward<Args>(args)...);
    }

    template <Meta::Contains<Ts...> T>
    [[gnu::always_inline]] T take(char const* msg = "taking wrong type") {
        if (_index != Meta::indexOf<T, Ts...>())
            panic(msg);
        return move(*reinterpret_cast<T*>(_buf));
    }

    [[gnu::always_inline]] auto visit(auto visitor) {
        return Meta::indexCast<Ts...>(_index, _buf, visitor);
    }

    [[gnu::always_inline]] auto visit(auto visitor) const {
        return Meta::indexCast<Ts...>(_index, _buf, visitor);
    }

    [[gnu::always_inline]] static auto any(auto visitor) {
        return any<Ts...>(visitor);
    }

    // template <Meta::Contains<Ts...> T>
    // [[gnu::always_inline]] MutCursor<T> is() {
    //     if (_index != Meta::indexOf<T, Ts...>())
    //         return nullptr;
    //     return (T*) _buf;
    // }

    template <Meta::Contains<Ts...> T>
    [[gnu::always_inline]] Cursor<T> is() const {
        if (_index != Meta::indexOf<T, Ts...>())
            return nullptr;
        return (T const*) _buf;
    }

    [[gnu::always_inline]] usize index() const { return _index; }

    [[gnu::always_inline]] bool valid() const { return _index < sizeof...(Ts); }

    // template <Meta::Contains<Ts...> T>
    // std::partial_ordering operator<=>(T const& other) const {
    //     if constexpr (Meta::Comparable<T>)
    //         if (is<T>())
    //             return unwrap<T>() <=> other;
    //     return std::partial_ordering::unordered;
    // }

    template <Meta::Contains<Ts...> T>
        requires Meta::Equatable<T>
    bool operator==(T const& other) const {
        if (is<T>())
            return unwrap<T>() == other;
        return false;
    }

    // std::partial_ordering operator<=>(Union const& other) const {
    //     if (_index == other._index)
    //         return visit([&]<typename T>(T const& ptr)
    //                          requires Meta::Comparable<T>
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
