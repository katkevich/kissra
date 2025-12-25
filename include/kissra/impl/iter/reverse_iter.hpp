#pragma once
#include "kissra/impl/compose.hpp"
#include "kissra/impl/iter/iter_base.hpp"

#ifndef KISSRA_MODULE
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#endif

KISSRA_EXPORT()
namespace kissra {
/**
 * The goal of this adapter is not to model the `iterator` but rather store the state so that it can be used to
 * store/override underlying cursor/sentinel. We could in fact model full blown iterator, but it would require to do so
 * for all non-trivial iterators out there: `take`, `zip` to name a few. For now it is not a priority.
 */
template <typename T>
struct reversed_range_iterator {
    T base{};
};

template <typename TBaseIter, template <typename> typename... TMixins>
    requires is_bidir_v<TBaseIter>
class reverse_iter : public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using result_t = typename TBaseIter::result_t;
    using cursor_t = reversed_range_iterator<typename TBaseIter::cursor_t>;
    using sentinel_t = reversed_range_iterator<typename TBaseIter::sentinel_t>;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;
    static constexpr bool is_contiguous = !TBaseIter::is_contiguous;
    static constexpr bool is_monotonic = TBaseIter::is_monotonic;

    template <kissra::not_the_same<reverse_iter> UBaseIter>
    constexpr explicit reverse_iter(UBaseIter&& base_iter)
        : base_iter(std::forward<UBaseIter>(base_iter)) {}

    [[nodiscard]] constexpr result_t next()
        requires is_common
    {
        return this->base_iter.next_back();
    }

    [[nodiscard]] constexpr result_t next_back()
        requires is_common
    {
        return this->base_iter.next();
    }

    [[nodiscard]] constexpr result_t nth(std::size_t n)
        requires is_common
    {
        return this->base_iter.nth_back(n);
    }

    [[nodiscard]] constexpr result_t nth_back(std::size_t n)
        requires is_common
    {
        return this->base_iter.nth(n);
    }

    constexpr std::size_t advance(std::size_t n) {
        return this->base_iter.advance_back(n);
    }

    constexpr std::size_t advance_back(std::size_t n) {
        return this->base_iter.advance(n);
    }

    constexpr auto size() const
        requires is_sized
    {
        return this->base_iter.size();
    }


    constexpr auto underlying_cursor() const
        requires is_common
    {
        return reversed_range_iterator{ this->base_iter.underlying_sentinel() };
    }

    constexpr auto underlying_sentinel() const
        requires is_common
    {
        return reversed_range_iterator{ this->base_iter.underlying_cursor() };
    }

    constexpr void underlying_cursor_override(cursor_t cursor)
        requires is_common
    {
        this->base_iter.underlying_sentinel_override(cursor.base);
    }

    constexpr void underlying_sentinel_override(sentinel_t sentinel)
        requires is_common
    {
        this->base_iter.underlying_cursor_override(sentinel.base);
    }

    constexpr auto& base() {
        return this->base_iter;
    }

private:
    [[no_unique_address]] TBaseIter base_iter;
};

template <typename Tag>
struct reverse_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
        requires is_bidir_v<TSelf>
    constexpr auto reverse(this TSelf&& self) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> {
            return reverse_iter<std::remove_cvref_t<TSelf>, TMixins...>{ std::forward<TSelf>(self) };
        });
    }
};


namespace compo {
template <typename TBaseCompose, template <typename> typename... TMixinsCompose>
struct reverse_compose : public builtin_mixins_compose<TBaseCompose>, public TMixinsCompose<TBaseCompose>... {
    [[no_unique_address]] TBaseCompose base_comp;

    template <template <typename> typename... TMixins, typename TSelf, kissra::iterator UBaseIter>
    constexpr auto make_iter(this TSelf&& self, UBaseIter&& base_iter) {
        return reverse_iter<std::remove_cvref_t<UBaseIter>, TMixins...>{ std::forward<UBaseIter>(base_iter) };
    }
};

template <typename Tag>
struct reverse_compose_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    constexpr auto reverse(this TSelf&& self) {
        return with_custom_mixins_compose<DeferInstantiation>([&]<template <typename> typename... TMixinsCompose> {
            return reverse_compose<std::remove_cvref_t<TSelf>, TMixinsCompose...>{ .base_comp = std::forward<TSelf>(self) };
        });
    }
};

template <typename DeferInstantiation = void>
constexpr auto reverse() {
    return compose<DeferInstantiation>().reverse();
}
} // namespace compo
} // namespace kissra