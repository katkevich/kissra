#pragma once
#include "kissra/concepts.hpp"
#include "kissra/type_traits.hpp"

namespace kissra {
struct empty_mixin {
    template <kissra::mut TSelf>
    bool empty(this TSelf&& self) {
        self.advance(0);
        return self.underlying_subrange().empty();
    }

    template <typename TSelf>
        requires is_sized_v<TSelf>
    bool empty(this const TSelf& self) {
        return self.size() == 0;
    }
};
} // namespace kissra