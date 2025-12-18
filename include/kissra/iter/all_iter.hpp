#pragma once
#include "kissra/impl/custom_mixins.hpp"
#include "kissra/misc/optional.hpp"
#include <memory>
#include <ranges>

namespace kissra {
template <std::ranges::range TContainer, template <typename> typename... TMixins>
class all_iter : public builtin_mixins<TContainer>, public TMixins<TContainer>... {
public:
    using value_type = typename TContainer::value_type;
    using reference = typename TContainer::reference;
    using result_t = kissra::optional<reference>;

    /* We need to be able to evaluate the size from iterators. Having member `size` is not enough in a general case. */
    static constexpr bool is_sized = std::ranges::random_access_range<TContainer>;
    static constexpr bool is_common = std::ranges::common_range<TContainer>;
    static constexpr bool is_forward = std::ranges::forward_range<TContainer>;
    static constexpr bool is_bidir = std::ranges::bidirectional_range<TContainer>;
    static constexpr bool is_random = std::ranges::random_access_range<TContainer>;

    explicit all_iter(TContainer& container)
        : cursor(std::ranges::begin(container))
        , sentinel(std::ranges::end(container)) {}

    [[nodiscard]] result_t next() {
        if (this->cursor != this->sentinel) {
            return *this->cursor++;
        }
        return {};
    }

    [[nodiscard]] result_t next_back()
        requires is_common && is_bidir
    {
        if (this->cursor != this->sentinel) {
            return *--this->sentinel;
        }
        return {};
    }

    [[nodiscard]] result_t nth(std::size_t n) {
        this->advance(n);

        if (this->cursor != this->sentinel) {
            return *this->cursor;
        }
        return {};
    }

    [[nodiscard]] result_t nth_back(std::size_t n)
        requires is_common && is_bidir
    {
        this->advance_back(n);

        if (this->cursor != this->sentinel) {
            auto sentinel_copy = this->sentinel;
            return *--sentinel_copy;
        }
        return {};
    }

    std::size_t advance(std::size_t n)
        requires is_random
    {
        const auto offset = std::min(n, std::size_t(this->sentinel - this->cursor));
        this->cursor += offset;
        return offset;
    }

    std::size_t advance(std::size_t n) {
        std::size_t offset = 0;
        while (offset != n && this->cursor != this->sentinel) {
            ++this->cursor;
            ++offset;
        }
        return offset;
    }

    std::size_t advance_back(std::size_t n)
        requires is_random
    {
        const auto offset = std::min(n, std::size_t(this->sentinel - this->cursor));
        this->sentinel -= offset;
        return offset;
    }

    std::size_t advance_back(std::size_t n)
        requires(is_bidir && !is_random)
    {
        std::size_t offset = 0;
        while (offset != n && this->cursor != this->sentinel) {
            --this->sentinel;
            ++offset;
        }
        return offset;
    }

    auto size() const
        requires is_sized
    {
        return std::size_t(this->sentinel - this->cursor);
    }

    auto underlying_subrange() const {
        return std::ranges::subrange{ this->cursor, this->sentinel };
    }

    auto underlying_cursor() const {
        return this->cursor;
    }

    auto underlying_sentinel() const {
        return this->sentinel;
    }

    template <typename TIt>
    void underlying_subrange_override(std::ranges::subrange<TIt> subrange) {
        this->cursor = subrange.begin();
        this->sentinel = subrange.end();
    }

private:
    std::ranges::iterator_t<TContainer> cursor{};
    std::ranges::sentinel_t<TContainer> sentinel{};
};

template <std::ranges::range TContainer, typename DeferInstantiation = void>
auto all(TContainer& container) {
    return with_custom_mixins<DeferInstantiation>([&]<template <typename> typename... CustomMixins> { //
        return all_iter<TContainer, CustomMixins...>{ container };
    });
}
} // namespace kissra