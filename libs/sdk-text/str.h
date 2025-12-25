#pragma once

#include <sdk-meta/iter.h>
#include <sdk-meta/str.h>
#include <sdk-text/_defs.h>
#include <sdk-text/rune.h>

namespace Sdk::Text {

template <StaticEncoding E, typename U = typename E::Unit>
struct _Str : Slice<U> {
    using Encoding = E;
    using Unit     = U;
    using Inner    = U;

    [[gnu::always_inline]] constexpr _Str() = default;

    [[gnu::always_inline]] constexpr _Str(U const* cstr)
        requires(Meta::Same<U, char>)
        : Slice<U>(cstr, strlen(cstr)) { }

    [[gnu::always_inline]] constexpr _Str(U const* buf, usize len)
        : Slice<U>(buf, len) { }

    [[gnu::always_inline]] constexpr _Str(U const* begin, U const* end)
        : Slice<U>(begin, end - begin) { }

    [[gnu::always_inline]] constexpr _Str(Sliceable<U> auto const& other)
        : Slice<U>(other.buf(), other.len()) { }

    [[gnu::always_inline]] constexpr auto operator<=>(U const* cstr) const
        requires(Meta::Same<U, char>)
    {
        return *this <=> _Str(cstr);
    }

    [[gnu::always_inline]] constexpr bool operator==(U const* cstr) const
        requires(Meta::Same<U, char>)
    {
        return *this == _Str(cstr);
    }

    [[gnu::always_inline]] constexpr explicit operator bool() const noexcept {
        return this->_len > 0;
    }

    [[gnu::always_inline]] constexpr auto it() {
        Cursor<U> cursor(*this);
        return Iter([cursor] mutable -> Opt<Rune> {
            if (cursor.ended()) {
                return None {};
            }

            Rune r;
            return E::decodeUnit(r, cursor) ? r : Text::Unknown;
        });
    }
};

using Str = _Str<Realms::Core::Encoding>;

template <StaticEncoding E>
struct _String {
    using Encoding  = E;
    using Unit      = typename E::Unit;
    using InnerType = Unit;

    static constexpr Array<Unit, 1> _EMPTY = { 0 };

    Unit* _buf { nullptr };
    usize _len { 0 };

    [[gnu::always_inline]] constexpr _String() = delete;

    [[gnu::always_inline]] constexpr _String(Unit const* buf, usize len)
        requires(Meta::Same<Unit, char>)
        : _len(len) {
        if (_buf == nullptr or len == 0) [[unlikely]] {
            panic("String: cannot create an empty string with a buffer");
        }

        auto temp = new Unit[len + 1]; // +1 for null-termination
        for (usize i = 0; i < len; i++) {
            temp[i] = buf[i];
        }
        temp[len] = '\0'; // Null-terminate the string
        _buf      = temp;
    }

    [[gnu::always_inline]] constexpr _String(Unit const* cstr)
        requires(Meta::Same<Unit, char>)
        : _String(cstr, strlen(cstr)) { }

    [[gnu::always_inline]] constexpr _String(_Str<E> str)
        : _String(str.buf(), str.len()) { }

    [[gnu::always_inline]] _String(Sliceable<Unit> auto const& other)
        : _String(other.buf(), other.len()) { }

    [[gnu::always_inline]] _String(_String const& other)
        : _String(other._buf, other._len) { }

    [[gnu::always_inline]] _String(_String&& other)
        : _buf(Meta::exchange(other._buf, nullptr)),
          _len(Meta::exchange(other._len, 0)) { }

    ~_String() {
        if (_buf) {
            _len = 0;
            delete[] Meta::exchange(_buf, nullptr);
        }
    }

    [[gnu::always_inline]] _String& operator=(_String const& other) {
        *this = _String(other);
        return *this;
    }

    [[gnu::always_inline]] _String& operator=(_String&& other) {
        Meta::swap(_buf, other._buf);
        Meta::swap(_len, other._len);
        return *this;
    }

    [[gnu::always_inline]] _Str<E> str() const { return *this; }

    [[gnu::always_inline]] Unit const& operator[](usize i) const {
        if (i >= _len) [[unlikely]]
            panic("index out of bounds");
        return buf()[i];
    }

    [[gnu::always_inline]] Unit* buf() { return _len ? _buf : _EMPTY.buf(); }

    [[gnu::always_inline]] Unit const* buf() const {
        return _len ? _buf : _EMPTY.buf();
    }

    [[gnu::always_inline]] usize len() const { return _len; }

    [[gnu::always_inline]] auto operator<=>(Unit const* cstr) const
        requires(Meta::Same<Unit, char>)
    {
        return str() <=> _Str<E>(cstr);
    }

    [[gnu::always_inline]] bool operator==(Unit const* cstr) const
        requires(Meta::Same<Unit, char>)
    {
        return str() == _Str<E>(cstr);
    }

    [[gnu::always_inline]] constexpr explicit operator bool() const {
        return _len > 0;
    }
};

using String = _String<Realms::Core::Encoding>;

} // namespace Sdk::Text

using Sdk::Text::Str;
using Sdk::Text::String;

inline constexpr Str operator""s(char const* cstr, usize len) {
    return { cstr, len };
}
