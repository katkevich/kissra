#pragma once
#include "kissra/iter/iter_base.hpp"
#include <functional>

namespace kissra {
template <typename TBaseIter, typename TFn, typename TMixins>
    requires std::regular_invocable<TFn, typename TBaseIter::reference>
class drop_last_while_iter : public iter_base<TBaseIter>, public TMixins {
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
    drop_last_while_iter(UBaseIter&& base_iter, TFn fn)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , fn(fn) {}

    template <typename TSelf>
    [[nodiscard]] result_t<TSelf> next(this TSelf&& self) {
        self.ff();
        return self.base_iter.next();
    }

    template <typename TSelf>
        requires is_common && is_bidir
    [[nodiscard]] result_t<TSelf> next_back(this TSelf&& self) {
        self.ff();
        return self.base_iter.next_back();
    }

    template <typename TSelf>
    [[nodiscard]] result_t<TSelf> nth(this TSelf&& self, std::size_t n) {
        self.ff();
        return self.base_iter.nth(n);
    }

    template <typename TSelf>
        requires is_common && is_bidir
    [[nodiscard]] result_t<TSelf> nth_back(this TSelf&& self, std::size_t n) {
        self.ff_self();
        return self.base_iter.nth_back(n);
    }

    template <typename TSelf>
    std::size_t advance(this TSelf&& self, std::size_t n) {
        self.ff();
        return self.base_iter.advance(n);
    }

    template <typename TSelf>
        requires is_common && is_bidir
    std::size_t advance_back(this TSelf&& self, std::size_t n) {
        self.ff_self();
        return self.base_iter.advance_back(n);
    }

private:
    template <typename TSelf>
    void ff(this TSelf&& self) {
        if (!self.dropped) {
            self.advance_back(0);
        }
    }

    template <typename TSelf>
    void ff_self(this TSelf&& self) {
        if (!std::exchange(self.dropped, true)) {
            for (auto item = self.base_iter.back(); item; item = self.base_iter.nth_back(1)) {
                if (!std::invoke(self.fn, *item)) {
                    break;
                }
            }
        }
    }

private:
    TFn fn;
    bool dropped{};
};

struct drop_last_while_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    auto drop_last_while(this TSelf&& self, TFn fn) {
        auto mixins = registered_mixins<DeferInstantiation>();
        return drop_last_while_iter<std::remove_cvref_t<TSelf>, TFn, decltype(mixins)>{ std::forward<TSelf>(self), fn };
    }
};
} // namespace kissra