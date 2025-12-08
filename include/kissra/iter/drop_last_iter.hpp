#pragma once
#include <functional>

#include "kissra/optional.hpp"
#include "kissra/impl/registration_macro.hpp"
#include "kissra/mixin/ssize_mixin.hpp"

namespace kissra {
template <typename TUnderlyingIter, typename... TMixins>
class drop_last_iter : public ssize_mixin, public TMixins... {
public:
    using value_type = typename TUnderlyingIter::value_type;
    using reference = typename TUnderlyingIter::reference;
    using const_reference = typename TUnderlyingIter::const_reference;

    template <typename TSelf>
    using result_t = typename TUnderlyingIter::template result_t<TSelf>;

    static constexpr bool is_sized = TUnderlyingIter::is_sized;
    static constexpr bool is_common = TUnderlyingIter::is_common;
    static constexpr bool is_forward = TUnderlyingIter::is_forward;
    static constexpr bool is_bidir = TUnderlyingIter::is_bidir;
    static constexpr bool is_random = TUnderlyingIter::is_random;

    template <typename UUnderlyingIter>
    drop_last_iter(std::size_t n, UUnderlyingIter&& underlying_iter)
        : curr_n(n)
        , n(n)
        , underlying_iter(std::forward<UUnderlyingIter>(underlying_iter)) {}

    template <typename TSelf>
        requires is_common && is_bidir
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
        const auto total = self.curr_n + n;
        self.curr_n = 0;
        return self.underlying_iter.advance_back(total);
    }

    template <typename TSelf>
        requires is_sized
    auto size(this TSelf&& self) {
        const std::size_t underlying_size = self.underlying_iter.size();
        return underlying_size - std::min(underlying_size, self.curr_n);
    }

    template <typename TSelf>
    result_t<TSelf> front(this TSelf&& self) {
        self.ff();
        return self.underlying_iter.front();
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> back(this TSelf&& self) {
        if (self.curr_n) {
            return self.advance_back(0);
        }
        return self.underlying_iter.back();
    }

private:
    template <typename TSelf>
    void ff(this TSelf&& self) {
        if (self.curr_n) {
            self.advance_back(0);
        }
    }

private:
    std::size_t curr_n;
    std::size_t n;
    TUnderlyingIter underlying_iter;
};

struct drop_last_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto drop_last(this TSelf&& self, std::size_t n) {
        auto [... mixins] = registered_mixins<DeferInstantiation>();
        return drop_last_iter<std::remove_cvref_t<TSelf>, decltype(mixins)...>{ n, std::forward<TSelf>(self) };
    }
};
} // namespace kissra