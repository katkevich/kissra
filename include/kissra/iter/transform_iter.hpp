#pragma once
#include "kissra/algo/size_mixin.hpp"
#include "kissra/misc/optional.hpp"
#include "kissra/registered_mixins_fwd.hpp"
#include <algorithm>
#include <type_traits>

namespace kissra {
template <typename TUnderlyingIter, typename TFn, typename TMixins>
    requires std::regular_invocable<TFn, typename TUnderlyingIter::reference>
class transform_iter : public size_mixin, public TMixins {
    friend class size_mixin;

public:
    using value_type = std::invoke_result_t<TFn, typename TUnderlyingIter::reference>;
    using reference = value_type;
    using const_reference = value_type;

    template <typename TSelf>
    using ref_t = reference;

    template <typename TSelf>
    using result_t = kissra::optional<ref_t<TSelf>>;

    static constexpr bool is_sized = TUnderlyingIter::is_sized;
    static constexpr bool is_common = TUnderlyingIter::is_common;
    static constexpr bool is_forward = TUnderlyingIter::is_forward;
    static constexpr bool is_bidir = TUnderlyingIter::is_bidir;
    static constexpr bool is_random = TUnderlyingIter::is_random;

    template <typename UUnderlyingIter>
    transform_iter(UUnderlyingIter&& underlying_iter, TFn fn)
        : underlying_iter(std::forward<UUnderlyingIter>(underlying_iter))
        , fn(fn) {}

    template <typename TSelf>
    result_t<TSelf> next(this TSelf&& self) {
        if (auto item = self.underlying_iter.next()) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> next_back(this TSelf&& self) {
        if (auto item = self.underlying_iter.next_back()) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
    result_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        if (auto item = self.underlying_iter.advance(n)) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> advance_back(this TSelf&& self, std::size_t n) {
        if (auto item = self.underlying_iter.advance_back(n)) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
    result_t<TSelf> front(this TSelf&& self) {
        if (auto item = self.underlying_iter.front()) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> back(this TSelf&& self) {
        if (auto item = self.underlying_iter.back()) {
            return std::invoke(self.fn, *item);
        }
        return {};
    }

private:
    TUnderlyingIter underlying_iter;
    TFn fn;
};

struct transform_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    auto transform(this TSelf&& self, TFn fn) {
        auto mixins = registered_mixins<DeferInstantiation>();
        return transform_iter<std::remove_cvref_t<TSelf>, TFn, decltype(mixins)>{ std::forward<TSelf>(self), fn };
    }
};
} // namespace kissra