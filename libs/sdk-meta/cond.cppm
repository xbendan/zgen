module;

export module sdk:cond;

export template <bool B, typename T, typename F>
struct _Cond {
    using Type = T;
};

export template <typename T, typename F>
struct _Cond<false, T, F> {
    using Type = F;
};

export template <bool B, typename T, typename F>
using Cond = typename _Cond<B, T, F>::Type;
