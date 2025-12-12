#pragma once
#include "kissra/misc/utility.hpp"
#include <functional>
#include <type_traits>

namespace kissra {
/**
 * Invoke `TFn` with either destructured aggregate or destructured tuple-like (`TArg` implements tuple protocol)
 * "Destructured" basically means this:
 *   auto [...xs] = arg;
 *   std::invoke(fn, xs...);
 */
template <typename TFn, typename TArg>
inline constexpr decltype(auto) invoke_with_destructured_arg(TFn&& fn, TArg&& arg) {
    using arg_t = decltype(arg);
    auto&& [... xs] = arg;
    return std::invoke(std::forward<TFn>(fn), kissra::forward_member<arg_t, decltype(xs)>(xs)...);
}

template <typename TFn, typename... TArgs>
inline constexpr decltype(auto) invoke(TFn&& fn, TArgs&&... args) {
    if constexpr (std::is_invocable_v<TFn, TArgs...>) {
        return std::invoke(std::forward<TFn>(fn), std::forward<TArgs>(args)...);
    } else if constexpr (sizeof...(TArgs) == 1) {
        using arg_t = decltype(args...[0]);
        return kissra::invoke_with_destructured_arg(std::forward<TFn>(fn), std::forward<arg_t>(args...[0]));
    } else {
        static_assert(always_false<TFn>,
            "TFn neither is invocable with TArgs... nor with decomposed-into-members TArgs...[0] "
            "(cannot decompose - sizeof...(TArgs) != 1 or TArgs...[0] is not an aggregate)");
    }
}
} // namespace kissra