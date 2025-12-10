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

    template <typename TSelf>
    auto underlying_subrange(this TSelf&& self) {
        return self.base_iter.underlying_subrange();
    }

    template <typename TSelf>
    auto underlying_cursor(this TSelf&& self) {
        return self.base_iter.underlying_cursor();
    }

    template <typename TSelf>
    auto underlying_sentinel(this TSelf&& self) {
        return self.base_iter.underlying_sentinel();
    }

    template <typename TSelf, typename TIt>
    void underlying_subrange_override(this TSelf&& self, std::ranges::subrange<TIt> subrange) {
        self.base_iter.underlying_subrange_override(subrange);
    }

protected:
    TBaseIter base_iter;
};
} // namespace kissra