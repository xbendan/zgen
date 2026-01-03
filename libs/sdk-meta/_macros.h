#pragma once

#define alignat(x) __attribute__((aligned(x)))
#define section(x) __attribute__((section(x)))

#if defined(__clang__)
#    define lifetimebound [[clang::lifetimebound]]
#elif defined(__GNUC__)
#    define lifetimebound
#    define deprecated [[gnu::deprecated]]
#endif

#define MakeFlags$(T)                                                          \
    constexpr inline T operator~(T rhs) {                                      \
        using U = UnderlyingType<T>;                                           \
        return (T) ~(U) rhs;                                                   \
    }                                                                          \
    constexpr inline T operator|(T lhs, T rhs) {                               \
        using U = UnderlyingType<T>;                                           \
        return (T) ((U) lhs | (U) rhs);                                        \
    }                                                                          \
    constexpr inline T operator&(T lhs, T rhs) {                               \
        using U = UnderlyingType<T>;                                           \
        return (T) ((U) lhs & (U) rhs);                                        \
    }                                                                          \
    constexpr inline T operator^(T lhs, T rhs) {                               \
        using U = UnderlyingType<T>;                                           \
        return (T) ((U) lhs ^ (U) rhs);                                        \
    }                                                                          \
    constexpr inline T& operator|=(T& lhs, T rhs) {                            \
        return lhs = lhs | rhs;                                                \
    }                                                                          \
    constexpr inline T& operator&=(T& lhs, T rhs) {                            \
        return lhs = lhs & rhs;                                                \
    }                                                                          \
    constexpr inline T& operator^=(T& lhs, T rhs) {                            \
        return lhs = lhs ^ rhs;                                                \
    }

// MARK: Try -------------------------------------------------------------------

// Give us a symbole to break one when debbuging error handling.
// This is a no-op in release mode.
#if defined(TRY_FAIL_REDIRECT)
extern "C" void __try_failed();
#    define __tryFail() __try_failed()
#else
#    define __tryFail() /* NOP */
#endif

#define __try$(EXPR, RET, AWAIT, FINAL)                                        \
    ({                                                                         \
        auto __expr = AWAIT(EXPR);                                             \
        if (not static_cast<bool>(__expr)) [[unlikely]] {                      \
            __tryFail();                                                       \
            RET __expr.none();                                                 \
            FINAL;                                                             \
        }                                                                      \
        __expr.take();                                                         \
    })

#define pre$(EXPR)                                                             \
    ({                                                                         \
        if (not(EXPR)) [[unlikely]] {                                          \
            logError("Precondition failed: " #EXPR);                           \
            return Error::invalidArgument("precondition failed: " #EXPR);      \
        }                                                                      \
    })

// clang-format off
#define try$(EXPR)      __try$(EXPR,    return,          ,          )
#define co_try$(EXPR)   __try$(EXPR, co_return,          ,          )
#define co_trya$(EXPR)  __try$(EXPR, co_return,  co_await,          )
#define co_tryy$(EXPR)  __try$(EXPR,  co_yield,          , co_return)
#define co_tryya$(EXPR) __try$(EXPR,  co_yield,  co_await, co_return)
// clang-format on
