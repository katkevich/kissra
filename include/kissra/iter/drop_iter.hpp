#pragma once
#include <functional>

#include "beman/optional/optional.hpp"
#include "kissra/impl/registration_macro.hpp"

namespace kissra {
template <typename TUnderlyingIter, typename... TMixins>
class drop_iter : public TMixins... {
public:
    using value_type = typename TUnderlyingIter::value_type;
    using reference = typename TUnderlyingIter::reference;
    using const_reference = typename TUnderlyingIter::const_reference;

    template <typename TSelf>
    using ref_t = typename TUnderlyingIter::template ref_t<TSelf>;

    static constexpr bool is_sized = TUnderlyingIter::is_sized;
    static constexpr bool is_common = TUnderlyingIter::is_common;
    static constexpr bool is_forward = TUnderlyingIter::is_forward;
    static constexpr bool is_bidir = TUnderlyingIter::is_bidir;
    static constexpr bool is_random = TUnderlyingIter::is_random;

    template <typename UUnderlyingIter>
    drop_iter(std::size_t n, UUnderlyingIter&& underlying_iter)
        : curr_n(n)
        , n(n)
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
        const auto total = self.curr_n + n;
        self.curr_n = 0;
        return self.underlying_iter.advance(total);
    }

    template <typename TSelf>
        requires is_common && is_bidir
    ref_t<TSelf> advance_back(this TSelf&& self, std::size_t n) {
        self.ff();
        return self.underlying_iter.advance_back(n);
    }

    template <typename TSelf>
    ref_t<TSelf> front(this TSelf&& self) {
        if (self.curr_n) {
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
        if (self.curr_n) {
            self.advance(0);
        }
    }

private:
    std::size_t curr_n;
    std::size_t n;
    TUnderlyingIter underlying_iter;
};

struct drop_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto drop(this TSelf&& self, std::size_t n) {
        auto [... mixins] = registered_mixins<DeferInstantiation>();
        return drop_iter<std::remove_cvref_t<TSelf>, decltype(mixins)...>{ n, std::forward<TSelf>(self) };
    }
};
} // namespace kissra