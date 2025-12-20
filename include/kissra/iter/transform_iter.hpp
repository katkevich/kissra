#pragma once
#include "kissra/iter/iter_base.hpp"
#include <algorithm>
#include <type_traits>

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
    transform_iter(UBaseIter&& base_iter, TFn fn)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , fn(fn) {}

    [[nodiscard]] result_t next() {
        if (auto item = this->base_iter.next()) {
            return kissra::invoke(this->fn.inst, std::forward_like<base_reference>(*item));
        }
        return {};
    }

    [[nodiscard]] result_t next_back()
        requires is_common && is_bidir
    {
        if (auto item = this->base_iter.next_back()) {
            return kissra::invoke(this->fn.inst, std::forward_like<base_reference>(*item));
        }
        return {};
    }

    [[nodiscard]] result_t nth(std::size_t n) {
        if (auto item = this->base_iter.nth(n)) {
            return kissra::invoke(this->fn.inst, std::forward_like<base_reference>(*item));
        }
        return {};
    }

    [[nodiscard]] result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        if (auto item = this->base_iter.nth_back(n)) {
            return kissra::invoke(this->fn.inst, std::forward_like<base_reference>(*item));
        }
        return {};
    }

    std::size_t advance(std::size_t n) {
        return this->base_iter.advance(n);
    }

    std::size_t advance_back(std::size_t n)
        requires is_bidir
    {
        return this->base_iter.advance_back(n);
    }

    auto size() const
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
    auto transform(this TSelf&& self, TFn fn) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... CustomMixins> {
            return transform_iter<std::remove_cvref_t<TSelf>, TFn, CustomMixins...>{ std::forward<TSelf>(self), fn };
        });
    }
};

namespace compo {
template <typename TBaseCompose, typename TFn, template <typename> typename... TMixins>
struct transform_compose : public builtin_mixins<TBaseCompose>, public TMixins<TBaseCompose>... {
    [[no_unique_address]] TBaseCompose base_comp;
    [[no_unique_address]] functor_ebo<TFn, TBaseCompose> fn;

    template <typename TSelf, template <typename> typename... UMixins, kissra::iterator UBaseIter>
    auto make_iter(this TSelf&& self, UBaseIter&& base_iter) {
        return transform_iter<std::remove_cvref_t<UBaseIter>, TFn, UMixins...>{
            std::forward<UBaseIter>(base_iter),
            std::forward<TSelf>(self).fn.inst,
        };
    }
};

template <typename Tag>
struct transform_compose_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    auto transform(this TSelf&& self, TFn fn) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... CustomMixins> {
            return transform_compose<std::remove_cvref_t<TSelf>, TFn, CustomMixins...>{
                .base_comp = std::forward<TSelf>(self),
                .fn = fn,
            };
        });
    }
};

template <typename TFn, typename DeferInstantiation = void>
auto transform(TFn fn) {
    return compose<DeferInstantiation>().transform(fn);
}
} // namespace compo
} // namespace kissra