#pragma once
#include "kissra/compose.hpp"
#include "kissra/iter/iter_base.hpp"
#include <functional>

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
    drop_last_iter(UBaseIter&& base_iter, std::size_t n)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , n(n) {}

    [[nodiscard]] result_t next()
        requires is_bidir
    {
        this->ff();
        return this->base_iter.next();
    }

    [[nodiscard]] result_t next_back()
        requires is_common && is_bidir
    {
        this->ff();
        return this->base_iter.next_back();
    }

    [[nodiscard]] result_t nth(std::size_t n)
        requires is_bidir
    {
        this->ff();
        return this->base_iter.nth(n);
    }

    [[nodiscard]] result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        const auto total = this->n + n;
        this->n = 0;
        return this->base_iter.nth_back(total);
    }

    std::size_t advance(std::size_t n)
        requires is_bidir
    {
        this->ff();
        return this->base_iter.advance(n);
    }

    std::size_t advance_back(std::size_t n)
        requires is_bidir
    {
        const auto total = this->n + n;
        this->n = 0;
        return this->base_iter.advance_back(total);
    }

    auto size() const
        requires is_sized
    {
        const std::size_t base_size = this->base_iter.size();
        return base_size - std::min(base_size, this->n);
    }

private:
    void ff() {
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
    auto drop_last(this TSelf&& self, std::size_t n) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... CustomMixins> {
            return drop_last_iter<std::remove_cvref_t<TSelf>, CustomMixins...>{ std::forward<TSelf>(self), n };
        });
    }
};


namespace compo {
template <typename TBaseCompose, template <typename> typename... TMixins>
struct drop_last_compose : public builtin_mixins<TBaseCompose>, public TMixins<TBaseCompose>... {
    [[no_unique_address]] TBaseCompose base_comp;
    std::size_t n;

    template <typename TSelf, template <typename> typename... UMixins, kissra::iterator UBaseIter>
    auto make_iter(this TSelf&& self, UBaseIter&& base_iter) {
        return drop_last_iter<std::remove_cvref_t<UBaseIter>, UMixins...>{
            std::forward<UBaseIter>(base_iter),
            self.n,
        };
    }
};

template <typename Tag>
struct drop_last_compose_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto drop_last(this TSelf&& self, std::size_t n) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... CustomMixins> {
            return drop_last_compose<std::remove_cvref_t<TSelf>, CustomMixins...>{
                .base_comp = std::forward<TSelf>(self),
                .n = n,
            };
        });
    }
};

template <typename DeferInstantiation = void>
auto drop_last(std::size_t n) {
    return compose<DeferInstantiation>().drop_last(n);
}
} // namespace compo
} // namespace kissra