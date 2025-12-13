#pragma once
#include "kissra/iter/iter_base.hpp"
#include <functional>

namespace kissra {
template <typename TBaseIter, typename TMixins>
class reverse_iter : public iter_base<TBaseIter>, public TMixins {
public:
    using value_type = typename TBaseIter::value_type;
    using reference = typename TBaseIter::reference;
    using result_t = typename TBaseIter::result_t;

    static constexpr bool is_sized = TBaseIter::is_sized;
    static constexpr bool is_common = TBaseIter::is_common;
    static constexpr bool is_forward = TBaseIter::is_forward;
    static constexpr bool is_bidir = TBaseIter::is_bidir;
    static constexpr bool is_random = TBaseIter::is_random;

    template <typename UBaseIter>
    reverse_iter(UBaseIter&& base_iter)
        : iter_base<TBaseIter>(std::forward<UBaseIter>(base_iter)) {}

    [[nodiscard]] result_t next()
        requires is_common && is_bidir
    {
        return this->base_iter.next_back();
    }

    [[nodiscard]] result_t next_back() {
        return this->base_iter.next();
    }

    [[nodiscard]] result_t nth(std::size_t n)
        requires is_common && is_bidir
    {
        return this->base_iter.nth_back(n);
    }

    [[nodiscard]] result_t nth_back(std::size_t n) {
        return this->base_iter.nth(n);
    }

    std::size_t advance(std::size_t n)
        requires is_bidir
    {
        return this->base_iter.advance_back(n);
    }

    std::size_t advance_back(std::size_t n) {
        return this->base_iter.advance(n);
    }

    auto size() const
        requires is_sized
    {
        return this->base_iter.size();
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