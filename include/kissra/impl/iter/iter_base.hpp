#pragma once
#include "kissra/concepts.hpp"
#include "kissra/impl/custom_mixins.hpp"
#include "kissra/impl/export.hpp"
#include "kissra/misc/optional.hpp"

#ifndef KISSRA_MODULE
#include <concepts>
#include <ranges>
#include <utility>
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename TBaseIter>
class iter_base {
public:
    template <typename UBaseIter>
    constexpr iter_base(UBaseIter&& base_iter)
        : base_iter(std::forward<UBaseIter>(base_iter)) {}

    constexpr auto underlying_subrange() const {
        return this->base_iter.underlying_subrange();
    }

    constexpr auto underlying_cursor() const {
        return this->base_iter.underlying_cursor();
    }

    constexpr auto underlying_sentinel() const {
        return this->base_iter.underlying_sentinel();
    }

    template <std::input_iterator TIt, std::sentinel_for<TIt> TSentinel>
    constexpr void underlying_subrange_override(std::ranges::subrange<TIt, TSentinel> subrange) {
        this->base_iter.underlying_subrange_override(subrange);
    }

    template <typename TSelf>
    constexpr auto& base(this TSelf&& self) {
        return self.base_iter;
    }

protected:
    [[no_unique_address]] TBaseIter base_iter;
};
} // namespace kissra