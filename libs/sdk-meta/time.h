#pragma once

#include <sdk-meta/str.h>
#include <sdk-meta/types.h>

// MARK: TimeSpan and TimeStamp

using _TimeVal = u64;

struct TimeSpan {
    _TimeVal _value; // microseconds (us) aka 1/1,000,000th of a second

    // clang-format off

    static constexpr TimeSpan zero() { return TimeSpan(0); }

    static constexpr TimeSpan infinite() { return TimeSpan(~0uz); }

    static constexpr TimeSpan ofMicroseconds(_TimeVal us) { return { us }; }

    static constexpr TimeSpan ofMilliseconds(_TimeVal ms) { return ofMicroseconds(ms * 1000); }

    static constexpr TimeSpan ofSeconds(_TimeVal s) { return ofMilliseconds(s * 1000); }

    static constexpr TimeSpan ofMinutes(_TimeVal m) { return ofSeconds(m * 60); }

    static constexpr TimeSpan ofHours(_TimeVal h) { return ofMinutes(h * 60); }

    static constexpr TimeSpan ofDays(_TimeVal d) { return ofHours(d * 24); }

    static constexpr TimeSpan ofWeeks(_TimeVal w) { return ofDays(w * 7); }

    // Approximation, as months vary in length
    static constexpr TimeSpan ofMonths(_TimeVal m) { return ofDays(m * 30); }

    // Approximation, as years can be leap years
    static constexpr TimeSpan ofYears(_TimeVal y) { return ofDays(y * 365); }

    constexpr TimeSpan() : _value(0) { }

    constexpr TimeSpan(_TimeVal value) : _value(value) { }

    constexpr operator _TimeVal() const { return _value; }

    constexpr bool isZero() const { return _value == 0; }

    constexpr bool isInfinite() const { return _value == ~0uz; }

    constexpr _TimeVal getAsMicroseconds() const { return _value; }

    constexpr _TimeVal getAsMilliseconds() const { return _value / 1000; }

    constexpr _TimeVal getAsSeconds() const { return getAsMilliseconds() / 1000; }

    constexpr _TimeVal getAsMinutes() const { return getAsSeconds() / 60; }

    constexpr _TimeVal getAsHours() const { return getAsMinutes() / 60; }

    constexpr _TimeVal getAsDays() const { return getAsHours() / 24; }

    constexpr _TimeVal getAsWeeks() const { return getAsDays() / 7; }

    constexpr _TimeVal getAsMonths() const { return getAsDays() / 30; }

    constexpr _TimeVal getAsYears() const { return getAsDays() / 365; }

    constexpr auto operator<=>(TimeSpan const&) const = default;

    constexpr bool operator==(TimeSpan const& other) const { return _value == other._value; }

    constexpr TimeSpan& operator+=(TimeSpan const& other) {
        _value += other._value;
        return *this;
    }

    constexpr TimeSpan& operator-=(TimeSpan const& other) {
        _value -= other._value;
        return *this;
    }

    constexpr TimeSpan operator+(TimeSpan const& other) const {
        return TimeSpan(_value + other._value);
    }

    constexpr TimeSpan operator-(TimeSpan const& other) const {
        return TimeSpan(_value - other._value);
    }

    // clang-format on
};

struct MonotonicClock {
    static constexpr bool monotonic = true;
};

struct SystemClock {
    static constexpr bool monotonic = false;
};

template <typename Clock>
struct _Instant {
    _TimeVal _value {}; // microseconds (us) aka 1/1,000,000th of a second

    static constexpr _TimeVal END_OF_TIME = ~0uz;

    static constexpr _Instant epoch() { return { 0 }; }

    static constexpr _Instant endOfTime() { return { END_OF_TIME }; }

    constexpr _Instant(_TimeVal value = 0) : _value(value) { }

    constexpr bool isEndOfTime() const { return _value == END_OF_TIME; }

    constexpr _Instant& operator+=(TimeSpan other) {
        *this = *this + other;
        return *this;
    }

    constexpr _Instant& operator-=(TimeSpan other) {
        *this = *this - other;
        return *this;
    }

    constexpr _Instant operator+(TimeSpan other) const {
        if (other.isInfinite()) {
            return endOfTime();
        }
        if (isEndOfTime()) {
            return *this;
        }
        return _value + other._value;
    }

    constexpr _Instant operator-(TimeSpan other) const {
        if (isEndOfTime()) {
            return *this;
        }
        return _value - other._value;
    }

    constexpr TimeSpan operator-(_Instant other) const {
        if (isEndOfTime() or other.isEndOfTime()) {
            return TimeSpan::infinite();
        }
        return _value - other._value;
    }

    constexpr auto val() const { return _value; }

    auto operator<=>(_Instant const&) const = default;
    bool operator==(_Instant const&) const  = default;
};

using Instant = _Instant<MonotonicClock>;

using SystemTime = _Instant<SystemClock>;

// TODO: I think _Instant<T> doesn't make sense.

// MARK: Time

struct Time {
    u8 sec;
    u8 min;
    u8 hour;
};

using Day = usize;

#define FOREACH_MONTH(MONTH)                                                   \
    MONTH(JANUARY, JAN)                                                        \
    MONTH(FEBRUARY, FEB)                                                       \
    MONTH(MARCH, MAR)                                                          \
    MONTH(APRIL, APR)                                                          \
    MONTH(MAY, MAY)                                                            \
    MONTH(JUNE, JUN)                                                           \
    MONTH(JULY, JUL)                                                           \
    MONTH(AUGUST, AUG)                                                         \
    MONTH(SEPTEMBER, SEP)                                                      \
    MONTH(OCTOBER, OCT)                                                        \
    MONTH(NOVEMBER, NOV)                                                       \
    MONTH(DECEMBER, DEC)

struct Month {
    enum _Name {

#define ITER(NAME, ...) NAME,
        FOREACH_MONTH(ITER)
#undef ITER
    };

    usize _raw;

    constexpr Month(isize raw = 0) : _raw(raw) { }

    constexpr Month(_Name name) : _raw(name) { }

    constexpr operator usize() const { return _raw; }

    Month next() const { return (_raw + 1) % 12; }

    char const* name() const {
        switch (_raw) {
#define ITER(NAME, ...)                                                        \
    case NAME: return #NAME;
            FOREACH_MONTH(ITER)
#undef ITER
        }
        panic("invalid month");
    }

    char const* abbr() const {
        switch (_raw) {
#define ITER(NAME, ABBR)                                                       \
    case NAME: return #ABBR;
            FOREACH_MONTH(ITER)
#undef ITER
        }
        panic("invalid month");
    }

    usize val() const { return _raw; }

    auto operator<=>(Month const&) const = default;
};

struct Year {
    isize _raw;

    constexpr Year(isize raw = 0) : _raw(raw) { }

    constexpr bool isLeap() const {
        return (_raw % 4 == 0 and _raw % 100 != 0) or _raw % 400 == 0;
    }

    constexpr operator isize() const { return _raw; }

    Year next() const { return { _raw + 1 }; }

    Day days() const { return isLeap() ? 366 : 365; }

    constexpr Day daysIn(Month month) const {
        switch (month) {
            case 0:
            case 2:
            case 4:
            case 6:
            case 7:
            case 9:
            case 11: return 31;
            case 3:
            case 5:
            case 8:
            case 10: return 30;
            case 1:  return isLeap() ? 29 : 28;
            default: return 0;
        }
    }

    auto operator<=>(Year const&) const = default;
};

// MARK: Date

#define FOREACH_DAY_OF_WEEK(DOW)                                               \
    DOW(MONDAY, MON)                                                           \
    DOW(TUESDAY, TUE)                                                          \
    DOW(WEDNESDAY, WED)                                                        \
    DOW(THURSDAY, THU)                                                         \
    DOW(FRIDAY, FRI)                                                           \
    DOW(SATURDAY, SAT)                                                         \
    DOW(SUNDAY, SUN)

struct DayOfWeek {
    enum _Raw : usize {

#define ITER(NAME, ...) NAME,
        FOREACH_DAY_OF_WEEK(ITER)
#undef ITER
    };

    _Raw _raw;

    constexpr DayOfWeek(_Raw raw) : _raw(raw) { }

    constexpr DayOfWeek(usize raw) : _raw((_Raw) raw) { }

    constexpr operator usize() const { return _raw; }

    char const* name() {
        switch (_raw) {
#define ITER(NAME, ...)                                                        \
    case NAME: return #NAME;
            FOREACH_DAY_OF_WEEK(ITER)
#undef ITER
            default: panic("invalid day of week");
        }
    }

    char const* abbr() {
        switch (_raw) {
#define ITER(NAME, ABBR)                                                       \
    case NAME: return #ABBR;
            FOREACH_DAY_OF_WEEK(ITER)
#undef ITER
            default: panic("invalid day of week");
        }
    }

    auto operator<=>(DayOfWeek const&) const = default;
};

struct Date {
    Day   day;
    Month month;
    Year  year;

    static Date epoch() { return Date { 0, Month::JANUARY, 1970 }; }
};

// MARK: DateTime

enum class DateTimeKind {
    Unspecified,
    Utc   = 2,
    Local = 3
};

struct DateTime {
    Time         time;
    Date         date;
    DateTimeKind kind = DateTimeKind::Unspecified;
};
