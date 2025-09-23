#pragma once

template <bool B, typename T, typename F>
struct _Cond {
    using Type = T;
};

template <typename T, typename F>
struct _Cond<false, T, F> {
    using Type = F;
};

template <bool B, typename T, typename F>
using Cond = typename _Cond<B, T, F>::Type;
