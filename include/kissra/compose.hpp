#pragma once
#include "kissra/impl/custom_mixins.hpp"
#include "kissra/misc/type_list.hpp"
#include <tuple>

namespace kissra {
namespace compo {
template <typename DeferInstantiation = void, template <typename> typename... TMixins>
struct root : builtin_mixins<DeferInstantiation>, TMixins<DeferInstantiation>... {
    static constexpr bool is_root = true;
};
} // namespace compo

template <typename DeferInstantiation = void>
auto compose() {
    return compo::with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... CustomMixins> { //
        return compo::root<DeferInstantiation, CustomMixins...>{};
    });
}
} // namespace kissra