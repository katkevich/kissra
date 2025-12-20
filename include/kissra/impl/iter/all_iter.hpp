#pragma once
#include "kissra/impl/custom_mixins.hpp"
#include "kissra/impl/into_iter.hpp"
#include "kissra/misc/optional.hpp"

#ifndef KISSRA_MODULE
#include <memory>
#include <ranges>
#endif

KISSRA_EXPORT()
namespace kissra {
template <std::ranges::range TRng, template <typename> typename... TMixins>
class all_iter : public builtin_mixins<TRng>, public TMixins<TRng>... {
public:
    using value_type = std::ranges::range_value_t<TRng>;
    using reference = std::ranges::range_reference_t<TRng>;
    using result_t = kissra::optional<reference>;

    /* We need to be able to evaluate the size from iterators. Having member `size` is not enough in a general case. */
    static constexpr bool is_sized = std::ranges::random_access_range<TRng>;
    static constexpr bool is_common = std::ranges::common_range<TRng>;
    static constexpr bool is_forward = std::ranges::forward_range<TRng>;
    static constexpr bool is_bidir = std::ranges::bidirectional_range<TRng>;
    static constexpr bool is_random = std::ranges::random_access_range<TRng>;

    constexpr explicit all_iter(TRng& rng)
        : cursor(std::ranges::begin(rng))
        , sentinel(std::ranges::end(rng)) {}

    [[nodiscard]] constexpr result_t next() {
        if (this->cursor != this->sentinel) {
            return *this->cursor++;
        }
        return {};
    }

    [[nodiscard]] constexpr result_t next_back()
        requires is_common && is_bidir
    {
        if (this->cursor != this->sentinel) {
            return *--this->sentinel;
        }
        return {};
    }

    [[nodiscard]] constexpr result_t nth(std::size_t n) {
        this->advance(n);

        if (this->cursor != this->sentinel) {
            return *this->cursor;
        }
        return {};
    }

    [[nodiscard]] constexpr result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        this->advance_back(n);

        if (this->cursor != this->sentinel) {
            auto sentinel_copy = this->sentinel;
            return *--sentinel_copy;
        }
        return {};
    }

    constexpr std::size_t advance(std::size_t n)
        requires is_random
    {
        const auto offset = std::min(n, std::size_t(this->sentinel - this->cursor));
        this->cursor += offset;
        return offset;
    }

    constexpr std::size_t advance(std::size_t n) {
        std::size_t offset = 0;
        while (offset != n && this->cursor != this->sentinel) {
            ++this->cursor;
            ++offset;
        }
        return offset;
    }

    constexpr std::size_t advance_back(std::size_t n)
        requires is_random
    {
        const auto offset = std::min(n, std::size_t(this->sentinel - this->cursor));
        this->sentinel -= offset;
        return offset;
    }

    constexpr std::size_t advance_back(std::size_t n)
        requires(is_bidir && !is_random)
    {
        std::size_t offset = 0;
        while (offset != n && this->cursor != this->sentinel) {
            --this->sentinel;
            ++offset;
        }
        return offset;
    }

    constexpr auto size() const
        requires is_sized
    {
        return std::size_t(this->sentinel - this->cursor);
    }

    constexpr auto underlying_subrange() const {
        return std::ranges::subrange{ this->cursor, this->sentinel };
    }

    constexpr auto underlying_cursor() const {
        return this->cursor;
    }

    constexpr auto underlying_sentinel() const {
        return this->sentinel;
    }

    template <typename TIt>
    constexpr void underlying_subrange_override(std::ranges::subrange<TIt> subrange) {
        this->cursor = subrange.begin();
        this->sentinel = subrange.end();
    }

private:
    std::ranges::iterator_t<TRng> cursor{};
    std::ranges::sentinel_t<TRng> sentinel{};
};

template <std::ranges::range TRng, typename DeferInstantiation = void>
constexpr auto all(TRng& rng) {
    return impl::into_kissra_iter<DeferInstantiation>(rng);
}
} // namespace kissra