#pragma once
#include "kissra/type_traits.hpp"
#include <vector>

namespace kissra {
struct size_mixin {
    template <typename TSelf>
        requires is_sized_v<TSelf>
    auto size(this TSelf&& self) {
        return self.underlying_iter.size();
    }
};
} // namespace kissra