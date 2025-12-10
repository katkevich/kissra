#pragma once
#include "kissra/misc/optional.hpp"
#include "kissra/registered_mixins_fwd.hpp"
#include <memory>
#include <ranges>

namespace kissra {
template <std::ranges::range TContainer, typename TMixins>
class all_iter : public TMixins {
public:
    using value_type = typename TContainer::value_type;
    using reference = typename TContainer::reference;
    using const_reference = typename TContainer::const_reference;

    template <typename TSelf>
    using ref_t = std::conditional_t<std::is_const_v<std::remove_reference_t<TSelf>>, const_reference, reference>;

    template <typename TSelf>
    using result_t = kissra::optional<ref_t<TSelf>>;

    /* We need to be able to evaluate the size from iterators. Having member `size` is not enough in a general case. */
    static constexpr bool is_sized = std::ranges::random_access_range<TContainer>;
    static constexpr bool is_common = std::ranges::common_range<TContainer>;
    static constexpr bool is_forward = std::ranges::forward_range<TContainer>;
    static constexpr bool is_bidir = std::ranges::bidirectional_range<TContainer>;
    static constexpr bool is_random = std::ranges::random_access_range<TContainer>;

    all_iter(TContainer& container)
        : container(std::addressof(container))
        , cursor(std::ranges::begin(container))
        , sentinel(std::ranges::end(container)) {}

    template <typename TSelf>
    [[nodiscard]] result_t<TSelf> next(this TSelf&& self) {
        if (self.cursor != self.sentinel) {
            return *self.cursor++;
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    [[nodiscard]] result_t<TSelf> next_back(this TSelf&& self) {
        if (self.cursor != self.sentinel) {
            return *--self.sentinel;
        }
        return {};
    }

    template <typename TSelf>
        requires is_random
    [[nodiscard]] result_t<TSelf> nth(this TSelf&& self, std::size_t n) {
        self.advance(n);

        if (self.cursor != self.sentinel) {
            return *self.cursor;
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    [[nodiscard]] result_t<TSelf> nth_back(this TSelf&& self, std::size_t n) {
        self.advance_back(n);

        if (self.cursor != self.sentinel) {
            auto sentinel_copy = self.sentinel;
            return *--sentinel_copy;
        }
        return {};
    }

    template <typename TSelf>
        requires is_random
    std::size_t advance(this TSelf&& self, std::size_t n) {
        const auto offset = std::min(n, std::size_t(self.sentinel - self.cursor));
        self.cursor += offset;
        return offset;
    }

    template <typename TSelf>
    std::size_t advance(this TSelf&& self, std::size_t n) {
        std::size_t offset = 0;
        while (offset != n && self.cursor != self.sentinel) {
            ++self.cursor;
            ++offset;
        }
        return offset;
    }

    template <typename TSelf>
        requires is_random
    std::size_t advance_back(this TSelf&& self, std::size_t n) {
        const auto offset = std::min(n, std::size_t(self.sentinel - self.cursor));
        self.sentinel -= offset;
        return offset;
    }

    template <typename TSelf>
        requires(is_common && is_bidir && !is_random)
    std::size_t advance_back(this TSelf&& self, std::size_t n) {
        std::size_t offset = 0;
        while (offset != n && self.cursor != self.sentinel) {
            --self.sentinel;
            ++offset;
        }
        return offset;
    }

    template <typename TSelf>
        requires is_sized
    auto size(this TSelf&& self) {
        return std::size_t(self.sentinel - self.cursor);
    }

    template <typename TSelf>
    auto underlying_subrange(this TSelf&& self) {
        return std::ranges::subrange{ self.cursor, self.sentinel };
    }

    template <typename TSelf>
    auto underlying_cursor(this TSelf&& self) {
        return self.cursor;
    }

    template <typename TSelf>
    auto underlying_sentinel(this TSelf&& self) {
        return self.sentinel;
    }

    template <typename TSelf, typename TIt>
    void underlying_subrange_override(this TSelf&& self, std::ranges::subrange<TIt> subrange) {
        self.cursor = subrange.begin();
        self.sentinel = subrange.end();
    }

private:
    TContainer* container{};
    std::ranges::iterator_t<TContainer> cursor{};
    std::ranges::sentinel_t<TContainer> sentinel{};
};

template <typename TContainer, typename DeferInstantiation = void>
auto all(TContainer& container) {
    auto mixins = registered_mixins<DeferInstantiation>();
    return all_iter<TContainer, decltype(mixins)>{ container };
}
} // namespace kissra