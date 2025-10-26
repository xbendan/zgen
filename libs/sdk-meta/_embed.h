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

// MARK: CPU -------------------------------------------------------------------

usize currentId();

} // namespace _Embed
