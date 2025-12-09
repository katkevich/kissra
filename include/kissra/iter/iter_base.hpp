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
    auto underlying_front(this TSelf&& self) {
        return self.base_iter.underlying_front();
    }

    template <typename TSelf>
    auto underlying_back(this TSelf&& self) {
        return self.base_iter.underlying_back();
    }

protected:
    TBaseIter base_iter;
};
} // namespace kissra