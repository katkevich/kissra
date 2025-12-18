#pragma once
#include "kissra/concepts.hpp"
#include "kissra/impl/custom_mixins.hpp"
#include "kissra/type_traits.hpp"

namespace kissra {
namespace impl {
template <template <typename> typename... CustomMixins, kissra::iterator TIter, kissra::composition_for<TIter> TComposition>
static auto apply(TIter&& iter, TComposition&& comp) {
    if constexpr (kissra::composition_root<TComposition>) {
        return KISSRA_FWD(iter);
    } else {
        return KISSRA_FWD(comp).template make_iter<CustomMixins...>(
            impl::apply<CustomMixins...>(KISSRA_FWD(iter), KISSRA_FWD(comp).base_comp));
    }
}
} // namespace impl

template <kissra::iterator_compatible T, typename TComposition, typename DeferInstantiation = void>
auto apply(T&& rng_or_kissra_iter, TComposition&& comp) {
    return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... CustomMixins> {
        return impl::apply<CustomMixins...>(into_kissra_iter<CustomMixins...>(KISSRA_FWD(rng_or_kissra_iter)), KISSRA_FWD(comp));
    });
}

template <typename Tag>
struct apply_mixin {
    template <kissra::mut TSelf, typename TComposition>
    auto apply(this TSelf&& self, TComposition&& comp) {
        return kissra::apply(KISSRA_FWD(self), KISSRA_FWD(comp));
    }
};
} // namespace kissra