#pragma once
#include "kissra/fn/member.hpp"
#include "kissra/impl/iter/transform_iter.hpp"

#ifndef KISSRA_MODULE
#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename TBaseIter, template <typename> typename... TMixins>
using values_iter = transform_iter<TBaseIter, functor::member_t<1>, TMixins...>;

template <typename Tag>
struct values_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    constexpr auto values(this TSelf&& self) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> {
            return values_iter<std::remove_cvref_t<TSelf>, TMixins...>{
                std::forward<TSelf>(self),
                fn::member<1>,
            };
        });
    }
};

template <kissra::iterator_compatible T, typename DeferInstantiation = void>
constexpr auto values(T&& rng_or_kissra_iter) {
    return impl::into_kissra_iter<DeferInstantiation>(KISSRA_FWD(rng_or_kissra_iter)).values();
}

namespace compo {
template <typename TBaseCompose, template <typename> typename... TMixinsCompose>
using values_compose = transform_compose<TBaseCompose, functor::member_t<1>, TMixinsCompose...>;

template <typename Tag>
struct values_compose_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    constexpr auto values(this TSelf&& self) {
        return with_custom_mixins_compose<DeferInstantiation>([&]<template <typename> typename... TMixinsCompose> {
            return values_compose<std::remove_cvref_t<TSelf>, TMixinsCompose...>{
                .base_comp = std::forward<TSelf>(self),
                .fn = fn::member<1>,
            };
        });
    }
};

template <typename DeferInstantiation = void>
constexpr auto values() {
    return compose<DeferInstantiation>().values();
}
} // namespace compo
} // namespace kissra