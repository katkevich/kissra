#pragma once
#include "kissra/misc/optional.hpp"
#include "kissra/registered_mixins_fwd.hpp"
#include <functional>

namespace kissra {
template <typename TBaseIter, typename TFn, typename TMixins>
    requires std::regular_invocable<TFn, typename TBaseIter::reference>
class filter_iter : public TMixins {
public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using const_reference = typename TBaseIter::const_reference;

    template <typename TSelf>
    using ref_t = typename TBaseIter::template ref_t<TSelf>;

    template <typename TSelf>
    using result_t = typename TBaseIter::template result_t<TSelf>;

    static constexpr bool is_sized = false;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = false;

    template <typename UBaseIter>
    filter_iter(UBaseIter&& base_iter, TFn fn)
        : base_iter(std::forward<UBaseIter>(base_iter))
        , fn(fn) {}

    template <typename TSelf>
    result_t<TSelf> next(this TSelf&& self) {
        while (auto item = self.base_iter.next()) {
            if (std::invoke(self.fn, *item)) {
                return item;
            }
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> next_back(this TSelf&& self) {
        while (auto item = self.base_iter.next_back()) {
            if (std::invoke(self.fn, *item)) {
                return item;
            }
        }
        return {};
    }

    template <typename TSelf>
    result_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        auto item = self.base_iter.front();
        for (; item; item = self.base_iter.advance(1)) {
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
        auto item = self.base_iter.back();
        for (; item; item = self.base_iter.advance_back(1)) {
            if (std::invoke(self.fn, *item)) {
                if (n-- == 0) {
                    break;
                }
            }
        }
        return item;
    }

private:
    TBaseIter base_iter;
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