#pragma once

#include <sdk-io/std.h>
#include <sdk-meta/flags.h>
#include <sdk-meta/types.h>
#include <sdk-terminal/style.h>
#include <sdk-text/format.h>
#include <sdk-text/str.h>

namespace Sdk {

struct Loc {
    Str   file {};
    Str   func {};
    usize line {};
    usize column {};

    static constexpr Loc current(Str   file   = __FILE__,
                                 Str   func   = __PRETTY_FUNCTION__,
                                 usize line   = __LINE__,
                                 usize column = 0) {
        return { file, func, line, column };
    }
};

struct Level {
    i8          val;
    char const* name;
    Cli::Style  style = Cli::Style::Default;

    constexpr auto operator<=>(Level const& other) const {
        return val <=> other.val;
    }

    constexpr operator i8() const { return val; }
};

using Graph::iRgb;

static Level const DEBUG = { -1, "debug" };
static Level const INFO  = { 0, "info" };
static Level const WARN  = { 1, "warn", Cli::style().fg(iRgb::Yellow) };
static Level const ERROR = { 2, "error", Cli::style().fg(iRgb::LightRed) };
static Level const FATAL
    = { 3, "fatal", Cli::style().fg(iRgb::White).bg(iRgb::LightRed).bold() };

inline void catch$(Res<> res) {
    if (res)
        return;
    // debug("failed to write to logger");
    panic(res.none().msg());
}

inline Level _level;
inline Lock  _lock;
inline bool  _useTime;

inline void _log(Level level, Str fmt, Text::_Args& va) {
    if (level.val < _level.val or not Sdk::out()) {
        return;
    }
    LockScoped lock(_lock);

    auto& dest = (level >= ERROR) ? Sdk::err().unwrap() : Sdk::out().unwrap();

    // catch$(Text::format(dest, "{}", Cli::Style::Default));
    catch$(Text::format(
        dest, "[{}", Text::aligned(level.name, Text::Align::LEFT, 5)));
    // catch$(Text::format(dest, "[{}", level.name));
    // if (_useTime) {
    //     // catch$(Text::format(dest, ", {}", Text::aligned(level.name, Text::Align::LEFT, 12)));
    // }
    catch$(Text::format(dest, "] "));
    catch$(Text::_format(dest, fmt, va));
    // catch$(Text::format(dest, "{}\n", Cli::Style::Default));

    // catch$(dest.flush());
}

inline void logEmptyLines(usize n) {
    auto& dest = Sdk::out().unwrap();
    for (usize i = 0; i < n; i++) {
        catch$(Text::format(
            dest, "[{}]\n", Text::aligned(INFO.name, Text::Align::LEFT, 5)));
    }
}

inline void logInfo(Str format, auto&&... va) {
    Text::Args<decltype(va)...> args { ::forward<decltype(va)>(va)... };
    _log(INFO, format, args);
}

inline void logDebug(Str format, auto&&... va) {
    Text::Args<decltype(va)...> args { ::forward<decltype(va)>(va)... };
    _log(DEBUG, format, args);
}

inline void logWarn(Str format, auto&&... va) {
    Text::Args<decltype(va)...> args { ::forward<decltype(va)>(va)... };
    _log(WARN, format, args);
}

inline void logError(Str format, auto&&... va) {
    Text::Args<decltype(va)...> args { ::forward<decltype(va)>(va)... };
    _log(ERROR, format, args);
}

[[noreturn]] inline void logFatal(Loc loc, Str format, auto&&... va) {
    Text::Args<decltype(va)...> args { ::forward<decltype(va)>(va)... };
    _log(FATAL, format, args);
    panic("fatal error");
}

} // namespace Sdk

using Sdk::Loc;
using Sdk::logDebug;
using Sdk::logError;
using Sdk::logFatal;
using Sdk::logInfo;
using Sdk::logWarn;
