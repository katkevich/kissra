#pragma once
#include "kissra/iter/iter_base.hpp"
#include <functional>

namespace kissra {
template <typename TBaseIter, typename TMixins>
class drop_last_iter : public iter_base<TBaseIter>, public TMixins {
public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using const_reference = typename TBaseIter::const_reference;

    template <typename TSelf>
    using ref_t = typename TBaseIter::template ref_t<TSelf>;

    template <typename TSelf>
    using result_t = typename TBaseIter::template result_t<TSelf>;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;

    template <typename UBaseIter>
    drop_last_iter(UBaseIter&& base_iter, std::size_t n)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter))
        , curr_n(n)
        , n(n) {}

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> next(this TSelf&& self) {
        self.ff();
        return self.base_iter.next();
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> next_back(this TSelf&& self) {
        self.ff();
        return self.base_iter.next_back();
    }

    template <typename TSelf>
    result_t<TSelf> nth(this TSelf&& self, std::size_t n) {
        self.ff();
        return self.base_iter.nth(n);
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> nth_back(this TSelf&& self, std::size_t n) {
        const auto total = self.curr_n + n;
        self.curr_n = 0;
        return self.base_iter.nth_back(total);
    }

    template <typename TSelf>
        requires is_sized
    auto size(this TSelf&& self) {
        const std::size_t base_size = self.base_iter.size();
        return base_size - std::min(base_size, self.curr_n);
    }

private:
    template <typename TSelf>
    void ff(this TSelf&& self) {
        if (self.curr_n) {
            self.nth_back(0);
        }
    }

private:
    std::size_t curr_n;
    std::size_t n;
};

struct drop_last_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto drop_last(this TSelf&& self, std::size_t n) {
        auto mixins = registered_mixins<DeferInstantiation>();
        return drop_last_iter<std::remove_cvref_t<TSelf>, decltype(mixins)>{ std::forward<TSelf>(self), n };
    }
};
} // namespace kissra