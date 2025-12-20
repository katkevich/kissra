#pragma once
#include "kissra/fn/member.hpp"
#include "kissra/iter/transform_iter.hpp"
#include <algorithm>
#include <type_traits>

namespace kissra {
template <typename TBaseIter, std::size_t MemberIdx, template <typename> typename... TMixins>
using members_iter = transform_iter<TBaseIter, functor::member_t<MemberIdx>, TMixins...>;

template <typename Tag>
struct members_mixin {
    template <std::size_t MemberIdx, typename TSelf, typename DeferInstantiation = void>
    constexpr auto members(this TSelf&& self) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> {
            return members_iter<std::remove_cvref_t<TSelf>, MemberIdx, TMixins...>{
                std::forward<TSelf>(self),
                fn::member<MemberIdx>,
            };
        });
    }
};

namespace compo {

template <typename TBaseCompose, std::size_t MemberIdx, template <typename> typename... TMixinsCompose>
using members_compose = transform_compose<TBaseCompose, functor::member_t<MemberIdx>, TMixinsCompose...>;

template <typename Tag>
struct members_compose_mixin {
    template <std::size_t MemberIdx, typename TSelf, typename DeferInstantiation = void>
    constexpr auto members(this TSelf&& self) {
        return with_custom_mixins_compose<DeferInstantiation>([&]<template <typename> typename... TMixinsCompose> {
            return members_compose<std::remove_cvref_t<TSelf>, MemberIdx, TMixinsCompose...>{
                .base_comp = std::forward<TSelf>(self),
                .fn = fn::member<MemberIdx>,
            };
        });
    }
};

template <std::size_t MemberIdx, typename DeferInstantiation = void>
constexpr auto members() {
    return compose<DeferInstantiation>().template members<MemberIdx>();
}
} // namespace compo
} // namespace kissra