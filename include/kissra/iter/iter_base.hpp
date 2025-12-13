#pragma once
#include "kissra/misc/optional.hpp"
#include "kissra/registered_mixins_fwd.hpp"
#include <utility>

namespace kissra {
template <typename TBaseIter>
class iter_base {
public:
    template <typename UBaseIter>
    iter_base(UBaseIter&& base_iter)
        : base_iter(std::forward<UBaseIter>(base_iter)) {}

    auto underlying_subrange() const {
        return this->base_iter.underlying_subrange();
    }

    auto underlying_cursor() const {
        return this->base_iter.underlying_cursor();
    }

    auto underlying_sentinel() const {
        return this->base_iter.underlying_sentinel();
    }

    template <typename TIt>
    void underlying_subrange_override(std::ranges::subrange<TIt> subrange) {
        this->base_iter.underlying_subrange_override(subrange);
    }

protected:
    TBaseIter base_iter;
};
} // namespace kissra