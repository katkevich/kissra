#pragma once
#include "kissra/misc/optional.hpp"
#include "kissra/registered_mixins_fwd.hpp"
#include <functional>

namespace kissra {
template <typename TUnderlyingIter, typename TFn, typename TMixins>
    requires std::regular_invocable<TFn, typename TUnderlyingIter::reference>
class filter_iter : public TMixins {
public:
    using value_type = typename TUnderlyingIter::value_type;
    using reference = typename TUnderlyingIter::reference;
    using const_reference = typename TUnderlyingIter::const_reference;

    template <typename TSelf>
    using ref_t = typename TUnderlyingIter::template ref_t<TSelf>;

    template <typename TSelf>
    using result_t = typename TUnderlyingIter::template result_t<TSelf>;

    static constexpr bool is_sized = false;
    static constexpr bool is_common = TUnderlyingIter::is_common;
    static constexpr bool is_forward = TUnderlyingIter::is_forward;
    static constexpr bool is_bidir = TUnderlyingIter::is_bidir;
    static constexpr bool is_random = TUnderlyingIter::is_random;

    template <typename UUnderlyingIter>
    filter_iter(UUnderlyingIter&& underlying_iter, TFn fn)
        : underlying_iter(std::forward<UUnderlyingIter>(underlying_iter))
        , fn(fn) {}

    template <typename TSelf>
    result_t<TSelf> next(this TSelf&& self) {
        while (auto item = self.underlying_iter.next()) {
            if (std::invoke(self.fn, *item)) {
                return item;
            }
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> next_back(this TSelf&& self) {
        while (auto item = self.underlying_iter.next_back()) {
            if (std::invoke(self.fn, *item)) {
                return item;
            }
        }
        return {};
    }

    template <typename TSelf>
    result_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        auto item = self.underlying_iter.front();
        for (; item; item = self.underlying_iter.advance(1)) {
            if (std::invoke(self.fn, *item)) {
                if (n-- == 0) {
                    break;
                }
            }
        }
        return item;
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> advance_back(this TSelf&& self, std::size_t n) {
        auto item = self.underlying_iter.back();
        for (; item; item = self.underlying_iter.advance_back(1)) {
            if (std::invoke(self.fn, *item)) {
                if (n-- == 0) {
                    break;
                }
            }
        }
        return item;
    }

private:
    TUnderlyingIter underlying_iter;
    TFn fn;
};

struct filter_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    auto filter(this TSelf&& self, TFn fn) {
        auto mixins = registered_mixins<DeferInstantiation>();
        return filter_iter<std::remove_cvref_t<TSelf>, TFn, decltype(mixins)>{ std::forward<TSelf>(self), fn };
    }
};
} // namespace kissra