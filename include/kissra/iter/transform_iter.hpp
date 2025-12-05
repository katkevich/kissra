#pragma once
#include "beman/optional/optional.hpp"
#include "kissra/impl/registration_macro.hpp"
#include <algorithm>

namespace kissra {
template <typename TFn, typename TUnderlyingIter, typename... TMixins>
class transform_iter : public TMixins... {
public:
    using value_type = std::invoke_result_t<TFn, typename TUnderlyingIter::reference>;
    using reference = std::invoke_result_t<TFn, typename TUnderlyingIter::reference>;
    using const_reference = std::invoke_result_t<TFn, typename TUnderlyingIter::const_reference>;

    template <typename TSelf>
    using ref_t = beman::optional::optional<std::conditional_t< //
        std::is_const_v<std::remove_reference_t<TSelf>>,
        const_reference,
        reference>>;

    static constexpr bool is_sized = TUnderlyingIter::is_sized;
    static constexpr bool is_common = TUnderlyingIter::is_common;
    static constexpr bool is_forward = TUnderlyingIter::is_forward;
    static constexpr bool is_bidir = TUnderlyingIter::is_bidir;
    static constexpr bool is_random = TUnderlyingIter::is_random;

    template <typename UUnderlyingIter>
    transform_iter(TFn fn, UUnderlyingIter&& underlying_iter)
        : fn(fn)
        , underlying_iter(std::forward<UUnderlyingIter>(underlying_iter)) {}

    template <typename TSelf>
    ref_t<TSelf> next(this TSelf&& self) {
        if (auto item = self.underlying_iter.next()) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    ref_t<TSelf> next_back(this TSelf&& self) {
        if (auto item = self.underlying_iter.next_back()) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
    ref_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        if (auto item = self.underlying_iter.advance(n)) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    ref_t<TSelf> advance_back(this TSelf&& self, std::size_t n) {
        if (auto item = self.underlying_iter.advance_back(n)) {
            return std::invoke(self.fn, *item);
        }
        return;
    }

    template <typename TSelf>
    ref_t<TSelf> front(this TSelf&& self) {
        if (auto item = self.underlying_iter.front()) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    ref_t<TSelf> back(this TSelf&& self) {
        if (auto item = self.underlying_iter.back()) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

private:
    TFn fn;
    TUnderlyingIter underlying_iter;
};

struct transform_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    auto transform(this TSelf&& self, TFn fn) {
        auto [... mixins] = registered_mixins<DeferInstantiation>();
        return transform_iter<TFn, std::remove_cvref_t<TSelf>, decltype(mixins)...>{ fn, std::forward<TSelf>(self) };
    }
};
} // namespace kissra