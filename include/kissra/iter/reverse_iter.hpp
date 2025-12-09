#pragma once
#include "kissra/iter/iter_base.hpp"
#include "kissra/algo/size_mixin.hpp"
#include <functional>

namespace kissra {
template <typename TBaseIter, typename TMixins>
class reverse_iter : public iter_base<TBaseIter>, public size_mixin, public TMixins {
    friend struct size_mixin;

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
    reverse_iter(UBaseIter&& base_iter)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter)) {}

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> next(this TSelf&& self) {
        return self.base_iter.next_back();
    }

    template <typename TSelf>
    result_t<TSelf> next_back(this TSelf&& self) {
        return self.base_iter.next();
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> nth(this TSelf&& self, std::size_t n) {
        return self.base_iter.nth_back(n);
    }

    template <typename TSelf>
    result_t<TSelf> nth_back(this TSelf&& self, std::size_t n) {
        return self.base_iter.nth(n);
    }
};

struct reverse_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto reverse(this TSelf&& self) {
        auto mixins = registered_mixins<DeferInstantiation>();
        return reverse_iter<std::remove_cvref_t<TSelf>, decltype(mixins)>{ std::forward<TSelf>(self) };
    }
};
} // namespace kissra