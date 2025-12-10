#pragma once
#include "kissra/type_traits.hpp"

namespace kissra {
struct empty_mixin {
    template <typename TSelf>
    bool empty(this TSelf&& self) {
        self.advance(0);
        const auto subrange = self.underlying_subrange();
        return subrange.empty();
    }
};
} // namespace kissra