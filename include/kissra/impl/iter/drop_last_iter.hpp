#pragma once
#include "kissra/impl/compose.hpp"
#include "kissra/impl/iter/iter_base.hpp"

#ifndef KISSRA_MODULE
#include <cstddef>
#include <type_traits>
#include <utility>
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename TBaseIter, template <typename> typename... TMixins>
class drop_last_iter : public iter_base<TBaseIter>, public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using result_t = typename TBaseIter::result_t;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;

    template <typename UBaseIter>
    constexpr drop_last_iter(UBaseIter&& base_iter, std::size_t n)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , n(n) {}

    [[nodiscard]] constexpr result_t next()
        requires is_bidir
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
        requires is_bidir
    {
        this->ff();
        return this->base_iter.nth(n);
    }

    [[nodiscard]] constexpr result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        const auto total = this->n + n;
        this->n = 0;
        return this->base_iter.nth_back(total);
    }

    constexpr std::size_t advance(std::size_t n)
        requires is_bidir
    {
        this->ff();
        return this->base_iter.advance(n);
    }

    constexpr std::size_t advance_back(std::size_t n)
        requires is_bidir
    {
        const auto total = this->n + n;
        this->n = 0;
        return this->base_iter.advance_back(total);
    }

    constexpr auto size() const
        requires is_sized
    {
        const std::size_t base_size = this->base_iter.size();
        return base_size - std::min(base_size, this->n);
    }

private:
    constexpr void ff() {
        if (this->n) {
            this->advance_back(0);
        }
    }

private:
    std::size_t n;
};

template <typename Tag>
struct drop_last_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
        requires is_bidir_v<TSelf>
    constexpr auto drop_last(this TSelf&& self, std::size_t n) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> {
            return drop_last_iter<std::remove_cvref_t<TSelf>, TMixins...>{ std::forward<TSelf>(self), n };
        });
    }
};


namespace compo {
template <typename TBaseCompose, template <typename> typename... TMixinsCompose>
struct drop_last_compose : public builtin_mixins_compose<TBaseCompose>, public TMixinsCompose<TBaseCompose>... {
    [[no_unique_address]] TBaseCompose base_comp;
    std::size_t n;

    template <template <typename> typename... TMixins, typename TSelf, kissra::iterator UBaseIter>
    constexpr auto make_iter(this TSelf&& self, UBaseIter&& base_iter) {
        return drop_last_iter<std::remove_cvref_t<UBaseIter>, TMixins...>{
            std::forward<UBaseIter>(base_iter),
            self.n,
        };
    }
};

template <typename Tag>
struct drop_last_compose_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    constexpr auto drop_last(this TSelf&& self, std::size_t n) {
        return with_custom_mixins_compose<DeferInstantiation>([&]<template <typename> typename... TMixinsCompose> {
            return drop_last_compose<std::remove_cvref_t<TSelf>, TMixinsCompose...>{
                .base_comp = std::forward<TSelf>(self),
                .n = n,
            };
        });
    }
};

template <typename DeferInstantiation = void>
constexpr auto drop_last(std::size_t n) {
    return compose<DeferInstantiation>().drop_last(n);
}
} // namespace compo
} // namespace kissra