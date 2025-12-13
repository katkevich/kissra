#pragma once
#include "kissra/concepts.hpp"
#include "kissra/type_traits.hpp"

namespace kissra {
struct empty_mixin {
    template <kissra::mut TSelf>
    bool empty(this TSelf&& self) {
        self.advance(0);
        const auto subrange = self.underlying_subrange();
        return subrange.empty();
    }
};
} // namespace kissra