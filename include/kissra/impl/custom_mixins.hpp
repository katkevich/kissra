#pragma once
#include "kissra/impl/export.hpp"

#ifndef KISSRA_MODULE
#include <type_traits>
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename Tag>
struct builtin_mixins;

template <typename Tag>
struct no_custom_mixins {};

template <typename DeferInstantiation>
struct custom_mixins_traits;

template <typename DeferInstantiation>
inline consteval bool has_custom_mixins() {
    return !std::is_same_v<typename custom_mixins_traits<DeferInstantiation>::template mixins<DeferInstantiation>, no_custom_mixins<DeferInstantiation>>;
}

template <typename DeferInstantiation, typename TMakeIterFn>
constexpr auto with_custom_mixins(TMakeIterFn fn) {
    if constexpr (has_custom_mixins<DeferInstantiation>()) {
        return fn.template operator()<custom_mixins_traits<DeferInstantiation>::template mixins>();
    } else {
        return fn.template operator()<>();
    }
}

namespace compo {
template <typename Tag>
struct builtin_mixins_compose;

template <typename Tag>
using no_custom_mixins_compose = kissra::no_custom_mixins<Tag>;

template <typename DeferInstantiation>
consteval bool has_custom_mixins_compose() {
    return !std::is_same_v<typename custom_mixins_traits<DeferInstantiation>::template compose_mixins<DeferInstantiation>, no_custom_mixins_compose<DeferInstantiation>>;
}

template <typename DeferInstantiation, typename TMakeComposePartFn>
constexpr auto with_custom_mixins_compose(TMakeComposePartFn fn) {
    if constexpr (has_custom_mixins<DeferInstantiation>()) {
        return fn.template operator()<custom_mixins_traits<DeferInstantiation>::template compose_mixins>();
    } else {
        return fn.template operator()<>();
    }
}
} // namespace compo
} // namespace kissra