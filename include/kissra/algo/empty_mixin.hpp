#pragma once
#include "kissra/type_traits.hpp"

namespace kissra {
struct empty_mixin {
    template <typename TSelf>
    bool empty(this TSelf&& self) {
        return static_cast<bool>(self.advance(0));
    }
};
} // namespace kissra