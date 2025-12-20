#pragma once
#include "kissra/compose.hpp"
#include "kissra/iter/iter_base.hpp"
#include "kissra/misc/functional.hpp"

namespace kissra {
template <typename TBaseIter, typename TFn, template <typename> typename... TMixins>
    requires kissra::regular_invocable<TFn, typename TBaseIter::reference>
// TODO: MSVC EBO is broken. Test MSVC specific intrinsics to fix that
class filter_iter : public iter_base<TBaseIter>, public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
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
    constexpr filter_iter(UBaseIter&& base_iter, TFn fn)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , fn(fn) {}

    [[nodiscard]] constexpr result_t next() {
        while (auto item = this->base_iter.next()) {
            if (kissra::invoke(this->fn.inst, std::forward_like<reference>(*item))) {
                return item;
            }
        }
        return {};
    }

    [[nodiscard]] constexpr result_t next_back()
        requires is_common && is_bidir
    {
        while (auto item = this->base_iter.next_back()) {
            if (kissra::invoke(this->fn.inst, std::forward_like<reference>(*item))) {
                return item;
            }
        }
        return {};
    }

    [[nodiscard]] constexpr result_t nth(std::size_t n) {
        for (auto item = this->base_iter.front(); item; item = this->base_iter.nth(1)) {
            if (kissra::invoke(this->fn.inst, std::forward_like<reference>(*item))) {
                if (n-- == 0) {
                    return item;
                }
            }
        }
        return {};
    }

    [[nodiscard]] constexpr result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        for (auto item = this->base_iter.back(); item; item = this->base_iter.nth_back(1)) {
            if (kissra::invoke(this->fn.inst, std::forward_like<reference>(*item))) {
                if (n-- == 0) {
                    return item;
                }
            }
        }
        return {};
    }

    constexpr std::size_t advance(std::size_t n) {
        std::size_t offset = 0;
        for (auto item = this->base_iter.front(); item; item = this->base_iter.nth(1)) {
            if (kissra::invoke(this->fn.inst, std::forward_like<reference>(*item))) {
                if (offset++ == n) {
                    break;
                }
            }
        }
        return offset;
    }

    constexpr std::size_t advance_back(std::size_t n)
        requires is_common && is_bidir
    {
        std::size_t offset = 0;
        for (auto item = this->base_iter.back(); item; item = this->base_iter.nth_back(1)) {
            if (kissra::invoke(this->fn.inst, std::forward_like<reference>(*item))) {
                if (offset++ == n) {
                    break;
                }
            }
        }
        return offset;
    }

private:
    // TODO: MSVC [[no_unique_address]] (EBO basically) is broken. Test MSVC specific intrinsics (iirc there is msvc specific attribute as well) to fix that
    [[no_unique_address]] functor_ebo<TFn, TBaseIter> fn;
};

template <typename Tag>
struct filter_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    constexpr auto filter(this TSelf&& self, TFn fn) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... CustomMixins> {
            return filter_iter<std::remove_cvref_t<TSelf>, TFn, CustomMixins...>{ std::forward<TSelf>(self), fn };
        });
    }
};


namespace compo {
template <typename TBaseCompose, typename TFn, template <typename> typename... TMixins>
struct filter_compose : public builtin_mixins<TBaseCompose>, public TMixins<TBaseCompose>... {
    [[no_unique_address]] TBaseCompose base_comp;
    [[no_unique_address]] functor_ebo<TFn, TBaseCompose> fn;

    template <typename TSelf, template <typename> typename... UMixins, kissra::iterator UBaseIter>
    constexpr auto make_iter(this TSelf&& self, UBaseIter&& base_iter) {
        return filter_iter<std::remove_cvref_t<UBaseIter>, TFn, UMixins...>{
            std::forward<UBaseIter>(base_iter),
            std::forward<TSelf>(self).fn.inst,
        };
    }
};

template <typename Tag>
struct filter_compose_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    constexpr auto filter(this TSelf&& self, TFn fn) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... CustomMixins> {
            return filter_compose<std::remove_cvref_t<TSelf>, TFn, CustomMixins...>{
                .base_comp = std::forward<TSelf>(self),
                .fn = fn,
            };
        });
    }
};

template <typename TFn, typename DeferInstantiation = void>
constexpr auto filter(TFn fn) {
    return compose<DeferInstantiation>().filter(fn);
}
} // namespace compo
} // namespace kissra