#pragma once
#include <functional>

#include "beman/optional/optional.hpp"
#include "kissra/impl/registration_macro.hpp"

namespace kissra {
template <typename TFn, typename TUnderlyingView, typename... TMixins>
class filter_view : public TMixins... {
public:
    using value_type = typename TUnderlyingView::value_type;
    using reference = typename TUnderlyingView::reference;
    using const_reference = typename TUnderlyingView::const_reference;

    template <typename TSelf>
    using next_result_t = typename TUnderlyingView::template next_result_t<TSelf>;

    static constexpr bool is_sized = false;
    static constexpr bool is_common = TUnderlyingView::is_common;
    static constexpr bool is_forward = TUnderlyingView::is_forward;
    static constexpr bool is_bidir = TUnderlyingView::is_bidir;
    static constexpr bool is_random = TUnderlyingView::is_random;

    template <typename UUnderlyingView>
    filter_view(TFn fn, UUnderlyingView&& underlying_view)
        : fn(fn)
        , underlying_view(std::forward<UUnderlyingView>(underlying_view)) {}

    template <typename TSelf>
    next_result_t<TSelf> next(this TSelf&& self) {
        while (auto item = self.underlying_view.next()) {
            if (std::invoke(self.fn, *item)) {
                return item;
            }
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    next_result_t<TSelf> next_tail(this TSelf&& self) {
        while (auto item = self.underlying_view.next_tail()) {
            if (std::invoke(self.fn, *item)) {
                return item;
            }
        }
        return {};
    }

    template <typename TSelf>
    void advance(this TSelf&& self, std::size_t n) {
        while (auto item = self.underlying_view.next()) {
            if (std::invoke(self.fn, *item) && --n == 0) {
                return;
            }
        }
    }

    template <typename TSelf>
        requires is_common && is_bidir
    void advance_tail(this TSelf&& self, std::size_t n) {
        while (auto item = self.underlying_view.next_tail()) {
            if (std::invoke(self.fn, *item) && --n == 0) {
                return;
            }
        }
    }

private:
    TFn fn;
    TUnderlyingView underlying_view;
};

struct filter_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    auto filter(this TSelf&& self, TFn fn) {
        auto [... mixins] = registered_mixins<DeferInstantiation>();
        return filter_view<TFn, std::remove_cvref_t<TSelf>, decltype(mixins)...>{ fn, std::forward<TSelf>(self) };
    }
};
} // namespace kissra