#pragma once
#include "kissra/misc/optional.hpp"
#include "kissra/registered_mixins_fwd.hpp"
#include <functional>

namespace kissra {
template <typename TUnderlyingIter, typename TFn, typename TMixins>
    requires std::regular_invocable<TFn, typename TUnderlyingIter::reference>
class drop_last_while_iter : public TMixins {
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
    static constexpr bool is_random = false;

    template <typename UUnderlyingIter>
    drop_last_while_iter(UUnderlyingIter&& underlying_iter, TFn fn)
        : underlying_iter(std::forward<UUnderlyingIter>(underlying_iter))
        , fn(fn) {}

    template <typename TSelf>
    result_t<TSelf> next(this TSelf&& self) {
        self.ff();
        return self.underlying_iter.next();
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> next_back(this TSelf&& self) {
        self.ff();
        return self.underlying_iter.next_back();
    }

    template <typename TSelf>
    result_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        self.ff();
        return self.underlying_iter.advance(n);
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> advance_back(this TSelf&& self, std::size_t n) {
        if (!std::exchange(self.dropped, true)) {
            for (auto item = self.underlying_iter.back(); item; item = self.underlying_iter.advance_back(1)) {
                if (!std::invoke(self.fn, *item)) {
                    break;
                }
            }
        }
        return self.underlying_iter.advance_back(n);
    }

private:
    template <typename TSelf>
    void ff(this TSelf&& self) {
        if (!self.dropped) {
            self.advance_back(0);
        }
    }

private:
    TUnderlyingIter underlying_iter;
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