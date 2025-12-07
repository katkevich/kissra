#pragma once
#include <functional>

#include "kissra/optional.hpp"
#include "kissra/impl/registration_macro.hpp"

namespace kissra {
template <typename TFn, typename TUnderlyingIter, typename... TMixins>
class drop_while_iter : public TMixins... {
public:
    using value_type = typename TUnderlyingIter::value_type;
    using reference = typename TUnderlyingIter::reference;
    using const_reference = typename TUnderlyingIter::const_reference;

    template <typename TSelf>
    using ref_t = typename TUnderlyingIter::template ref_t<TSelf>;

    static constexpr bool is_sized = false;
    static constexpr bool is_common = TUnderlyingIter::is_common;
    static constexpr bool is_forward = TUnderlyingIter::is_forward;
    static constexpr bool is_bidir = TUnderlyingIter::is_bidir;
    static constexpr bool is_random = TUnderlyingIter::is_random;

    template <typename UUnderlyingIter>
    drop_while_iter(TFn fn, UUnderlyingIter&& underlying_iter)
        : fn(fn)
        , underlying_iter(std::forward<UUnderlyingIter>(underlying_iter)) {}

    template <typename TSelf>
    ref_t<TSelf> next(this TSelf&& self) {
        self.ff();
        return self.underlying_iter.next();
    }

    template <typename TSelf>
        requires is_common && is_bidir
    ref_t<TSelf> next_back(this TSelf&& self) {
        self.ff();
        return self.underlying_iter.next_back();
    }

    template <typename TSelf>
    ref_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        if (!std::exchange(self.dropped, true)) {
            for (auto item = self.underlying_iter.front(); item; item = self.underlying_iter.advance(1)) {
                if (!std::invoke(self.fn, *item)) {
                    break;
                }
            }
        }

        return self.underlying_iter.advance(n);
    }

    template <typename TSelf>
        requires is_common && is_bidir
    ref_t<TSelf> advance_back(this TSelf&& self, std::size_t n) {
        self.ff();
        return self.underlying_iter.advance_back(n);
    }

    template <typename TSelf>
    ref_t<TSelf> front(this TSelf&& self) {
        if (!self.dropped) {
            return self.advance(0);
        }
        return self.underlying_iter.front();
    }

    template <typename TSelf>
        requires is_common && is_bidir
    ref_t<TSelf> back(this TSelf&& self) {
        self.ff();
        return self.underlying_iter.back();
    }

private:
    template <typename TSelf>
    void ff(this TSelf&& self) {
        if (!self.dropped) {
            self.advance(0);
        }
    }

private:
    TFn fn;
    bool dropped{};
    TUnderlyingIter underlying_iter;
};

struct drop_while_mixin {
    template <typename TSelf, typename TFn, typename DeferInstantiation = void>
    auto drop_while(this TSelf&& self, TFn fn) {
        auto [... mixins] = registered_mixins<DeferInstantiation>();
        return drop_while_iter<TFn, std::remove_cvref_t<TSelf>, decltype(mixins)...>{ fn, std::forward<TSelf>(self) };
    }
};
} // namespace kissra