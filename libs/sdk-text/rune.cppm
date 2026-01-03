module;

export module sdk.text:rune;

import sdk;

export namespace Realms::Text {

using Rune = u32;

constexpr Rune NewLine        = U'\n';
constexpr Rune CarriageReturn = U'\r';
constexpr Rune Tab            = U'\t';
constexpr Rune BackSpace      = U'\b';
constexpr Rune Unknown        = U'�';

template <typename T>
concept StaticEncoding = requires(T                         t,
                                  Rune&                     r,
                                  typename T::Unit          u,
                                  Cursor<typename T::Unit>& c,
                                  Cursor<typename T::Unit>& m) {
    { T::unitLen(u) } -> Same<usize>;
    { T::runeLen(r) } -> Same<usize>;
    { T::decodeUnit(r, c) } -> Same<bool>;
    { T::encodeUnit(Rune {}, m) } -> Same<bool>;
};

template <typename U, usize N>
struct _Multiple {
    using E    = U;
    using Unit = U;

    Meta::InlineBuf<Unit, N> _buf {};

    [[gnu::always_inline]] void put(Unit u) { _buf.emplace(_buf.len(), u); }

    [[gnu::always_inline]] constexpr Unit& operator[](usize i) {
        return _buf[i];
    }

    [[gnu::always_inline]] constexpr Unit const& operator[](usize i) const {
        return _buf[i];
    }

    [[gnu::always_inline]] constexpr Unit* buf() { return _buf.buf(); }

    [[gnu::always_inline]] constexpr Unit const* buf() const {
        return _buf.buf();
    }

    [[gnu::always_inline]] constexpr usize len() const { return _buf.len(); }

    [[gnu::always_inline]] constexpr usize rem() const { return N - len(); }

    [[gnu::always_inline]] constexpr auto begin() { return _buf.buf(); }

    [[gnu::always_inline]] constexpr auto end() {
        return _buf.buf() + _buf.len();
    }
};

template <typename U>
struct _Single {
    using E    = U;
    using Unit = U;

    Unit _buf;

    [[gnu::always_inline]] _Single() = default;

    [[gnu::always_inline]] _Single(Unit u) : _buf(u) { }

    [[gnu::always_inline]] void put(Unit u) { _buf = u; }

    [[gnu::always_inline]] operator Unit() { return _buf; }

    [[gnu::always_inline]] constexpr Unit& operator[](usize) { return _buf; }

    [[gnu::always_inline]] constexpr Unit const& operator[](usize) const {
        return _buf;
    }

    [[gnu::always_inline]] constexpr Unit* buf() { return &_buf; }

    [[gnu::always_inline]] constexpr Unit const* buf() const { return &_buf; }

    [[gnu::always_inline]] constexpr usize len() const { return 1; }

    [[gnu::always_inline]] constexpr auto begin() { return &_buf; }

    [[gnu::always_inline]] constexpr auto end() { return &_buf + 1; }
};

template <typename T, typename U>
concept EncodeOutput = requires(T t, U u) {
    { t.put(u) };
};

template <typename T, typename U>
concept DecodeInput = requires(T t, U u) {
    { t.next() };
    { t.rem() };
};

// MARK: Utf8 ------------------------------------------------------------------

struct Utf8 {
    using Unit = char;
    using One  = _Multiple<Unit, 4>;

    [[gnu::always_inline]] static constexpr usize unitLen(Unit first) {
        if ((first & 0xf8) == 0xf0)
            return 4;
        else if ((first & 0xf0) == 0xe0)
            return 3;
        else if ((first & 0xe0) == 0xc0)
            return 2;
        else
            return 1;
    }

    [[gnu::always_inline]] static constexpr usize runeLen(Rune rune) {
        if (rune <= 0x7f)
            return 1;
        else if (rune <= 0x7ff)
            return 2;
        else if (rune <= 0xffff)
            return 3;
        else
            return 4;
    }

    [[gnu::always_inline]] static bool decodeUnit(Rune& result,
                                                  DecodeInput<Unit> auto& in) {
        if (in.rem() == 0) {
            result = U'�';
            return false;
        }

        Unit first = in.next();

        if (unitLen(first) > in.rem() + 1) {
            result = U'�';
            return false;
        }

        if ((first & 0xf8) == 0xf0) {
            result = ((0x07 & first) << 18)
                   | ((0x3f & in.next()) << 12)
                   | ((0x3f & in.next()) << 6)
                   | ((0x3f & in.next()));
        } else if ((first & 0xf0) == 0xe0) {
            result = ((0x0f & first) << 12)
                   | ((0x3f & in.next()) << 6)
                   | ((0x3f & in.next()));
        } else if ((first & 0xe0) == 0xc0) {
            result = ((0x1f & first) << 6) | ((0x3f & in.next()));
        } else {
            result = first;
        }

        return true;
    }

    static inline bool encodeUnit(Rune c, EncodeOutput<Unit> auto& out) {
        if (unitLen(c) > out.rem()) [[unlikely]]
            panic("bad");

        if (c <= 0x7f) {
            out.put(c);
        } else if (c <= 0x7ff) {
            out.put(0xc0 | (c >> 6));
            out.put(0x80 | (c & 0x3f));
        } else if (c <= 0xffff) {
            out.put(0xe0 | (c >> 12));
            out.put(0x80 | ((c >> 6) & 0x3f));
            out.put(0x80 | (c & 0x3f));
        } else if (c <= 0x10'ffff) {
            out.put(0xf0 | (c >> 18));
            out.put(0x80 | ((c >> 12) & 0x3f));
            out.put(0x80 | ((c >> 6) & 0x3f));
            out.put(0x80 | (c & 0x3f));
        } else {
            return encodeUnit(U'�', out);
        }

        return true;
    }
};

inline Utf8 UTF8 [[maybe_unused]];

static_assert(StaticEncoding<Utf8>);

// MARK: Utf16 -----------------------------------------------------------------

struct Utf16 {
    using Unit = u16;
    using One  = _Multiple<Unit, 2>;

    [[gnu::always_inline]] static constexpr usize unitLen(Unit first) {
        if (first >= 0xd800 and first <= 0xdbff)
            return 2;
        else
            return 1;
    }

    [[gnu::always_inline]] static constexpr usize runeLen(Rune rune) {
        if (rune <= 0xffff)
            return 1;
        else
            return 2;
    }

    [[gnu::always_inline]] static bool decodeUnit(Rune& result,
                                                  DecodeInput<Unit> auto& in) {
        Unit first = in.next();

        if (unitLen(first) > in.rem()) {
            result = U'�';
            return false;
        }

        if (first >= 0xd800 and first <= 0xdbff) {
            if (in.rem() < 2) {
                return false;
            }

            Unit second = in.next();

            if (second < 0xdc00 or second > 0xdfff) {
                return false;
            }

            result = ((first - 0xd800) << 10) | (second - 0xdc00) + 0x1'0000;
        } else {
            result = first;
        }

        return true;
    }

    [[gnu::always_inline]] static bool encodeUnit(
        Rune                     c,
        EncodeOutput<Unit> auto& out) {
        if (c <= 0xffff) {
            out.put(c);
            return true;
        } else if (c <= 0x10'ffff) {
            out.put(0xd800 | ((c - 0x1'0000) >> 10));
            out.put(0xdc00 | ((c - 0x1'0000) & 0x3ff));
            return true;
        } else {
            return encodeUnit(U'�', out);
        }
    }
};

inline Utf16 UTF16 [[maybe_unused]];

static_assert(StaticEncoding<Utf16>);

// MARK: Utf32 -----------------------------------------------------------------

struct Utf32 {
    using Unit = char32_t;
    using One  = _Single<Unit>;

    [[gnu::always_inline]] static constexpr usize unitLen(Unit) { return 1; }

    [[gnu::always_inline]] static constexpr usize runeLen(Rune) { return 1; }

    [[gnu::always_inline]] static bool decodeUnit(Rune& result,
                                                  DecodeInput<Unit> auto& in) {
        result = in.next();
        return true;
    }

    [[gnu::always_inline]] static bool encodeUnit(
        Rune                     c,
        EncodeOutput<Unit> auto& out) {
        out.put(c);
        return true;
    }
};

inline Utf32 UTF32 [[maybe_unused]];

static_assert(StaticEncoding<Utf32>);

// MARK: Ascii -----------------------------------------------------------------

struct Ascii {
    using Unit = char;
    using One  = _Single<Unit>;

    [[gnu::always_inline]] static constexpr usize unitLen(Unit) { return 1; }

    [[gnu::always_inline]] static constexpr usize runeLen(Rune) { return 1; }

    [[gnu::always_inline]] static bool decodeUnit(Rune& result,
                                                  DecodeInput<Unit> auto& in) {
        auto c = in.next();
        if (c >= 0) {
            result = c;
            return true;
        } else {
            result = U'�';
            return false;
        }
    }

    [[gnu::always_inline]] static bool encodeUnit(
        Rune                     c,
        EncodeOutput<Unit> auto& out) {
        if (c < 0) {
            out.put('?');
            return false;
        }

        out.put(c);
        return true;
    }
};

inline Ascii ASCII [[maybe_unused]];

static_assert(StaticEncoding<Ascii>);

// MARK: Extended Ascii --------------------------------------------------------

template <typename Mapper>
struct EAscii {
    using Unit = u8;
    using One  = _Single<Unit>;

    [[gnu::always_inline]] static constexpr usize unitLen(Unit) { return 1; }

    [[gnu::always_inline]] static constexpr usize runeLen(Rune) { return 1; }

    [[gnu::always_inline]] static bool decodeUnit(Rune& result,
                                                  DecodeInput<Unit> auto& in) {
        Mapper mapper;
        result = mapper(in.next());
        return true;
    }

    [[gnu::always_inline]] static bool encodeUnit(
        Rune                     c,
        EncodeOutput<Unit> auto& out) {
        Mapper mapper;
        for (usize i = 0; i <= 255; i++) {
            if (mapper(i) == c) {
                out.put(i);
                return true;
            }
        }
        out.put('?');
        return false;
    }
};

#if defined(__sdk_encoding_utf8) or not defined(__sdk_encoding)
using Encoding = Text::Utf8;
#elif defined(__sdk_encoding_utf16)
using Encoding = Text::Utf16;
#elif defined(__sdk_encoding_ascii)
using Encoding = Text::Ascii;
#else
#    error "Unknown system encoding"
#endif

} // namespace Realms::Text
