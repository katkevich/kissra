#pragma once
#include "kissra/impl/custom_mixins.hpp"
#include "kissra/misc/type_list.hpp"
#include <tuple>

namespace kissra {
namespace compo {
template <typename DeferInstantiation = void, template <typename> typename... TMixinsCompose>
struct root : builtin_mixins_compose<DeferInstantiation>, TMixinsCompose<DeferInstantiation>... {
    using is_composition_root = void;
};
} // namespace compo

template <typename DeferInstantiation = void>
constexpr auto compose() {
    return compo::with_custom_mixins_compose<DeferInstantiation>([&]<template <typename> typename... TMixinsCompose> {
        return compo::root<DeferInstantiation, TMixinsCompose...>{};
    });
}
} // namespace kissra