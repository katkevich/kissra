#pragma once
#include "kissra/iter/iter_base.hpp"
#include <functional>

namespace kissra {
template <typename TBaseIter, typename TFn, typename TMixins>
    requires std::regular_invocable<TFn, typename TBaseIter::reference>
class filter_iter : public iter_base<TBaseIter>, public TMixins {
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
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , fn(fn) {}

    template <typename TSelf>
    [[nodiscard]] result_t<TSelf> next(this TSelf&& self) {
        while (auto item = self.base_iter.next()) {
            if (std::invoke(self.fn, *item)) {
                return item;
            }
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    [[nodiscard]] result_t<TSelf> next_back(this TSelf&& self) {
        while (auto item = self.base_iter.next_back()) {
            if (std::invoke(self.fn, *item)) {
                return item;
            }
        }
        return {};
    }

    template <typename TSelf>
    [[nodiscard]] result_t<TSelf> nth(this TSelf&& self, std::size_t n) {
        for (auto item = self.base_iter.front(); item; item = self.base_iter.nth(1)) {
            if (std::invoke(self.fn, *item)) {
                if (n-- == 0) {
                    return item;
                }
            }
        }
        return {};
    }

    template <typename TSelf>
        requires is_common && is_bidir
    [[nodiscard]] result_t<TSelf> nth_back(this TSelf&& self, std::size_t n) {
        for (auto item = self.base_iter.back(); item; item = self.base_iter.nth_back(1)) {
            if (std::invoke(self.fn, *item)) {
                if (n-- == 0) {
                    return item;
                }
            }
        }
        return {};
    }

    template <typename TSelf>
    std::size_t advance(this TSelf&& self, std::size_t n) {
        std::size_t offset = 0;
        for (auto item = self.base_iter.front(); item; item = self.base_iter.nth(1)) {
            if (std::invoke(self.fn, *item)) {
                if (offset++ == n) {
                    break;
                }
            }
        }
        return offset;
    }

    template <typename TSelf>
        requires is_common && is_bidir
    std::size_t advance_back(this TSelf&& self, std::size_t n) {
        std::size_t offset = 0;
        for (auto item = self.base_iter.back(); item; item = self.base_iter.nth_back(1)) {
            if (std::invoke(self.fn, *item)) {
                if (offset++ == n) {
                    break;
                }
            }
        }
        return offset;
    }

private:
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