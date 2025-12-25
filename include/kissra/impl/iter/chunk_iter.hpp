#pragma once
#include "kissra/impl/compose.hpp"
#include "kissra/impl/iter/iter_base.hpp"

#ifndef KISSRA_MODULE
#include <cstddef>
#include <ranges>
#include <type_traits>
#include <utility>
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename TBaseIter, template <typename> typename... TMixins>
    requires is_monotonic_v<TBaseIter> && is_common_v<TBaseIter>
class chunk : public iter_base<TBaseIter>, public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
    template <typename UBaseIter, template <typename> typename... UMixins>
        requires is_monotonic_v<UBaseIter>
    friend class chunk_iter;

public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using result_t = typename TBaseIter::result_t;
    using cursor_t = typename TBaseIter::cursor_t;
    using sentinel_t = typename TBaseIter::sentinel_t;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = true;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;
    static constexpr bool is_contiguous = TBaseIter::is_contiguous;
    static constexpr bool is_monotonic = true;

private:
    constexpr chunk(TBaseIter base_iter, cursor_t cursor, cursor_t sentinel)
        : iter_base<TBaseIter>(std::move(base_iter)) {
        this->base_iter.underlying_cursor_override(cursor);
        this->base_iter.underlying_sentinel_override(sentinel);
    }

public:
    [[nodiscard]] constexpr result_t next() {
        return this->base_iter.next();
    }

    [[nodiscard]] constexpr result_t next_back()
        requires is_bidir
    {
        return this->base_iter.next_back();
    }

    [[nodiscard]] constexpr result_t nth(std::size_t n) {
        return this->base_iter.nth(n);
    }

    [[nodiscard]] constexpr result_t nth_back(std::size_t n)
        requires is_bidir
    {
        return this->base_iter.nth_back(n);
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
};

template <typename TBaseIter, template <typename> typename... TMixins>
    requires is_monotonic_v<TBaseIter>
class chunk_iter : public iter_base<TBaseIter>, public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
public:
    using value_type = chunk<TBaseIter, TMixins...>;
    using reference = value_type;
    using result_t = kissra::optional<reference>;
    using cursor_t = typename TBaseIter::cursor_t;
    using sentinel_t = typename TBaseIter::sentinel_t;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;
    static constexpr bool is_contiguous = false;
    static constexpr bool is_monotonic = true;

    template <typename UBaseIter>
    constexpr chunk_iter(UBaseIter&& base_iter, std::size_t n)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , n(n) {}

    [[nodiscard]] constexpr result_t next()
        requires is_forward && is_common
    {
        /**
         * Fast-forward underlying cursor & sentinel if needed as well as update `base_iter` inner state.
         * We want to make sure that after `advance` it is safe to use raw underlying cursor & sentinel.
         */
        this->base_iter.advance(0);
        const auto chunk_begin = this->base_iter.underlying_cursor();
        const auto chunk_advancement = this->base_iter.advance(this->n);
        if (chunk_advancement == 0) {
            return {};
        }
        const auto chunk_end = this->base_iter.underlying_cursor();

        return reference{ this->base_iter, chunk_begin, chunk_end };
    }

    [[nodiscard]] constexpr result_t next_back()
        requires is_sized && is_bidir && is_common
    {
        const auto last_chunk_size = this->base_iter.size() % this->n;
        const auto chunk_size = last_chunk_size ? last_chunk_size : this->n;

        this->base_iter.advance_back(0);
        const auto chunk_end = this->base_iter.underlying_sentinel();
        const auto chunk_advancement = this->base_iter.advance_back(chunk_size);
        if (chunk_advancement == 0) {
            return {};
        }
        const auto chunk_begin = this->base_iter.underlying_sentinel();

        return reference{ this->base_iter, chunk_begin, chunk_end };
    }

    [[nodiscard]] constexpr result_t nth(std::size_t n)
        requires is_forward && is_common
    {
        this->base_iter.advance(n * this->n);

        /* We will revert the underlying sentinel to this state later (to keep `nth_back` semantics correct). */
        const auto chunk_begin = this->base_iter.underlying_cursor();
        const auto chunk_advancement = this->base_iter.advance(this->n);
        if (chunk_advancement == 0) {
            return {};
        }
        const auto chunk_end = this->base_iter.underlying_cursor();

        reference nth_chunk{ this->base_iter, chunk_begin, chunk_end };

        /* Chunk advancement consumed elements (moved the underlying cursor), so we need to restore cursor old position. */
        this->underlying_cursor_override(chunk_begin);

        return nth_chunk;
    }

    [[nodiscard]] constexpr result_t nth_back(std::size_t n)
        requires is_sized && is_bidir && is_common
    {
        const auto last_chunk_size = this->base_iter.size() % this->n;
        const auto chunk_size = last_chunk_size ? last_chunk_size : this->n;

        const auto initial_advancement = n > 0 ? chunk_size + (n - 1) * this->n : 0;
        this->base_iter.advance_back(initial_advancement);

        /* We will revert the underlying sentinel to this state later (to keep `nth_back` semantics correct). */
        const auto chunk_end = this->base_iter.underlying_sentinel();
        const auto chunk_advancement = this->base_iter.advance_back(n > 0 ? this->n : chunk_size);
        if (chunk_advancement == 0) {
            return {};
        }
        const auto chunk_begin = this->base_iter.underlying_sentinel();

        reference nth_chunk{ this->base_iter, chunk_begin, chunk_end };

        /* Chunk advancement consumed elements (moved the underlying sentinel), so we need to restore sentinel old position. */
        this->underlying_sentinel_override(chunk_end);

        return nth_chunk;
    }

    constexpr std::size_t advance(std::size_t n) {
        const auto offset = this->base_iter.advance(n * this->n);
        if (offset) {
            return (offset - 1) / this->n + 1;
        }
        return 0;
    }

    constexpr std::size_t advance_back(std::size_t n)
        requires is_sized && is_bidir
    {
        const auto last_chunk_size = this->base_iter.size() % this->n;
        const auto chunk_size = last_chunk_size ? last_chunk_size : this->n;

        const auto advancement = n > 0 ? chunk_size + (n - 1) * this->n : 0;
        const auto offset = this->base_iter.advance_back(advancement);
        if (offset) {
            return (offset - 1) / this->n + 1;
        }
        return 0;
    }

    constexpr auto size() const
        requires is_sized
    {
        const std::size_t base_size = this->base_iter.size();
        return (base_size - 1) / this->n + 1;
    }

private:
    std::size_t n;
};

template <typename Tag>
struct chunk_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    constexpr auto chunk(this TSelf&& self, std::size_t n) {
        return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... TMixins> {
            return chunk_iter<std::remove_cvref_t<TSelf>, TMixins...>{ std::forward<TSelf>(self), n };
        });
    }
};


namespace compo {
template <typename TBaseCompose, template <typename> typename... TMixinsCompose>
struct chunk_compose : public builtin_mixins_compose<TBaseCompose>, public TMixinsCompose<TBaseCompose>... {
    [[no_unique_address]] TBaseCompose base_comp;
    std::size_t n;

    template <template <typename> typename... TMixins, typename TSelf, kissra::iterator UBaseIter>
    constexpr auto make_iter(this TSelf&& self, UBaseIter&& base_iter) {
        return chunk_iter<std::remove_cvref_t<UBaseIter>, TMixins...>{
            std::forward<UBaseIter>(base_iter),
            self.n,
        };
    }
};

template <typename Tag>
struct chunk_compose_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    constexpr auto chunk(this TSelf&& self, std::size_t n) {
        return with_custom_mixins_compose<DeferInstantiation>([&]<template <typename> typename... TMixinsCompose> {
            return chunk_compose<std::remove_cvref_t<TSelf>, TMixinsCompose...>{
                .base_comp = std::forward<TSelf>(self),
                .n = n,
            };
        });
    }
};

template <typename DeferInstantiation = void>
constexpr auto chunk(std::size_t n) {
    return compose<DeferInstantiation>().chunk(n);
}
} // namespace compo
} // namespace kissra