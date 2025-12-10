#pragma once
#include "kissra/iter/iter_base.hpp"
#include <functional>

namespace kissra {

template <typename TBaseIter, typename TMixins>
class chunk : public iter_base<TBaseIter>, public size_mixin, public TMixins {
    template <typename UBaseIter, typename UMixins>
    friend class chunk_iter;
    friend struct size_mixin;

public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using const_reference = typename TBaseIter::const_reference;

    template <typename TSelf>
    using ref_t = typename TBaseIter::template ref_t<TSelf>;

    template <typename TSelf>
    using result_t = typename TBaseIter::template result_t<TSelf>;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;

private:
    template <typename TUnderlyingIt>
    chunk(iter_base<TBaseIter> base_iter, std::ranges::subrange<TUnderlyingIt> subrange)
        : iter_base<TBaseIter>(std::move(base_iter)) {
        this->underlying_subrange_override(subrange);
    }

public:
    template <typename TSelf>
    [[nodiscard]] result_t<TSelf> next(this TSelf&& self) {
        return self.base_iter.next();
    }

    template <typename TSelf>
        requires is_common && is_bidir
    [[nodiscard]] result_t<TSelf> next_back(this TSelf&& self) {
        return self.base_iter.next_back();
    }

    template <typename TSelf>
    [[nodiscard]] result_t<TSelf> nth(this TSelf&& self, std::size_t n) {
        return self.base_iter.nth(n);
    }

    template <typename TSelf>
        requires is_common && is_bidir
    [[nodiscard]] result_t<TSelf> nth_back(this TSelf&& self, std::size_t n) {
        return self.base_iter.nth_back(n);
    }

    template <typename TSelf>
    std::size_t advance(this TSelf&& self, std::size_t n) {
        return self.base_iter.advance(n);
    }

    // TODO: advance_back ain't require is_common in fact
    template <typename TSelf>
        requires is_common && is_bidir
    std::size_t advance_back(this TSelf&& self, std::size_t n) {
        return self.base_iter.advance_back(n);
    }
};

template <typename TBaseIter, typename TMixins>
class chunk_iter : public iter_base<TBaseIter>, public TMixins {
public:
    using value_type = chunk<TBaseIter, TMixins>;
    using reference = value_type;
    using const_reference = value_type;

    template <typename TSelf>
    using ref_t = reference;

    template <typename TSelf>
    using result_t = kissra::optional<ref_t<TSelf>>;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;

    template <typename UBaseIter>
    chunk_iter(UBaseIter&& base_iter, std::size_t n)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , n(n) {}

    template <typename TSelf>
        requires is_forward
    [[nodiscard]] result_t<TSelf> next(this TSelf&& self) {
        /* Fast-forward underlying cursor & sentinel if needed. */
        self.base_iter.advance(0);
        /**
         * Underlying cursor & sentinel now point to "real" iteratable positions.
         * `self.base_iter state is potentially being updated (see `drop_iter` for instance).
         */

        const auto chunk_begin = self.base_iter.underlying_cursor();
        const auto chunk_advancement = self.base_iter.advance(self.n);
        if (chunk_advancement == 0) {
            return {};
        }
        const auto chunk_end = self.base_iter.underlying_cursor();

        return chunk<TBaseIter, TMixins>{ self.base_iter, std::ranges::subrange{ chunk_begin, chunk_end } };
    }

    template <typename TSelf>
        requires is_sized && is_bidir && is_common
    [[nodiscard]] result_t<TSelf> next_back(this TSelf&& self) {
        const auto last_chunk_size = self.base_iter.size() % self.n;
        const auto chunk_size = last_chunk_size ? last_chunk_size : self.n;

        self.base_iter.advance_back(0);
        const auto chunk_end = self.base_iter.underlying_sentinel();
        const auto chunk_advancement = self.base_iter.advance_back(chunk_size);
        if (chunk_advancement == 0) {
            return {};
        }
        const auto chunk_begin = self.base_iter.underlying_sentinel();

        return chunk<TBaseIter, TMixins>{ self.base_iter, std::ranges::subrange{ chunk_begin, chunk_end } };
    }

    template <typename TSelf>
        requires is_forward
    [[nodiscard]] result_t<TSelf> nth(this TSelf&& self, std::size_t n) {
        self.base_iter.advance(n * self.n);

        /* We will revert the underlying sentinel to this state later (to keep `nth_back` semantics correct). */
        const auto underlying_subrange_before = self.base_iter.underlying_subrange();
        const auto chunk_advancement = self.base_iter.advance(self.n);
        if (chunk_advancement == 0) {
            return {};
        }
        const auto chunk_end = self.base_iter.underlying_cursor();

        chunk<TBaseIter, TMixins> nth_chunk{ self.base_iter, std::ranges::subrange{ underlying_subrange_before.begin(), chunk_end } };

        /* Chunk advancement consumed elements (moved the underlying cursor), so we need to restore cursor old position. */
        self.underlying_subrange_override(underlying_subrange_before);

        return nth_chunk;
    }

    template <typename TSelf>
        requires is_sized && is_bidir && is_common
    [[nodiscard]] result_t<TSelf> nth_back(this TSelf&& self, std::size_t n) {
        const auto last_chunk_size = self.base_iter.size() % self.n;
        const auto chunk_size = last_chunk_size ? last_chunk_size : self.n;

        const auto initial_advancement = n > 0 ? chunk_size + (n - 1) * self.n : 0;
        self.base_iter.advance_back(initial_advancement);

        /* We will revert the underlying sentinel to this state later (to keep `nth_back` semantics correct). */
        const auto underlying_subrange_before = self.base_iter.underlying_subrange();
        const auto chunk_advancement = self.base_iter.advance_back(n > 0 ? self.n : chunk_size);
        if (chunk_advancement == 0) {
            return {};
        }
        const auto chunk_begin = self.base_iter.underlying_sentinel();

        chunk<TBaseIter, TMixins> nth_chunk{ self.base_iter,
            std::ranges::subrange{ chunk_begin, underlying_subrange_before.end() } };

        /* Chunk advancement consumed elements (moved the underlying sentinel), so we need to restore sentinel old position. */
        self.underlying_subrange_override(underlying_subrange_before);

        return nth_chunk;
    }

    template <typename TSelf>
    std::size_t advance(this TSelf&& self, std::size_t n) {
        const auto offset = self.base_iter.advance(n * self.n);
        if (offset) {
            return (offset - 1) / self.n + 1;
        }
        return 0;
    }

    template <typename TSelf>
        requires is_sized && is_bidir
    std::size_t advance_back(this TSelf&& self, std::size_t n) {
        const auto last_chunk_size = self.base_iter.size() % self.n;
        const auto chunk_size = last_chunk_size ? last_chunk_size : self.n;

        const auto advancement = n > 0 ? chunk_size + (n - 1) * self.n : 0;
        const auto offset = self.base_iter.advance_back(advancement);
        if (offset) {
            return (offset - 1) / self.n + 1;
        }
        return 0;
    }

    template <typename TSelf>
        requires is_sized
    auto size(this TSelf&& self) {
        const std::size_t base_size = self.base_iter.size();
        return (base_size - 1) / self.n + 1;
    }

private:
    std::size_t n;
};

struct chunk_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto chunk(this TSelf&& self, std::size_t n) {
        auto mixins = registered_mixins<DeferInstantiation>();
        return chunk_iter<std::remove_cvref_t<TSelf>, decltype(mixins)>{ std::forward<TSelf>(self), n };
    }
};
} // namespace kissra