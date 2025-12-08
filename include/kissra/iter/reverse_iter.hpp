#pragma once
#include "kissra/algo/size_mixin.hpp"
#include "kissra/misc/optional.hpp"
#include "kissra/registered_mixins_fwd.hpp"
#include <functional>

namespace kissra {
template <typename TUnderlyingIter, typename TMixins>
class reverse_iter : public size_mixin, public TMixins {
    friend struct size_mixin;

public:
    using value_type = typename TUnderlyingIter::value_type;
    using reference = typename TUnderlyingIter::reference;
    using const_reference = typename TUnderlyingIter::const_reference;

    template <typename TSelf>
    using ref_t = typename TUnderlyingIter::template ref_t<TSelf>;
    
    template <typename TSelf>
    using result_t = typename TUnderlyingIter::template result_t<TSelf>;

    static constexpr bool is_sized = TUnderlyingIter::is_sized;
    static constexpr bool is_common = TUnderlyingIter::is_common;
    static constexpr bool is_forward = TUnderlyingIter::is_forward;
    static constexpr bool is_bidir = TUnderlyingIter::is_bidir;
    static constexpr bool is_random = TUnderlyingIter::is_random;

    template <typename UUnderlyingIter>
    reverse_iter(UUnderlyingIter&& underlying_iter)
        : underlying_iter(std::forward<UUnderlyingIter>(underlying_iter)) {}

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> next(this TSelf&& self) {
        return self.underlying_iter.next_back();
    }

    template <typename TSelf>
    result_t<TSelf> next_back(this TSelf&& self) {
        return self.underlying_iter.next();
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> advance(this TSelf&& self, std::size_t n) {
        return self.underlying_iter.advance_back(n);
    }

    template <typename TSelf>
    result_t<TSelf> advance_back(this TSelf&& self, std::size_t n) {
        return self.underlying_iter.advance(n);
    }

    template <typename TSelf>
        requires is_common && is_bidir
    result_t<TSelf> front(this TSelf&& self) {
        return self.underlying_iter.back();
    }

    template <typename TSelf>
    result_t<TSelf> back(this TSelf&& self) {
        return self.underlying_iter.front();
    }

private:
    TUnderlyingIter underlying_iter;
};

struct reverse_mixin {
    template <typename TSelf, typename DeferInstantiation = void>
    auto reverse(this TSelf&& self) {
        auto mixins = registered_mixins<DeferInstantiation>();
        return reverse_iter<std::remove_cvref_t<TSelf>, decltype(mixins)>{ std::forward<TSelf>(self) };
    }
};
} // namespace kissra