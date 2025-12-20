#pragma once
#include "kissra/compose.hpp"
#include "kissra/iter/iter_base.hpp"
#include "kissra/misc/functional.hpp"
#include <functional>

namespace kissra {
template <typename TBaseIter, typename TFn, template <typename> typename... TMixins>
    requires kissra::regular_invocable<TFn, typename TBaseIter::reference>
class drop_while_iter : public iter_base<TBaseIter>, public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
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
    constexpr drop_while_iter(UBaseIter&& base_iter, TFn fn)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , fn(fn) {}

    [[nodiscard]] constexpr result_t next() {
        this->ff();
        return this->base_iter.next();
    }

    [[nodiscard]] constexpr result_t next_back()
        requires is_common && is_bidir
    {
        this->ff();
        return this->base_iter.next_back();
    }

    [[nodiscard]] constexpr result_t nth(std::size_t n) {
        this->ff_self();
        return this->base_iter.nth(n);
    }

    [[nodiscard]] constexpr result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        this->ff();
        return this->base_iter.nth_back(n);
    }

    constexpr std::size_t advance(std::size_t n) {
        this->ff_self();
        return this->base_iter.advance(n);
    }

    constexpr std::size_t advance_back(std::size_t n)
        requires is_common && is_bidir
    {
        this->ff();
        return this->base_iter.advance_back(n);
    }

private:
    constexpr void ff() {
        if (!this->dropped) {
            this->advance(0);
        }
    }

    constexpr void ff_self() {
        if (!std::exchange(this->dropped, true)) {
            for (auto item = this->base_iter.front(); item; item = this->base_iter.nth(1)) {
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
struct drop_while_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    constexpr auto drop_while(this TSelf&& self, TFn fn) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... CustomMixins> {
            return drop_while_iter<std::remove_cvref_t<TSelf>, TFn, CustomMixins...>{ std::forward<TSelf>(self), fn };
        });
    }
};


namespace compo {
template <typename TBaseCompose, typename TFn, template <typename> typename... TMixins>
struct drop_while_compose : public builtin_mixins<TBaseCompose>, public TMixins<TBaseCompose>... {
    [[no_unique_address]] TBaseCompose base_comp;
    [[no_unique_address]] functor_ebo<TFn, TBaseCompose> fn;

    template <typename TSelf, template <typename> typename... UMixins, kissra::iterator UBaseIter>
    constexpr auto make_iter(this TSelf&& self, UBaseIter&& base_iter) {
        return drop_while_iter<std::remove_cvref_t<UBaseIter>, TFn, UMixins...>{
            std::forward<UBaseIter>(base_iter),
            std::forward<TSelf>(self).fn.inst,
        };
    }
};

template <typename Tag>
struct drop_while_compose_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    constexpr auto drop_while(this TSelf&& self, TFn fn) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... CustomMixins> {
            return drop_while_compose<std::remove_cvref_t<TSelf>, TFn, CustomMixins...>{
                .base_comp = std::forward<TSelf>(self),
                .fn = fn,
            };
        });
    }
};

template <typename TFn, typename DeferInstantiation = void>
constexpr auto drop_while(TFn fn) {
    return compose<DeferInstantiation>().drop_while(fn);
}
} // namespace compo
} // namespace kissra