#pragma once
#include "beman/optional/optional.hpp"
#include "kissra/impl/registration_macro.hpp"
#include <memory>
#include <ranges>

namespace kissra {
template <std::ranges::range TContainer, typename... TMixins>
class all_view : public TMixins... {
public:
    using value_type = typename TContainer::value_type;
    using reference = typename TContainer::reference;
    using const_reference = typename TContainer::const_reference;

    template <typename TSelf>
    using next_result_t = beman::optional::optional<std::conditional_t< //
        std::is_const_v<std::remove_reference_t<TSelf>>,
        const_reference,
        reference>>;

    static constexpr bool is_sized = std::ranges::sized_range<TContainer>;
    static constexpr bool is_common = std::ranges::common_range<TContainer>;
    static constexpr bool is_forward = std::ranges::forward_range<TContainer>;
    static constexpr bool is_bidir = std::ranges::bidirectional_range<TContainer>;
    static constexpr bool is_random = std::ranges::random_access_range<TContainer>;

    all_view(TContainer& container)
        : container(std::addressof(container))
        , cursor(std::ranges::begin(container))
        , sentinel(std::ranges::end(container)) {}

    template <typename TSelf>
    next_result_t<TSelf> next(this TSelf&& self) {
        if (self.cursor != self.sentinel) {
            return *self.cursor++;
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    next_result_t<TSelf> reverse_next(this TSelf&& self) {
        if (self.cursor != self.sentinel) {
            return *--self.sentinel;
        }
        return {};
    }

    template <typename TSelf>
        requires is_random
    next_result_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        self.cursor += std::min(n, std::size_t(std::distance(self.cursor, self.sentinel)));
        if (self.cursor != self.sentinel) {
            return *self.cursor++;
        } else {
            return {};
        }
    }

    template <typename TSelf>
    next_result_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        while (self.cursor != self.sentinel) {
            if (n-- == 0) {
                return *self.cursor;
            }
            ++self.cursor;
        }
        return {};
    }

private:
    TContainer* container{};
    std::ranges::iterator_t<TContainer> cursor{};
    std::ranges::sentinel_t<TContainer> sentinel{};
};

template <typename TContainer, typename DeferInstantiation = void>
auto all(TContainer& container) {
    auto [... mixins] = registered_mixins<DeferInstantiation>();
    return all_view<TContainer, decltype(mixins)...>{ container };
}
} // namespace kissra