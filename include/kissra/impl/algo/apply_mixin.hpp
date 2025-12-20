#pragma once
#include "kissra/concepts.hpp"
#include "kissra/impl/custom_mixins.hpp"
#include "kissra/impl/into_iter.hpp"
#include "kissra/type_traits.hpp"

namespace kissra {
namespace impl {
template <template <typename> typename... TMixins, kissra::iterator TIter, typename TComposition>
static constexpr auto apply(TIter&& iter, TComposition&& comp) {
    if constexpr (kissra::composition_root<TComposition>) {
        return KISSRA_FWD(iter);
    } else {
        return KISSRA_FWD(comp).template make_iter<TMixins...>(
            impl::apply<TMixins...>(KISSRA_FWD(iter), KISSRA_FWD(comp).base_comp));
    }
}
} // namespace impl

template <kissra::iterator_compatible T, typename TComposition, typename DeferInstantiation = void>
constexpr auto apply(T&& rng_or_kissra_iter, TComposition&& comp) {
    return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> {
        return impl::apply<TMixins...>(
            impl::into_kissra_iter_with_mixins<TMixins...>(KISSRA_FWD(rng_or_kissra_iter)), KISSRA_FWD(comp));
    });
}

template <typename Tag>
struct apply_mixin {
    template <kissra::mut TSelf, typename TComposition>
    constexpr auto apply(this TSelf&& self, TComposition&& comp) {
        return kissra::apply(KISSRA_FWD(self), KISSRA_FWD(comp));
    }
};
} // namespace kissra