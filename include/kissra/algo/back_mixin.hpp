#pragma once
#include "kissra/type_traits.hpp"

namespace kissra {
struct back_mixin {
    template <typename TSelf>
        requires is_bidir_v<TSelf> && is_common_v<TSelf>
    auto back(this TSelf&& self) {
        return self.nth_back(0);
    }
};
} // namespace kissra