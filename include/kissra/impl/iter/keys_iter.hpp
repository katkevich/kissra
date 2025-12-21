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
using keys_iter = transform_iter<TBaseIter, functor::member_t<0>, TMixins...>;

template <typename Tag>
struct keys_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    constexpr auto keys(this TSelf&& self) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> {
            return keys_iter<std::remove_cvref_t<TSelf>, TMixins...>{
                std::forward<TSelf>(self),
                fn::member<0>,
            };
        });
    }
};

template <kissra::iterator_compatible T, typename DeferInstantiation = void>
constexpr auto keys(T&& rng_or_kissra_iter) {
    return impl::into_kissra_iter<DeferInstantiation>(KISSRA_FWD(rng_or_kissra_iter)).keys();
}

namespace compo {
template <typename TBaseCompose, template <typename> typename... TMixinsCompose>
using keys_compose = transform_compose<TBaseCompose, functor::member_t<0>, TMixinsCompose...>;

template <typename Tag>
struct keys_compose_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    constexpr auto keys(this TSelf&& self) {
        return with_custom_mixins_compose<DeferInstantiation>([&]<template <typename> typename... TMixinsCompose> {
            return keys_compose<std::remove_cvref_t<TSelf>, TMixinsCompose...>{
                .base_comp = std::forward<TSelf>(self),
                .fn = fn::member<0>,
            };
        });
    }
};

template <typename DeferInstantiation = void>
constexpr auto keys() {
    return compose<DeferInstantiation>().keys();
}
} // namespace compo
} // namespace kissra