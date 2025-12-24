#pragma once
#include "kissra/impl/compose.hpp"
#include "kissra/impl/iter/all_iter.hpp"
#include "kissra/impl/iter/iter_base.hpp"

#ifndef KISSRA_MODULE
#include <cstddef>
#include <type_traits>
#include <utility>
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename TBaseIter, template <typename> typename... TMixins>
class take_iter : public iter_base<TBaseIter>, public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using result_t = typename TBaseIter::result_t;
    using underlying_cursor_t = typename TBaseIter::underlying_cursor_t;
    using underlying_sentinel_t = typename TBaseIter::underlying_sentinel_t;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    /**
     * In order to evaluate the "end" we would need to traverse the whole sequence which is bad,
     * hence we only do that if underlying iterator is random-access.
     */
    static constexpr bool is_bidir = false;
    static constexpr bool is_random = false;

    template <typename UBaseIter>
    constexpr take_iter(UBaseIter&& base_iter, std::size_t n)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , n(n) {}

    [[nodiscard]] constexpr result_t next() {
        if (n != 0) {
            --n;
            return this->base_iter.next();
        }
        return {};
    }

    [[nodiscard]] constexpr result_t nth(std::size_t n) {
        auto advancement = std::min(n, this->n);
        this->n -= advancement;
        return this->base_iter.nth(advancement);
    }

    constexpr std::size_t advance(std::size_t n) {
        auto advancement = std::min(n, this->n);
        this->n -= advancement;
        return this->base_iter.advance(advancement);
    }

    constexpr auto size() const
        requires is_sized
    {
        return std::min(this->base_iter.size(), this->n);
    }

private:
    std::size_t n;
};


template <typename TBaseIter, template <typename> typename... TMixins>
    requires is_random_v<TBaseIter>
class take_iter<TBaseIter, TMixins...>
    : public iter_base<TBaseIter>, public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using result_t = typename TBaseIter::result_t;
    using underlying_cursor_t = typename TBaseIter::underlying_cursor_t;
    using underlying_sentinel_t = typename TBaseIter::underlying_sentinel_t;

    static constexpr bool is_sized = true;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = true;
    static constexpr bool is_bidir = true;
    static constexpr bool is_random = true;

    template <typename UBaseIter>
    constexpr take_iter(UBaseIter&& base_iter, std::size_t n)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter)) {
        this->base_iter.advance(0);

        const auto size_min = std::min(this->base_iter.size(), n);
        const auto cursor = this->base_iter.underlying_cursor();

        this->base_iter.underlying_subrange_override(std::ranges::subrange{ cursor, cursor + size_min });
    }

    [[nodiscard]] constexpr result_t next() {
        return this->base_iter.next();
    }

    [[nodiscard]] constexpr result_t next_back()
        requires is_common
    {
        return this->base_iter.next_back();
    }

    [[nodiscard]] constexpr result_t nth(std::size_t n) {
        return this->base_iter.nth(n);
    }

    [[nodiscard]] constexpr result_t nth_back(std::size_t n)
        requires is_common
    {
        return this->base_iter.nth_back(n);
    }

    constexpr std::size_t advance(std::size_t n) {
        return this->base_iter.advance(n);
    }

    constexpr std::size_t advance_back(std::size_t n) {
        return this->base_iter.advance_back(n);
    }

    constexpr auto size() const {
        return this->base_iter.size();
    }
};


template <typename Tag>
struct take_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    constexpr auto take(this TSelf&& self, std::size_t n) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> {
            return take_iter<std::remove_cvref_t<TSelf>, TMixins...>{ std::forward<TSelf>(self), n };
        });
    }
};

template <kissra::iterator_compatible T, typename DeferInstantiation = void>
constexpr auto take(T&& rng_or_kissra_iter, std::size_t n) {
    return impl::into_kissra_iter<DeferInstantiation>(KISSRA_FWD(rng_or_kissra_iter)).take(n);
}


namespace compo {
template <typename TBaseCompose, template <typename> typename... TMixinsCompose>
struct take_compose : public builtin_mixins_compose<TBaseCompose>, public TMixinsCompose<TBaseCompose>... {
    [[no_unique_address]] TBaseCompose base_comp;
    std::size_t n;

    template <template <typename> typename... TMixins, typename TSelf, kissra::iterator UBaseIter>
    constexpr auto make_iter(this TSelf&& self, UBaseIter&& base_iter) {
        return take_iter<std::remove_cvref_t<UBaseIter>, TMixins...>{
            std::forward<UBaseIter>(base_iter),
            self.n,
        };
    }
};

template <typename Tag>
struct take_compose_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    constexpr auto take(this TSelf&& self, std::size_t n) {
        return with_custom_mixins_compose<DeferInstantiation>([&]<template <typename> typename... TMixinsCompose> {
            return take_compose<std::remove_cvref_t<TSelf>, TMixinsCompose...>{
                .base_comp = std::forward<TSelf>(self),
                .n = n,
            };
        });
    }
};

template <typename DeferInstantiation = void>
constexpr auto take(std::size_t n) {
    return compose<DeferInstantiation>().take(n);
}
} // namespace compo
} // namespace kissra