#pragma once
#include <functional>

#include "beman/optional/optional.hpp"
#include "kissra/impl/registration_macro.hpp"
#include "kissra/mixin/advance_mixin.hpp"

namespace kissra {
template <typename TUnderlyingView, typename... TMixins>
class drop_view : public TMixins... {
public:
    using value_type = typename TUnderlyingView::value_type;
    using reference = typename TUnderlyingView::reference;
    using const_reference = typename TUnderlyingView::const_reference;

    template <typename TSelf>
    using next_result_t = typename TUnderlyingView::template next_result_t<TSelf>;

    static constexpr bool is_sized = TUnderlyingView::is_sized;
    static constexpr bool is_common = TUnderlyingView::is_common;
    static constexpr bool is_forward = TUnderlyingView::is_forward;
    static constexpr bool is_bidir = TUnderlyingView::is_bidir;
    static constexpr bool is_random = TUnderlyingView::is_random;

    template <typename UUnderlyingView>
    drop_view(std::size_t n, UUnderlyingView&& underlying_view)
        : curr_n(n)
        , n(n)
        , underlying_view(std::forward<UUnderlyingView>(underlying_view)) {}

    template <typename TSelf>
    next_result_t<TSelf> next(this TSelf&& self) {
        if (self.curr_n == 0) {
            return self.underlying_view.next();
        } else {
            self.curr_n = 0;
            return self.advance(self.n);
        }
    }

    template <typename TSelf>
        requires is_common && is_random
    next_result_t<TSelf> reverse_next(this TSelf&& self) {
        // while (auto item = self.underlying_view.reverse_next()) {
        //     if (std::invoke(self.fn, *item)) {
        //         return item;
        //     }
        // }
        return {};
    }

    template <typename TSelf>
    next_result_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        const auto total = self.curr_n + n;
        self.curr_n = 0;
        return self.underlying_view.advance(total);
    }

private:
    std::size_t curr_n;
    std::size_t n;
    TUnderlyingView underlying_view;
};

struct drop_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto drop(this TSelf&& self, std::size_t n) {
        auto [... mixins] = registered_mixins<DeferInstantiation>();
        return drop_view<std::remove_cvref_t<TSelf>, decltype(mixins)...>{ n, std::forward<TSelf>(self) };
    }
};
} // namespace kissra