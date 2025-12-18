#pragma once
#include <type_traits>

namespace kissra {
template <typename Tag>
struct builtin_mixins;

template <typename Tag>
struct no_custom_mixins;

template <typename DeferInstantiation>
struct custom_mixins_traits;

template <typename DeferInstantiation>
inline consteval bool has_custom_mixins() {
    return !std::is_same_v<typename custom_mixins_traits<DeferInstantiation>::template mixins<DeferInstantiation>, no_custom_mixins<DeferInstantiation>>;
}

template <typename DeferInstantiation, typename TMakeIterFn>
auto with_custom_mixins(TMakeIterFn fn) {
    if constexpr (has_custom_mixins<DeferInstantiation>()) {
        return fn.template operator()<custom_mixins_traits<DeferInstantiation>::template mixins>();
    } else {
        return fn.template operator()<>();
    }
}

namespace compo {
template <typename Tag>
struct builtin_mixins;

template <typename Tag>
using no_custom_mixins = kissra::no_custom_mixins<Tag>;

template <typename DeferInstantiation>
inline consteval bool has_custom_mixins() {
    return !std::is_same_v<typename custom_mixins_traits<DeferInstantiation>::template compose_mixins<DeferInstantiation>, no_custom_mixins<DeferInstantiation>>;
}

template <typename DeferInstantiation, typename TMakeComposePartFn>
auto with_custom_mixins(TMakeComposePartFn fn) {
    if constexpr (has_custom_mixins<DeferInstantiation>()) {
        return fn.template operator()<custom_mixins_traits<DeferInstantiation>::template compose_mixins>();
    } else {
        return fn.template operator()<>();
    }
}
} // namespace compo
} // namespace kissra