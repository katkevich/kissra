#pragma once
#include <utility>

#define KISSRA_FWD(x) std::forward<decltype(x)>(x)

namespace kissra {
template <typename TArg, typename TMember, typename T>
constexpr decltype(auto) forward_member(T&& member) {
    // T&& + int&& - can move
    // T&& + int&
    // T&& + int - can move

    // T& + int&&
    // T& + int&
    // T& + int
    if constexpr (std::is_rvalue_reference_v<TArg&&> && std::is_rvalue_reference_v<TMember&&>) {
        return static_cast<TMember&&>(member);
    } else {
        return static_cast<TMember&>(member);
    }
}

template <typename>
constexpr bool always_false = false;
} // namespace kissra