#pragma once
#include "kissra/impl/compose.hpp"
#include "kissra/impl/into_iter.hpp"
#include "kissra/impl/iter/all_iter.hpp"
#include "kissra/impl/iter/iter_base.hpp"
#include "kissra/misc/type_list.hpp"
#include "kissra/misc/utility.hpp"

#ifndef KISSRA_MODULE
#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename TBaseIter, typename TItersTypeList, template <typename> typename... TMixins>
class zip_iter;

template <typename TBaseIter, typename... TIters, template <typename> typename... TMixins>
class zip_iter<TBaseIter, tmp::type_list<TIters...>, TMixins...>
    : public iter_base<TBaseIter>, public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
public:
    using value_type = std::tuple<typename TBaseIter::reference, typename TIters::reference...>;
    using reference = value_type;
    using result_t = kissra::optional<reference>;

    static constexpr bool is_sized = TBaseIter::is_sized && (TIters::is_sized && ...);
    static constexpr bool is_common = TBaseIter::is_common && (TIters::is_common && ...);
    static constexpr bool is_forward = TBaseIter::is_forward && (TIters::is_forward && ...);
    static constexpr bool is_bidir = TBaseIter::is_bidir && (TIters::is_bidir && ...);
    static constexpr bool is_random = TBaseIter::is_random && (TIters::is_random && ...);

    template <typename UBaseIter, typename... UIters>
    constexpr explicit zip_iter(UBaseIter&& base_iter, UIters&&... iters)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , iters(std::move(iters)...) {}

    template <typename TSelf>
    [[nodiscard]] constexpr result_t next(this TSelf&& self) {
        auto& [... iters_pack] = self.iters;
        auto [... nexts_pack] = std::tuple{ self.base_iter.next(), iters_pack.next()... };

        if ((nexts_pack.has_value() && ...)) {
            return reference{ KISSRA_FWD(*nexts_pack)... };
        }

        return {};
    }

    template <typename TSelf>
        requires is_sized && is_bidir && is_common && ((is_sized_v<TIters> && is_bidir_v<TIters> && is_common_v<TIters>) && ...)
    [[nodiscard]] constexpr result_t next_back(this TSelf&& self) {
        /* align the tails */
        self.advance_back(0);

        auto& [... iters_pack] = self.iters;
        auto [... next_backs_pack] = std::tuple{ self.base_iter.next_back(), iters_pack.next_back()... };

        if ((next_backs_pack.has_value() && ...)) {
            return reference{ KISSRA_FWD(*next_backs_pack)... };
        }

        return {};
    }

    template <typename TSelf>
    [[nodiscard]] constexpr result_t nth(this TSelf&& self, std::size_t n) {
        auto& [... iters_pack] = self.iters;
        auto [... nths_pack] = std::tuple{ self.base_iter.nth(n), iters_pack.nth(n)... };

        if ((nths_pack.has_value() && ...)) {
            return reference{ KISSRA_FWD(*nths_pack)... };
        }

        return {};
    }

    template <typename TSelf>
        requires is_sized && is_bidir && is_common && ((is_sized_v<TIters> && is_bidir_v<TIters> && is_common_v<TIters>) && ...)
    [[nodiscard]] constexpr result_t nth_back(this TSelf&& self, std::size_t n) {
        /* align the tails */
        self.advance_back(0);

        auto& [... iters_pack] = self.iters;
        auto [... next_backs_pack] = std::tuple{ self.base_iter.nth_back(n), iters_pack.nth_back(n)... };

        if ((next_backs_pack.has_value() && ...)) {
            return reference{ KISSRA_FWD(*next_backs_pack)... };
        }

        return {};
    }

    template <typename TSelf>
    constexpr std::size_t advance(this TSelf&& self, std::size_t n) {
        auto& [... iters_pack] = self.iters;
        std::array advancements{ self.base_iter.advance(n), iters_pack.advance(n)... };

        return *std::ranges::min_element(advancements);
    }

    template <typename TSelf>
        requires is_sized && is_bidir && ((is_sized_v<TIters> && is_bidir_v<TIters>) && ...)
    constexpr std::size_t advance_back(this TSelf&& self, std::size_t n) {
        auto& [... iters_pack] = self.iters;

        const auto base_iter_size = self.base_iter.size();
        const auto [... iter_sizes] = std::array{ iters_pack.size()... };

        const auto sizes = std::array{ base_iter_size, iter_sizes... };
        const auto min_size = *std::ranges::min_element(sizes);

        const auto base_iter_tail = base_iter_size - min_size;
        const auto [... iter_tails] = std::array{ (iter_sizes - min_size)... };

        const auto advancement = self.base_iter.advance_back(base_iter_tail + n);
        (iters_pack.advance_back(iter_tails + n), ...);

        return advancement - base_iter_tail;
    }

    template <typename TSelf>
        requires is_sized
    constexpr auto size(this const TSelf& self) {
        auto& [... iters_pack] = self.iters;
        const auto sizes = std::array{ self.base_iter.size(), iters_pack.size()... };

        return *std::ranges::min_element(sizes);
    }

private:
    [[no_unique_address]] std::tuple<TIters...> iters;
};


template <kissra::iterator_compatible T, kissra::iterator_compatible... Ts, typename DeferInstantiation = void>
constexpr auto zip(T&& rng_or_kissra_iter, Ts&&... rngs_or_kissra_iters) {
    using iter_first =
        std::remove_reference_t<decltype(impl::into_kissra_iter<DeferInstantiation>(KISSRA_FWD(rng_or_kissra_iter)))>;
    using iters_type_list =
        tmp::type_list<std::remove_reference_t<decltype(impl::into_kissra_iter<DeferInstantiation>(KISSRA_FWD(rngs_or_kissra_iters)))>...>;

    return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> {
        return zip_iter<iter_first, iters_type_list, TMixins...>{ //
            impl::into_kissra_iter<DeferInstantiation>(KISSRA_FWD(rng_or_kissra_iter)),
            impl::into_kissra_iter<DeferInstantiation>(KISSRA_FWD(rngs_or_kissra_iters))...
        };
    });
}

template <typename Tag>
struct zip_mixin {
    template <kissra::iterator_compatible TSelf, kissra::iterator_compatible... Ts>
    constexpr auto zip(this TSelf&& self, Ts&&... rngs_or_kissra_iters) {
        return kissra::zip(KISSRA_FWD(self), KISSRA_FWD(rngs_or_kissra_iters)...);
    }
};


namespace compo {
template <typename TBaseCompose, typename TItersTypeList, template <typename> typename... TMixinsCompose>
struct zip_compose;

template <typename TBaseCompose, kissra::iterator_compatible... TIters, template <typename> typename... TMixinsCompose>
struct zip_compose<TBaseCompose, tmp::type_list<TIters...>, TMixinsCompose...>
    : public builtin_mixins_compose<TBaseCompose>, public TMixinsCompose<TBaseCompose>... {

    [[no_unique_address]] TBaseCompose base_comp;
    [[no_unique_address]] std::tuple<TIters...> iters;

    template <template <typename> typename... TMixins, typename TSelf, kissra::iterator UBaseIter>
    constexpr auto make_iter(this TSelf&& self, UBaseIter&& base_iter) {
        auto&& [... iters] = KISSRA_FWD(self).iters;

        return zip_iter<std::remove_cvref_t<UBaseIter>, tmp::type_list<TIters...>, TMixins...>{
            KISSRA_FWD(base_iter),
            kissra::forward_member<TSelf, decltype(iters)>(iters)...,
        };
    }
};

template <typename Tag>
struct zip_compose_mixin {
    template <typename TSelf, kissra::iterator_compatible... Ts, typename DeferInstantiation = void>
    constexpr auto zip(this TSelf&& self, Ts&&... rngs_or_kissra_iters) {
        using iters_type_list =
            tmp::type_list<std::remove_reference_t<decltype(impl::into_kissra_iter<DeferInstantiation>(KISSRA_FWD(rngs_or_kissra_iters)))>...>;

        return with_custom_mixins_compose<DeferInstantiation>([&]<template <typename> typename... TMixinsCompose> {
            return zip_compose<std::remove_cvref_t<TSelf>, iters_type_list, TMixinsCompose...>{
                .base_comp = KISSRA_FWD(self),
                .iters = std::make_tuple(impl::into_kissra_iter<DeferInstantiation>(KISSRA_FWD(rngs_or_kissra_iters))...),
            };
        });
    }
};

template <kissra::iterator_compatible T, kissra::iterator_compatible... Ts, typename DeferInstantiation = void>
constexpr auto zip(T&& rng_or_kissra_iter, Ts&&... rngs_or_kissra_iters) {
    return compose<DeferInstantiation>().zip(KISSRA_FWD(rng_or_kissra_iter), KISSRA_FWD(rngs_or_kissra_iters)...);
}
} // namespace compo
} // namespace kissra