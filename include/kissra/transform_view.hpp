#pragma once
#include "beman/optional/optional.hpp"
#include "kissra/impl/registration_macro.hpp"
#include <algorithm>

namespace kissra {
template <typename TFn, typename TUnderlyingView, typename... TMixins>
class transform_view : public TMixins... {
public:
    using value_type = std::invoke_result_t<TFn, typename TUnderlyingView::reference>;
    using reference = std::invoke_result_t<TFn, typename TUnderlyingView::reference>;
    using const_reference = std::invoke_result_t<TFn, typename TUnderlyingView::const_reference>;

    template <typename TSelf>
    using ref_t = beman::optional::optional<std::conditional_t< //
        std::is_const_v<std::remove_reference_t<TSelf>>,
        const_reference,
        reference>>;

    static constexpr bool is_sized = TUnderlyingView::is_sized;
    static constexpr bool is_common = TUnderlyingView::is_common;
    static constexpr bool is_forward = TUnderlyingView::is_forward;
    static constexpr bool is_bidir = TUnderlyingView::is_bidir;
    static constexpr bool is_random = TUnderlyingView::is_random;

    template <typename UUnderlyingView>
    transform_view(TFn fn, UUnderlyingView&& underlying_view)
        : fn(fn)
        , underlying_view(std::forward<UUnderlyingView>(underlying_view)) {}

    template <typename TSelf>
    ref_t<TSelf> next(this TSelf&& self) {
        if (auto item = self.underlying_view.next()) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    ref_t<TSelf> next_back(this TSelf&& self) {
        if (auto item = self.underlying_view.next_back()) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
    ref_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        if (auto item = self.underlying_view.advance(n)) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    ref_t<TSelf> advance_back(this TSelf&& self, std::size_t n) {
        if (auto item = self.underlying_view.advance_back(n)) {
            return std::invoke(self.fn, *item);
        }
        return;
    }

    template <typename TSelf>
    ref_t<TSelf> front(this TSelf&& self) {
        if (auto item = self.underlying_view.front()) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    ref_t<TSelf> back(this TSelf&& self) {
        if (auto item = self.underlying_view.back()) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

private:
    TFn fn;
    TUnderlyingView underlying_view;
};

struct transform_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    auto transform(this TSelf&& self, TFn fn) {
        auto [... mixins] = registered_mixins<DeferInstantiation>();
        return transform_view<TFn, std::remove_cvref_t<TSelf>, decltype(mixins)...>{ fn, std::forward<TSelf>(self) };
    }
};
} // namespace kissra