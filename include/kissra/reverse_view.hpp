#pragma once
#include <functional>

#include "beman/optional/optional.hpp"
#include "kissra/impl/registration_macro.hpp"

namespace kissra {
template <typename TUnderlyingView, typename... TMixins>
class reverse_view : public TMixins... {
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
    reverse_view(UUnderlyingView&& underlying_view)
        : underlying_view(std::forward<UUnderlyingView>(underlying_view)) {}

    template <typename TSelf>
        requires is_common && is_bidir
    next_result_t<TSelf> next(this TSelf&& self) {
        return self.underlying_view.next_tail();
    }

    template <typename TSelf>
    next_result_t<TSelf> next_tail(this TSelf&& self) {
        return self.underlying_view.next();
    }

    template <typename TSelf>
        requires is_common && is_bidir
    void advance(this TSelf&& self, std::size_t n) {
        self.underlying_view.advance_tail(n);
    }

    template <typename TSelf>
    void advance_tail(this TSelf&& self, std::size_t n) {
        self.underlying_view.advance(n);
    }

private:
    TUnderlyingView underlying_view;
};

struct reverse_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto reverse(this TSelf&& self) {
        auto [... mixins] = registered_mixins<DeferInstantiation>();
        return reverse_view<std::remove_cvref_t<TSelf>, decltype(mixins)...>{ std::forward<TSelf>(self) };
    }
};
} // namespace kissra