#pragma once
#include "kissra/impl/export.hpp"

#define KISSRA_FWD(x) std::forward<decltype(x)>(x)

#ifndef KISSRA_MODULE
#include <type_traits>
#include <utility>
#endif

KISSRA_EXPORT()
namespace kissra {
// TArg&& + TMember&& -> rvalue
// TArg&& + TMember&
// TArg&& + TMember -> rvalue
//
// TArg& + TMember&&
// TArg& + TMember&
// TArg& + TMember
template <typename TArg, typename TMember, typename T>
constexpr decltype(auto) forward_member(T&& member) {
    if constexpr (std::is_rvalue_reference_v<TArg&&> && std::is_rvalue_reference_v<TMember&&>) {
        return static_cast<TMember&&>(member);
    } else {
        return static_cast<TMember&>(member);
    }
}

template <typename>
constexpr bool always_false = false;
} // namespace kissra