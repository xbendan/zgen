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

// clang-format off
#define try$(EXPR)      __try$(EXPR,    return,          ,          )
#define co_try$(EXPR)   __try$(EXPR, co_return,          ,          )
#define co_trya$(EXPR)  __try$(EXPR, co_return,  co_await,          )
#define co_tryy$(EXPR)  __try$(EXPR,  co_yield,          , co_return)
#define co_tryya$(EXPR) __try$(EXPR,  co_yield,  co_await, co_return)
// clang-format on
