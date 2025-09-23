#pragma once

struct Backtrace;

namespace _Embed {

// MARK: Backtrace -------------------------------------------------------------

Backtrace captureBacktrace();

Backtrace forceCaptureBacktrace();

// MARK: Locks -----------------------------------------------------------------

void relaxe();

void enterCritical();

void leaveCritical();

} // namespace _Embed
