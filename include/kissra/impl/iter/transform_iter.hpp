#pragma once
#include "kissra/impl/iter/iter_base.hpp"

#ifndef KISSRA_MODULE
#include <cstddef>
#include <type_traits>
#include <utility>
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename TBaseIter, typename TFn, template <typename> typename... TMixins>
    requires kissra::regular_invocable<TFn, typename TBaseIter::reference>
class transform_iter : public iter_base<TBaseIter>, public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
    using base_reference = typename TBaseIter::reference;

public:
    using reference = kissra::invoke_result_t<TFn, base_reference>;
    using value_type = std::remove_cvref_t<reference>;
    using result_t = kissra::optional<reference>;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;

    template <typename UBaseIter>
    constexpr transform_iter(UBaseIter&& base_iter, TFn fn)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , fn(fn) {}

    [[nodiscard]] constexpr result_t next() {
        if (auto item = this->base_iter.next()) {
            return kissra::invoke(this->fn.inst, std::forward_like<base_reference>(*item));
        }
        return {};
    }

    [[nodiscard]] constexpr result_t next_back()
        requires is_common && is_bidir
    {
        if (auto item = this->base_iter.next_back()) {
            return kissra::invoke(this->fn.inst, std::forward_like<base_reference>(*item));
        }
        return {};
    }

    [[nodiscard]] constexpr result_t nth(std::size_t n) {
        if (auto item = this->base_iter.nth(n)) {
            return kissra::invoke(this->fn.inst, std::forward_like<base_reference>(*item));
        }
        return {};
    }

    [[nodiscard]] constexpr result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        if (auto item = this->base_iter.nth_back(n)) {
            return kissra::invoke(this->fn.inst, std::forward_like<base_reference>(*item));
        }
        return {};
    }

    constexpr std::size_t advance(std::size_t n) {
        return this->base_iter.advance(n);
    }

    constexpr std::size_t advance_back(std::size_t n)
        requires is_bidir
    {
        return this->base_iter.advance_back(n);
    }

    constexpr auto size() const
        requires is_sized
    {
        return this->base_iter.size();
    }

private:
    [[no_unique_address]] functor_ebo<TFn, TBaseIter> fn;
};

template <typename Tag>
struct transform_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    constexpr auto transform(this TSelf&& self, TFn fn) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> {
            return transform_iter<std::remove_cvref_t<TSelf>, TFn, TMixins...>{ std::forward<TSelf>(self), fn };
        });
    }
};

namespace compo {
template <typename TBaseCompose, typename TFn, template <typename> typename... TMixinsCompose>
struct transform_compose : public builtin_mixins_compose<TBaseCompose>, public TMixinsCompose<TBaseCompose>... {
    [[no_unique_address]] TBaseCompose base_comp;
    [[no_unique_address]] functor_ebo<TFn, TBaseCompose> fn;

    template <template <typename> typename... TMixins, typename TSelf, kissra::iterator UBaseIter>
    constexpr auto make_iter(this TSelf&& self, UBaseIter&& base_iter) {
        return transform_iter<std::remove_cvref_t<UBaseIter>, TFn, TMixins...>{
            std::forward<UBaseIter>(base_iter),
            std::forward<TSelf>(self).fn.inst,
        };
    }
};

template <typename Tag>
struct transform_compose_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    constexpr auto transform(this TSelf&& self, TFn fn) {
        return with_custom_mixins_compose<DeferInstantiation>([&]<template <typename> typename... TMixinsCompose> {
            return transform_compose<std::remove_cvref_t<TSelf>, TFn, TMixinsCompose...>{
                .base_comp = std::forward<TSelf>(self),
                .fn = fn,
            };
        });
    }
};

template <typename TFn, typename DeferInstantiation = void>
constexpr auto transform(TFn fn) {
    return compose<DeferInstantiation>().transform(fn);
}
} // namespace compo
} // namespace kissra