#pragma once
#include "kissra/impl/compose.hpp"
#include "kissra/impl/iter/iter_base.hpp"
#include "kissra/misc/functional.hpp"

#ifndef KISSRA_MODULE
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename TBaseIter, typename TFn, template <typename> typename... TMixins>
    requires kissra::regular_invocable<TFn, typename TBaseIter::reference>
class drop_last_while_iter : public iter_base<TBaseIter>, public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using result_t = typename TBaseIter::result_t;

    static constexpr bool is_sized = false;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = false;

    template <typename UBaseIter>
    constexpr drop_last_while_iter(UBaseIter&& base_iter, TFn fn)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , fn(fn) {}

    [[nodiscard]] constexpr result_t next()
        requires is_common && is_bidir
    {
        this->ff();
        return this->base_iter.next();
    }

    [[nodiscard]] constexpr result_t next_back()
        requires is_common && is_bidir
    {
        this->ff();
        return this->base_iter.next_back();
    }

    [[nodiscard]] constexpr result_t nth(std::size_t n)
        requires is_common && is_bidir
    {
        this->ff();
        return this->base_iter.nth(n);
    }

    [[nodiscard]] constexpr result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        this->ff_self();
        return this->base_iter.nth_back(n);
    }

    constexpr std::size_t advance(std::size_t n)
        requires is_common && is_bidir
    {
        this->ff();
        return this->base_iter.advance(n);
    }

    constexpr std::size_t advance_back(std::size_t n)
        requires is_common && is_bidir
    {
        this->ff_self();
        return this->base_iter.advance_back(n);
    }

private:
    constexpr void ff() {
        if (!this->dropped) {
            this->advance_back(0);
        }
    }

    constexpr void ff_self() {
        if (!std::exchange(this->dropped, true)) {
            for (auto item = this->base_iter.back(); item; item = this->base_iter.nth_back(1)) {
                if (!kissra::invoke(this->fn.inst, std::forward_like<reference>(*item))) {
                    break;
                }
            }
        }
    }

private:
    [[no_unique_address]] functor_ebo<TFn, TBaseIter> fn;
    bool dropped{};
};

template <typename Tag>
struct drop_last_while_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
        requires is_common_v<TSelf> && is_bidir_v<TSelf>
    constexpr auto drop_last_while(this TSelf&& self, TFn fn) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> {
            return drop_last_while_iter<std::remove_cvref_t<TSelf>, TFn, TMixins...>{ std::forward<TSelf>(self), fn };
        });
    }
};


namespace compo {
template <typename TBaseCompose, typename TFn, template <typename> typename... TMixinsCompose>
struct drop_last_while_compose : public builtin_mixins_compose<TBaseCompose>, public TMixinsCompose<TBaseCompose>... {
    [[no_unique_address]] TBaseCompose base_comp;
    [[no_unique_address]] functor_ebo<TFn, TBaseCompose> fn;

    template <template <typename> typename... TMixins, typename TSelf, kissra::iterator UBaseIter>
    constexpr auto make_iter(this TSelf&& self, UBaseIter&& base_iter) {
        return drop_last_while_iter<std::remove_cvref_t<UBaseIter>, TFn, TMixins...>{
            std::forward<UBaseIter>(base_iter),
            std::forward<TSelf>(self).fn.inst,
        };
    }
};

template <typename Tag>
struct drop_last_while_compose_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    constexpr auto drop_last_while(this TSelf&& self, TFn fn) {
        return with_custom_mixins_compose<DeferInstantiation>([&]<template <typename> typename... TMixinsCompose> {
            return drop_last_while_compose<std::remove_cvref_t<TSelf>, TFn, TMixinsCompose...>{
                .base_comp = std::forward<TSelf>(self),
                .fn = fn,
            };
        });
    }
};

template <typename TFn, typename DeferInstantiation = void>
constexpr auto drop_last_while(TFn fn) {
    return compose<DeferInstantiation>().drop_last_while(fn);
}
} // namespace compo
} // namespace kissra