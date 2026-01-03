module;

export module sdk:limits;

import :types;

template <typename T>
struct Limits { };

template <>
struct Limits<bool> {
    static constexpr bool  MIN       = false;
    static constexpr bool  MAX       = true;
    static constexpr bool  IS_SIGNED = false;
    static constexpr usize DIGITS    = 1;
    static constexpr bool  BITS      = 1;
};

template <>
struct Limits<signed char> {
    static constexpr signed char MIN    = -__SCHAR_MAX__ - 1;
    static constexpr signed char MAX    = __SCHAR_MAX__;
    static constexpr bool        SIGNED = true;
    static constexpr usize       DIGITS = __CHAR_BIT__ - 1;
    static constexpr usize       BITS   = __CHAR_BIT__;
};

template <>
struct Limits<char> {
    static constexpr char  MIN    = -__SCHAR_MAX__ - 1;
    static constexpr char  MAX    = __SCHAR_MAX__;
    static constexpr bool  SIGNED = true;
    static constexpr usize DIGITS = __CHAR_BIT__ - 1;
    static constexpr usize BITS   = __CHAR_BIT__;
};

template <>
struct Limits<short> {
    static constexpr short MIN    = -__SHRT_MAX__ - 1;
    static constexpr short MAX    = __SHRT_MAX__;
    static constexpr bool  SIGNED = true;
    static constexpr usize DIGITS = __CHAR_BIT__ * sizeof(short) - 1;
    static constexpr usize BITS   = __CHAR_BIT__ * sizeof(short);
};

template <>
struct Limits<int> {
    static constexpr int   MIN    = -__INT_MAX__ - 1;
    static constexpr int   MAX    = __INT_MAX__;
    static constexpr bool  SIGNED = true;
    static constexpr usize DIGITS = __CHAR_BIT__ * sizeof(int) - 1;
    static constexpr usize BITS   = __CHAR_BIT__ * sizeof(int);
};

template <>
struct Limits<long> {
    static constexpr long  MIN    = -__LONG_MAX__ - 1;
    static constexpr long  MAX    = __LONG_MAX__;
    static constexpr bool  SIGNED = true;
    static constexpr usize DIGITS = __CHAR_BIT__ * sizeof(long) - 1;
    static constexpr usize BITS   = __CHAR_BIT__ * sizeof(long);
};

template <>
struct Limits<long long> {
    static constexpr long long MIN    = -__LONG_LONG_MAX__ - 1;
    static constexpr long long MAX    = __LONG_LONG_MAX__;
    static constexpr bool      SIGNED = true;
    static constexpr usize     DIGITS = __CHAR_BIT__ * sizeof(long long) - 1;
    static constexpr usize     BITS   = __CHAR_BIT__ * sizeof(long long);
};

template <>
struct Limits<unsigned char> {
    static constexpr unsigned char MIN    = 0;
    static constexpr unsigned char MAX    = __SCHAR_MAX__ * 2u + 1;
    static constexpr bool          SIGNED = false;
    static constexpr usize         DIGITS = __CHAR_BIT__;
    static constexpr usize         BITS   = __CHAR_BIT__;
};

template <>
struct Limits<unsigned short> {
    static constexpr unsigned short MIN    = 0;
    static constexpr unsigned short MAX    = __SHRT_MAX__ * 2u + 1;
    static constexpr bool           SIGNED = false;
    static constexpr usize          DIGITS = __CHAR_BIT__ * sizeof(short);
    static constexpr usize          BITS   = __CHAR_BIT__ * sizeof(short);
};

template <>
struct Limits<unsigned> {
    static constexpr unsigned MIN    = 0;
    static constexpr unsigned MAX    = __INT_MAX__ * 2u + 1;
    static constexpr bool     SIGNED = false;
    static constexpr usize    DIGITS = __CHAR_BIT__ * sizeof(int);
    static constexpr usize    BITS   = __CHAR_BIT__ * sizeof(int);
};

template <>
struct Limits<unsigned long> {
    static constexpr unsigned long MIN    = 0;
    static constexpr unsigned long MAX    = __LONG_MAX__ * 2ul + 1;
    static constexpr bool          SIGNED = false;
    static constexpr usize         DIGITS = __CHAR_BIT__ * sizeof(long);
    static constexpr usize         BITS   = __CHAR_BIT__ * sizeof(long);
};

template <>
struct Limits<unsigned long long> {
    static constexpr unsigned long long MIN    = 0;
    static constexpr unsigned long long MAX    = __LONG_LONG_MAX__ * 2ull + 1;
    static constexpr bool               SIGNED = false;
    static constexpr usize DIGITS = __CHAR_BIT__ * sizeof(long long);
    static constexpr usize BITS   = __CHAR_BIT__ * sizeof(long long);
};
