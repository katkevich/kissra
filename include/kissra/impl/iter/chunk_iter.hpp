#pragma once
#include "kissra/compose.hpp"
#include "kissra/impl/iter/iter_base.hpp"
#include <functional>

namespace kissra {

template <typename TBaseIter, template <typename> typename... TMixins>
class chunk : public iter_base<TBaseIter>, public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
    template <typename UBaseIter, template <typename> typename... UMixins>
    friend class chunk_iter;

public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using result_t = typename TBaseIter::result_t;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;

private:
    template <typename TUnderlyingIt>
    constexpr chunk(iter_base<TBaseIter> base_iter, std::ranges::subrange<TUnderlyingIt> subrange)
        : iter_base<TBaseIter>(std::move(base_iter)) {
        this->underlying_subrange_override(subrange);
    }

public:
    [[nodiscard]] constexpr result_t next() {
        return this->base_iter.next();
    }

    [[nodiscard]] constexpr result_t next_back()
        requires is_common && is_bidir
    {
        return this->base_iter.next_back();
    }

    [[nodiscard]] constexpr result_t nth(std::size_t n) {
        return this->base_iter.nth(n);
    }

    [[nodiscard]] constexpr result_t nth_back(std::size_t n)
        requires is_common && is_bidir
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
class chunk_iter : public iter_base<TBaseIter>, public builtin_mixins<TBaseIter>, public TMixins<TBaseIter>... {
public:
    using value_type = chunk<TBaseIter, TMixins...>;
    using reference = value_type;
    using result_t = kissra::optional<reference>;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;

    template <typename UBaseIter>
    constexpr chunk_iter(UBaseIter&& base_iter, std::size_t n)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , n(n) {}

    [[nodiscard]] constexpr result_t next()
        requires is_forward
    {
        /* Fast-forward underlying cursor & sentinel if needed. */
        this->base_iter.advance(0);
        /**
         * Underlying cursor & sentinel now point to "real" iteratable positions.
         * `this->base_iter` state is potentially being updated (see `drop_iter` for instance).
         */

        const auto chunk_begin = this->base_iter.underlying_cursor();
        const auto chunk_advancement = this->base_iter.advance(this->n);
        if (chunk_advancement == 0) {
            return {};
        }
        const auto chunk_end = this->base_iter.underlying_cursor();

        return reference{ this->base_iter, std::ranges::subrange{ chunk_begin, chunk_end } };
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

        return reference{ this->base_iter, std::ranges::subrange{ chunk_begin, chunk_end } };
    }

    [[nodiscard]] constexpr result_t nth(std::size_t n)
        requires is_forward
    {
        this->base_iter.advance(n * this->n);

        /* We will revert the underlying sentinel to this state later (to keep `nth_back` semantics correct). */
        const auto underlying_subrange_before = this->base_iter.underlying_subrange();
        const auto chunk_advancement = this->base_iter.advance(this->n);
        if (chunk_advancement == 0) {
            return {};
        }
        const auto chunk_end = this->base_iter.underlying_cursor();

        reference nth_chunk{ this->base_iter, std::ranges::subrange{ underlying_subrange_before.begin(), chunk_end } };

        /* Chunk advancement consumed elements (moved the underlying cursor), so we need to restore cursor old position. */
        this->underlying_subrange_override(underlying_subrange_before);

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
        const auto underlying_subrange_before = this->base_iter.underlying_subrange();
        const auto chunk_advancement = this->base_iter.advance_back(n > 0 ? this->n : chunk_size);
        if (chunk_advancement == 0) {
            return {};
        }
        const auto chunk_begin = this->base_iter.underlying_sentinel();

        reference nth_chunk{ this->base_iter, std::ranges::subrange{ chunk_begin, underlying_subrange_before.end() } };

        /* Chunk advancement consumed elements (moved the underlying sentinel), so we need to restore sentinel old position. */
        this->underlying_subrange_override(underlying_subrange_before);

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
        requires is_forward_v<TSelf>
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