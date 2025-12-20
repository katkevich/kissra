#pragma once
#include "kissra/concepts.hpp"
#include "kissra/impl/export.hpp"

#ifndef KISSRA_MODULE
#include <ranges>
#include <utility>
#endif

KISSRA_EXPORT()
namespace kissra {
template <std::ranges::range TRng, template <typename> typename... TMixins>
class all_iter;

namespace impl {
template <template <typename> typename... TMixins, std::ranges::range TRng>
constexpr auto into_kissra_iter_with_mixins(TRng& rng) {
    return all_iter<TRng, TMixins...>{ rng };
}

template <template <typename> typename... TMixins, kissra::iterator TIter>
constexpr decltype(auto) into_kissra_iter_with_mixins(TIter&& iter) {
    return std::forward<TIter>(iter);
}

template <typename DeferInstantiation, std::ranges::range TRng>
constexpr auto into_kissra_iter(TRng& rng) {
    return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> { //
        return all_iter<TRng, TMixins...>{ rng };
    });
}

template <typename DeferInstantiation, kissra::iterator TIter>
constexpr decltype(auto) into_kissra_iter(TIter&& iter) {
    return std::forward<TIter>(iter);
}
} // namespace impl
} // namespace kissra