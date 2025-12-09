#pragma once
#include "kissra/misc/optional.hpp"
#include "kissra/registered_mixins_fwd.hpp"
#include <functional>

namespace kissra {
template <typename TBaseIter, typename TMixins>
class drop_iter : public TMixins {
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
    drop_iter(UBaseIter&& base_iter, std::size_t n)
        : base_iter(std::forward<UBaseIter>(base_iter))
        , curr_n(n)
        , n(n) {}

    template <typename TSelf>
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
    result_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        const auto total = self.curr_n + n;
        self.curr_n = 0;
        return self.base_iter.advance(total);
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> advance_back(this TSelf&& self, std::size_t n) {
        self.ff();
        return self.base_iter.advance_back(n);
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
            self.advance(0);
        }
    }

private:
    TBaseIter base_iter;
    std::size_t curr_n;
    std::size_t n;
};

struct drop_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto drop(this TSelf&& self, std::size_t n) {
        auto mixins = registered_mixins<DeferInstantiation>();
        return drop_iter<std::remove_cvref_t<TSelf>, decltype(mixins)>{ std::forward<TSelf>(self), n };
    }
};
} // namespace kissra